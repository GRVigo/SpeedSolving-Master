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
    // Class to search a solve for a Rubik's cube using LBL method
	class LBL
	{
	public:
		// Constructor with cube scramble
		LBL(const Algorithm& Scr, const int NumCores = 0) { Scramble = Scr; Reset(); Cores = NumCores; }

		// Destructor
		~LBL() {}

		// Reset the search results
		void Reset();
	
		// Search the best first layer cross solve algorithms
		// Return false if no cross found
		bool SearchFLCross(const Lyr);
        
        // Search the first layer (four corners)
        bool SearchFL();

		// Search the second layer
        bool SearchSL();
		
		// Search the last layer cross (only orientation)
		void SearchLLCross1();
		
		// Search the last layer cross (permutation)
		void SearchLLCross2();
		
		// Search the last layer corners (only permutation)
		void SearchLLCorners1();
		
		// Search the last layer corners (orientation)
		void SearchLLCorners2();

		// Check if the first layer cross is solved
        bool IsFLCrossSolved() const;
		
		// Check if first layer is solved
        bool IsFLSolved() const;

		// Check if second layer is solved
        bool IsSLSolved() const;

		// Get search algorithms texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
        std::string GetTextInspection() const { return Inspections[FLi].ToString(); }
 
        std::string GetTextFLCrossFirstSolve() const { return FLCross.size() > 0 ? FLCross[0].ToString() : ""; }
        std::string GetTextFLCrossSecondSolve() const { return FLCross.size() > 1 ? FLCross[1].ToString() : ""; }
        std::string GetTextFLCrossThirdSolve() const { return FLCross.size() > 2 ? FLCross[2].ToString() : ""; }
		std::string GetTextFLCrossFourthSolve() const { return FLCross.size() > 3 ? FLCross[3].ToString() : ""; }
		
        std::string GetTextFLFirstSolve() const { return FL.size() > 0 ? FL[0].ToString() : ""; }
        std::string GetTextFLSecondSolve() const { return FL.size() > 1 ? FL[1].ToString() : ""; }
        std::string GetTextFLThirdSolve() const { return FL.size() > 2 ? FL[2].ToString() : ""; }
		std::string GetTextFLFourthSolve() const { return FL.size() > 3 ? FL[3].ToString() : ""; }
		
		std::string GetTextSLFirstSolve() const { return SL.size() > 0 ? SL[0].ToString() : ""; }
        std::string GetTextSLSecondSolve() const { return SL.size() > 1 ? SL[1].ToString() : ""; }
        std::string GetTextSLThirdSolve() const { return SL.size() > 2 ? SL[2].ToString() : ""; }
		std::string GetTextSLFourthSolve() const { return SL.size() > 3 ? SL[3].ToString() : ""; }

        std::string GetTextLLCross1Solve(bool = false) const;
		std::string GetTextLLCross2Solve(bool = false) const;
		std::string GetTextLLCorners1Solve(bool = false) const;
		std::string GetTextLLCorners2Solve(bool = false) const;

        // Get search algorithms lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
        uint GetLengthInspection() const { return Inspections[FLi].GetNumSteps(); }
		
		uint GetLengthFLCrossFirstSolve() const { return FLCross.size() > 0 ? FLCross[0].GetNumSteps() : 0u; }
        uint GetLengthFLCrossSecondSolve() const { return FLCross.size() > 1 ? FLCross[1].GetNumSteps() : 0u; }
        uint GetLengthFLCrossThirdSolve() const { return FLCross.size() > 2 ? FLCross[2].GetNumSteps() : 0u; }
        uint GetLengthFLCrossFourthSolve() const { return FLCross.size() > 3 ? FLCross[3].GetNumSteps() : 0u; }
		uint GetLengthFLCrossSolve() const
		{
			return GetLengthFLCrossFirstSolve() + GetLengthFLCrossSecondSolve() + 
			       GetLengthFLCrossThirdSolve() + GetLengthFLCrossFourthSolve();
		}
		
        uint GetLengthFLFirstSolve() const { return FL.size() > 0 ? FL[0].GetNumSteps() : 0u; }
        uint GetLengthFLSecondSolve() const { return FL.size() > 1 ? FL[1].GetNumSteps() : 0u; }
        uint GetLengthFLThirdSolve() const { return FL.size() > 2 ? FL[2].GetNumSteps() : 0u; }
        uint GetLengthFLFourthSolve() const	{ return FL.size() > 3 ? FL[3].GetNumSteps() : 0u; }
		uint GetLengthFirstLayer() const
		{
			return GetLengthFLFirstSolve() + GetLengthFLSecondSolve() + 
			       GetLengthFLThirdSolve() + GetLengthFLFourthSolve();
		}
		
		uint GetLengthSLFirstSolve() const { return SL.size() > 0 ? SL[0].GetNumSteps() : 0u; }
        uint GetLengthSLSecondSolve() const { return SL.size() > 1 ? SL[1].GetNumSteps() : 0u; }
        uint GetLengthSLThirdSolve() const { return SL.size() > 2 ? SL[2].GetNumSteps() : 0u; }
        uint GetLengthSLFourthSolve() const { return SL.size() > 3 ? SL[3].GetNumSteps() : 0u; }
		uint GetLengthSecondLayer() const
		{
			return GetLengthSLFirstSolve() + GetLengthSLSecondSolve() + 
			       GetLengthSLThirdSolve() + GetLengthSLFourthSolve();
		}
		
        uint GetLengthLLCross1Solve() const { return LLCross1.GetNumSteps(); }
		uint GetLengthLLCross2Solve() const { return LLCross2.GetNumSteps(); }
		uint GetLengthLLCorners1Solve() const { return LLCorners1.GetNumSteps(); }
		uint GetLengthLLCorners2Solve() const { return LLCorners2.GetNumSteps(); }
		uint GetLengthLastLayer() const
		{
			return GetLengthLLCross1Solve() + GetLengthLLCross2Solve() +
				   GetLengthLLCorners1Solve() + GetLengthLLCorners2Solve();
		}

        // Get the full solve lengh (total number of movements) - inspection is not included
        uint GetLengthSolve() const
        {
            return GetLengthFLCrossSolve() + GetLengthFirstLayer() + 
                   GetLengthSecondLayer() + GetLengthLastLayer();
        }

        // Get the times elapsed searching
        double GetFLCrossTime() const { return TimeFLCross; }
        double GetFLTime() const { return TimeFL; }
        double GetSLTime() const { return TimeSL; }
        double GetLLCross1Time() const { return TimeLLCross1; }
		double GetLLCross2Time() const { return TimeLLCross2; }
		double GetLLCorners1Time() const { return TimeLLCorners1; }
		double GetLLCorners2Time() const { return TimeLLCorners2; }
        double GetLastLayerTime() const { return GetLLCross1Time() + GetLLCross2Time() +
                                                 GetLLCorners1Time() + GetLLCorners2Time(); }
												 
        // Get the time for the full search
        double GetFullTime() const { return GetFLCrossTime() + GetFLTime() +
                                            GetSLTime() + GetLastLayerTime(); }

        // Get a solve report
        std::string GetReport() const;

        // Get a solve time report
        std::string GetTimeReport() const;

		// Get used cores in the solve
		int GetUsedCores() const { return Cores; }

		// Returns the shorter LBL last layer corners orientation algorithm from the solves
		static uint EvaluateLLCOResult(Algorithm& , const std::vector<Algorithm>&);

	private:
		
		Algorithm Scramble, // Cube scramble
				  LLCross1, LLCross2, // Orientation and permutation for last layer crosses
				  LLCorners1, LLCorners2; // Permutation and orientation for last layer corners
		std::vector<Algorithm> FLCross, // Algorithms vector for first layer cross
							   FL, // Algorithms vector for first layer (four corners)
							   SL; // Algorithms vector for second layer (four edges)

		Lyr FirstLayer; // First layer
		Fce FLFace; // First layer face
		int FLi; // First layer/face index
        Cube CubeBase; // Cube base

        // Times
        double TimeFLCross, TimeFL, TimeSL, TimeLLCross1, TimeLLCross2, TimeLLCorners1, TimeLLCorners2;

		int Cores; // Cores to use in the search: -1 = no multithreading, 0 = all avaliable cores, other = use this amount of cores

		const static Algorithm Inspections[6], 
			                   Cross1Algorithm,
							   Cross2Algorithm,
							   Corners1Algorithm,
							   Corners2Algorithm, Corners2Algorithm2;
	};
}
