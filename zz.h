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

namespace grcube3
{
    // Class to search a solve for a Rubik's cube using ZZ method
    class ZZ
    {
    public:
        // Constructor with scramble
        ZZ(const Algorithm& Scr, const int NumCores = 0)
        {
            Scramble = Scr;
            CubeBase = Cube(Scramble);
            Cores = NumCores;
            Reset();
        }

		// Destructor
        ~ZZ() {}

		// Reset the search results
		void Reset();

        // Set spins for search
        void SetSearchSpins(const std::vector<Spn>& SS)
        {
            SearchSpins.clear();
            if (SS.empty()) for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s));
            else for (const auto s : SS) SearchSpins.push_back(s);
        }
		
		// Set the metric for evaluations
		void SetMetric(const Metrics m) { Metric = m; }

        // Search the best EOX solve with the given search depth
        // Return false if no EOX found
        bool SearchEOX(const uint, const uint = 1u);
        void SearchF2L(); // Complete the two first layers (F2L)
        void SearchZBLL(); // Complete the last layer using ZBLL algorithms
        void SearchOCLL(); // Complete the last layer using OCLL algorithms
        void SearchPLL(); // Complete the last layer using PLL algorithms
        void SearchCOLL(); // Complete the last layer using COLL algorithms
        void SearchEPLL(); // Complete the last layer using EPLL algorithms

        // Search the best EOX solve algorithms from an algorithms vector
        void EvaluateEOX(const std::vector<Algorithm>&, const uint = 1u);

        // Set regrips
        void SetRegrips();
		
		// If the EOLine is search externally, use this function to set the EOLine search time
        void SetTimeEOX(double t) { TimeEOX = t; }

        // If the EOLine is search externally, use this function to set the EOLine search depth
        void SetDepthEOX(uint d) { MaxDepthEOX = d; }

		// Get search algorithms texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
        std::string GetTextEOLine(const Spn sp, const uint n) const { return EOX[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextF2L_1(const Spn sp, const uint n) const { return F2L_1[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextF2L_2(const Spn sp, const uint n) const { return F2L_2[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextF2L_3(const Spn sp, const uint n) const { return F2L_3[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextF2L_4(const Spn sp, const uint n) const { return F2L_4[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextF2L(const Spn, const uint) const; // Get full F2L string

        std::string GetTextZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].ToString(); }

        // Get search solves lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
        uint GetLengthEOX(const Spn sp, const uint n) const { return EOX[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2L_1(const Spn sp, const uint n) const { return F2L_1[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2L_2(const Spn sp, const uint n) const { return F2L_2[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2L_3(const Spn sp, const uint n) const { return F2L_3[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2L_4(const Spn sp, const uint n) const { return F2L_4[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2L(const Spn sp, const uint n) const { return GetLengthF2L_1(sp, n) + GetLengthF2L_2(sp, n) +
                                                                     GetLengthF2L_3(sp, n) + GetLengthF2L_4(sp, n); }
        uint GetLengthZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthLL(const Spn sp, const uint n) const { return GetLengthZBLL(sp, n) + GetLengthOCLL(sp, n) +
                                                                    GetLengthPLL(sp, n) + GetLengthCOLL(sp, n) + GetLengthEPLL(sp, n); }

        // Get metric values
        float GetMetricSolve(const Spn, const uint) const; // Get the full solve metric
		float GetMetricScramble() const { return Scramble.GetMetric(Metric); }
        float GetMetricEOX(const Spn sp, const uint n) const { return EOX[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricF2L_1(const Spn sp, const uint n) const { return F2L_1[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricF2L_2(const Spn sp, const uint n) const { return F2L_2[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricF2L_3(const Spn sp, const uint n) const { return F2L_3[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricF2L_4(const Spn sp, const uint n) const { return F2L_4[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricF2L(const Spn sp, const uint n) const { return GetMetricF2L_1(sp, n) + GetMetricF2L_2(sp, n) +
                                                                       GetMetricF2L_3(sp, n) + GetMetricF2L_4(sp, n); }
        float GetMetricZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricLL(const Spn sp, const uint n) const { return GetMetricZBLL(sp, n) + GetMetricOCLL(sp, n) +
                                                                      GetMetricPLL(sp, n) + GetMetricCOLL(sp, n) + GetMetricEPLL(sp, n); }
        
        // Get text for current cases
        std::string GetTextZBLLCase(const Spn sp, const uint n) const { return CasesZBLL[static_cast<int>(sp)][n]; }
        std::string GetTextOCLLCase(const Spn sp, const uint n) const { return CasesOCLL[static_cast<int>(sp)][n]; }
        std::string GetTextPLLCase(const Spn sp, const uint n) const { return CasesPLL[static_cast<int>(sp)][n]; }
        std::string GetTextCOLLCase(const Spn sp, const uint n) const { return CasesCOLL[static_cast<int>(sp)][n]; }
        std::string GetTextEPLLCase(const Spn sp, const uint n) const { return CasesEPLL[static_cast<int>(sp)][n]; }
        
        // Get a general solve report (all spins with results)
        std::string GetReport(const bool, bool = false) const; // cancellations, debug
        std::string GetReport(const Spn, const uint n) const; // Get a solve report for given spin
        std::string GetTimeReport() const; // Get a solve time report
        std::string GetBestReport(const bool = false) const; // Get the best solve report (STM with or without cancellations)

        // Get the full solve with cancellations
        Algorithm GetCancellations(const Spn, const uint) const;

        // Get the solve with cancellations metric
		float GetMetricCancellations(const Spn spin, const uint n) const { return GetCancellations(spin, n).GetMetric(Metric); }

		// Get used cores in the solve
		int GetUsedCores() const { return Cores; }

        // Get the time elapsed searching ZZ
        double GetTimeEOX() const { return TimeEOX; }
        double GetTimeF2L() const { return TimeF2L; }
        double GetTimeZBLL() const { return TimeZBLL; }
        double GetTimeOCLL() const { return TimeOCLL; }
        double GetTimePLL() const { return TimePLL; }
        double GetTimeCOLL() const { return TimeCOLL; }
        double GetTimeEPLL() const { return TimeEPLL; }
        double GetTimeLL() const { return GetTimeZBLL() + GetTimeOCLL() + GetTimePLL() + GetTimeCOLL() + GetTimeEPLL(); }
        double GetTime() const { return GetTimeEOX() + GetTimeF2L() + GetTimeLL(); }

        // Check if in the given spin the solve is OK
        bool IsSolved(const Spn, const uint n) const;

        // Check if ZZ EOX is built for given spin
        static bool IsEOLineBuilt(const Cube&, const Spn);
        static bool IsEOArrowBuilt(const Cube&, const Spn);
        static bool IsEOCrossBuilt(const Cube&, const Spn);
        static bool IsXEOLineBuilt(const Cube&, const Spn);
        static bool IsXEOCrossBuilt(const Cube&, const Spn);
        static bool IsEO223Built(const Cube&, const Spn);

        // Returns best EOX/F2L solve from the Solves vector class member and his score
        static bool EvaluateEOXResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);
        static bool EvaluateF2LResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);
		
	private:
		
        Algorithm Scramble; // Cube scramble
                  
        std::vector<Spn> SearchSpins;
        
        std::vector<Algorithm> Inspections[24], // Algorithms for inspections
                               EOX[24], // Algorithms for EOLine (ZZ first step)
                               F2L_1[24], // Algorithms for F2L 1
                               F2L_2[24], // Algorithms for F2L 2
                               F2L_3[24], // Algorithms for F2L 3
                               F2L_4[24], // Algorithms for F2L 4
                               AlgZBLL[24], // ZBLL algorithms
                               AlgOCLL[24], // OCLL algorithms
                               AlgPLL[24], // PLL algorithms
                               AlgCOLL[24], // COLL algorithms
                               AlgEPLL [24]; // EPLL algorithms
		
		Cube CubeBase;
		
		Metrics Metric; // Metric for measures

        // Last used maximum EOX depth
        uint MaxDepthEOX;
        
        std::vector<std::string> CasesZBLL[24], // ZBLL cases found for each spin
                                 CasesOCLL[24], // OCLL cases found for each spin
                                 CasesPLL[24], // PLL cases found for each spin
                                 CasesCOLL[24], // COLL cases found for each spin
                                 CasesEPLL[24]; // EPLL cases found for each spin
        
		// Cores to use in the search: -1 = no multithreading, 0 = all avaliable cores, other = use this amount of cores
		int Cores;

		// Times
        double TimeEOX, TimeF2L, TimeZBLL, TimeOCLL, TimePLL, TimeCOLL, TimeEPLL;

        // Complete the two first layers (F2L) after EO Line (No EO Cross)
        void SearchF2L_EOLine(const Spn, const uint);

        // Complete the two first layers (F2L) after EO Cross (similar to CFOP F2L)
        void SearchF2L_EOCross(const Spn, const uint);

        // Check if the solves for the given spin are consistent (all needed algorithms are present)
        bool CheckSolveConsistency(const Spn) const;
	};
}
