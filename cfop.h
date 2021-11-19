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

#include "cube_definitions.h"
#include "deep_search.h"

namespace grcube3
{
    // Class to search a solve for a Rubik's cube using CFOP method
	class CFOP
	{
	public:
		// Constructor with cube scramble
		CFOP(const Algorithm& Scr, const int NumCores = 0) 
		{ 
			Scramble = Scr;
			CubeBase = Cube(Scramble);
			Cores = NumCores;
			Reset();
		}

		// Destructor
		~CFOP() {}

		// Reset the search results
		void Reset();

		// Set cross layers for search
		void SetCrossLayers(const std::vector<Lyr>& CL)
		{
			CrossLayers.clear();
			if (CL.empty()) for (const auto ELy : Cube::ExtLayers) CrossLayers.push_back(ELy);
			else for (const auto Ly : CL) CrossLayers.push_back(Ly);
		}

		// Search the best crosses solve algorithms with the given search depth and the maximun number of solves
		// Return false if no crosses found
		bool SearchCrosses(const uint, const uint = 1u);

		// Search the best crosses solve algorithms from an algorithms vector
		void EvaluateCrosses(const std::vector<Algorithm>&, const uint = 1u);

		// Search the best F2L algorithms for the Scramble
        void SearchF2L();

		// Orientation of last layer search
		void SearchOLL();

		// Permutation of last layer search
		void SearchPLL();

        // One-Look last layer search (1LLL)
        void Search1LLL();

		// Orientation of last layer edges search (EO)
		void SearchEO();

		// ZBLL last layer search
		void SearchZBLL();

		// Set regrips
		void SetRegrips();

		// If the cross is search externally, use this function to set the cross search time
		void SetCrossTime(double t) { TimeCross = t; }

        // If the cross is search externally, use this function to set the cross search depth
        void SetCrossDepth(uint d) { CrossDepth = d; }

		// Get current OLL case in given cross face
		std::string GetOLLCase(const Fce CrossFace, const uint n) const { return CaseOLL[static_cast<int>(CrossFace)][n]; }

		// Get current PLL case in given cross face
		std::string GetPLLCase(const Fce CrossFace, const uint n) const { return CasePLL[static_cast<int>(CrossFace)][n]; }

		// Get current 1LLL case in given cross face
		std::string Get1LLLCase(const Fce CrossFace, const uint n) const { return Case1LLL[static_cast<int>(CrossFace)][n]; }

		// Get current ZBLL case in given cross face
		std::string GetZBLLCase(const Fce CrossFace, const uint n) const { return CaseZBLL[static_cast<int>(CrossFace)][n]; }

