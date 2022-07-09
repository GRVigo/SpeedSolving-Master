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
	grvigo@hotmail.com
*/

#pragma once

#include "cube_definitions.h"
#include "deep_search.h"
#include "method.h"

namespace grcube3
{
    // Class to search a solve for a Rubik's cube using Nautilus method
	class Nautilus : public Method
	{
	public:
		// Constructor
		Nautilus(const Algorithm& Scr, const int NumCores = 0) : Method(Scr, NumCores) { Reset(); }

		// Reset the search results
		void Reset();

        // Search the best first block solves with the given search depth and the maximun number of solves
        bool SearchFB(const uint, const uint = 1u); // Return false if no first block solves found
        void SearchSB(const uint); // Search best second block solves with given search depth (second block in a single step)
		void SearchPairdFR(bool = false); // Add the pair containing the DFR corner and FR edge
		void SearchNCLL(const Plc = Plc::FIRST, const bool = false); // NCLL search
		void SearchNCOLL(const Plc = Plc::FIRST, const bool = false); // NCOLL search
		void SearchTNCLL(const Plc = Plc::FIRST, const bool = false); // TNCLL search
		void SearchL5E(const Plc = Plc::FIRST); // Solve the cube by permuting the last 5 edges
		void SearchEODF(const Plc = Plc::FIRST); // Orient the remaining six edges while placing the DF edge
		void SearchF2L(); // Solve the last F2L pair (complete the first two layers)
		void SearchZBLL(const Plc = Plc::FIRST); // ZBLL search to complete the last layer
		void SearchOCLL(const Plc = Plc::FIRST); // Complete the last layer using OCLL algorithms
        void SearchPLL(const Plc = Plc::FIRST); // Complete the last layer using PLL algorithms
        void SearchCOLL(const Plc = Plc::FIRST); // Complete the last layer using COLL algorithms
        void SearchEPLL(const Plc = Plc::FIRST); // Complete the last layer using EPLL algorithms
		
        // Search the best first block solve algorithms from an algorithms vector
        void EvaluateFB(const std::vector<Algorithm>&, const uint = 1u);

		// Set regrips
		void SetRegrips();
		
		// If the first block is search externally, use this function to set the first block search time
        void SetTimeFS(double t) { TimeFB = t; }

        // If the first block is search externally, use this function to set the first block search depth
        void SetDepthFS(uint d) { MaxDepthFB = d; }

		Algorithm GetFullSolve(const Spn, const uint) const; // Get the full solve metric

