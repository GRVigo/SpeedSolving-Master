/*  This file is part of "GR Cube"

	Copyright (C) 2022 German Ramos Rodriguez

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	German Ramos Rodriguez
	Vigo, Spain
	<mailto:grvigo@hotmail.com>
*/

#include <thread>
#include <mutex>
#include <cmath>

#include "deep_eval.h"

namespace grcube3
{
	// Usual steps for a scramble
	const std::array<Stp, 18> DeepEval::MainSteps = 
	{ 
		Stp::U, Stp::Up, Stp::U2, Stp::D, Stp::Dp, Stp::D2, 
		Stp::F, Stp::Fp, Stp::F2, Stp::B, Stp::Bp, Stp::B2, 
		Stp::R, Stp::Rp, Stp::R2, Stp::L, Stp::Lp, Stp::L2
	};
	
	// Scramble evaluation class constructor
    DeepEval::DeepEval(const Algorithm& scr, const uint d) // Scramble to evaluate, maximum depth
	{	
		MaxDepth = (d < 5u ? 5u : (d > 8u ? 8u : d)); // Between 5 and 8
		Time = 0.0f;
		UsedCores = 0; // Not used cores yet
		Cores = GetSystemCores(); // For multithreading
        RootBranches = 0u; // Will be updated during the evaluation
        SetNewScramble(scr); // Apply the scramble
        ResetGroups(); // No groups of pieces to evaluate
	}

	// Add a group of positions to be solved and/or oriented
	void DeepEval::AddToGroups(const std::vector<Pcp>& SolvedGroup, const std::vector<Pcp>& OrientedGroup, const Spn sp, const std::string Id)
	{
		if (SolvedGroup.empty() && OrientedGroup.empty()) return;
		
		GroupCheck G;
		for (const auto P : SolvedGroup)
		{
			// Separate pieces in edge and corner masks
			if (Cube::IsEdge(P)) G.EMask |= Cube::GetMaskSolvedEdge(Cube::PcpToEdp(P));
			else if (Cube::IsCorner(P)) G.CMask |= Cube::GetMaskSolvedCorner(Cube::PcpToCnp(P));
		}
		G.Orient = OrientedGroup; // List of pieces to orient
		G.Spin = sp; // Spin, relevant only for orientation
		
		Groups.push_back(G);
		GroupIds.push_back(Id);
		for (auto& R : Results) R.push_back(0u);
		SolvedFlagsBase.push_back(false);
	}

    // Add groups for standard search
    void DeepEval::StandardGroups(const std::vector<Spn>& SearchSpins)
    {
        ResetGroups();
        const std::vector<Pcp> Empty;

        nSpins = static_cast<uint>(SearchSpins.size());

        for (const auto spin : SearchSpins)
        {
            std::vector<Pcp> Line;
            Line.push_back(Cube::FromAbsPosition(App::DF, spin));
            Line.push_back(Cube::FromAbsPosition(App::DB, spin));
            AddToGroups(Line, Empty, spin, "Line " + Cube::GetSpinText(spin));
        }
        for (const auto spin : SearchSpins)
        {
            std::vector<Pcp> B311;
            B311.push_back(Cube::FromAbsPosition(App::DFL, spin));
            B311.push_back(Cube::FromAbsPosition(App::DL, spin));
            B311.push_back(Cube::FromAbsPosition(App::DBL, spin));
            AddToGroups(B311, Empty, spin, "Block 3x1x1 " + Cube::GetSpinText(spin));
        }
        for (const auto spin : SearchSpins)
        {
            std::vector<Pcp> B221;
            B221.push_back(Cube::FromAbsPosition(App::DBL, spin));
            B221.push_back(Cube::FromAbsPosition(App::DL, spin));
            B221.push_back(Cube::FromAbsPosition(App::BL, spin));
            AddToGroups(B221, Empty, spin, "Block 2x2x1 " + Cube::GetSpinText(spin));
        }
        for (const auto spin : SearchSpins)
        {
            std::vector<Pcp> EO;
            EO.push_back(Cube::FromAbsPosition(App::UF, spin));
            EO.push_back(Cube::FromAbsPosition(App::UB, spin));
            EO.push_back(Cube::FromAbsPosition(App::UR, spin));
            EO.push_back(Cube::FromAbsPosition(App::UL, spin));
            EO.push_back(Cube::FromAbsPosition(App::DF, spin));
            EO.push_back(Cube::FromAbsPosition(App::DB, spin));
            EO.push_back(Cube::FromAbsPosition(App::DR, spin));
            EO.push_back(Cube::FromAbsPosition(App::DL, spin));
            EO.push_back(Cube::FromAbsPosition(App::FR, spin));
            EO.push_back(Cube::FromAbsPosition(App::FL, spin));
            EO.push_back(Cube::FromAbsPosition(App::BR, spin));
            EO.push_back(Cube::FromAbsPosition(App::BL, spin));
            AddToGroups(Empty, EO, spin, "EO " + Cube::GetSpinText(spin));
        }
        for (const auto spin : SearchSpins)
        {
            std::vector<Pcp> CO;
            CO.push_back(Cube::FromAbsPosition(App::UFR, spin));
            CO.push_back(Cube::FromAbsPosition(App::UFL, spin));
            CO.push_back(Cube::FromAbsPosition(App::UBR, spin));
            CO.push_back(Cube::FromAbsPosition(App::UBL, spin));
            CO.push_back(Cube::FromAbsPosition(App::DFR, spin));
            CO.push_back(Cube::FromAbsPosition(App::DFL, spin));
            CO.push_back(Cube::FromAbsPosition(App::DBR, spin));
            CO.push_back(Cube::FromAbsPosition(App::DBL, spin));
            AddToGroups(Empty, CO, spin, "CO " + Cube::GetSpinText(spin));
        }
    }

