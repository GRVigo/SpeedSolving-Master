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
	enum class SequenceTypes
	{
		SINGLE, // Single step from a steps list
		DOUBLE, // Two single steps from a steps list (to generate more branches for multithreading optimization)
		TRIPLE, // Thre single steps from a steps list (to generate even more branches for multithreading optimization)
		SEQUENCE, // Constant sequence of steps (algorithm)
		CONJUGATE_SINGLE, // Sequence with an auxiliary start step, a main step and the inverse of auxiliary start step as final step (conjugate) [length = 3 steps]
		CONJUGATE, // Sequence with an auxiliary start step, an algorithm and the inverse of auxiliary start step as final step (conjugate)
	};

	// Check or no check the solve in current level
	enum class SearchCheck { CHECK, NO_CHECK };

	// Struct to store information about unit search sequences
	struct SearchUnit
	{
		static uint UnitsAmount; // Increased each time a search unit is created, used for the Id
		uint Id; // Unique identifier for the search unit
		
		SequenceTypes Type; // Type of sequence
		
		std::vector<Stp> MainSteps; // Steps allowed for the sequence main part
		std::vector<Algorithm> MainAlgs; // Algorithms allowed for the sequence main part
		std::vector<Stp> AuxSteps; // Auxiliary steps allowed for the conjugate (first and last step)	

		// Constructor with default parameters
		SearchUnit(const SequenceTypes _Type, const Sst _MainSteps = Sst::SINGLE_ALL, const Sst _AuxSteps = Sst::NONE)
		{
			UnitsAmount++; // Increase the amount of units created
			Id = UnitsAmount; // Set the search unit unique identifier
			
			Type = _Type;
			if (Type == SequenceTypes::SINGLE || Type == SequenceTypes::DOUBLE || Type == SequenceTypes::TRIPLE || Type == SequenceTypes::CONJUGATE_SINGLE) 
				Algorithm::AddToStepsList(_MainSteps, MainSteps);
			
			if (Type == SequenceTypes::CONJUGATE_SINGLE || Type == SequenceTypes::CONJUGATE)
				Algorithm::AddToStepsList(_AuxSteps, AuxSteps);
		}
		
        void AddAlgorithm(const Algorithm &A) { MainAlgs.push_back(A); }
		void ClearAlgorithms() { MainAlgs.clear(); }
	};

	// Search level data (each search level can have several search units)
	struct SearchLevel
	{
		std::vector<SearchUnit> Units;
		SearchCheck Check; // Check or no check solve

		// Constructor with default parameters (check enabled)
		SearchLevel(const SearchCheck _Check = SearchCheck::CHECK) { Check = _Check; }
		
		void SetCheck() { Check = SearchCheck::CHECK; }
		void SetNoCheck() { Check = SearchCheck::NO_CHECK; }
		
        void Add(const SearchUnit& SU) { Units.push_back(SU); } // Add a unit to the level
		void Clear() { Units.clear(); } // Clear level
		uint NumUnits() const { return static_cast<uint>(Units.size()); } // Get number of search units in this level
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

    // Class to do a multithread tree search
	class DeepSearch
	{
	public:
		
		
        // Algorithm search class constructor - If search policie is SHORT, the deep will be reduced by the length of the finded solves
        DeepSearch(const Algorithm&, const Plc = Plc::BEST); // Scramble to start the search

		// Clear search parameters
        void Clear() 
		{ 
			Solves.clear();
			ResetPieces();
			MaxDepth = MinDepth = 0u;
			Scramble.Clear();
			CubeBase.Reset();
			SetBestPolicy();
			UsedCores = 0u;
			RootBranches = 0u;
			Roots.clear();
			Levels.clear();
		}

		std::vector<Algorithm>& GetSolves() { return Solves; }
		
        uint* GetMaxDepthPointer() { return &MaxDepth; }
        bool CheckSkipSearch() { return MaxDepth == 0u && !Levels.empty(); }
		   
		uint GetUsedCores() const { return UsedCores; } // Get system cores used in the last search
		uint GetBranches() const { return RootBranches; } // Get number of root branches in the search
        uint GetMaxDepth() const { return MaxDepth; } // Get the maximum depth in the search
        uint GetMinDepth() const { return MinDepth; } // Get the minimum depth in the search
		
		void SetBestPolicy() { SearchPolicy = Plc::BEST; }
		void SetShortPolicy() { SearchPolicy = Plc::SHORT; }
		
        void SetMinDepth(const uint d) { MinDepth = d <= MaxDepth ? d : MaxDepth; }
		
        void Run(const int = 0); // Run the search; -1: use no threads, 0: use all threads avaliable, other: use specified number of threads

		// Icrement the depth and return true if the search branch is complete
		bool IncCheckDepth(uint& Depth)
		{
			Depth++;
			return Depth >= MaxDepth || (MinDepth > 0u && !Solves.empty() && Depth >= MinDepth);
		}

        // Returns the shorter algorithm from the solves (optionally priorize some steps)
        uint EvaluateShortestResult(Algorithm&, const bool = false);
		
		// Get the real number of cores used
        int GetRealCores(const int RequestedCores) const { return RequestedCores < 0 ? -1 : RequestedCores <= GetSystemCores() ? RequestedCores : GetSystemCores(); }

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

		// Clear solved/oriented pieces to search (mandatory + optional)
		void ResetPieces() 
		{ 
			MandatorySolvedMasks.Clear();
			OptionalSolvedMasks.clear();
			MandatoryOrientedPos.clear();
		}

		// Add a search level
		void AddSearchLevel(const SearchLevel& sl) { Levels.push_back(sl); MaxDepth = static_cast<uint>(Levels.size()); }

		// Reset search levels
		void ResetSearchLevels() { Levels.clear(); MaxDepth = MinDepth = 0u; }

		bool UpdateRootData(); // Updates root algorithms and sequence data

		// Sets a new scramble for the search
        void SetScramble(const Algorithm& scr) { Scramble = scr; CubeBase.Reset(); CubeBase.ApplyAlgorithm(Scramble); }

		// Search the best solve algorithms with the given search depth
		void SearchBase(const uint, const Plc, const int = 0);

        // Get the number of cores (threads) in the current system
        static int GetSystemCores() { return static_cast<int>(std::thread::hardware_concurrency()); }

	private:	

		std::vector<Algorithm> Solves; // Algorithms that solve the search
		
		uint MaxDepth, // Maximum depth for the search (levels)
			 MinDepth; // Minimum depth for the search (if there is at least a solve in MinDepth search depth, stops current branch search)
			 
        Plc SearchPolicy; // Search policy (default best solves)
		
		uint UsedCores; // Number of cores to limit the threads
		uint RootBranches; // Number of search root branches to be distributed among cores

		Algorithm Scramble; // Cube scramble to start the search
		Cube CubeBase; // Cube with the scramble already applyed

        std::vector<Algorithm> Roots; // Root algorithms (no check) to distribute threads

        MasksPair MandatorySolvedMasks; // Masks for mandatory pieces to be solved
		std::vector<MasksPair> OptionalSolvedMasks; // Masks pairs for optional pieces solved
        std::vector<Pcp> MandatoryOrientedPos; // Mandatory pieces positions for orientation
        
		std::vector<SearchLevel> Levels; // Levels for search

		std::mutex SearchMutex; // Mutex for multithreading
		
		void RunThread(const uint); // Divide search branches into threads
        void RunSearch(const Algorithm&, uint, const uint = 0u, uint = 0u); // Run a search branch
        void CheckSolve(const Algorithm&); // Check if an algorithm solves the pieces
        void NextLevel(const Algorithm&, const uint); // Prepare the branches for the next level of the search
	};
}

