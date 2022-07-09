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

#pragma once

#include <vector>
#include <array>
#include <mutex>
#include <thread>

#include "cube.h"

namespace grcube3
{
    // Struct to store group check conditions
	struct GroupCheck
	{
        S64 EMask, CMask; // Masks for check the solve condition in edges and corners
		std::vector<Pcp> Orient; // List of positions to be oriented
		Spn Spin; // Spin to evaluate the orientation

		GroupCheck() { Clear(); }

		// Clear masks
		void Clear() { EMask = CMask = 0ull; Orient.clear(); Spin = Spn::Default; }
	};

    // Class to do an evaluation
	class DeepEval
	{
	public:
		
        // Algorithm evaluation class constructor
        DeepEval(const Algorithm&, const uint); // Scramble to start the evaluation, maximum depth

		uint GetCores() const { return Cores; } // Get system cores
		uint GetCoresUsed() const { return UsedCores; } // Get system cores used in the last evaluation
		uint GetBranches() const { return RootBranches; } // Get number of root branches in the evaluation
		uint GetMaxDepth() const { return MaxDepth; } // Get the maximum depth in the evaluation
		
        void Run(const int = 0); // Run the evaluation; -1: use no threads, 0: use all threads avaliable, other: use specified number of threads
		
		// Get the real number of cores used (a evaluation should be done)
        int GetRealCores(const int RequestedCores) const { return RequestedCores < 0 ? -1 : RequestedCores <= Cores ? RequestedCores : Cores; }

		void ResetResults() { for (auto& RL : Results) for (auto& RS : RL) RS = 0u; }

		// Sets a new scramble for the evaluation
		void SetNewScramble(const Algorithm& scr) {	Scramble = scr; CubeBase.Reset(); CubeBase.ApplyAlgorithm(Scramble); ResetResults(); }

        // Get the number of cores (threads) in the current system
        static uint GetSystemCores() { return std::thread::hardware_concurrency(); }

        void StandardGroups(const std::vector<Spn>&);

        float GetScore(const uint = 5u) const; // Depth 5 for default score

		double GetTime() const { return Time; } // Evaluation time

		static const std::array<Stp, 18> MainSteps;

	private:

		uint MaxDepth; // Maximum depth for the evaluation (= maximum size of the algorithms)
		
		int Cores; // Number of phisical cores in the machine
		uint UsedCores; // Number of cores to limit the threads
		uint RootBranches; // Number of search root branches to be distributed among cores
		double Time; // Evaluation time

		Algorithm Scramble; // Cube scramble to start the evaluation
		Cube CubeBase; // Cube with the scramble already applyed

		// Evaluation results
		std::array<std::vector<uint>, 10> Results;

		std::vector<std::string> GroupIds; // Ids for groups

		std::vector<bool> SolvedFlagsBase; // Base flags to know if this branch has been solved upper in the tree

        std::vector<Algorithm> Roots; // Root algorithms to distribute threads

		std::vector<std::vector<bool>> RootFlags; // Solve flags for root algorithms

		std::vector<GroupCheck> Groups; // Groups to solve / orient

        uint nSpins; // Number of spins to evaluate

		std::mutex SearchMutex; // Mutex for multithreading

		void UpdateRootData(); // Updates root algorithms

        // Reset groups of pieces positions to evaluate (solved and oriented)
        void ResetGroups()
        {
            Groups.clear();
            GroupIds.clear();
            SolvedFlagsBase.clear();
            for (auto& R : Results) R.clear();
            nSpins = 0u;
        }

        // Add a group of positions to be solved and/or oriented
        void AddToGroups(const std::vector<Pcp>&, const std::vector<Pcp>&, const Spn, const std::string = "");
		
		void RunThread(const uint); // Divide root evaluation branches into threads

        void RunEvaluation(const Algorithm&, const std::vector<bool>&); // Run a evaluation branch
		
        void CheckAlg(const Algorithm&, std::vector<bool>&); // Check if an algorithm solve or orient the pieces
		
        void NextLevel(const Algorithm&, const std::vector<bool>&); // Prepare the branches for the next level of the evaluation
	};
}