	// Updates root algorithms
	void DeepEval::UpdateRootData()
	{
		Roots.clear();
		
		Algorithm A; // Empty algorithm

		std::vector<bool> Flags0 = SolvedFlagsBase; // Flags for depth 0 results
		CheckAlg(A, Flags0); // Depth 0 results
		
		for (const auto S1 : MainSteps)
		{
			A.Clear();
			A.Append(S1);

			std::vector<bool> Flags1 = Flags0; // Flags for depth 1 results
			CheckAlg(A, Flags1); // Depth 1 results
			
			for (const auto S2 : MainSteps)
			{
				A.Clear();
				A.Append(S1);
				A.AppendShrink(S2);
				
				if (A.GetSize() != 2u) continue;
				if (Algorithm::OppositeSteps(A[0], A[1]) && A[0] > A[1]) continue; // "D U2" is the same than "U2 D" 
				
				std::vector<bool> Flags2 = Flags1; // Flags for depth 2 results
				CheckAlg(A, Flags2); // Depth 2 results
			
				for (const auto S3 : MainSteps)
				{
					A.Clear();
					A.Append(S1);
					A.AppendShrink(S2);
					A.AppendShrink(S3);

					if (A.GetSize() != 3u) continue;
					if (Algorithm::OppositeSteps(A[1], A[2]) && A[1] > A[2]) continue; // "F D U2" is the same than "F U2 D" 
					
					std::vector<bool> Flags3 = Flags2; // Flags for depth 3 results
					CheckAlg(A, Flags3); // Depth 3 results
					
					Roots.push_back(A);
					RootFlags.push_back(Flags3);
				}
			}
		}
				
		RootBranches = static_cast<uint>(Roots.size());
	}
	
	// Run the evaluation -> -1: use no threads, 0: use all threads avaliable, other: use specified number of threads
	void DeepEval::Run(const int UseThreads)
	{
		const auto time_eval_start = std::chrono::system_clock::now();

		UpdateRootData();

		if (RootBranches == 0u) return;

		if (UseThreads >= 0) // Multithreading
		{
			UsedCores = (UseThreads == 0) || (UseThreads >= Cores) ? Cores : UseThreads;

			std::vector<std::thread> Pool; // Threads pool

			for (uint nc = 0; nc < UsedCores; nc++) Pool.push_back(std::thread(&DeepEval::RunThread, this, nc)); // Add a thread per core
            for (auto& t : Pool) t.join(); // Wait for all threads
		}
        else // Without multithreading (for debugging, slower)
		{
            for (uint n = 0; n < Roots.size(); n++) RunEvaluation(Roots[n], RootFlags[n]);
			UsedCores = -1;
		}

		const std::chrono::duration<double> eval_elapsed_seconds = std::chrono::system_clock::now() - time_eval_start;
        Time = eval_elapsed_seconds.count();
	}
	
    // Run search a thread
    void DeepEval::RunThread(const uint NCore) { for (uint n = NCore; n < RootBranches; n += UsedCores) RunEvaluation(Roots[n], RootFlags[n]); }
    
	// Recursive evaluation code
    void DeepEval::RunEvaluation(const Algorithm& A, const std::vector<bool>& Flags)
	{
		std::vector<bool> NewFlags = Flags; // Copy the solve flags for this branch
		CheckAlg(A, NewFlags); // Check if current algorithm solves or orient the pieces
		if (A.GetSize() < MaxDepth) NextLevel(A, NewFlags);  // Launch the branch for the next level of the evaluation
    }
	
	// Check if an algorithm solve or orient the pieces
    inline void DeepEval::CheckAlg(const Algorithm& A, std::vector<bool>& Flags)
	{
		Cube CubeTest = CubeBase;
		CubeTest.ApplyAlgorithm(A);

		const uint CurrentDepth = A.GetSize();
		
		if (CurrentDepth < Results.size())
		{
			for (uint i = 0u; i < Groups.size(); i++)
			{
				CubeTest.SetSpin(Groups[i].Spin);

				if (!Flags[i] && CubeTest.IsSolved(Groups[i].EMask, Groups[i].CMask) && CubeTest.CheckOrientation(Groups[i].Orient))
				{
					std::lock_guard<std::mutex> guard(SearchMutex);
					Results[CurrentDepth][i]++;
					Flags[i] = true; // Set the solve flag for this branch
				}
			}
		}
	}
	
	// Prepare the branches for the next level of the evaluation
    inline void DeepEval::NextLevel(const Algorithm& A, const std::vector<bool>& Flags)
	{
		const Stp LastStep = A.Last();

		for (const auto S : MainSteps)
		{
			if (Algorithm::OppositeSteps(LastStep, S) && LastStep > S) continue; // As in an algorithm "... U D ..." branch is the same than "... D U ..." branch, compute only one.
			Algorithm NewA = A;
			if (!NewA.AppendShrink(S)) RunEvaluation(NewA, Flags); // Continue the search only on a longer algorithm (shorter or equal lenght means that will be calculated upper in the tree)
		}
	}

    // Get score
    float DeepEval::GetScore(const uint SDepth) const
    {
		uint D = SDepth > MaxDepth ? MaxDepth : SDepth;
        if (Results[D].size() % nSpins != 0u) return -1.0f;

        float Score = 0.0f;

        for (uint i = 0u; i < Results[D].size(); i += nSpins)
        {
            float GScore = 0.0f;
            for (uint j = 0u; j < nSpins; j++) GScore += std::log10(1.0f + Results[D][i+j]);
            GScore /= nSpins;
            Score += GScore;
        }
        return Score;
    }
}
