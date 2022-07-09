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
    // Class to search a solve for a Rubik's cube using LEOR method
	class LEOR : public Method
	{
	public:
		// Constructor
        LEOR(const Algorithm& Scr, const int NumCores = 0) : Method(Scr, NumCores) { Reset(); }

		// Reset the search results
		void Reset();

        // Search the best first block solve with the given search depth and the maximun number of solves
		// Return false if no first blocks found
        bool SearchFBA(const uint, const uint = 1u); // Build LEOR-A 1x2x3 Blocks on the left side of the cube
        void SearchEOStripe(const uint); // Orient all edges while simultaneously solving DF and DB
        void SearchSB(); // Build LEOR-A / LEOR-B / EOMR second 1x2x3 blocks on the right side of the cube
		void SearchZBLL(const Plc = Plc::FIRST); // Last layers using ZBLL algorithms
		void SearchOCLL(const Plc = Plc::FIRST); // Complete the last layer using OCLL algorithms
        void SearchPLL(const Plc = Plc::FIRST); // Complete the last layer using PLL algorithms
        void SearchCOLL(const Plc = Plc::FIRST); // Complete the last layer using COLL algorithms
        void SearchEPLL(const Plc = Plc::FIRST); // Complete the last layer using EPLL algorithms
		bool SearchFBB(const uint, const uint = 1u); // Build LEOR-B 2x2x2 Blocks on DBL
		void SearchFLPair(); // Solve the FL pairs (LEOR-B)

        // Search the best first block solve algorithms from an algorithms vector
        void EvaluateFBA(const std::vector<Algorithm>&, const uint = 1u);
		void EvaluateFBB(const std::vector<Algorithm>&, const uint = 1u);

		// Set regrips
		void SetRegrips();
		
		// If the first block is search externally, use this function to set the first block search time
        void SetTimeFS(double t) { TimeFB = t; }

        // If the first block is search externally, use this function to set the first block search depth
        void SetDepthFS(uint d) { MaxDepthFB = d; }

        Algorithm GetFullSolve(const Spn, const uint) const; // Get the full solve

		// Get search algorithms texts
		std::string GetTextFBA(const Spn sp, const uint n) const { return AlgFBA[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextEOStripe(const Spn sp, const uint n) const { return AlgEOStripe[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextSB(const Spn sp, const uint n) const { return AlgSB[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextFBB(const Spn sp, const uint n) const { return AlgFBB[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextFLPair(const Spn sp, const uint n) const { return AlgFLPair[static_cast<int>(sp)][n].ToString(); }

        // Get search algorithms lengths
		uint GetLengthFBA(const Spn sp, const uint n) const { return AlgFBA[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthEOStripe(const Spn sp, const uint n) const { return AlgEOStripe[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthSB(const Spn sp, const uint n) const { return AlgSB[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthLL(const Spn sp, const uint n) const { return  GetLengthZBLL(sp, n) + GetLengthOCLL(sp, n) +
                                                                     GetLengthPLL(sp, n) + GetLengthCOLL(sp, n) + GetLengthEPLL(sp, n); }
		uint GetLengthFBB(const Spn sp, const uint n) const { return AlgFBB[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthFLPair(const Spn sp, const uint n) const { return AlgFLPair[static_cast<int>(sp)][n].GetNumSteps(); }

		// Get metric values
        float GetMetricFBA(const Spn sp, const uint n) const { return AlgFBA[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricEOStripe(const Spn sp, const uint n) const { return AlgEOStripe[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricSB(const Spn sp, const uint n) const { return AlgSB[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricLL(const Spn sp, const uint n) const { return  GetMetricZBLL(sp, n) + GetMetricOCLL(sp, n) +
                                                                     GetMetricPLL(sp, n) + GetMetricCOLL(sp, n) + GetMetricEPLL(sp, n); }
        float GetMetricFBB(const Spn sp, const uint n) const { return AlgFBB[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricFLPair(const Spn sp, const uint n) const { return AlgFLPair[static_cast<int>(sp)][n].GetMetric(Metric); }
		
        // Get text for current cases
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
        double GetTimeEOStripe() const { return TimeEOStripe; }
        double GetTimeSB() const { return TimeSB; }
		double GetTimeZBLL() const { return TimeZBLL; }
		double GetTimeOCLL() const { return TimeOCLL; }
        double GetTimePLL() const { return TimePLL; }
        double GetTimeCOLL() const { return TimeCOLL; }
        double GetTimeEPLL() const { return TimeEPLL; }
        double GetTimeLL() const { return GetTimeZBLL() + GetTimeOCLL() + GetTimePLL() + GetTimeCOLL() + GetTimeEPLL(); }
        double GetTimeFLPair() const { return TimeFLPair; }
        double GetFullTime() const { return GetTimeFB() + GetTimeEOStripe() + GetTimeSB() + GetTimeLL() + GetTimeFLPair(); }

	private:

		std::vector<Algorithm> AlgFBA[24], // LEOR-A 1x2x3 Blocks on the left side of the cube
							   AlgEOStripe[24], // Orient all edges while simultaneously solving DF and DB
							   AlgSB[24], // LEOR-A / LEOR-B / EOMR second 1x2x3 blocks on the right side of the cube
							   AlgZBLL[24], // Last layer ZBLL algorithms
							   AlgOCLL[24], // OCLL algorithms
                               AlgPLL[24], // PLL algorithms
                               AlgCOLL[24], // COLL algorithms
                               AlgEPLL [24], // EPLL algorithms
							   AlgFBB[24], // LEOR-B 2x2x2 Blocks on DBL
                               AlgFLPair[24]; // FL pairs (LEOR-B)

		// Last used maximum first blocks deep
        uint MaxDepthFB, MaxDepthSB, MaxDepthEOStripe, MaxDepthFLPair;
		
        std::vector<std::string> CasesZBLL[24], // ZBLL cases found for each spin
                                 CasesOCLL[24], // OCLL cases found for each spin
                                 CasesPLL[24], // PLL cases found for each spin
                                 CasesCOLL[24], // COLL cases found for each spin
                                 CasesEPLL[24]; // EPLL cases found for each spin

		// Times
        double TimeFB, TimeEOStripe, TimeSB, TimeZBLL, TimeOCLL, TimePLL, TimeCOLL, TimeEPLL, TimeFLPair;

		// Check if the solves for the given spin are consistent (all needed algorithms are present)
		bool CheckSolveConsistency(const Spn) const;
	};
}
