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

namespace grcube3
{
    // Class to search a solve for a Rubik's cube using CEOR YruRU method
	class YruRU
	{
	public:
		// Constructor with scramble
        YruRU(const Algorithm& Scr, const int NumCores = 0) 
		{ 
			Scramble = Scr; 
			CubeBase = Cube(Scramble);
			Cores = NumCores;
			Reset();
		}

		// Destructor
		~YruRU() {}

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

        // Search the best first block solve with the given search deep and the maximun number of solves
		// Return false if no first block found
		// CP-line: first, a 1x1x3 block is solved in the bottom-left of the cube, 
		// while simultaneously solving corner permutation. We get a CP-skip once every 6 solves.
		// This step takes 4-6 moves on average, and following this step the entire cube is solved using <r, u, R, U>
        bool SearchLines(const uint, const uint = 1u);
		void SearchCP();

		bool SearchCPLines(const uint, const uint = 1u); // CP skip (Line + CP together)

        // pEO-extension: in this step, the 1x1x3 line is extended to form a 1x2x3 block, while simultaneously
        // orienting 2-3 of the remaining edges and ensuring one of the oriented edges ends up in DB
        void SearchpEO(const uint);
        
        // EO-BF: this step attempts to achieve a solved edge orientation, followed by solving of the DB and DF edges. 
        void SearchEOBF(const uint);

		// As EO-BF search can last many time, can be solved in two steps instead (EO first)
		void SearchEO(const uint);
		
        // F2L: this is identical to solving a block in ZZ
		void SearchF2L(const uint);
		
		// Last Layer: there are 84 + skip possible cases for last layer called 2GLL
        void Search2GLL();

        // Search the best CP-Line solve algorithms from an algorithms vector
        void EvaluateLines(const std::vector<Algorithm>&, const uint = 1u);
		void EvaluateCPLines(const std::vector<Algorithm>&, const uint = 1u);

		// Set regrips
		void SetRegrips();
		
		// If the CP-Lines is search externally, use this function to set the CP-Lines search time
        void SetTimeCPLines(const double t) { TimeCPLines = t; }
		
		// If the Lines is search externally, use this function to set the Lines search time
        void SetTimeLines(const double t) { TimeLines = t; }

        // If the CP-Lines are search externally, use this function to set the CP-Lines search depth
        void SetDepthCPLines(const uint d) { MaxDepthCPLines = d; }