		// Get search algorithms texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
        std::string GetTextInspection(const Fce CrossFace, const uint n) const { return Inspections[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextCrossSolve(const Fce CrossFace, const uint n) const { return Crosses[static_cast<int>(CrossFace)][n].ToString(); }	
		std::string GetTextF2LFirstSolve(const Fce, const uint) const;
		std::string GetTextF2LSecondSolve(const Fce, const uint) const;
		std::string GetTextF2LThirdSolve(const Fce, const uint) const;
		std::string GetTextF2LFourthSolve(const Fce, const uint) const;
		std::string GetTextF2L(const Fce, const uint) const;
        std::string GetTextOLLSolve(const Fce CrossFace, const uint n) const { return A_OLL[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextAUFSolve(const Fce CrossFace, const uint n) const { return AUF[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextPLLSolve(const Fce CrossFace, const uint n) const { return A_PLL[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetText1LLLSolve(const Fce CrossFace, const uint n) const { return A_1LLL[static_cast<int>(CrossFace)][n].ToString(); }
		std::string GetTextEOSolve(const Fce CrossFace, const uint n) const { return A_EO[static_cast<int>(CrossFace)][n].ToString(); }
		std::string GetTextZBLLSolve(const Fce CrossFace, const uint n) const { return A_ZBLL[static_cast<int>(CrossFace)][n].ToString(); }

        // Get search algorithms lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
        uint GetLengthInspection(const Fce CrossFace, const uint n) const { return Inspections[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthCrossSolve(const Fce CrossFace, const uint n) const { return Crosses[static_cast<int>(CrossFace)][n].GetNumSteps(); }
		uint GetLengthF2LFirstSolve(const Fce, const uint) const;
		uint GetLengthF2LSecondSolve(const Fce, const uint) const;
		uint GetLengthF2LThirdSolve(const Fce, const uint) const;
		uint GetLengthF2LFourthSolve(const Fce, const uint) const;
        uint GetLengthOLLSolve(const Fce CrossFace, const uint n) const { return A_OLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthAUFSolve(const Fce CrossFace, const uint n) const { return AUF[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthPLLSolve(const Fce CrossFace, const uint n) const { return A_PLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLength1LLLSolve(const Fce CrossFace, const uint n) const { return A_1LLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }
		uint GetLengthEOSolve(const Fce CrossFace, const uint n) const { return A_EO[static_cast<int>(CrossFace)][n].GetNumSteps(); }
		uint GetLengthZBLLSolve(const Fce CrossFace, const uint n) const { return A_ZBLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }

		// Get the solve STM metric
		uint GetSolveSTM(const Fce, const uint) const;
		
		// Get text for cases
        std::string GetTextOLLCase(const Fce CrossFace, const uint n) const { return CaseOLL[static_cast<int>(CrossFace)][n]; }
		std::string GetTextPLLCase(const Fce CrossFace, const uint n) const { return CasePLL[static_cast<int>(CrossFace)][n]; }
        std::string GetText1LLLCase(const Fce CrossFace, const uint n) const { return Case1LLL[static_cast<int>(CrossFace)][n]; }
		std::string GetTextZBLLCase(const Fce CrossFace, const uint n) const { return CaseZBLL[static_cast<int>(CrossFace)][n]; }

        // Get the times elapsed searching
        double GetCrossTime() const { return TimeCross; }
		double GetF2LTime() const { return TimeF2L; }
		double GetOLLTime() const { return TimeOLL; }
		double GetPLLTime() const { return TimePLL; }
		double Get1LLLTime() const { return Time1LLL; }
		double GetEOTime() const { return TimeEO; }
		double GetZBLLTime() const { return TimeZBLL; }
		double GetLLTime() const { return GetOLLTime() + GetPLLTime() + Get1LLLTime() + GetEOTime() + GetZBLLTime(); }
		double GetFullTime() const { return GetCrossTime() + GetF2LTime() + GetLLTime();	}

        // Get a solve report (for each cross layer)
        std::string GetReport(const bool, const bool = false) const; // cancellations, debug

        // Get a solve report
        std::string GetReport(const Lyr, const uint) const;

        // Get a solve time report
        std::string GetTimeReport() const;

        // Get the best solve report (STM with or without cancellations)
        std::string GetBestReport(bool = false) const;

		// Get the full solve with cancellations
		Algorithm GetCancellations(const Lyr, const uint) const;

		// Get the solve with cancellations STM metric
		uint GetCancellationsSTM(const Fce CF, const uint n) const { return GetCancellations(Cube::FaceToLayer(CF), n).GetSTM(); }

		// Get used cores in the solve
		int GetUsedCores() const { return Cores; }

		// Returns best F2L solve from the Solves vector class member
		// F2L pieces are used as evaluation condition
		static bool EvaluateF2LResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Lyr, const Plc, const bool = true);

		// Check if the CFOP first cross is built
		static bool IsCrossBuilt(const Cube&, const Lyr);

		// Check if first two layers are built
		static bool IsF2LBuilt(const Cube&, const Lyr);

		// Check if the last layer is oriented (OLL search completed)
		static bool IsLastLayerOriented(const Cube& C) { return C.IsFaceOriented(Cube::GetUpSliceLayer(C.GetSpin())); }

		// Check if the given F2L pair is adjacent (the corner next to the edge), orientation is not checked
		static bool IsF2LAdjacent(const Pgr, const Cube&);

		// Check if the common colors for F2L pair are in the same face
		static bool IsF2LOriented(const Pgr, const Cube&);

	private:
		
		Algorithm Scramble; // Cube scramble

		std::vector<Lyr> CrossLayers; // Cross layers allowed for search
			      
		std::vector<Algorithm> Inspections[6], // Algorithms for inspections
							   Crosses[6], // Algorithms for crosses
							   F2L_1[6], // Algorithms for F2L 1
							   F2L_2[6], // Algorithms for F2L 2
							   F2L_3[6], // Algorithms for F2L 3
							   F2L_4[6], // Algorithms for F2L 4
							   A_OLL[6], // Algorithms for OLL
							   A_PLL[6], // Algorithms for PLL
							   A_1LLL[6], // Algorithms for 1LLL
							   A_EO[6], // Algorithms for LL edges orientation
							   A_ZBLL[6], // Algorithms for ZBLL
							   AUF[6]; // Algorithms for AUF
		
		uint CrossDepth; // Cross search depth

		std::vector<std::string> CaseOLL[6], // OLL cases
								 CasePLL[6], // PLL cases
								 CaseZBLL[6], // ZBLL cases
								 Case1LLL[6]; // One-Look Last Layer cases

        Cube CubeBase; // Cube base

		double TimeCross, TimeF2L, TimeOLL, TimePLL, Time1LLL, TimeEO, TimeZBLL; // Times

		int Cores; // Cores to use in the search: -1 = no multithreading, 0 = all avaliable cores, other = use this amount of cores

		const static Algorithm DefaultInspections[6]; 
		
		// Check if the solves for the given cross layer are consistent (all needed algorithms are present)
		bool CheckSolveConsistency(const Lyr) const;

		// Check if in the given cross face the solve is OK
		bool IsSolved(const Fce, const uint) const;
	};
}
