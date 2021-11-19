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
	<mailto:grvigo@hotmail.com>
*/

#pragma once

#include <vector>
#include <mutex>
#include <thread>

#include "cube.h"

namespace grcube3
{
	// Types of sequences for the search
	enum class SequenceType
	{
		SINGLE, // Single step from a steps list
		DOUBLE, // Two single steps from a steps list (to generate more branches for multithreading optimization)
		TRIPLE, // Thre single steps from a steps list (to generate even more branches for multithreading optimization)
		RETURN_FIXED_SINGLE, // Sequence with a start step, a main step and the inverse start step as final step (length = 3 steps)
		CONST, // Constant sequence of steps (algorithm)
		RETURN_CONST, // Sequence with a start step, an algorithm and the inverse start step as final step 
	};

	// Check or no check the solve in current level
	enum class SearchCheck { CHECK, NO_CHECK };

	// Struct to store information about unit search sequences
	struct SearchUnit
	{
		static uint UnitsAmount; // Increased each time a search unit is created, used for the Id
		uint Id; // Unique identifier for the search unit
		
		SequenceType Type; // Type of sequence
		
		std::vector<Stp> MainSteps; // Step for the sequence main part
		std::vector<Algorithm> MainAlgs; // Algorithms for the sequence main part
		std::vector<Stp> AuxSteps; // Auxiliary steps for the sequence (first and/or last step)	

		// Constructor with default parameters
		SearchUnit(const SequenceType _Type,
                   const Sst _MainSteps = Sst::SINGLE_ALL,
                   const Sst _AuxSteps = Sst::NONE)
                   {
						UnitsAmount++; // Increase the amount of units created
						Id = UnitsAmount; // Set the search unit unique identifier
						Type = _Type;
						Algorithm::AddToStepsList(_MainSteps, MainSteps);
						ClearAlgorithms();
                        Algorithm::AddToStepsList(_AuxSteps, AuxSteps);
                   }
        void AddAlgorithm(const Algorithm &A) { MainAlgs.push_back(A); }
		void ClearAlgorithms() { MainAlgs.clear(); }
	};

	// Search level data (each search level can have several search units)
	struct SearchLevel
	{
		std::vector<SearchUnit> Unit;
		SearchCheck Check; // Check or no check solve

		SearchLevel(const SearchCheck _Check = SearchCheck::CHECK) { Check = _Check; } // Constructor with default parameters
        void Add(const SearchUnit& SU) { Unit.push_back(SU); } // Add a unit to the level
		void Clear() { Unit.clear(); } // Clear level
		uint NumUnits() const { return static_cast<uint>(Unit.size()); } // Get number of units in this level
	};

    // Struct to store a pair of masks (edges and corners status masks)
	struct MasksPair
	{
        S64 MaskE, MaskC; // Masks for check the solve condition in edges and corners

        // Constructor with default parameters
		MasksPair(const S64 EMask = 0ull, const S64 CMask = 0ull) { MaskE = EMask; MaskC = CMask; }

		// Clear masks
		void Clear() { MaskE = MaskC = 0ull; }
	};

    // Class to do a search
	class DeepSearch
	{
	public:
		// Algorithms that solve the search
		std::vector<Algorithm> Solves;
		
        // Algorithm search class constructor - If search policie is SHORT, the deep will be reduced by the length of the finded solves
        DeepSearch(const Algorithm&, const Plc = Plc::BEST_SOLVES); // Scramble to start the search
		   
		uint GetCores() const { return Cores; } // Get system cores
		uint GetCoresUsed() const { return UsedCores; } // Get system cores used in the last search
		uint GetBranches() const { return RootBranches; } // Get number of root branches in the search
		uint GetMaxDeep() const { return MaxSearchDeep; } // Get the maximum depth in the search
		uint GetMinDeep() const { return MinSearchDeep; } // Get the minimum depth in the search
		
        void SetMinDeep(const uint MinD) { MinSearchDeep = MinD <= MaxSearchDeep ? MinD : MaxSearchDeep; }
		
