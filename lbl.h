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
		// Constructor
		LBL(const Algorithm& Scr, const int NumCores = 0) { Scramble = Scr; Reset(); Cores = NumCores; }

		// Reset the search results
		void Reset();

		bool SearchFLCross(const Lyr); // Search the best first layer cross solves
        bool SearchFLCorners(); // Search the first layer (four corners)
        bool SearchSLEdges(); // Search the second layer
		void SearchLLCross1(); // Search the last layer cross (only orientation)
		void SearchLLCross2(); // Search the last layer cross (permutation)
		void SearchLLCorners1(); // Search the last layer corners (only permutation)
		void SearchLLCorners2(); // Search the last layer corners (orientation)
		
		// Set the metric for evaluations
		void SetMetric(const Metrics m) { Metric = m; }
		
		// Get the current metric for evaluations
		Metrics GetMetric() const { return Metric; }
		
        bool IsFLCrossSolved() const; // Check if the first layer cross is solved	
        bool IsFLSolved() const; // Check if first layer is solved
        bool IsSLSolved() const; // Check if second layer is solved
		bool IsSolved() const; // Check full solve

		// Get search algorithms texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
        std::string GetTextInspection() const { return Inspections[FLi].ToString(); }
 
        std::string GetTextFLEdge_1() const { return FLCross.size() > 0 ? FLCross[0].ToString() : ""; }
        std::string GetTextFLEdge_2() const { return FLCross.size() > 1 ? FLCross[1].ToString() : ""; }
        std::string GetTextFLEdge_3() const { return FLCross.size() > 2 ? FLCross[2].ToString() : ""; }
		std::string GetTextFLEdge_4() const { return FLCross.size() > 3 ? FLCross[3].ToString() : ""; }
		
        std::string GetTextFLCorner_1() const { return FLCorners.size() > 0 ? FLCorners[0].ToString() : ""; }
        std::string GetTextFLCorner_2() const { return FLCorners.size() > 1 ? FLCorners[1].ToString() : ""; }
        std::string GetTextFLCorner_3() const { return FLCorners.size() > 2 ? FLCorners[2].ToString() : ""; }
		std::string GetTextFLCorner_4() const { return FLCorners.size() > 3 ? FLCorners[3].ToString() : ""; }
		
		std::string GetTextSLEdge_1() const { return SLEdges.size() > 0 ? SLEdges[0].ToString() : ""; }
        std::string GetTextSLEdge_2() const { return SLEdges.size() > 1 ? SLEdges[1].ToString() : ""; }
        std::string GetTextSLEdge_3() const { return SLEdges.size() > 2 ? SLEdges[2].ToString() : ""; }
		std::string GetTextSLEdge_4() const { return SLEdges.size() > 3 ? SLEdges[3].ToString() : ""; }

        std::string GetTextLLCross_1(bool = false) const;
		std::string GetTextLLCross_2(bool = false) const;
		std::string GetTextLLCorners_1(bool = false) const;
		std::string GetTextLLCorners_2(bool = false) const;

        // Get search algorithms lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
        uint GetLengthInspection() const { return Inspections[FLi].GetNumSteps(); }
		
		uint GetLengthFLEdge_1() const { return FLCross.size() > 0 ? FLCross[0].GetNumSteps() : 0u; }
        uint GetLengthFLEdge_2() const { return FLCross.size() > 1 ? FLCross[1].GetNumSteps() : 0u; }
        uint GetLengthFLEdge_3() const { return FLCross.size() > 2 ? FLCross[2].GetNumSteps() : 0u; }
        uint GetLengthFLEdge_4() const { return FLCross.size() > 3 ? FLCross[3].GetNumSteps() : 0u; }
		uint GetLengthFLCross() const
		{
			return GetLengthFLEdge_1() + GetLengthFLEdge_2() + 
			       GetLengthFLEdge_3() + GetLengthFLEdge_4();
		}
		
        uint GetLengthFLCorner_1() const { return FLCorners.size() > 0 ? FLCorners[0].GetNumSteps() : 0u; }
        uint GetLengthFLCorner_2() const { return FLCorners.size() > 1 ? FLCorners[1].GetNumSteps() : 0u; }
        uint GetLengthFLCorner_3() const { return FLCorners.size() > 2 ? FLCorners[2].GetNumSteps() : 0u; }
        uint GetLengthFLCorner_4() const { return FLCorners.size() > 3 ? FLCorners[3].GetNumSteps() : 0u; }
		uint GetLengthFLCorners() const
		{
			return GetLengthFLCorner_1() + GetLengthFLCorner_2() + 
			       GetLengthFLCorner_3() + GetLengthFLCorner_4();
		}
		
		uint GetLengthSLEdge_1() const { return SLEdges.size() > 0 ? SLEdges[0].GetNumSteps() : 0u; }
        uint GetLengthSLEdge_2() const { return SLEdges.size() > 1 ? SLEdges[1].GetNumSteps() : 0u; }
        uint GetLengthSLEdge_3() const { return SLEdges.size() > 2 ? SLEdges[2].GetNumSteps() : 0u; }
        uint GetLengthSLEdge_4() const { return SLEdges.size() > 3 ? SLEdges[3].GetNumSteps() : 0u; }
		uint GetLengthSL() const
		{
			return GetLengthSLEdge_1() + GetLengthSLEdge_2() + 
			       GetLengthSLEdge_3() + GetLengthSLEdge_4();
		}
		
        uint GetLengthLLCross1() const { return LLCross1.GetNumSteps(); }
		uint GetLengthLLCross2() const { return LLCross2.GetNumSteps(); }
		uint GetLengthLLCorners1() const { return LLCorners1.GetNumSteps(); }
		uint GetLengthLLCorners2() const { return LLCorners2.GetNumSteps(); }
		uint GetLengthLL() const
		{
			return GetLengthLLCross1() + GetLengthLLCross2() +
				   GetLengthLLCorners1() + GetLengthLLCorners2();
		}

        // Get metric values
        float GetMetricSolve() const; // Get the full solve metric
		float GetMetricScramble() const { return Scramble.GetMetric(Metric); }
        float GetMetricInspection() const { return Inspections[FLi].GetMetric(Metric); }
		
		float GetMetricFLEdge_1() const { return FLCross.size() > 0 ? FLCross[0].GetMetric(Metric) : 0.0f; }
        float GetMetricFLEdge_2() const { return FLCross.size() > 1 ? FLCross[1].GetMetric(Metric) : 0.0f; }
        float GetMetricFLEdge_3() const { return FLCross.size() > 2 ? FLCross[2].GetMetric(Metric) : 0.0f; }
        float GetMetricFLEdge_4() const { return FLCross.size() > 3 ? FLCross[3].GetMetric(Metric) : 0.0f; }
		float GetMetricFLCross() const
		{
			return GetMetricFLEdge_1() + GetMetricFLEdge_2() + 
			       GetMetricFLEdge_3() + GetMetricFLEdge_4();
		}
		
        float GetMetricFLCorner_1() const { return FLCorners.size() > 0 ? FLCorners[0].GetMetric(Metric) : 0.0f; }
        float GetMetricFLCorner_2() const { return FLCorners.size() > 1 ? FLCorners[1].GetMetric(Metric) : 0.0f; }
        float GetMetricFLCorner_3() const { return FLCorners.size() > 2 ? FLCorners[2].GetMetric(Metric) : 0.0f; }
        float GetMetricFLCorner_4() const { return FLCorners.size() > 3 ? FLCorners[3].GetMetric(Metric) : 0.0f; }
		float GetMetricFLCorners() const
		{
			return GetMetricFLCorner_1() + GetMetricFLCorner_2() + 
			       GetMetricFLCorner_3() + GetMetricFLCorner_4();
		}
		
		float GetMetricSLEdge_1() const { return SLEdges.size() > 0 ? SLEdges[0].GetMetric(Metric) : 0.0f; }
        float GetMetricSLEdge_2() const { return SLEdges.size() > 1 ? SLEdges[1].GetMetric(Metric) : 0.0f; }
        float GetMetricSLEdge_3() const { return SLEdges.size() > 2 ? SLEdges[2].GetMetric(Metric) : 0.0f; }
        float GetMetricSLEdge_4() const { return SLEdges.size() > 3 ? SLEdges[3].GetMetric(Metric) : 0.0f; }
		float GetMetricSL() const
		{
			return GetMetricSLEdge_1() + GetMetricSLEdge_2() + 
			       GetMetricSLEdge_3() + GetMetricSLEdge_4();
		}
		
        float GetMetricLLCross1() const { return LLCross1.GetMetric(Metric); }
		float GetMetricLLCross2() const { return LLCross2.GetMetric(Metric); }
		float GetMetricLLCorners1() const { return LLCorners1.GetMetric(Metric); }
		float GetMetricLLCorners2() const { return LLCorners2.GetMetric(Metric); }
		float GetMetricLL() const
		{
			return GetMetricLLCross1() + GetMetricLLCross2() +
				   GetMetricLLCorners1() + GetMetricLLCorners2();
		}

        // Get the times elapsed searching
        double GetTimeFLCross() const { return TimeFLCross; }
        double GetTimeFLCorners() const { return TimeFLCorners; }
        double GetTimeSL() const { return TimeSL; }
        double GetTimeLLCross1() const { return TimeLLCross1; }
		double GetTimeLLCross2() const { return TimeLLCross2; }
		double GetTimeLLCorners1() const { return TimeLLCorners1; }
		double GetTimeLLCorners2() const { return TimeLLCorners2; }
        double GetTimeLL() const { return GetTimeLLCross1() + GetTimeLLCross2() + GetTimeLLCorners1() + GetTimeLLCorners2(); }
												 
        // Get the time for the full search
        double GetTime() const { return GetTimeFLCross() + GetTimeFLCorners() + GetTimeSL() + GetTimeLL(); }

        // Get a solve report
        std::string GetReport() const;

        // Get a solve time report
        std::string GetTimeReport() const;

		// Get used cores in the solve
		int GetUsedCores() const { return Cores; }

        char GetCrossLayer() const { return Cube::GetLayerChar(FirstLayer); }

		// Returns the shorter LBL last layer corners orientation algorithm from the solves
		static uint EvaluateLLCOResult(Algorithm& , const std::vector<Algorithm>&);

	private:
		
		Algorithm Scramble, // Cube scramble
				  LLCross1, LLCross2, // Orientation and permutation for last layer crosses
				  LLCorners1, LLCorners2; // Permutation and orientation for last layer corners
				  
		std::vector<Algorithm> FLCross, // Algorithms vector for first layer cross
							   FLCorners, // Algorithms vector for first layer (four corners)
							   SLEdges; // Algorithms vector for second layer (four edges)

		Lyr FirstLayer; // First layer
		Fce FLFace; // First layer face
		int FLi; // First layer/face index
		
        Cube CubeBase; // Cube base
		
		Metrics Metric; // Metric for measures

        // Times
        double TimeFLCross, TimeFLCorners, TimeSL, TimeLLCross1, TimeLLCross2, TimeLLCorners1, TimeLLCorners2;

		int Cores; // Cores to use in the search: -1 = no multithreading, 0 = all avaliable cores, other = use this amount of cores

		const static Algorithm Inspections[6], 
			                   Cross1Algorithm,
							   Cross2Algorithm,
							   Corners1Algorithm,
							   Corners2Algorithm, Corners2Algorithm2;
	};
}
