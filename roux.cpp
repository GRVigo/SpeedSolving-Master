/*  This file is part of "GR Cube"

	Copyright (C) 2021 German Ramos Rodriguez

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
	grvigo@hotmail.com
*/

#include "roux.h"
#include "cfop.h"

#include <chrono>
#include <algorithm>

namespace grcube3
{	
	// Reset the search results
	void Roux::Reset()
	{
        for (int i = 0; i < 24; i++)
        {
			Inspections[i].clear();
            FirstBlocks[i].clear();
            SecondBlocksFS[i].clear();
			SecondBlocksSS[i].clear();
            AlgCMLL[i].clear();
			AlgCOLL[i].clear();
            AlgL6EO[i].clear();
            AlgL6E2E[i].clear();
            AlgL6E[i].clear();
			
            CasesCMLL[i].clear();
            CasesCOLL[i].clear();
        }
		 
        MaxDepthFB = MaxDepthSBFS = MaxDepthSBSS = MaxDepthL6E = MaxDepthL6EO = 0u;
        TimeFB = TimeSBFS = TimeSBSS = TimeCMLL = TimeCOLL = TimeL6EO = TimeL6E2E = TimeL6E = 0.0;

		SearchSpins.clear();
		for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s));
		
		Metric = Metrics::Movements; // Default metric
	}

    // Search the best first blocks solve algorithm with the given search depth
	// Return false if no first blocks found
    bool Roux::SearchFirstBlock(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_fb_start = std::chrono::system_clock::now();

        MaxDepthFB = (MaxDepth <= 4u ? 4u : MaxDepth);

        DeepSearch DSFB(Scramble); // Deep search for first block
		
        DSFB.AddToOptionalPieces(Pgr::UF_B1);
        DSFB.AddToOptionalPieces(Pgr::UB_B1);
        DSFB.AddToOptionalPieces(Pgr::UR_B1);
        DSFB.AddToOptionalPieces(Pgr::UL_B1);

        DSFB.AddToOptionalPieces(Pgr::DF_B1);
        DSFB.AddToOptionalPieces(Pgr::DB_B1);
        DSFB.AddToOptionalPieces(Pgr::DR_B1);
        DSFB.AddToOptionalPieces(Pgr::DL_B1);

        DSFB.AddToOptionalPieces(Pgr::FU_B1);
        DSFB.AddToOptionalPieces(Pgr::FD_B1);
        DSFB.AddToOptionalPieces(Pgr::FR_B1);
        DSFB.AddToOptionalPieces(Pgr::FL_B1);

        DSFB.AddToOptionalPieces(Pgr::BU_B1);
        DSFB.AddToOptionalPieces(Pgr::BD_B1);
        DSFB.AddToOptionalPieces(Pgr::BR_B1);
        DSFB.AddToOptionalPieces(Pgr::BL_B1);

        DSFB.AddToOptionalPieces(Pgr::RU_B1);
        DSFB.AddToOptionalPieces(Pgr::RD_B1);
        DSFB.AddToOptionalPieces(Pgr::RF_B1);
        DSFB.AddToOptionalPieces(Pgr::RB_B1);

        DSFB.AddToOptionalPieces(Pgr::LU_B1);
        DSFB.AddToOptionalPieces(Pgr::LD_B1);
        DSFB.AddToOptionalPieces(Pgr::LF_B1);
        DSFB.AddToOptionalPieces(Pgr::LB_B1);

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
        const SearchUnit URoot(SequenceType::DOUBLE);
        const SearchUnit U(SequenceType::SINGLE);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(URoot);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U);

        SearchLevel L_NoCheck(SearchCheck::NO_CHECK);
        L_NoCheck.Add(U);

        DSFB.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        DSFB.AddSearchLevel(L_NoCheck); // Level 2
        DSFB.AddSearchLevel(L_Check); // Level 3
        for (uint l = 4u; l < MaxDepthFB; l++) DSFB.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

        DSFB.UpdateRootData();
		// DSFB.SetMinDeep(DSFB.GetMaxDepth() - 2u);

        DSFB.Run(Cores);

        Cores = DSFB.GetCoresUsed(); // Update to the real number of cores used

        EvaluateFirstBlock(DSFB.Solves, MaxSolves);

        const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
        TimeFB = fb_elapsed_seconds.count();

        return !DSFB.Solves.empty();
	}
	
	// Search the best first block solve algorithms from an algorithms vector
    void Roux::EvaluateFirstBlock(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best first block solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			EvaluateFBResult(FirstBlocks[sp], MaxSolves, Solves, CubeBase, spin, Plc::BEST_SOLVES);
        }

		for (int sp = 0; sp < 24; sp++)
		{
			Spn spin = static_cast<Spn>(sp);
			Stp T1, T2;
			Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
			Algorithm Insp;
			if (T1 != Stp::NONE) Insp.Append(T1);
			if (T2 != Stp::NONE) Insp.Append(T2);

			for (auto& block : FirstBlocks[sp])
			{
				if (T1 != Stp::NONE) block.TransformTurn(T1);
				if (T2 != Stp::NONE) block.TransformTurn(T2);
				Inspections[sp].push_back(Insp);
			}
		}
	}

    // Search the best second block first square solve algorithm with the given search depth
	void Roux::SearchSecondBlocksFirstSquare(const uint MaxDepth)
    {
        const auto time_sb_start = std::chrono::system_clock::now();

        MaxDepthSBFS = (MaxDepth <= 4u ? 4u : MaxDepth);
		
		const SearchUnit U_SB(SequenceType::SINGLE, Sst::ROUX_SB);
        const SearchUnit U_SB_Seq(SequenceType::RETURN_FIXED_SINGLE, Sst::SINGLE_R, Sst::LATERAL_SINGLE_FBp);

		SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_SB);
		L_Check.Add(U_SB_Seq);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			SecondBlocksFS[sp].clear();

			Pgr B1, B2S1, B2S2;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; B2S1 = Pgr::UF_B2S1; B2S2 = Pgr::UF_B2S2; break;
			case Spn::UB: B1 = Pgr::UB_B1; B2S1 = Pgr::UB_B2S1; B2S2 = Pgr::UB_B2S2; break;
			case Spn::UR: B1 = Pgr::UR_B1; B2S1 = Pgr::UR_B2S1;	B2S2 = Pgr::UR_B2S2; break;
			case Spn::UL: B1 = Pgr::UL_B1; B2S1 = Pgr::UL_B2S1; B2S2 = Pgr::UL_B2S2; break;

			case Spn::DF: B1 = Pgr::DF_B1; B2S1 = Pgr::DF_B2S1; B2S2 = Pgr::DF_B2S2; break;
			case Spn::DB: B1 = Pgr::DB_B1; B2S1 = Pgr::DB_B2S1; B2S2 = Pgr::DB_B2S2; break;
			case Spn::DR: B1 = Pgr::DR_B1; B2S1 = Pgr::DR_B2S1; B2S2 = Pgr::DR_B2S2; break;
			case Spn::DL: B1 = Pgr::DL_B1; B2S1 = Pgr::DL_B2S1; B2S2 = Pgr::DL_B2S2; break;

			case Spn::FU: B1 = Pgr::FU_B1; B2S1 = Pgr::FU_B2S1; B2S2 = Pgr::FU_B2S2; break;
			case Spn::FD: B1 = Pgr::FD_B1; B2S1 = Pgr::FD_B2S1; B2S2 = Pgr::FD_B2S2; break;
			case Spn::FR: B1 = Pgr::FR_B1; B2S1 = Pgr::FR_B2S1; B2S2 = Pgr::FR_B2S2; break;
			case Spn::FL: B1 = Pgr::FL_B1; B2S1 = Pgr::FL_B2S1; B2S2 = Pgr::FL_B2S2; break;

			case Spn::BU: B1 = Pgr::BU_B1; B2S1 = Pgr::BU_B2S1;	B2S2 = Pgr::BU_B2S2; break;
			case Spn::BD: B1 = Pgr::BD_B1; B2S1 = Pgr::BD_B2S1; B2S2 = Pgr::BD_B2S2; break;
			case Spn::BR: B1 = Pgr::BR_B1; B2S1 = Pgr::BR_B2S1; B2S2 = Pgr::BR_B2S2; break;
			case Spn::BL: B1 = Pgr::BL_B1; B2S1 = Pgr::BL_B2S1; B2S2 = Pgr::BL_B2S2; break;

			case Spn::RU: B1 = Pgr::RU_B1; B2S1 = Pgr::RU_B2S1; B2S2 = Pgr::RU_B2S2; break;
			case Spn::RD: B1 = Pgr::RD_B1; B2S1 = Pgr::RD_B2S1; B2S2 = Pgr::RD_B2S2; break;
			case Spn::RF: B1 = Pgr::RF_B1; B2S1 = Pgr::RF_B2S1; B2S2 = Pgr::RF_B2S2; break;
			case Spn::RB: B1 = Pgr::RB_B1; B2S1 = Pgr::RB_B2S1; B2S2 = Pgr::RB_B2S2; break;

			case Spn::LU: B1 = Pgr::LU_B1; B2S1 = Pgr::LU_B2S1; B2S2 = Pgr::LU_B2S2; break;
			case Spn::LD: B1 = Pgr::LD_B1; B2S1 = Pgr::LD_B2S1; B2S2 = Pgr::LD_B2S2; break;
			case Spn::LF: B1 = Pgr::LF_B1; B2S1 = Pgr::LF_B2S1; B2S2 = Pgr::LF_B2S2; break;
			case Spn::LB: B1 = Pgr::LB_B1; B2S1 = Pgr::LB_B2S1; B2S2 = Pgr::LB_B2S2; break;
			}

			uint n = 0u;
			for (const auto& FB : FirstBlocks[sp])
			{
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FB);

				DeepSearch DSSB(AlgStart); // Deep search for second block

				DSSB.AddToMandatoryPieces(B1);
				DSSB.AddToOptionalPieces(B2S1);
				DSSB.AddToOptionalPieces(B2S2);

				for (uint l = 1u; l < MaxDepthSBFS; l++) DSSB.AddSearchLevel(L_Check); // Add needed search levels

				DSSB.UpdateRootData();

				DSSB.Run(Cores);
				
				Cores = DSSB.GetCoresUsed(); // Update to the real number of cores used

				Cube CubeRoux(AlgStart);

				std::vector<Algorithm> Solves;
				// This function is made for CFOP, but, without the cross check ('false' parameter), is useful here
				CFOP::EvaluateF2LResult(Solves, 1u, DSSB.Solves, CubeRoux, Cube::GetDownSliceLayer(spin), Plc::BEST_SOLVES, false);

				if (Solves.empty()) SecondBlocksFS[sp].push_back(Algorithm(""));
				else
				{
					CubeRoux.ApplyAlgorithm(Solves[0]);
					if (IsFBAndSBSquareBuilt(CubeRoux)) SecondBlocksFS[sp].push_back(Solves[0]);
					else SecondBlocksFS[sp].push_back(Algorithm(""));
				}
				n++;
			}
        }

        const std::chrono::duration<double> sb_elapsed_seconds = std::chrono::system_clock::now() - time_sb_start;
        TimeSBFS = sb_elapsed_seconds.count();
    }
	
	// Complete the second square for the second blocks
    void Roux::SearchSecondBlocksSecondSquare(const uint MaxDepth)
    {
        auto time_SBSS_start = std::chrono::system_clock::now();

		MaxDepthSBSS = (MaxDepth <= 4u ? 4u : MaxDepth);

		const SearchUnit U_Up(SequenceType::SINGLE, Sst::SINGLE_U);
        const SearchUnit U_Lat(SequenceType::RETURN_FIXED_SINGLE, Sst::SINGLE_U, Sst::LATERAL_UD);

        SearchLevel L_F2L_Check(SearchCheck::CHECK);
        L_F2L_Check.Add(U_Up);
        L_F2L_Check.Add(U_Lat);
		
        SearchLevel L_F2L_End(SearchCheck::CHECK);
        L_F2L_End.Add(U_Lat);

		Pgr B1, B2;

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			SecondBlocksSS[sp].clear();

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; B2 = Pgr::UF_B2; break;
			case Spn::UB: B1 = Pgr::UB_B1; B2 = Pgr::UB_B2; break;
			case Spn::UR: B1 = Pgr::UR_B1; B2 = Pgr::UR_B2; break;
			case Spn::UL: B1 = Pgr::UL_B1; B2 = Pgr::UL_B2; break;

			case Spn::DF: B1 = Pgr::DF_B1; B2 = Pgr::DF_B2; break;
			case Spn::DB: B1 = Pgr::DB_B1; B2 = Pgr::DB_B2; break;
			case Spn::DR: B1 = Pgr::DR_B1; B2 = Pgr::DR_B2; break;
			case Spn::DL: B1 = Pgr::DL_B1; B2 = Pgr::DL_B2; break;

			case Spn::FU: B1 = Pgr::FU_B1; B2 = Pgr::FU_B2; break;
			case Spn::FD: B1 = Pgr::FD_B1; B2 = Pgr::FD_B2; break;
			case Spn::FR: B1 = Pgr::FR_B1; B2 = Pgr::FR_B2; break;
			case Spn::FL: B1 = Pgr::FL_B1; B2 = Pgr::FL_B2; break;

			case Spn::BU: B1 = Pgr::BU_B1; B2 = Pgr::BU_B2; break;
			case Spn::BD: B1 = Pgr::BD_B1; B2 = Pgr::BD_B2; break;
			case Spn::BR: B1 = Pgr::BR_B1; B2 = Pgr::BR_B2; break;
			case Spn::BL: B1 = Pgr::BL_B1; B2 = Pgr::BL_B2; break;

			case Spn::RU: B1 = Pgr::RU_B1; B2 = Pgr::RU_B2; break;
			case Spn::RD: B1 = Pgr::RD_B1; B2 = Pgr::RD_B2; break;
			case Spn::RF: B1 = Pgr::RF_B1; B2 = Pgr::RF_B2; break;
			case Spn::RB: B1 = Pgr::RB_B1; B2 = Pgr::RB_B2; break;

			case Spn::LU: B1 = Pgr::LU_B1; B2 = Pgr::LU_B2; break;
			case Spn::LD: B1 = Pgr::LD_B1; B2 = Pgr::LD_B2; break;
			case Spn::LF: B1 = Pgr::LF_B1; B2 = Pgr::LF_B2; break;
			case Spn::LB: B1 = Pgr::LB_B1; B2 = Pgr::LB_B2; break;
			}
			
			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);

				Cube CubeRoux(AlgStart);

				// No second block or second block already complete
				if (!IsFBAndSBSquareBuilt(CubeRoux) || AreFirstBlocksBuilt(CubeRoux))
				{
					SecondBlocksSS[sp].push_back(Algorithm(""));
					continue; 
				}

				DeepSearch DSSB(AlgStart, Plc::SHORT); // Deep search to complete second block

				DSSB.AddToMandatoryPieces(B1);
				DSSB.AddToMandatoryPieces(B2);

				for (uint l = 1u; l < MaxDepthSBSS; l++) DSSB.AddSearchLevel(L_F2L_Check); // Add needed search levels
				DSSB.AddSearchLevel(L_F2L_End);

				DSSB.UpdateRootData();

				DSSB.Run(Cores);

				std::vector<Algorithm> Solves;
				// This function is made for CFOP, but, without the cross check ('false' parameter), is useful here
				CFOP::EvaluateF2LResult(Solves, 1u, DSSB.Solves, CubeRoux, Cube::GetDownSliceLayer(spin), Plc::SHORT, false);

				if (Solves.empty()) SecondBlocksSS[sp].push_back(Algorithm(""));
				else
				{
					CubeRoux.ApplyAlgorithm(Solves[0]);
					if (AreFirstBlocksBuilt(CubeRoux)) SecondBlocksSS[sp].push_back(Solves[0]);
					else SecondBlocksSS[sp].push_back(Algorithm(""));
				}
			}
		}

        const std::chrono::duration<double> SBSS_elapsed_seconds = std::chrono::system_clock::now() - time_SBSS_start;
        TimeSBSS = SBSS_elapsed_seconds.count();
    }
	
	// CMLL search
    void Roux::SearchCMLL(const bool AddLastUMovement)
	{
        const auto time_CMLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgCMLL[sp].clear();

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				AlgCMLL[sp].push_back(Algorithm(""));
                CasesCMLL[sp].push_back("");

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);

				const Cube CubeRoux(AlgStart);

				if (!AreFirstBlocksBuilt(CubeRoux)) continue;

				Stp LastUStep;

                Cube::CornersLL(AlgCMLL[sp][n], CasesCMLL[sp][n], LastUStep, AlgSets::CMLL, CubeRoux);

				if (AddLastUMovement) AlgCMLL[sp][n].Append(LastUStep);
            }
		}

        const std::chrono::duration<double> CMLL_elapsed_seconds = std::chrono::system_clock::now() - time_CMLL_start;
        TimeCMLL = CMLL_elapsed_seconds.count();
	}
    
    // COLL search
    void Roux::SearchCOLL(const bool AddLastUMovement)
	{
        const auto time_COLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgCOLL[sp].clear();

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				AlgCOLL[sp].push_back(Algorithm(""));
                CasesCOLL[sp].push_back("");

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);

				const Cube CubeRoux(AlgStart);

				if (!AreFirstBlocksBuilt(CubeRoux)) continue;

				Stp LastUStep;

                Cube::CornersLL(AlgCOLL[sp][n], CasesCOLL[sp][n], LastUStep, AlgSets::COLL, CubeRoux);

				if (AddLastUMovement) AlgCOLL[sp][n].Append(LastUStep);
			}
		}

        const std::chrono::duration<double> COLL_elapsed_seconds = std::chrono::system_clock::now() - time_COLL_start;
        TimeCOLL = COLL_elapsed_seconds.count();
	}

    // Search the last six edges (U & M layer movements)
    void Roux::SearchL6E(const uint MaxDepth)
    {
        const auto time_L6E_start = std::chrono::system_clock::now();

        MaxDepthL6E = (MaxDepth <= 4u ? 4u : MaxDepth);

        // First level is extended in the search to improve the multithreading
        const SearchUnit U_Root(SequenceType::TRIPLE, Sst::SINGLE_UM);
        const SearchUnit U_Level(SequenceType::SINGLE, Sst::SINGLE_UM);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(U_Root);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_Level);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgL6E[sp].clear();

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				AlgL6E[sp].push_back(Algorithm(""));

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);
				if (!AlgCMLL[sp].empty()) AlgStart.Append(AlgCMLL[sp][n]);
				if (!AlgCOLL[sp].empty()) AlgStart.Append(AlgCOLL[sp][n]);
                if (!AlgL6EO[sp].empty()) AlgStart.Append(AlgL6EO[sp][n]);
                if (!AlgL6E2E[sp].empty()) AlgStart.Append(AlgL6E2E[sp][n]);
				
				Cube CubeRoux(AlgStart);

				if (!AreFirstBlocksBuilt(CubeRoux) || CubeRoux.IsSolved()) continue;

				// Auxiliar deep search for last six edges in 3 movements (very fast)
				DeepSearch PreDSL6E(AlgStart, Plc::SHORT);
				PreDSL6E.AddToMandatoryPieces(Pgr::ALL); // Search the full cube
				PreDSL6E.AddSearchLevel(L_Check);
				PreDSL6E.AddSearchLevel(L_Check);
				PreDSL6E.AddSearchLevel(L_Check);
				PreDSL6E.UpdateRootData();
				PreDSL6E.Run(Cores);
				if (!PreDSL6E.Solves.empty())
				{
					PreDSL6E.EvaluateShortestResult(AlgL6E[sp][n], true);
					continue; // L6E solve in three movements found
				}
				
				// Full deep search for last six edges
				DeepSearch DSL6E(AlgStart, Plc::SHORT);

				DSL6E.AddToMandatoryPieces(Pgr::ALL); // Search the full cube

				DSL6E.AddSearchLevel(L_Root); // Level 1 (three steps -TRIPLE- root algorithms
				for (uint l = 2; l < MaxDepthL6E; l++) DSL6E.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

				DSL6E.UpdateRootData();
				DSL6E.SetMinDeep(DSL6E.GetMaxDeep() - 4u);

				DSL6E.Run(Cores);

				DSL6E.EvaluateShortestResult(AlgL6E[sp][n], true);
			}
        }

        const std::chrono::duration<double> L6E_elapsed_seconds = std::chrono::system_clock::now() - time_L6E_start;
        TimeL6E = L6E_elapsed_seconds.count();
    }
	
	// Search the last six edges orientated (U & M layer movements) with the given search deep
    void Roux::SearchL6EO(const uint MaxDepth)
    {
        const auto time_L6EO_start = std::chrono::system_clock::now();

        MaxDepthL6EO = (MaxDepth <= 4u ? 4u : MaxDepth);

        // First level is extended in the search to improve the multithreading - first three levels will not be checked
        // (it's supose that the last six edges will not be solved in three movements)
        const SearchUnit U_Root(SequenceType::TRIPLE, Sst::SINGLE_UM);
        const SearchUnit U_Level(SequenceType::SINGLE, Sst::SINGLE_UM);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(U_Root);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_Level);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

            AlgL6EO[sp].clear();

			Pgr B1, B2, CORNERS, EDGES;

			switch (static_cast<Spn>(sp))
			{
			case Spn::UF: B1 = Pgr::UF_B1; B2 = Pgr::UF_B2;	CORNERS = Pgr::CORNERS_U; EDGES = Pgr::ROUX_L6E_UF; break;
			case Spn::UB: B1 = Pgr::UB_B1; B2 = Pgr::UB_B2;	CORNERS = Pgr::CORNERS_U; EDGES = Pgr::ROUX_L6E_UB; break;
			case Spn::UR: B1 = Pgr::UR_B1; B2 = Pgr::UR_B2; CORNERS = Pgr::CORNERS_U; EDGES = Pgr::ROUX_L6E_UR; break;
			case Spn::UL: B1 = Pgr::UL_B1; B2 = Pgr::UL_B2; CORNERS = Pgr::CORNERS_U; EDGES = Pgr::ROUX_L6E_UL; break;

			case Spn::DF: B1 = Pgr::DF_B1; B2 = Pgr::DF_B2; CORNERS = Pgr::CORNERS_D; EDGES = Pgr::ROUX_L6E_DF; break;
			case Spn::DB: B1 = Pgr::DB_B1; B2 = Pgr::DB_B2; CORNERS = Pgr::CORNERS_D; EDGES = Pgr::ROUX_L6E_DB; break;
			case Spn::DR: B1 = Pgr::DR_B1; B2 = Pgr::DR_B2; CORNERS = Pgr::CORNERS_D; EDGES = Pgr::ROUX_L6E_DR; break;
			case Spn::DL: B1 = Pgr::DL_B1; B2 = Pgr::DL_B2; CORNERS = Pgr::CORNERS_D; EDGES = Pgr::ROUX_L6E_DL; break;

			case Spn::FU: B1 = Pgr::FU_B1; B2 = Pgr::FU_B2; CORNERS = Pgr::CORNERS_F; EDGES = Pgr::ROUX_L6E_FU; break;
			case Spn::FD: B1 = Pgr::FD_B1; B2 = Pgr::FD_B2; CORNERS = Pgr::CORNERS_F; EDGES = Pgr::ROUX_L6E_FD; break;
			case Spn::FR: B1 = Pgr::FR_B1; B2 = Pgr::FR_B2; CORNERS = Pgr::CORNERS_F; EDGES = Pgr::ROUX_L6E_FR; break;
			case Spn::FL: B1 = Pgr::FL_B1; B2 = Pgr::FL_B2; CORNERS = Pgr::CORNERS_F; EDGES = Pgr::ROUX_L6E_FL; break;

			case Spn::BU: B1 = Pgr::BU_B1; B2 = Pgr::BU_B2; CORNERS = Pgr::CORNERS_B; EDGES = Pgr::ROUX_L6E_BU; break;
			case Spn::BD: B1 = Pgr::BD_B1; B2 = Pgr::BD_B2; CORNERS = Pgr::CORNERS_B; EDGES = Pgr::ROUX_L6E_BD; break;
			case Spn::BR: B1 = Pgr::BR_B1; B2 = Pgr::BR_B2; CORNERS = Pgr::CORNERS_B; EDGES = Pgr::ROUX_L6E_BR; break;
			case Spn::BL: B1 = Pgr::BL_B1; B2 = Pgr::BL_B2; CORNERS = Pgr::CORNERS_B; EDGES = Pgr::ROUX_L6E_BL; break;

			case Spn::RU: B1 = Pgr::RU_B1; B2 = Pgr::RU_B2; CORNERS = Pgr::CORNERS_R; EDGES = Pgr::ROUX_L6E_RU; break;
			case Spn::RD: B1 = Pgr::RD_B1; B2 = Pgr::RD_B2; CORNERS = Pgr::CORNERS_R; EDGES = Pgr::ROUX_L6E_RD; break;
			case Spn::RF: B1 = Pgr::RF_B1; B2 = Pgr::RF_B2; CORNERS = Pgr::CORNERS_R; EDGES = Pgr::ROUX_L6E_RF; break;
			case Spn::RB: B1 = Pgr::RB_B1; B2 = Pgr::RB_B2; CORNERS = Pgr::CORNERS_R; EDGES = Pgr::ROUX_L6E_RB; break;

			case Spn::LU: B1 = Pgr::LU_B1; B2 = Pgr::LU_B2; CORNERS = Pgr::CORNERS_L; EDGES = Pgr::ROUX_L6E_LU; break;
			case Spn::LD: B1 = Pgr::LD_B1; B2 = Pgr::LD_B2; CORNERS = Pgr::CORNERS_L; EDGES = Pgr::ROUX_L6E_LD; break;
			case Spn::LF: B1 = Pgr::LF_B1; B2 = Pgr::LF_B2; CORNERS = Pgr::CORNERS_L; EDGES = Pgr::ROUX_L6E_LF; break;
			case Spn::LB: B1 = Pgr::LB_B1; B2 = Pgr::LB_B2; CORNERS = Pgr::CORNERS_L; EDGES = Pgr::ROUX_L6E_LB; break;
			}

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
                AlgL6EO[sp].push_back(Algorithm(""));

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);
				if (!AlgCMLL[sp].empty()) AlgStart.Append(AlgCMLL[sp][n]);
				if (!AlgCOLL[sp].empty()) AlgStart.Append(AlgCOLL[sp][n]);

				Cube CubeRoux(AlgStart);

				if (!AreFirstBlocksBuilt(CubeRoux) || IsL6EOriented(CubeRoux)) continue; // Last six edges already oriented or no solve

                DeepSearch DSOL6EO(AlgStart, Plc::SHORT); // Deep search for last six edges

                DSOL6EO.AddToMandatoryPieces(B1);
                DSOL6EO.AddToMandatoryPieces(B2);
                DSOL6EO.AddToMandatoryPieces(CORNERS);
                DSOL6EO.AddToMandatoryOrientations(EDGES);

                DSOL6EO.AddSearchLevel(L_Root); // Level 1 (three steps -TRIPLE- root algorithms
                for (uint l = 2; l < MaxDepthL6EO; l++) DSOL6EO.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

                DSOL6EO.UpdateRootData();

                DSOL6EO.Run(Cores);

                DSOL6EO.EvaluateShortestResult(AlgL6EO[sp][n], true);
			}
        }

        const std::chrono::duration<double> L6EO_elapsed_seconds = std::chrono::system_clock::now() - time_L6EO_start;
        TimeL6EO = L6EO_elapsed_seconds.count();
    }

	// Search the UR & UL edges (L6E)
    void Roux::SearchL6E2E(const uint MaxDepth)
    {
        const auto time_L6E2E_start = std::chrono::system_clock::now();

        MaxDepthL6E2E = (MaxDepth <= 4u ? 4u : MaxDepth);

        // First level is extended in the search to improve the multithreading - first two levels will not be checked
        // (it's supose that the last six edges will not be solved in two movements)
        SearchUnit U_Root(SequenceType::DOUBLE, Sst::SINGLE_UM);
        SearchUnit U_Level(SequenceType::SINGLE, Sst::SINGLE_UM);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(U_Root);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_Level);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

            AlgL6E2E[sp].clear();

			Pgr LR, LL, LM;

			switch (static_cast<Spn>(sp))
			{
			case Spn::UF:
			case Spn::UB:
			case Spn::DF:
			case Spn::DB:
			case Spn::FU:
			case Spn::FD:
			case Spn::BU:
			case Spn::BD: LR = Pgr::LAYER_R; LL = Pgr::LAYER_L; LM = Pgr::LAYER_M; break;

			case Spn::UR:
			case Spn::UL:
			case Spn::DR:
			case Spn::DL:
			case Spn::RU:
			case Spn::RD:
			case Spn::LU:
			case Spn::LD: LR = Pgr::LAYER_F; LL = Pgr::LAYER_B; LM = Pgr::LAYER_S; break;

			case Spn::FR:
			case Spn::FL:
			case Spn::BR:
			case Spn::BL:
			case Spn::RF:
			case Spn::RB:
			case Spn::LF:
			case Spn::LB: LR = Pgr::LAYER_U; LL = Pgr::LAYER_D; LM = Pgr::LAYER_E; break;
			}

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
                AlgL6E2E[sp].push_back(Algorithm(""));

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);
				if (!AlgCMLL[sp].empty()) AlgStart.Append(AlgCMLL[sp][n]);
				if (!AlgCOLL[sp].empty()) AlgStart.Append(AlgCOLL[sp][n]);
                AlgStart.Append(AlgL6EO[sp][n]);

				Cube CubeRoux(AlgStart);

                if (!IsL6EOriented(CubeRoux) || IsL6EO2E(CubeRoux)) continue; // L6E two edges solved or no solve

                DeepSearch DSL6E2E(AlgStart, Plc::SHORT); // Deep search for last six edges (UR & UL edges)

                DSL6E2E.AddToMandatoryPieces(LR);
                DSL6E2E.AddToMandatoryPieces(LL);
                DSL6E2E.AddToMandatoryOrientations(LM);

                DSL6E2E.AddSearchLevel(L_Root); // Level 1 (three steps -TRIPLE- root algorithms
                for (uint l = 2; l < MaxDepthL6E2E; l++) DSL6E2E.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

                DSL6E2E.UpdateRootData();

                DSL6E2E.Run(Cores);

                DSL6E2E.EvaluateShortestResult(AlgL6E2E[sp][n], true);
			}
		}

        const std::chrono::duration<double> L6E2E_elapsed_seconds = std::chrono::system_clock::now() - time_L6E2E_start;
        TimeL6E2E = L6E2E_elapsed_seconds.count();
    }

	// Set regrips
	void Roux::SetRegrips()
	{
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			if (!CheckSolveConsistency(spin)) continue;

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				FirstBlocks[sp][n] = FirstBlocks[sp][n].GetRegrip();

				if (Algorithm::IsTurn(FirstBlocks[sp][n].First()))
				{
					Inspections[sp][n].AppendShrink(FirstBlocks[sp][n].First());
					if (Inspections[sp][n].GetSize() == 3u) Inspections[sp][n] = Inspections[sp][n].GetCancellations();
					FirstBlocks[sp][n].EraseFirst();
				}
			}
		}
	}

	// Check if the left Roux block is built (first block)
	bool Roux::IsFBBuilt(const Cube& C)
	{
		// Roux cube with desired spin
		switch (C.GetSpin())
		{
		case Spn::UF: return C.IsSolved(Pgr::UF_B1);
		case Spn::UB: return C.IsSolved(Pgr::UB_B1);
		case Spn::UR: return C.IsSolved(Pgr::UR_B1);
		case Spn::UL: return C.IsSolved(Pgr::UL_B1);

		case Spn::DF: return C.IsSolved(Pgr::DF_B1);
		case Spn::DB: return C.IsSolved(Pgr::DB_B1);
		case Spn::DR: return C.IsSolved(Pgr::DR_B1);
		case Spn::DL: return C.IsSolved(Pgr::DL_B1);

		case Spn::FU: return C.IsSolved(Pgr::FU_B1);
		case Spn::FD: return C.IsSolved(Pgr::FD_B1);
		case Spn::FR: return C.IsSolved(Pgr::FR_B1);
		case Spn::FL: return C.IsSolved(Pgr::FL_B1);

		case Spn::BU: return C.IsSolved(Pgr::BU_B1);
		case Spn::BD: return C.IsSolved(Pgr::BD_B1);
		case Spn::BR: return C.IsSolved(Pgr::BR_B1);
		case Spn::BL: return C.IsSolved(Pgr::BL_B1);

		case Spn::RU: return C.IsSolved(Pgr::RU_B1);
		case Spn::RD: return C.IsSolved(Pgr::RD_B1);
		case Spn::RF: return C.IsSolved(Pgr::RF_B1);
		case Spn::RB: return C.IsSolved(Pgr::RB_B1);

		case Spn::LU: return C.IsSolved(Pgr::LU_B1);
		case Spn::LD: return C.IsSolved(Pgr::LD_B1);
		case Spn::LF: return C.IsSolved(Pgr::LF_B1);
		case Spn::LB: return C.IsSolved(Pgr::LB_B1);

		default: return false; // Should not happend
		}
	}

	// Check if the first Roux block is built with the given spin
	bool Roux::IsFBBuilt(const Cube& C, const Spn S)
	{
		// Roux cube with desired spin
		switch (S)
		{
		case Spn::UF: return C.IsSolved(Pgr::UF_B1);
		case Spn::UB: return C.IsSolved(Pgr::UB_B1);
		case Spn::UR: return C.IsSolved(Pgr::UR_B1);
		case Spn::UL: return C.IsSolved(Pgr::UL_B1);

		case Spn::DF: return C.IsSolved(Pgr::DF_B1);
		case Spn::DB: return C.IsSolved(Pgr::DB_B1);
		case Spn::DR: return C.IsSolved(Pgr::DR_B1);
		case Spn::DL: return C.IsSolved(Pgr::DL_B1);

		case Spn::FU: return C.IsSolved(Pgr::FU_B1);
		case Spn::FD: return C.IsSolved(Pgr::FD_B1);
		case Spn::FR: return C.IsSolved(Pgr::FR_B1);
		case Spn::FL: return C.IsSolved(Pgr::FL_B1);

		case Spn::BU: return C.IsSolved(Pgr::BU_B1);
		case Spn::BD: return C.IsSolved(Pgr::BD_B1);
		case Spn::BR: return C.IsSolved(Pgr::BR_B1);
		case Spn::BL: return C.IsSolved(Pgr::BL_B1);

		case Spn::RU: return C.IsSolved(Pgr::RU_B1);
		case Spn::RD: return C.IsSolved(Pgr::RD_B1);
		case Spn::RF: return C.IsSolved(Pgr::RF_B1);
		case Spn::RB: return C.IsSolved(Pgr::RB_B1);

		case Spn::LU: return C.IsSolved(Pgr::LU_B1);
		case Spn::LD: return C.IsSolved(Pgr::LD_B1);
		case Spn::LF: return C.IsSolved(Pgr::LF_B1);
		case Spn::LB: return C.IsSolved(Pgr::LB_B1);

		default: return false; // Should not happend
		}
	}

	// Check if the right Roux block is built (second block)
	bool Roux::IsSBBuilt(const Cube& C)
	{
		// Roux cube with desired spin
		switch (C.GetSpin())
		{
		case Spn::UF: return C.IsSolved(Pgr::UF_B2);
		case Spn::UB: return C.IsSolved(Pgr::UB_B2);
		case Spn::UR: return C.IsSolved(Pgr::UR_B2);
		case Spn::UL: return C.IsSolved(Pgr::UL_B2);

		case Spn::DF: return C.IsSolved(Pgr::DF_B2);
		case Spn::DB: return C.IsSolved(Pgr::DB_B2);
		case Spn::DR: return C.IsSolved(Pgr::DR_B2);
		case Spn::DL: return C.IsSolved(Pgr::DL_B2);

		case Spn::FU: return C.IsSolved(Pgr::FU_B2);
		case Spn::FD: return C.IsSolved(Pgr::FD_B2);
		case Spn::FR: return C.IsSolved(Pgr::FR_B2);
		case Spn::FL: return C.IsSolved(Pgr::FL_B2);

		case Spn::BU: return C.IsSolved(Pgr::BU_B2);
		case Spn::BD: return C.IsSolved(Pgr::BD_B2);
		case Spn::BR: return C.IsSolved(Pgr::BR_B2);
		case Spn::BL: return C.IsSolved(Pgr::BL_B2);

		case Spn::RU: return C.IsSolved(Pgr::RU_B2);
		case Spn::RD: return C.IsSolved(Pgr::RD_B2);
		case Spn::RF: return C.IsSolved(Pgr::RF_B2);
		case Spn::RB: return C.IsSolved(Pgr::RB_B2);

		case Spn::LU: return C.IsSolved(Pgr::LU_B2);
		case Spn::LD: return C.IsSolved(Pgr::LD_B2);
		case Spn::LF: return C.IsSolved(Pgr::LF_B2);
		case Spn::LB: return C.IsSolved(Pgr::LB_B2);

		default: return false; // Should not happend
		}
	}

	// Check if the left Roux block is built and an square in the right block
	bool Roux::IsFBAndSBSquareBuilt(const Cube& C)
	{
		switch (C.GetSpin())
		{
		case Spn::UF: return C.IsSolved(Pgr::UF_B1) &&
			(C.IsSolved(Pgr::UF_B2S1) || C.IsSolved(Pgr::UF_B2S2));
		case Spn::UB: return C.IsSolved(Pgr::UB_B1) &&
			(C.IsSolved(Pgr::UB_B2S1) || C.IsSolved(Pgr::UB_B2S2));
		case Spn::UR: return C.IsSolved(Pgr::UR_B1) &&
			(C.IsSolved(Pgr::UR_B2S1) || C.IsSolved(Pgr::UR_B2S2));
		case Spn::UL: return C.IsSolved(Pgr::UL_B1) &&
			(C.IsSolved(Pgr::UL_B2S1) || C.IsSolved(Pgr::UL_B2S2));

		case Spn::DF: return C.IsSolved(Pgr::DF_B1) &&
			(C.IsSolved(Pgr::DF_B2S1) || C.IsSolved(Pgr::DF_B2S2));
		case Spn::DB: return C.IsSolved(Pgr::DB_B1) &&
			(C.IsSolved(Pgr::DB_B2S1) || C.IsSolved(Pgr::DB_B2S2));
		case Spn::DR: return C.IsSolved(Pgr::DR_B1) &&
			(C.IsSolved(Pgr::DR_B2S1) || C.IsSolved(Pgr::DR_B2S2));
		case Spn::DL: return C.IsSolved(Pgr::DL_B1) &&
			(C.IsSolved(Pgr::DL_B2S1) || C.IsSolved(Pgr::DL_B2S2));

		case Spn::FU: return C.IsSolved(Pgr::FU_B1) &&
			(C.IsSolved(Pgr::FU_B2S1) || C.IsSolved(Pgr::FU_B2S2));
		case Spn::FD: return C.IsSolved(Pgr::FD_B1) &&
			(C.IsSolved(Pgr::FD_B2S1) || C.IsSolved(Pgr::FD_B2S2));
		case Spn::FR: return C.IsSolved(Pgr::FR_B1) &&
			(C.IsSolved(Pgr::FR_B2S1) || C.IsSolved(Pgr::FR_B2S2));
		case Spn::FL: return C.IsSolved(Pgr::FL_B1) &&
			(C.IsSolved(Pgr::FL_B2S1) || C.IsSolved(Pgr::FL_B2S2));

		case Spn::BU: return C.IsSolved(Pgr::BU_B1) &&
			(C.IsSolved(Pgr::BU_B2S1) || C.IsSolved(Pgr::BU_B2S2));
		case Spn::BD: return C.IsSolved(Pgr::BD_B1) &&
			(C.IsSolved(Pgr::BD_B2S1) || C.IsSolved(Pgr::BD_B2S2));
		case Spn::BR: return C.IsSolved(Pgr::BR_B1) &&
			(C.IsSolved(Pgr::BR_B2S1) || C.IsSolved(Pgr::BR_B2S2));
		case Spn::BL: return C.IsSolved(Pgr::BL_B1) &&
			(C.IsSolved(Pgr::BL_B2S1) || C.IsSolved(Pgr::BL_B2S2));

		case Spn::RU: return C.IsSolved(Pgr::RU_B1) &&
			(C.IsSolved(Pgr::RU_B2S1) || C.IsSolved(Pgr::RU_B2S2));
		case Spn::RD: return C.IsSolved(Pgr::RD_B1) &&
			(C.IsSolved(Pgr::RD_B2S1) || C.IsSolved(Pgr::RD_B2S2));
		case Spn::RF: return C.IsSolved(Pgr::RF_B1) &&
			(C.IsSolved(Pgr::RF_B2S1) || C.IsSolved(Pgr::RF_B2S2));
		case Spn::RB: return C.IsSolved(Pgr::RB_B1) &&
			(C.IsSolved(Pgr::RB_B2S1) || C.IsSolved(Pgr::RB_B2S2));

		case Spn::LU: return C.IsSolved(Pgr::LU_B1) &&
			(C.IsSolved(Pgr::LU_B2S1) || C.IsSolved(Pgr::LU_B2S2));
		case Spn::LD: return C.IsSolved(Pgr::LD_B1) &&
			(C.IsSolved(Pgr::LD_B2S1) || C.IsSolved(Pgr::LD_B2S2));
		case Spn::LF: return C.IsSolved(Pgr::LF_B1) &&
			(C.IsSolved(Pgr::LF_B2S1) || C.IsSolved(Pgr::LF_B2S2));
		case Spn::LB: return C.IsSolved(Pgr::LB_B1) &&
			(C.IsSolved(Pgr::LB_B2S1) || C.IsSolved(Pgr::LB_B2S2));

		default: return false; // Should not happend
		}
	}

	// Check if the first Roux blocks and the upper layer corners (CMLL) are built
	bool Roux::IsCMLLBuilt(const Cube& C)
	{
		if (!AreFirstBlocksBuilt(C)) return false;

		switch (C.GetSpin())
		{
		case Spn::UF:
		case Spn::UB:
		case Spn::UR:
		case Spn::UL: return C.IsSolved(Pgr::CORNERS_U);

		case Spn::DF:
		case Spn::DB:
		case Spn::DR:
		case Spn::DL: return C.IsSolved(Pgr::CORNERS_D);

		case Spn::FU:
		case Spn::FD:
		case Spn::FR:
		case Spn::FL: return C.IsSolved(Pgr::CORNERS_F);

		case Spn::BU:
		case Spn::BD:
		case Spn::BR:
		case Spn::BL: return C.IsSolved(Pgr::CORNERS_B);

		case Spn::RU:
		case Spn::RD:
		case Spn::RF:
		case Spn::RB: return C.IsSolved(Pgr::CORNERS_R);

		case Spn::LU:
		case Spn::LD:
		case Spn::LF:
		case Spn::LB: return C.IsSolved(Pgr::CORNERS_L);

		default: return false; // Should not happend
		}
	}
	
	// Returns best solve algorithm from the Solves vector class member and his score for the given spin
	// F2L pieces are used as evaluation condition for the Roux first blocks (first block in the left side)
	bool Roux::EvaluateFBResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
	{
		BestSolves.clear();

		if (Solves.empty()) return false; // No solves

		// Pieces to search
		Pgr F2L_1, F2L_2, F2L_3, F2L_4;

		// Get the required F2L pieces groups based on the spin
		switch (Sp)
		{
		case Spn::DF:
		case Spn::DB:
		case Spn::DR:
		case Spn::DL:
			F2L_1 = Pgr::F2L_U_UFR;
			F2L_2 = Pgr::F2L_U_UFL;
			F2L_3 = Pgr::F2L_U_UBR;
			F2L_4 = Pgr::F2L_U_UBL;
			break;
		case Spn::UF:
		case Spn::UB:
		case Spn::UR:
		case Spn::UL:
			F2L_1 = Pgr::F2L_D_DFR;
			F2L_2 = Pgr::F2L_D_DFL;
			F2L_3 = Pgr::F2L_D_DBR;
			F2L_4 = Pgr::F2L_D_DBL;
			break;
		case Spn::BU:
		case Spn::BD:
		case Spn::BR:
		case Spn::BL:
			F2L_1 = Pgr::F2L_F_UFR;
			F2L_2 = Pgr::F2L_F_UFL;
			F2L_3 = Pgr::F2L_F_DFR;
			F2L_4 = Pgr::F2L_F_DFL;
			break;
		case Spn::FU:
		case Spn::FD:
		case Spn::FR:
		case Spn::FL:
			F2L_1 = Pgr::F2L_B_UBR;
			F2L_2 = Pgr::F2L_B_UBL;
			F2L_3 = Pgr::F2L_B_DBR;
			F2L_4 = Pgr::F2L_B_DBL;
			break;
		case Spn::LU:
		case Spn::LD:
		case Spn::LF:
		case Spn::LB:
			F2L_1 = Pgr::F2L_R_UFR;
			F2L_2 = Pgr::F2L_R_UBR;
			F2L_3 = Pgr::F2L_R_DFR;
			F2L_4 = Pgr::F2L_R_DBR;
			break;
		case Spn::RU:
		case Spn::RD:
		case Spn::RF:
		case Spn::RB:
			F2L_1 = Pgr::F2L_L_UFL;
			F2L_2 = Pgr::F2L_L_UBL;
			F2L_3 = Pgr::F2L_L_DFL;
			F2L_4 = Pgr::F2L_L_DBL;
			break;
		default: return 0u; // Invalid spin (should not happend)
		}

		// Based on the search policy, different conditions have different weights in the final score
		uint SolveSizeMagnitude, SolveF2LMagnitude, SolveOppositeLayerMagnitude, SolveOrientedMagnitude, SolveNotAdjacentMagnitude;
		switch (Policy)
		{
		case Plc::SHORT:
			SolveSizeMagnitude = 1000u;
			SolveF2LMagnitude = 100u;
			SolveOppositeLayerMagnitude = 10u;
			SolveOrientedMagnitude = 50u;
			SolveNotAdjacentMagnitude = 10u;
			break;
		case Plc::BEST_SOLVES:
		default:
			SolveSizeMagnitude = 100u;
			SolveF2LMagnitude = 1000u;
			SolveOppositeLayerMagnitude = 10u;
			SolveOrientedMagnitude = 50u;
			SolveNotAdjacentMagnitude = 10u;
			break;
		}

		uint Score;

		std::vector<std::pair<uint, Algorithm>> ScoredSolves;

		for (const auto& s : Solves) // Check each solve to get the best one
		{
			Score = 0u;

			Cube CheckCube = CBase;
			CheckCube.ApplyAlgorithm(s);

			if (!IsFBBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

			// Evaluate solve size
			Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
			Score += s.GetSubjectiveScore();

			// Check F2L solves
			if (CheckCube.IsSolved(F2L_1)) Score += SolveF2LMagnitude;
			if (CheckCube.IsSolved(F2L_2)) Score += SolveF2LMagnitude;
			if (CheckCube.IsSolved(F2L_3)) Score += SolveF2LMagnitude;
			if (CheckCube.IsSolved(F2L_4)) Score += SolveF2LMagnitude;

			// Check both F2L pieces condition in the opposite layer
			const Lyr EndLayer = Cube::GetUpSliceLayer(Sp);

			if (CFOP::IsF2LOriented(F2L_1, CheckCube)) Score += SolveOrientedMagnitude;
			else
			{
				std::vector<Pcp> AuxF2LPos;
				Cube::AddToPositionsList(F2L_1, AuxF2LPos);
				std::vector<Pce> AuxF2L;
				AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
				AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

				if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
				{
					Score += SolveOppositeLayerMagnitude;
					if (CFOP::IsF2LAdjacent(F2L_1, CheckCube)) Score += SolveNotAdjacentMagnitude;
				}
			}

			if (CFOP::IsF2LOriented(F2L_2, CheckCube)) Score += SolveOrientedMagnitude;
			else
			{
				std::vector<Pcp> AuxF2LPos;
				Cube::AddToPositionsList(F2L_2, AuxF2LPos);
				std::vector<Pce> AuxF2L;
				AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
				AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

				if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
				{
					Score += SolveOppositeLayerMagnitude;
					if (CFOP::IsF2LAdjacent(F2L_2, CheckCube)) Score += SolveNotAdjacentMagnitude;
				}
			}

			if (CFOP::IsF2LOriented(F2L_3, CheckCube)) Score += SolveOrientedMagnitude;
			else
			{
				std::vector<Pcp> AuxF2LPos;
				Cube::AddToPositionsList(F2L_3, AuxF2LPos);
				std::vector<Pce> AuxF2L;
				AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
				AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

				if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
				{
					Score += SolveOppositeLayerMagnitude;
					if (CFOP::IsF2LAdjacent(F2L_3, CheckCube)) Score += SolveNotAdjacentMagnitude;
				}
			}

			if (CFOP::IsF2LOriented(F2L_4, CheckCube)) Score += SolveOrientedMagnitude;
			else
			{
				std::vector<Pcp> AuxF2LPos;
				Cube::AddToPositionsList(F2L_4, AuxF2LPos);
				std::vector<Pce> AuxF2L;
				AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
				AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

				if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
				{
					Score += SolveOppositeLayerMagnitude;
					if (CFOP::IsF2LAdjacent(F2L_4, CheckCube)) Score += SolveNotAdjacentMagnitude;
				}
			}

			if (Score > 0u)
			{
				std::pair<uint, Algorithm> ScoredSolve;
				ScoredSolve.first = Score;
				ScoredSolve.second = s;
				ScoredSolves.push_back(ScoredSolve);
			}
		}

		if (ScoredSolves.empty()) return false;

		std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

		const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

		for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
		return true;
	}

    // Get a solve report
    std::string Roux::GetReport(const bool cancellations, bool debug) const
    {
        std::string Report  = "Roux search for Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble(), ReportLine;
        Report += "\n--------------------------------------------------------------------------------\n";

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			if (!CheckSolveConsistency(spin)) continue;

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				ReportLine.clear();

				Cube C = CubeBase;
				C.ApplyAlgorithm(Inspections[sp][n]);
				C.ApplyAlgorithm(FirstBlocks[sp][n]);

				if (IsFBBuilt(C))
				{
					ReportLine += "[" + C.GetSpinText() + "|" + Algorithm::GetMetricValue(GetMetricSolve(C.GetSpin(), n));
					if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
					ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
					if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
					ReportLine += "(" + FirstBlocks[sp][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
						ReportLine += "[" + C.GetSpinText() + "]: ";
						if (Inspections[sp][n].GetSize() > 0u) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += "First block not built in " + std::to_string(MaxDepthFB) + " steps";
						if (FirstBlocks[sp][n].GetSize() > 0u) ReportLine += ": (" + FirstBlocks[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
					}
					if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(SecondBlocksFS[sp][n]);

				if (IsFBAndSBSquareBuilt(C)) ReportLine += " (" + SecondBlocksFS[sp][n].ToString() + ")";
				else
				{
					ReportLine += "First square for second block not built in " + std::to_string(MaxDepthSBFS) + " steps";
					if (SecondBlocksFS[sp][n].GetSize() > 0u) ReportLine += ": (" + SecondBlocksFS[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				if (!AreFirstBlocksBuilt(C))
				{
					C.ApplyAlgorithm(SecondBlocksSS[sp][n]);

					if (AreFirstBlocksBuilt(C)) ReportLine += " (" + SecondBlocksSS[sp][n].ToString() + ")";
					else
					{
						ReportLine += "Second square for second block not built in " + std::to_string(MaxDepthSBSS) + " steps";
						if (SecondBlocksSS[sp][n].GetSize() > 0u) ReportLine += ": (" + SecondBlocksSS[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}
				}

				if (!AlgCMLL[sp].empty())
				{
					C.ApplyAlgorithm(AlgCMLL[sp][n]);
                    if (debug) ReportLine += " {CMLL: " + CasesCMLL[sp][n] + "}";
					if (AlgCMLL[sp][n].GetSize() > 0u) ReportLine += " (" + AlgCMLL[sp][n].ToString() + ")";
				}

				if (!AlgCOLL[sp].empty())
				{
					C.ApplyAlgorithm(AlgCOLL[sp][n]);
                    if (debug) ReportLine += " {COLL: " + CasesCOLL[sp][n] + "}";
					if (AlgCOLL[sp][n].GetSize() > 0u) ReportLine += " (" + AlgCOLL[sp][n].ToString() + ")";
				}

                if (!AlgL6EO[sp].empty())
				{
                    C.ApplyAlgorithm(AlgL6EO[sp][n]);
                    if (AlgL6EO[sp][n].GetSize() > 0u) ReportLine += " (" + AlgL6EO[sp][n].ToString() + ")";
				}
				else if (debug) ReportLine += " [No L6E orient.]"; // Debug code

                if (!AlgL6E2E[sp].empty())
				{
                    C.ApplyAlgorithm(AlgL6E2E[sp][n]);
                    if (AlgL6E2E[sp][n].GetSize() > 0u) ReportLine += " (" + AlgL6E2E[sp][n].ToString() + ")";
				}
				else if (debug) ReportLine += " [No L6E URUL]"; // Debug code

				C.ApplyAlgorithm(AlgL6E[sp][n]);
				if (AlgL6E[sp][n].GetSize() > 0u) ReportLine += " (" + AlgL6E[sp][n].ToString() + ")";

				if (debug)
				{
					if (C.IsSolved()) ReportLine += " [OK]\n"; // Debug code
					else ReportLine += " [Not OK]\n"; // Debug code
				}
				else ReportLine.push_back('\n');

				if (debug || C.IsSolved()) Report += ReportLine;
			}
        }

        return Report;
    }

    // Get a solve time report
    std::string Roux::GetTimeReport() const
    {
        std::string Report;

        Report += "Total search time: " + std::to_string(GetFullTime()) + " s\n";
        Report += "First blocks search time: " + std::to_string(GetTimeFB()) + " s\n";
        Report += "Second blocks search time: " + std::to_string(GetTimeSB()) + " s\n";
        if (GetTimeCMLL() > 0.0) Report += "CMLL search time: " + std::to_string(GetTimeCMLL()) + " s\n";
        if (GetTimeCOLL() > 0.0) Report += "COLL search time: " + std::to_string(GetTimeCOLL()) + " s\n";
        Report += "L6E search time: " + std::to_string(GetTimeL6E() + GetTimeL6EO() + GetTimeL6E2E()) + " s\n";
        Report += "Threads used: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : 0) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string Roux::GetReport(const Spn sp, const uint n) const
	{
		const int si = static_cast<int>(sp);

		if (!CheckSolveConsistency(sp) || FirstBlocks[si].size() <= n)
		{
			std::string FReport = "No solve for spin ";
			FReport += Cube::GetSpinText(sp);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

        std::string Report  = "Roux search with orientation " + Cube::GetSpinText(sp);
        Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += "Inspection [" + C.GetSpinText() + "]: " + Inspections[si][n].ToString() + "\n\n";

        C.ApplyAlgorithm(FirstBlocks[si][n]);
        if (!IsFBBuilt(C))
        {
			Report += "First block not built in " + std::to_string(MaxDepthFB) + " movements\n";
            return Report;
        }
        Report += "First block (" + Algorithm::GetMetricValue(GetMetricFirstBlock(sp, n)) + "): " + FirstBlocks[si][n].ToString() + "\n";

        C.ApplyAlgorithm(SecondBlocksFS[si][n]);
        C.ApplyAlgorithm(SecondBlocksSS[si][n]);
        if (!AreFirstBlocksBuilt(C))
        {
            Report += "Second block not built!\n";
            return Report;
        }
        if (GetLengthSecondBlockSS(sp, n) == 0u)
			Report += "Second block (" + Algorithm::GetMetricValue(GetMetricSecondBlock(sp,n)) + "): " + SecondBlocksFS[si][n].ToString() + "\n";
        else
        {
            Report += "Second block - square (" + Algorithm::GetMetricValue(GetMetricSecondBlockFS(sp, n)) + "): " + SecondBlocksFS[si][n].ToString() + "\n";
            Report += "Second block (" + Algorithm::GetMetricValue(GetMetricSecondBlockSS(sp, n)) + "): " + SecondBlocksSS[si][n].ToString() + "\n";
        }

		if (!AlgCMLL[si].empty())
        {
            C.ApplyAlgorithm(AlgCMLL[si][n]);
            Report += "CMLL (" + Algorithm::GetMetricValue(GetMetricCMLL(sp, n)) + "): " + AlgCMLL[si][n].ToString() + "\n";
        }
        else if (!AlgCOLL[si].empty())
        {
            C.ApplyAlgorithm(AlgCOLL[si][n]);
            Report += "COLL (" + Algorithm::GetMetricValue(GetMetricCOLL(sp, n)) + "): " + AlgCOLL[si][n].ToString() + "\n";
        }
        else
        {
            Report += "CMLL/COLL not solved!";
            return Report;
        }

        if (!AlgL6EO[si].empty())
        {
            if (!AlgL6EO[si][n].Empty())
            {
                C.ApplyAlgorithm(AlgL6EO[si][n]);

                if (!C.IsFaceOriented2(Cube::GetUpSliceLayer(sp)))
                {
                    Report += "L6E not orientated: " + AlgL6EO[si][n].ToString() + "\n";
                    return Report;
                }
                else if (!AlgL6EO[si][n].Empty())
                    Report += "L6E orientation (" + Algorithm::GetMetricValue(GetMetricL6EO(sp, n)) + "): " + AlgL6EO[si][n].ToString() + "\n";
            }
        }
        if (!AlgL6E2E[si].empty())
		{
            if (!AlgL6E2E[si][n].Empty())
            {
                C.ApplyAlgorithm(AlgL6E2E[si][n]);
                if (!AlgL6E2E[si][n].Empty())
                    Report += "UR & UL edges (" + Algorithm::GetMetricValue(GetMetricL6E2E(sp, n)) + "): " + AlgL6E2E[si][n].ToString() + "\n";
            }
		}

		C.ApplyAlgorithm(AlgL6E[si][n]);
		if (C.IsSolved())
		{
			if (!AlgL6E[si][n].Empty()) Report += "L6E (" + Algorithm::GetMetricValue(GetMetricL6E(sp, n)) + "): " + AlgL6E[si][n].ToString() + "\n";
		}
		else Report += "L6E not solved: " + AlgL6E[si][n].ToString() + "\n";

        // Show summary
        Report += "\nSolve metric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
		if (AlgCMLL[si].empty()) // COLL
			Report += "COLL case: " + GetTextCOLLCase(sp, n) + "\n";
		else // CMLL 
			Report += "CMLL case: " + GetTextCMLLCase(sp, n) + "\n";

		return Report;
	}

	// Check if the solves for the given cross layer are consistent (all needed algorithms are present)
	bool Roux::CheckSolveConsistency(const Spn Spin) const
	{
		const int si = static_cast<int>(Spin); // Spin index

		if (Inspections[si].empty()) return false;

		auto n = Inspections[si].size();

		return (FirstBlocks[si].size() == n) &&
			   (SecondBlocksFS[si].size() == n) && (SecondBlocksSS[si].size() == n) && ((AlgCMLL[si].size() == n) || (AlgCOLL[si].size() == n)) &&
			   (AlgL6E[si].size() == n); // && (AlgOL6E[si].size() == n) && (AlgL6E_URUL[si].size() == n);
	}

    // Check if in the given spin the solve is OK
    bool Roux::IsSolved(const Spn CurrentSpin, const uint n) const
    {
        const int Si = static_cast<int>(CurrentSpin); // Spin index

		if (!CheckSolveConsistency(CurrentSpin) || FirstBlocks[Si].size() <= n) return 0u;

        Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[Si][n]);
        C.ApplyAlgorithm(FirstBlocks[Si][n]);
        C.ApplyAlgorithm(SecondBlocksFS[Si][n]);
        C.ApplyAlgorithm(SecondBlocksSS[Si][n]);
		if (!AlgCMLL[Si].empty()) C.ApplyAlgorithm(AlgCMLL[Si][n]);
		if (!AlgCOLL[Si].empty()) C.ApplyAlgorithm(AlgCOLL[Si][n]);
        if (!AlgL6EO[Si].empty()) C.ApplyAlgorithm(AlgL6EO[Si][n]);
        if (!AlgL6E2E[Si].empty()) C.ApplyAlgorithm(AlgL6E2E[Si][n]);
        C.ApplyAlgorithm(AlgL6E[Si][n]);
		
        return C.IsSolved();
    }

	// Get the solve metric
    float Roux::GetMetricSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || FirstBlocks[si].size() <= n) return 0u;

		Algorithm A = FirstBlocks[si][n];
		A += SecondBlocksFS[si][n];
		A += SecondBlocksSS[si][n];
		if (!AlgCMLL[si].empty()) A += AlgCMLL[si][n];
		if (!AlgCOLL[si].empty()) A += AlgCOLL[si][n];
        if (!AlgL6EO[si].empty()) A += AlgL6EO[si][n];
        if (!AlgL6E2E[si].empty()) A += AlgL6E2E[si][n];
		A += AlgL6E[si][n];
		
		return A.GetMetric(Metric);
	}

	// Get the full solve with cancellations
	Algorithm Roux::GetCancellations(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || FirstBlocks[si].size() <= n)
		{
			std::string FReport = "No solve with cancellations for spin ";
			FReport += Cube::GetSpinText(spin);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

		Algorithm A = Inspections[si][n];
		A += FirstBlocks[si][n];
		A += SecondBlocksFS[si][n];
		A += SecondBlocksSS[si][n];
		if (!AlgCMLL[si].empty()) A += AlgCMLL[si][n];
		if (!AlgCOLL[si].empty()) A += AlgCOLL[si][n];
        if (!AlgL6EO[si].empty()) A += AlgL6EO[si][n];
        if (!AlgL6E2E[si].empty()) A += AlgL6E2E[si][n];
		A += AlgL6E[si][n];

		return A.GetCancellations();
	}

	// Get the best solve report
	std::string Roux::GetBestReport(const bool Cancellations) const
	{
		float M, min_M = 0.0f;
		uint Bestn;
		Spn BestSpin;

		for (int sp = 0; sp < 24; sp++)
		{
			const Spn Spin = static_cast<Spn>(sp);

			if (!CheckSolveConsistency(Spin)) continue;

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				if (!IsSolved(Spin, n)) continue;

				if (Cancellations) M = GetMetricCancellations(Spin, n);
                else M = GetMetricSolve(Spin, n);
				
				if (min_M == 0.0f || M < min_M)
				{
					min_M = M;
					BestSpin = Spin;
					Bestn = n;
				}
			}
		}
		if (min_M == 0.0f) return "No Roux solves!\n";

		if (Cancellations)
		{
			Algorithm C = GetCancellations(BestSpin, Bestn);
			return GetReport(BestSpin, Bestn) + "\nCancellations (" + Algorithm::GetMetricValue(C.GetMetric(Metric)) + " " + 
			       Algorithm::GetMetricString(Metric) + "): " + C.ToString() + "\n";
		}

		return GetReport(BestSpin, Bestn);
	}
}