        void Run(const int = 0); // Run the search; -1: use no threads, 0: use all threads avaliable, other: use specified number of threads

		// Icrements the deep and return true if the search branch is complete
		bool IncCheckDeep(uint& Deep)
		{
			Deep++;
			return Deep >= MaxSearchDeep || (MinSearchDeep > 0u && !Solves.empty() && Deep >= MinSearchDeep);
		}

        // Returns the shorter algorithm from the solves (optionally priorize some steps)
        uint EvaluateShortestResult(Algorithm&, const bool = false);
		
		// Get the real number of cores used (a search should be done )
        int GetRealCores(const int RequestedCores) const { return RequestedCores < 0 ? -1 : RequestedCores <= Cores ? RequestedCores : Cores; }

		// Add a piece as mandatory solved piece position
		void AddToMandatoryPieces(const Pcp);

		// Add a pieces group as mandatory solved pieces
		void AddToMandatoryPieces(const Pgr);
		
		// Add a piece as optional solved piece
		void AddToOptionalPieces(const Pcp);
		
		// Add a pieces group as optional solved pieces
		void AddToOptionalPieces(const Pgr);

        // Add a piece as mandatory piece in his position (orientation not needed)
        void AddToMandatoryPositions(const Pcp);

        // Add a pieces group as mandatory pieces in his position (orientation not needed)
        void AddToMandatoryPositions(const Pgr);
		
		// Add a piece as mandatory solved piece positions for orientation
		void AddToMandatoryOrientations(const Pcp);
		
		// Add a pieces group as mandatory orientated pieces positions
		void AddToMandatoryOrientations(const Pgr);

		// Reset solved pieces to search (mandatory + optional)
		void ResetSolvedPieces() 
		{ 
			MandatorySolveMask.Clear();
			MandatoryOrientedPos.clear();
			OptionalSolveMasks.clear();
		}

		// Add a search level
		void AddSearchLevel(const SearchLevel& SL) { Levels.push_back(SL); MaxSearchDeep = static_cast<uint>(Levels.size()); }

		// Reset search levels
		void ResetSearchLevels() { Levels.clear(); MaxSearchDeep = MinSearchDeep = 0u; }

		bool UpdateRootData(); // Updates root algorithms and sequence data

		// Sets a new scramble for the search
        void SetNewScramble(const Algorithm& scr) {	Scramble = scr; CubeBase.Reset(); CubeBase.ApplyAlgorithm(Scramble); }

		// Search the best solve algorithms with the given search depth
		void SearchBase(const uint, const int = 0);

        // Get the number of cores (threads) in the current system
        static uint GetSystemCores() { return std::thread::hardware_concurrency(); }

	private:	

		uint MaxSearchDeep, // Maximum deep for the search (levels)
			 MinSearchDeep; // Minimum deep for the search (if there is at least a solve in MinSearchDeep search deep, stops the search)
        Plc SearchPolicy; // Search policy (default best solves)
		
		int Cores; // Number of phisical cores in the machine
		uint UsedCores; // Number of cores to limit the threads
		uint RootBranches; // Number of search root branches to be distributed among cores

		Algorithm Scramble; // Cube scramble to start the search
		Cube CubeBase; // Cube with the scramble already applyed

        std::vector<Algorithm> Roots; // Root algorithms (no check) to distribute threads

        MasksPair MandatorySolveMask; // Masks for mandatory pieces to be solved
        std::vector<Pcp> MandatoryOrientedPos; // Mandatory pieces positions for orientation
        std::vector<MasksPair> OptionalSolveMasks; // Masks pairs for optional pieces solved

		std::vector<SearchLevel> Levels; // Levels for search

		std::mutex SearchMutex; // Mutex for multithreading
		
		void RunThread(const uint); // Divide search branches into threads

        void RunSearch(const Algorithm&, uint, const uint = 0u, uint = 0u); // Run a search branch
		
        void CheckSolve(const Algorithm&); // Check if an algorithm solves the pieces
		
        void NextLevel(const Algorithm&, const uint); // Prepare the branches for the next level of the search
	};
}

