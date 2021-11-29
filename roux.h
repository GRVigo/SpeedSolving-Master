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
    // Class to search a solve for a Rubik's cube using Roux method
	class Roux
	{
	public:
		// Constructor with scramble
        Roux(const Algorithm& Scr, const int NumCores = 0) 
		{ 
			Scramble = Scr; 
			CubeBase = Cube(Scramble);
			Cores = NumCores;
			Reset();
		}

		// Destructor
		~Roux() {}

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
        bool SearchFirstBlock(const uint, const uint = 1u);

        void SearchSecondBlocksFirstSquare(const uint); // Search best second block first square solve with given search depth
        void SearchSecondBlocksSecondSquare(const uint); // Complete second square for second blocks with the given search depth
		
        // CMLL/CMLL search (optional parameter = true to keep the last U movement)
		void SearchCMLL(const bool = false);
		void SearchCOLL(const bool = false);

        void SearchL6EO(const uint); // Search the last six edges orientated (U & M layer movements) with the given search depth
        void SearchL6E2E(const uint); // Search UR & UL edges in the last six edges (U & M layer movements) with the given search depth
        void SearchL6E(const uint); // Search the last six edges (U & M layer movements) with the given search deep

        // Search the best first block solve algorithms from an algorithms vector
        void EvaluateFirstBlock(const std::vector<Algorithm>&, const uint = 1u);

		// Set regrips
		void SetRegrips();
		
		// If the first block is search externally, use this function to set the first block search time
        void SetTimeFB(double t) { TimeFB = t; }

        // If the first block is search externally, use this function to set the first block search depth
        void SetDepthFB(uint d) { MaxDepthFB = d; }

		// Get search algorithms texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
		std::string GetTextFirstBlock(const Spn sp, const uint n) const { return FirstBlocks[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextSecondBlockFS(const Spn sp, const uint n) const { return SecondBlocksFS[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextSecondBlockSS(const Spn sp, const uint n) const { return SecondBlocksSS[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextCMLL(const Spn sp, const uint n) const { return AlgCMLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextL6EO(const Spn sp, const uint n) const { return AlgL6EO[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextL6E2E(const Spn sp, const uint n) const { return AlgL6E2E[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextL6E(const Spn sp, const uint n) const { return AlgL6E[static_cast<int>(sp)][n].ToString(); }

        // Get search algorithms lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
		uint GetLengthFirstBlock(const Spn sp, const uint n) const { return FirstBlocks[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthSecondBlockFS(const Spn sp, const uint n) const { return SecondBlocksFS[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthSecondBlockSS(const Spn sp, const uint n) const { return SecondBlocksSS[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthSecondBlock(const Spn sp, const uint n) const { return GetLengthSecondBlockFS(sp, n) + GetLengthSecondBlockSS(sp, n); }
		uint GetLengthCMLL(const Spn sp, const uint n) const { return AlgCMLL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthL6EO(const Spn sp, const uint n) const { return AlgL6EO[static_cast<int>(sp)][n].GetNumSteps(); }
        uint GetLengthL6E2E(const Spn sp, const uint n) const { return AlgL6E2E[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthL6E(const Spn sp, const uint n) const { return AlgL6E[static_cast<int>(sp)][n].GetNumSteps(); }

		// Get metric values
        float GetMetricSolve(const Spn, const uint) const; // Get the full solve metric
		float GetMetricScramble() const { return Scramble.GetMetric(Metric); }
        float GetMetricFirstBlock(const Spn sp, const uint n) const { return FirstBlocks[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricSecondBlockFS(const Spn sp, const uint n) const { return SecondBlocksFS[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricSecondBlockSS(const Spn sp, const uint n) const { return SecondBlocksSS[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricSecondBlock(const Spn sp, const uint n) const { return GetMetricSecondBlockFS(sp, n) + GetMetricSecondBlockSS(sp, n); }
        float GetMetricCMLL(const Spn sp, const uint n) const { return AlgCMLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricCOLL(const Spn sp, const uint n) const { return AlgCOLL[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricL6EO(const Spn sp, const uint n) const { return AlgL6EO[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricL6E2E(const Spn sp, const uint n) const { return AlgL6E2E[static_cast<int>(sp)][n].GetMetric(Metric); }
        float GetMetricL6E(const Spn sp, const uint n) const { return AlgL6E[static_cast<int>(sp)][n].GetMetric(Metric); }
		
        // Get text for current cases
        std::string GetTextCMLLCase(const Spn sp, const uint n) const { return CasesCMLL[static_cast<int>(sp)][n]; }
        std::string GetTextCOLLCase(const Spn sp, const uint n) const { return CasesCOLL[static_cast<int>(sp)][n]; }
	
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
        double GetTimeFB() const { return TimeFB; }
        double GetTimeSBFS() const { return TimeSBFS; }
        double GetTimeSBSS() const { return TimeSBSS; }
		double GetTimeSB() const { return GetTimeSBFS() + GetTimeSBSS(); }
        double GetTimeCMLL() const { return TimeCMLL; }
        double GetTimeCOLL() const { return TimeCOLL; }
        double GetTimeL6EO() const { return TimeL6EO; }
        double GetTimeL6E2E() const { return TimeL6E2E; }
        double GetTimeL6E() const { return TimeL6E; }
        double GetFullTime() const { return GetTimeFB() + GetTimeSB() + GetTimeCMLL() + GetTimeCOLL() + GetTimeL6EO() + GetTimeL6E2E() + GetTimeL6E(); }

        // Check if in the given spin the solve is OK
        bool IsSolved(const Spn, const uint) const;

		// Check if the first Roux blocks are built
		static bool IsFBBuilt(const Cube &); // First block (left)
		static bool IsFBBuilt(const Cube&, const Spn); // First block with given spin
		static bool IsSBBuilt(const Cube&); // Second block (right)
		static bool IsFBAndSBSquareBuilt(const Cube&);
		static bool AreFirstBlocksBuilt(const Cube& c) { return IsFBBuilt(c) && IsSBBuilt(c); }
		static bool IsCMLLBuilt(const Cube&);
		static bool IsCOLLBuilt(const Cube& c) { return IsCMLLBuilt(c); }
		static bool IsL6EOriented(const Cube& c) // Check if the last six edges are oriented in roux method
		{
			return AreFirstBlocksBuilt(c) &&
				   c.IsFaceOriented2(Cube::GetUpSliceLayer(c.GetSpin())) && 
				   c.IsFaceOriented2(Cube::GetDownSliceLayer(c.GetSpin()));
		}
        static bool IsL6EO2E(const Cube& c) // Check if the last six edges oriented and two edges solved in roux method
		{
			return c.IsSolved(Cube::GetLeftSliceLayer(c.GetSpin())) && c.IsSolved(Cube::GetRightSliceLayer(c.GetSpin())) &&
				   c.IsFaceOriented2(Cube::GetUpSliceLayer(c.GetSpin())) && c.IsFaceOriented2(Cube::GetDownSliceLayer(c.GetSpin()));
		}

		// Returns best solve algorithm from the Solves vector class member and his score for the given spin
		// F2L pieces are used as evaluation condition for the Roux first blocks (first block in the left side)
		static bool EvaluateFBResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);

	private:
		
		Algorithm Scramble; // Cube scramble
				 
		std::vector<Spn> SearchSpins;

		std::vector<Algorithm> Inspections[24], // Inspection turns before solving
							   FirstBlocks[24], // Algorithms for the left block (Roux first block)
							   SecondBlocksFS[24], // Algorithms for the right block first square (Roux second block)
							   SecondBlocksSS[24], // Algorithms for the right block second square (Roux second block)
							   AlgCMLL[24], // CMLL algorithms
							   AlgCOLL[24], // COLL algorithms
                               AlgL6EO[24], // Last six edges orientation algorithms
                               AlgL6E2E[24], // UR & UL edges for last six edges
							   AlgL6E[24]; // Last six edges final algorithms

		Cube CubeBase;
		
		Metrics Metric; // Metric for measures

		// Last used maximum first blocks deep
        uint MaxDepthFB, MaxDepthSBFS, MaxDepthSBSS, MaxDepthL6EO, MaxDepthL6E2E, MaxDepthL6E;
		
        std::vector<std::string> CasesCMLL[24], // CMLL cases found for each spin
                                 CasesCOLL[24]; // COLL cases found for each spin
				
		// Cores to use in the search: -1 = no multithreading, 0 = all avaliable cores, other = use this amount of cores
		int Cores;

		// Times
        double TimeFB, TimeSBFS, TimeSBSS, TimeCMLL, TimeCOLL, TimeL6EO, TimeL6E2E, TimeL6E;

		// Check if the solves for the given spin are consistent (all needed algorithms are present)
		bool CheckSolveConsistency(const Spn) const;
	};
}