		// Get search algorithms texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
		std::string GetTextLines(const Spn sp, const uint n) const { return Lines[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextCPLines(const Spn sp, const uint n) const { return CPLines[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextpEO(const Spn sp, const uint n) const { return pEO[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextEOBF(const Spn sp, const uint n) const { return EOBF[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextEO(const Spn sp, const uint n) const { return EO[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextF2L(const Spn sp, const uint n) const { return F2L[static_cast<int>(sp)][n].ToString(); }
		std::string GetText2GLL(const Spn sp, const uint n) const { return Alg2GLL[static_cast<int>(sp)][n].ToString(); }

        // Get search algorithms lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
		uint GetLengthLines(const Spn sp, const uint n) const { return Lines[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthCPLines(const Spn sp, const uint n) const { return CPLines[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthpEO(const Spn sp, const uint n) const { return pEO[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthEOBF(const Spn sp, const uint n) const { return EOBF[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthEO(const Spn sp, const uint n) const { return EO[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthF2L(const Spn sp, const uint n) const { return F2L[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLength2GLL(const Spn sp, const uint n) const { return Alg2GLL[static_cast<int>(sp)][n].GetNumSteps(); }

		// Get metric values
        float GetMetricSolve(const Spn, const uint) const; // Get the full solve metric
		float GetMetricScramble() const { return Scramble.GetMetric(Metric); }
		float GetMetricLines(const Spn sp, const uint n) const { return Lines[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricCPLines(const Spn sp, const uint n) const { return CPLines[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricpEO(const Spn sp, const uint n) const { return pEO[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricEOBF(const Spn sp, const uint n) const { return EOBF[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricEO(const Spn sp, const uint n) const { return EO[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricF2L(const Spn sp, const uint n) const { return F2L[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetric2GLL(const Spn sp, const uint n) const { return Alg2GLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		
        // Get text for current cases
        std::string GetText2GLLCase(const Spn sp, const uint n) const { return Cases2GLL[static_cast<int>(sp)][n]; }
	
        // Get a general solve report (all spins with results)
        std::string GetReport(const bool, bool = false) const; // cancellations, debug
        std::string GetReport(const Spn, const uint) const; // Get a solve report for given spin
        std::string GetTimeReport() const; // Get a solve time report
        std::string GetBestReport(const bool = false) const; // Get the best solve report (STM with or without cancellations)

		// Get the full solve with cancellations
		Algorithm GetCancellations(const Spn, const uint) const;

		// Get the solve with cancellations metric
		float GetMetricCancellations(const Spn spin, const uint n) const { return GetCancellations(spin, n).GetMetric(Metric); }

		// Get used cores in the solve
		int GetUsedCores() const { return Cores; }

		// Get the time elapsed searching
		double GetTimeLines() const { return TimeLines; }
        double GetTimeCPLines() const { return TimeCPLines; }
        double GetTimepEO() const { return TimepEO; }
		double GetTimeEO() const { return TimeEO; }
        double GetTimeEOBF() const { return TimeEOBF; }
        double GetTimeF2L() const { return TimeF2L; }
        double GetTime2GLL() const { return Time2GLL; }
        double GetFullTime() const { return TimeLines + TimeCPLines + TimepEO + TimeEO + TimeEOBF + TimeF2L + Time2GLL; }

        // Check if in the given spin the solve is OK
        bool IsSolved(const Spn, const uint) const;

		// Check if structures are built
		static bool IsLineBuilt(const Cube &); // CP-Line (only line)
		static bool IsLineBuilt(const Cube &, const Spn); // CP-Line (only line)
		static bool IsCPBuilt(const Cube&); // Corners permutation (begginers)
		// static bool IsCPBuilt2(const Cube&); // Corners permutation (advanced - don't use, not ready!)
		static bool IsCPBuilt(const Cube&, const Spn); // Corners permutation

		// Returns best solve algorithms from the Solves vector class member and his score for the given spin
		static bool EvaluateLinesResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn); // CP not solved
		static bool EvaluateCPLinesResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn); // CP solved

		// Returns best solve for pEO extension from the Solves vector class member and his score for the given spin
		static bool EvaluatepEOResult(Algorithm&, const std::vector<Algorithm>&, const Cube&, const Spn);

	private:
		
		Algorithm Scramble; // Cube scramble
				 
		std::vector<Spn> SearchSpins;

		std::vector<Algorithm> Inspections[24], // Inspection turns before solving
							   Lines[24], // Algorithms for the lines (no CP)
							   CPLines[24], // Algorithms for the CP-Lines
							   pEO[24], // Algorithms for pEO extension
							   EO[24], // Algorithms for EO
							   EOBF[24], // Algorithms for EO / EO-BF
							   F2L[24], // F2L algorithms
							   Alg2GLL[24]; // 2GLL algorithms

		Cube CubeBase;
		
		Metrics Metric; // Metric for measures

		// Depths
        uint MaxDepthCPLines, MaxDepthpEO, MaxDepthEO, MaxDepthEOBF, MaxDepthF2L;
		
        std::vector<std::string> Cases2GLL[24]; // 2GLL cases found for each spin
                                 
		// Cores to use in the search: -1 = no multithreading, 0 = all avaliable cores, other = use this amount of cores
		int Cores;

		// Times
        double TimeLines, TimeCPLines, TimepEO, TimeEO, TimeEOBF, TimeF2L, Time2GLL;

		// Check if the solves for the given spin are consistent (all needed algorithms are present)
		bool CheckSolveConsistency(const Spn) const;
	};
}
