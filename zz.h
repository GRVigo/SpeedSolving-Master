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

        // Search the best EOLine solve algorithm with the given search depth
        // Return false if no EOLine found
        bool SearchEOLine(const uint, const uint = 1u);
		
		// Search the best EOLine solve algorithms from an algorithms vector
		void EvaluateEOLine(const std::vector<Algorithm>&, const uint = 1u);

        // Complete the two first layers (F2L)
        void SearchF2L();

        // Complete the last layer using ZBLL algorithms
        void SearchZBLL();

        // Complete the last layer using OCLL algorithms
        void SearchOCLL();

        // Complete the last layer using PLL algorithms
        void SearchPLL();
		
        // Complete the last layer using COLL algorithms
        void SearchCOLL();

        // Complete the last layer using EPLL algorithms
        void SearchEPLL();

        // Set regrips
        void SetRegrips();
		
		// If the EOLine is search externally, use this function to set the EOLine search time
		void SetEOLineTime(double t) { TimeEOLine = t; }

        // If the EOLine is search externally, use this function to set the EOLine search depth
        void SetEOLineDepth(uint d) { MaxDepthEO = d; }

		// Get search algorithms texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
        std::string GetTextEOLine(const Spn sp, const uint n) const { return EOLine[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextF2LFirstSolve(const Spn sp, const uint n) const
        {
            int si = static_cast<int>(sp);
            return F2L_1[si].empty() ? "" : F2L_1[si][n].ToString();
        }
        std::string GetTextF2LSecondSolve(const Spn sp, const uint n) const
        {
            int si = static_cast<int>(sp);
            return F2L_2[si].empty() ? "" : F2L_2[si][n].ToString();
        }
        std::string GetTextF2LThirdSolve(const Spn sp, const uint n) const
        {
            int si = static_cast<int>(sp);
            return F2L_3[si].empty() ? "" : F2L_3[si][n].ToString();
        }
        std::string GetTextF2LFourthSolve(const Spn sp, const uint n) const
        {
            int si = static_cast<int>(sp);
            return F2L_4[si].empty() ? "" : F2L_4[si][n].ToString();
        }

        std::string GetTextF2L(const Spn sp, const uint n) const
        {
            std::string F2LString, Aux = GetTextF2LFirstSolve(sp, n);
            if (Aux.size() > 0u) F2LString += " (" + Aux + ")";
            Aux = GetTextF2LSecondSolve(sp, n);
            if (Aux.size() > 0u) F2LString += " (" + Aux + ")";
            Aux = GetTextF2LThirdSolve(sp, n);
            if (Aux.size() > 0u) F2LString += " (" + Aux + ")";
            Aux = GetTextF2LFourthSolve(sp, n);
            if (Aux.size() > 0u) F2LString += " (" + Aux + ")";
            return F2LString;
        }
        std::string GetTextZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].ToString(); }

        // Get search algorithms lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
        uint GetLengthEOLine(const Spn sp, const uint n) const { return EOLine[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2LFirstSolve(const Spn sp, const uint n) const { return F2L_1[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2LSecondSolve(const Spn sp, const uint n) const { return F2L_2[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2LThirdSolve(const Spn sp, const uint n) const { return F2L_3[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2LFourthSolve(const Spn sp, const uint n) const { return F2L_4[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthF2L(const Spn sp, const uint n) const { return GetLengthF2LFirstSolve(sp, n) + GetLengthF2LSecondSolve(sp, n) +
                                                                     GetLengthF2LThirdSolve(sp, n) + GetLengthF2LFourthSolve(sp, n); }
        uint GetLengthZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthOCLL(const Spn sp, const uint n) const { return AlgOCLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthEPLL(const Spn sp, const uint n) const { return AlgEPLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthLastLayer(const Spn sp, const uint n) const { return GetLengthZBLL(sp, n) + GetLengthOCLL(sp, n) +
                                                                           GetLengthPLL(sp, n) + GetLengthCOLL(sp, n) + GetLengthEPLL(sp, n); }

        // Get the solve STM metric
        uint GetSolveSTM(const Spn, const uint n) const;
        
        // Get text for current ZBLL case
        std::string GetTextZBLLCase(const Spn sp, const uint n) const { return CaseZBLL[static_cast<int>(sp)][n]; }
		
		// Get text for current OCLL case
        std::string GetTextOCLLCase(const Spn sp, const uint n) const { return CaseOCLL[static_cast<int>(sp)][n]; }
		
		// Get text for current PLL case
        std::string GetTextPLLCase(const Spn sp, const uint n) const { return CasePLL[static_cast<int>(sp)][n]; }
		
		// Get text for current COLL case
        std::string GetTextCOLLCase(const Spn sp, const uint n) const { return CaseCOLL[static_cast<int>(sp)][n]; }
		
		// Get text for current EPLL case
        std::string GetTextEPLLCase(const Spn sp, const uint n) const { return CaseEPLL[static_cast<int>(sp)][n]; }
        
        // Get a general solve report (all spins with results)
        std::string GetReport(const bool, bool = false) const; // cancellations, debug

		// Get a solve report for given spin
    	std::string GetReport(const Spn, const uint n) const;

        // Get a solve time report
        std::string GetTimeReport() const;

        // Get the best solve report (STM with or without cancellations)
        std::string GetBestReport(const bool = false) const;

        // Get the full solve with cancellations
        Algorithm GetCancellations(const Spn, const uint) const;

        // Get the solve with cancellations STM metric
        uint GetCancellationsSTM(const Spn spin, const uint n) const { return GetCancellations(spin, n).GetSTM(); }

		// Get used cores in the solve
		int GetUsedCores() const { return Cores; }

        // Get the time elapsed searching ZZ
        double GetTimeEOLine() const { return TimeEOLine; }
        double GetTimeF2L() const { return TimeF2L; }
        double GetTimeZBLL() const { return TimeZBLL; }
        double GetTimeOCLL() const { return TimeOCLL; }
        double GetTimePLL() const { return TimePLL; }
        double GetTimeCOLL() const { return TimeCOLL; }
        double GetTimeEPLL() const { return TimeEPLL; }
        double GetTimeLastLayer() const { return GetTimeZBLL() + GetTimeOCLL() + GetTimePLL() + GetTimeCOLL() + GetTimeEPLL(); }
        double GetFullTime() const { return GetTimeEOLine() + GetTimeF2L() + GetTimeLastLayer(); }

        // Check if in the given spin the solve is OK
        bool IsSolved(const Spn, const uint n) const;

		// Check if ZZ EOLine is built for given spin
        static bool IsEOLineBuilt(const Cube&, const Spn);

		// Check if ZZ EOArrow is built for given spin
        static bool IsEOArrowBuilt(const Cube&, const Spn);

        // Check if ZZ EOCross is built for given spin
        static bool IsEOCrossBuilt(const Cube&, const Spn);

		// Check if ZZ EOXLine is built for given spin
        static bool IsXEOLineBuilt(const Cube&, const Spn);

        // Check if ZZ XEOCross is built for given spin
        static bool IsXEOCrossBuilt(const Cube&, const Spn);

		// Check if ZZ EO223 is built for given spin
        static bool IsEO223Built(const Cube&, const Spn);

        // Returns best EO solve from the Solves vector class member and his score
        static bool EvaluateEOResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);
        static bool EvaluateF2LResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);
		
	private:
		
        Algorithm Scramble; // Cube scramble
                  
        std::vector<Spn> SearchSpins;
        
        std::vector<Algorithm> Inspections[24], // Algorithms for inspections
                               EOLine[24], // Algorithms for EOLine (ZZ first step)
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

        // Last used maximum EO depth
        uint MaxDepthEO;
        
        std::vector<std::string> CaseZBLL[24], // ZBLL cases found for each spin
                                 CaseOCLL[24], // OCLL cases found for each spin
                                 CasePLL[24], // PLL cases found for each spin
		                         CaseCOLL[24], // COLL cases found for each spin
                                 CaseEPLL[24]; // EPLL cases found for each spin
        
		// Cores to use in the search: -1 = no multithreading, 0 = all avaliable cores, other = use this amount of cores
		int Cores;

		// Times
        double TimeEOLine, TimeF2L, TimeZBLL, TimeOCLL, TimePLL, TimeCOLL, TimeEPLL;

        // Complete the two first layers (F2L) after EO Line (No EO Cross)
        void SearchF2L_EOLine(const Spn, const uint);

        // Complete the two first layers (F2L) after EO Cross (similar to CFOP F2L)
        void SearchF2L_EOCross(const Spn, const uint);

        // Check if the solves for the given spin are consistent (all needed algorithms are present)
        bool CheckSolveConsistency(const Spn) const;
	};
}