		// Get search algorithms texts
		std::string GetTextFB(const Spn sp, const uint n) const { return AlgFB[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextSB(const Spn sp, const uint n) const { return AlgSB[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextdFR(const Spn sp, const uint n) const { return AlgdFR[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextNCLL(const Spn sp, const uint n) const { return AlgNCLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextNCOLL(const Spn sp, const uint n) const { return AlgNCOLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextTNCLL(const Spn sp, const uint n) const { return AlgTNCLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextL5E(const Spn sp, const uint n) const { return AlgL5E[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextEODF(const Spn sp, const uint n) const { return AlgEODF[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextF2L(const Spn sp, const uint n) const { return AlgF2L[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].ToString(); }

        // Get search algorithms lengths
		uint GetLengthFB(const Spn sp, const uint n) const { return AlgFB[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthSB(const Spn sp, const uint n) const { return AlgSB[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthdFR(const Spn sp, const uint n) const { return AlgdFR[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthNCLL(const Spn sp, const uint n) const { return AlgNCLL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthNCOLL(const Spn sp, const uint n) const { return AlgNCOLL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthTNCLL(const Spn sp, const uint n) const { return AlgTNCLL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthL5E(const Spn sp, const uint n) const { return AlgL5E[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthEODF(const Spn sp, const uint n) const { return AlgEODF[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthF2L(const Spn sp, const uint n) const { return AlgF2L[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthLL(const Spn sp, const uint n) const { return  GetLengthZBLL(sp, n) + GetLengthOCLL(sp, n) +
                                                                     GetLengthPLL(sp, n) + GetLengthCOLL(sp, n) + GetLengthEPLL(sp, n); }

		// Get metric values
        float GetMetricFB(const Spn sp, const uint n) const { return AlgFB[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricSB(const Spn sp, const uint n) const { return AlgSB[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricdFR(const Spn sp, const uint n) const { return AlgdFR[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricNCLL(const Spn sp, const uint n) const { return AlgNCLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricNCOLL(const Spn sp, const uint n) const { return AlgNCOLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricTNCLL(const Spn sp, const uint n) const { return AlgTNCLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricL5E(const Spn sp, const uint n) const { return AlgL5E[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricEODF(const Spn sp, const uint n) const { return AlgEODF[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricF2L(const Spn sp, const uint n) const { return AlgF2L[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricLL(const Spn sp, const uint n) const { return  GetMetricZBLL(sp, n) + GetMetricOCLL(sp, n) +
                                                                      GetMetricPLL(sp, n) + GetMetricCOLL(sp, n) + GetMetricEPLL(sp, n); }

		// Get text for current cases
        std::string GetTextNCLLCase(const Spn sp, const uint n) const { return CasesNCLL[static_cast<int>(sp)][n]; }
		std::string GetTextNCOLLCase(const Spn sp, const uint n) const { return CasesNCOLL[static_cast<int>(sp)][n]; }
		std::string GetTextTNCLLCase(const Spn sp, const uint n) const { return CasesTNCLL[static_cast<int>(sp)][n]; }
		std::string GetTextL5ECase(const Spn sp, const uint n) const { return CasesL5E[static_cast<int>(sp)][n]; }
		std::string GetTextEODFCase(const Spn sp, const uint n) const { return CasesEODF[static_cast<int>(sp)][n]; }
		std::string GetTextZBLLCase(const Spn sp, const uint n) const { return CasesZBLL[static_cast<int>(sp)][n]; }
		std::string GetTextOCLLCase(const Spn sp, const uint n) const { return CasesOCLL[static_cast<int>(sp)][n]; }
        std::string GetTextPLLCase(const Spn sp, const uint n) const { return CasesPLL[static_cast<int>(sp)][n]; }
        std::string GetTextCOLLCase(const Spn sp, const uint n) const { return CasesCOLL[static_cast<int>(sp)][n]; }
        std::string GetTextEPLLCase(const Spn sp, const uint n) const { return CasesEPLL[static_cast<int>(sp)][n]; }
		
        // Get a general solve report (all spins with results)
        std::string GetReport(const bool, bool = false) const; // cancellations, debug
        std::string GetReport(const Spn, const uint) const; // Get a solve report for given spin
        std::string GetTimeReport() const; // Get a solve time report

		// Get the time elapsed searching
        double GetTimeFB() const { return TimeFB; }
        double GetTimeSB() const { return TimeSB; }
		double GetTimedFR() const { return TimedFR; }
		double GetTimeNCLL() const { return TimeNCLL; }
		double GetTimeNCOLL() const { return TimeNCOLL; }
		double GetTimeTNCLL() const { return TimeTNCLL; }
		double GetTimeL5E() const { return TimeL5E; }
		double GetTimeEODF() const { return TimeEODF; }
		double GetTimeF2L() const { return TimeF2L; }
		double GetTimeZBLL() const { return TimeZBLL; }
		double GetTimeOCLL() const { return TimeOCLL; }
        double GetTimePLL() const { return TimePLL; }
        double GetTimeCOLL() const { return TimeCOLL; }
        double GetTimeEPLL() const { return TimeEPLL; }
        double GetTimeLL() const { return GetTimeZBLL() + GetTimeOCLL() + GetTimePLL() + GetTimeCOLL() + GetTimeEPLL(); }
		
        double GetFullTime() const { return GetTimeFB() + GetTimeSB() + GetTimedFR() +
		                                    GetTimeNCLL() + GetTimeNCOLL() + GetTimeTNCLL() + GetTimeL5E() + 
											GetTimeEODF() + GetTimeF2L() + GetTimeLL(); }

	private:
		std::vector<Algorithm> AlgFB[24], // Algorithms for the left 3x2x1 block (Nautilus first block)
							   AlgSB[24], // Algorithms for the 2x2x2 block (Nautilus second block)
							   AlgdFR[24], // Algorithms for solving DFR corner and FR edge
							   AlgNCLL[24], // Algorithms for solving the four corners that are on the up layer
							   AlgNCOLL[24], // Algorithms for solving the four corners that are on the up layer
							   AlgTNCLL[24], // Algorithms for solving the four corners that are on the up layer
							   AlgL5E[24], // Solve the cube by permuting the last 5 edges
							   AlgEODF[24], // Orient the remaining six edges while placing the DF edge
							   AlgF2L[24], // Last F2L pair
							   AlgZBLL[24], // ZBLL algorithms
							   AlgOCLL[24], // OCLL algorithms
                               AlgPLL[24], // PLL algorithms
                               AlgCOLL[24], // COLL algorithms
                               AlgEPLL [24]; // EPLL algorithms
							
		// Last used maximum blocks depth
        uint MaxDepthFB, MaxDepthSB, MaxDepthdFR, MaxDepthF2L;

		std::vector<std::string> CasesNCLL[24], // NCLL cases found for each spin
								 CasesNCOLL[24], // NCOLL cases found for each spin
								 CasesTNCLL[24], // TNCLL cases found for each spin
								 CasesL5E[24], // L5E cases found for each spin
								 CasesEODF[24], // EODF cases found for each spin
								 CasesZBLL[24], // ZBLL cases found for each spin
								 CasesOCLL[24], // OCLL cases found for each spin
                                 CasesPLL[24], // PLL cases found for each spin
                                 CasesCOLL[24], // COLL cases found for each spin
                                 CasesEPLL[24]; // EPLL cases found for each spin

		// Times
        double TimeFB, TimeSB, TimedFR, TimeNCLL, TimeNCOLL, TimeTNCLL, TimeL5E, TimeEODF, TimeF2L, TimeZBLL, TimeOCLL, TimePLL, TimeCOLL, TimeEPLL;

		// Check if the solves for the given spin are consistent (all needed algorithms are present)
		bool CheckSolveConsistency(const Spn) const;
	};
}
