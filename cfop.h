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
            else for (const auto Ly : CL) if (Cube::IsExternalLayer(Ly)) CrossLayers.push_back(Ly);
		}
		
		// Set the metric for evaluations
		void SetMetric(const Metrics m) { Metric = m; }

        // Solves searchs
        bool SearchCrosses(const uint, const uint = 1u); // Search best solves for given search depth and the maximun inspections, return false if no crosses found
        void SearchF2L();
		void SearchOLL();
		void SearchPLL();
        void Search1LLL();
        void SearchEOLL();
		void SearchZBLL();

        // Search the best crosses solves from a given algorithms vector
        void EvaluateCrosses(const std::vector<Algorithm>&, const uint = 1u);

		// Set regrips
		void SetRegrips();

        // If the crosses are searched externally, use this function to set the crosses search time
        void SetTimeCrosses(double t) { TimeCrosses = t; }

        // If the crosses are searched externally, use this function to set the crosses search depth
        void SetDepthCrosses(uint d) { DepthCrosses = d; }

        // Get current cases in given cross face
        std::string GetOLLCase(const Fce CrossFace, const uint n) const { return CasesOLL[static_cast<int>(CrossFace)][n]; }
        std::string GetPLLCase(const Fce CrossFace, const uint n) const { return CasesPLL[static_cast<int>(CrossFace)][n]; }
        std::string Get1LLLCase(const Fce CrossFace, const uint n) const { return Cases1LLL[static_cast<int>(CrossFace)][n]; }
        std::string GetZBLLCase(const Fce CrossFace, const uint n) const { return CasesZBLL[static_cast<int>(CrossFace)][n]; }

        // Get search solves texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
        std::string GetTextInspection(const Fce CrossFace, const uint n) const { return Inspections[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextCross(const Fce CrossFace, const uint n) const { return Crosses[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextF2L_1(const Fce CrossFace, const uint n) const { return F2L_1[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextF2L_2(const Fce CrossFace, const uint n) const { return F2L_2[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextF2L_3(const Fce CrossFace, const uint n) const { return F2L_3[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextF2L_4(const Fce CrossFace, const uint n) const { return F2L_4[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextF2L(const Fce, const uint) const; // Get full F2L text with parentheses
        std::string GetTextOLL(const Fce CrossFace, const uint n) const { return A_OLL[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextPLL(const Fce CrossFace, const uint n) const { return A_PLL[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetText1LLL(const Fce CrossFace, const uint n) const { return A_1LLL[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextEOLL(const Fce CrossFace, const uint n) const { return EOLL[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextZBLL(const Fce CrossFace, const uint n) const { return A_ZBLL[static_cast<int>(CrossFace)][n].ToString(); }
        std::string GetTextAUF(const Fce CrossFace, const uint n) const { return AUF[static_cast<int>(CrossFace)][n].ToString(); }

        // Get search solves lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
        uint GetLengthInspection(const Fce CrossFace, const uint n) const { return Inspections[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthCross(const Fce CrossFace, const uint n) const { return Crosses[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthF2L_1(const Fce CrossFace, const uint n) const { return F2L_1[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthF2L_2(const Fce CrossFace, const uint n) const { return F2L_2[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthF2L_3(const Fce CrossFace, const uint n) const { return F2L_3[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthF2L_4(const Fce CrossFace, const uint n) const { return F2L_4[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthOLL(const Fce CrossFace, const uint n) const { return A_OLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthPLL(const Fce CrossFace, const uint n) const { return A_PLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLength1LLL(const Fce CrossFace, const uint n) const { return A_1LLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthEOLL(const Fce CrossFace, const uint n) const { return EOLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthZBLL(const Fce CrossFace, const uint n) const { return A_ZBLL[static_cast<int>(CrossFace)][n].GetNumSteps(); }
        uint GetLengthAUF(const Fce CrossFace, const uint n) const { return AUF[static_cast<int>(CrossFace)][n].GetNumSteps(); }

        // Get metric values
        float GetMetricSolve(const Fce, const uint) const; // Get the full solve metric
		float GetMetricScramble() const { return Scramble.GetMetric(Metric); }
        float GetMetricInspection(const Fce CrossFace, const uint n) const { return Inspections[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricCross(const Fce CrossFace, const uint n) const { return Crosses[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricF2L_1(const Fce CrossFace, const uint n) const { return F2L_1[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricF2L_2(const Fce CrossFace, const uint n) const { return F2L_2[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricF2L_3(const Fce CrossFace, const uint n) const { return F2L_3[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricF2L_4(const Fce CrossFace, const uint n) const { return F2L_4[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricOLL(const Fce CrossFace, const uint n) const { return A_OLL[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricPLL(const Fce CrossFace, const uint n) const { return A_PLL[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetric1LLL(const Fce CrossFace, const uint n) const { return A_1LLL[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricEOLL(const Fce CrossFace, const uint n) const { return EOLL[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricZBLL(const Fce CrossFace, const uint n) const { return A_ZBLL[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
        float GetMetricAUF(const Fce CrossFace, const uint n) const { return AUF[static_cast<int>(CrossFace)][n].GetMetric(Metric); }
		
		// Get text for cases
        std::string GetTextOLLCase(const Fce CrossFace, const uint n) const { return CasesOLL[static_cast<int>(CrossFace)][n]; }
        std::string GetTextPLLCase(const Fce CrossFace, const uint n) const { return CasesPLL[static_cast<int>(CrossFace)][n]; }
        std::string GetText1LLLCase(const Fce CrossFace, const uint n) const { return Cases1LLL[static_cast<int>(CrossFace)][n]; }
        std::string GetTextZBLLCase(const Fce CrossFace, const uint n) const { return CasesZBLL[static_cast<int>(CrossFace)][n]; }

        // Get the times elapsed searching
        double GetTimeCrosses() const { return TimeCrosses; }
        double GetTimeF2L() const { return TimeF2L; }
        double GetTimeOLL() const { return TimeOLL; }
        double GetTimePLL() const { return TimePLL; }
        double GetTime1LLL() const { return Time1LLL; }
        double GetTimeEOLL() const { return TimeEOLL; }
        double GetTimeZBLL() const { return TimeZBLL; }
        double GetTimeLL() const { return GetTimeOLL() + GetTimePLL() + GetTime1LLL() + GetTimeEOLL() + GetTimeZBLL(); }
        double GetTime() const { return GetTimeCrosses() + GetTimeF2L() + GetTimeLL();	}

        // Get solves report
        std::string GetReport(const bool, const bool = false) const; // cancellations, debug
        std::string GetReport(const Lyr, const uint) const; // Get a single solve report
        std::string GetTimeReport() const; // Get solves time report
        std::string GetBestReport(bool = false) const; // Get the best solve report (STM with or without cancellations)

		// Get the full solve with cancellations
		Algorithm GetCancellations(const Lyr, const uint) const;

        // Get solve metric with cancellations
		float GetMetricCancellations(const Fce CF, const uint n) const { return GetCancellations(Cube::FaceToLayer(CF), n).GetMetric(Metric); }

		// Get used cores in the solve
        int GetCores() const { return Cores; }

		// Returns best F2L solve from the Solves vector class member
		// F2L pieces are used as evaluation condition
		static bool EvaluateF2LResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Lyr, const Plc, const bool = true);

        static bool IsCrossBuilt(const Cube&, const Lyr); // Check if the CFOP first cross is built
        static bool IsF2LBuilt(const Cube&, const Lyr); // Check if first two layers are built

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
                               EOLL[6], // Algorithms for last layer edges orientation
							   A_ZBLL[6], // Algorithms for ZBLL
							   AUF[6]; // Algorithms for AUF
		
        uint DepthCrosses; // Crosses search depth

        std::vector<std::string> CasesOLL[6], // OLL cases
                                 CasesPLL[6], // PLL cases
                                 CasesZBLL[6], // ZBLL cases
                                 Cases1LLL[6]; // One-Look Last Layer cases

        Cube CubeBase; // Cube base
		
		Metrics Metric; // Metric for measures

        double TimeCrosses, TimeF2L, TimeOLL, TimePLL, Time1LLL, TimeEOLL, TimeZBLL; // Times

		int Cores; // Cores to use in the search: -1 = no multithreading, 0 = all avaliable cores, other = use this amount of cores

		const static Algorithm DefaultInspections[6]; 
		
		// Check if the solves for the given cross layer are consistent (all needed algorithms are present)
		bool CheckSolveConsistency(const Lyr) const;

		// Check if in the given cross face the solve is OK
		bool IsSolved(const Fce, const uint) const;
	};
}
