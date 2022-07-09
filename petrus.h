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

#pragma once

#include "cube_definitions.h"
#include "deep_search.h"
#include "method.h"

namespace grcube3
{
    // Class to search a solve for a Rubik's cube using Petrus method
    class Petrus : public Method
	{
	public:
		// Constructor
        Petrus(const Algorithm& Scr, const int NumCores = 0) : Method(Scr, NumCores) { Reset(); }

		// Reset the search results
		void Reset();

        // Search the best block solve algorithm with the given search depth
        // Return false if no block found
        bool SearchBlock(const uint, const uint = 1u);
        void SearchExpandedBlock(); // Search the best expanded block solve algorithm
        void SearchEO(); // Orientate the other edges outside the block
        void SearchF2L(const uint = 12u); // Complete the two first layers (F2L) with the given search depth
        void SearchF2L_Alt(const uint); // Complete two first layers (F2L) - Alternative version (don't use it, slower and debug needed)
        void SearchZBLL(const Plc = Plc::FIRST); // Complete the last layer using ZBLL algorithms
        void SearchOCLL(const Plc = Plc::FIRST); // Complete the last layer using OCLL algorithms
        void SearchPLL(const Plc = Plc::FIRST); // Complete the last layer using PLL algorithms
        void SearchCOLL(const Plc = Plc::FIRST); // Complete the last layer using COLL algorithms
        void SearchEPLL(const Plc = Plc::FIRST); // Complete the last layer using EPLL algorithms

        // Search the best block solve algorithms from an algorithms vector
        void EvaluateBlock(const std::vector<Algorithm>&, const uint = 1u);

        // Set regrips
        void SetRegrips();
		
		// If the block is search externally, use this function to set the block search time
        void SetTimeFS(double t) { TimeBlock = t; }

        // If the block is search externally, use this function to set the block search depth
        void SetDepthFS(uint d) { MaxDepthBlock = d; }

        Algorithm GetFullSolve(const Spn, const uint) const; // Get the full solve

		// Get search algorithms texts
        std::string GetTextBlock(const Spn sp, const uint n) const { return Blocks[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextExpandedBlock(const Spn sp, const uint n) const { return ExpandedBlocks[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextEO(const Spn sp, const uint n) const { return EO[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextF2L(const Spn sp, const uint n) const { return F2L[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].ToString(); }

        // Get search algorithms lengths
        uint GetLengthBlock(const Spn sp, const uint n) const { return Blocks[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthExpandedBlock(const Spn sp, const uint n) const { return ExpandedBlocks[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthEO(const Spn sp, const uint n) const { return EO[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2L(const Spn sp, const uint n) const { return F2L[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthLL(const Spn sp, const uint n) const { return  GetLengthZBLL(sp, n) + GetLengthOCLL(sp, n) +
                                                                     GetLengthPLL(sp, n) + GetLengthCOLL(sp, n) + GetLengthEPLL(sp, n); }

        // Get metric values
        float GetMetricBlock(const Spn sp, const uint n) const { return Blocks[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricExpandedBlock(const Spn sp, const uint n) const { return ExpandedBlocks[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricEO(const Spn sp, const uint n) const { return EO[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricF2L(const Spn sp, const uint n) const { return F2L[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricLL(const Spn sp, const uint n) const { return  GetMetricZBLL(sp, n) + GetMetricOCLL(sp, n) +
                                                                     GetMetricPLL(sp, n) + GetMetricCOLL(sp, n) + GetMetricEPLL(sp, n); }
        
        // Get text for current cases
        std::string GetTextZBLLCase(const Spn sp, const uint n) const { return CasesZBLL[static_cast<int>(sp)][n]; }
        std::string GetTextOCLLCase(const Spn sp, const uint n) const { return CasesOCLL[static_cast<int>(sp)][n]; }
        std::string GetTextPLLCase(const Spn sp, const uint n) const { return CasesPLL[static_cast<int>(sp)][n]; }
        std::string GetTextCOLLCase(const Spn sp, const uint n) const { return CasesCOLL[static_cast<int>(sp)][n]; }
        std::string GetTextEPLLCase(const Spn sp, const uint n) const { return CasesEPLL[static_cast<int>(sp)][n]; }
        
        // Get a general solve report (all spins with results)
        std::string GetReport(const bool, bool = false) const; // cancellations, debug

		// Get a solve report for given spin
    	std::string GetReport(const Spn, const uint) const;

        // Get a solve time report
        std::string GetTimeReport() const;

        // Get the time elapsed searching
        double GetTimeBlock() const { return TimeBlock; }
        double GetTimeExpBlock() const { return TimeExpBlock; }
        double GetTimeEO() const { return TimeEO; }
        double GetTimeF2L() const { return TimeF2L; }
        double GetTimeZBLL() const { return TimeZBLL; }
        double GetTimeOCLL() const { return TimeOCLL; }
        double GetTimePLL() const { return TimePLL; }
        double GetTimeCOLL() const { return TimeCOLL; }
        double GetTimeEPLL() const { return TimeEPLL; }
        double GetTimeLL() const { return GetTimeZBLL() + GetTimeOCLL() + GetTimePLL() + GetTimeCOLL() + GetTimeEPLL(); }
        double GetTime() const { return GetTimeBlock() + GetTimeExpBlock() + GetTimeEO() + GetTimeF2L() + GetTimeLL(); }

	private:

        std::vector<Algorithm> Blocks[24], // Algorithms for the block (Petrus first block)
                               ExpandedBlocks[24], // Algorithms for the expanded block
                               EO[24], // Algorithms for edges orientation
                               F2L[24], // First two layers algorithms
                               AlgZBLL[24], // ZBLL algorithms
                               AlgOCLL[24], // OCLL algorithms
                               AlgPLL[24], // PLL algorithms
                               AlgCOLL[24], // COLL algorithms
                               AlgEPLL[24]; // EPLL algorithms

        // Last used maximum blocks & F2L depth
        uint MaxDepthBlock, MaxDepthF2L;
        
        std::vector<std::string> CasesZBLL[24], // ZBLL cases found for each spin
                                 CasesOCLL[24], // OCLL cases found for each spin
                                 CasesPLL[24], // PLL cases found for each spin
                                 CasesCOLL[24], // COLL cases found for each spin
                                 CasesEPLL[24]; // EPLL cases found for each spin

		// Times
        double TimeBlock, TimeExpBlock, TimeEO, TimeF2L, TimeZBLL, TimeOCLL, TimePLL, TimeCOLL, TimeEPLL;

        // Check if the solves for the given spin are consistent (all needed algorithms are present)
        bool CheckSolveConsistency(const Spn) const;
	};
}
