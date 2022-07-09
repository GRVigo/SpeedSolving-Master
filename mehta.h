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
    // Class to search a solve for a Rubik's cube using Mehta method
	class Mehta : public Method
	{
	public:
		// Constructor
        Mehta(const Algorithm& Scr, const int NumCores = 0) : Method(Scr, NumCores) { Reset(); }

		// Reset the search results
		void Reset();

        // Solve a 1x2x3 block on the D layer, with 1x1x3 of the block in the DL position
		// Return false if no first block found
        bool SearchFB(const uint, const uint = 1u);
        void Search3QB(const uint); // Search 3 Quarters Belt - Solve 3 E-slice edges relative to the centers
		void SearchEOLE(const Plc = Plc::FIRST); // Edge Orientation + Last Edge - Insert the remaining E-slice edge while orienting all the edges
		void Search6CO(const Plc = Plc::FIRST); // Orient the 6 remaining corners
        void Search6CP(const Plc = Plc::FIRST); // Permute the 6 remaining corners
        void SearchL5EP(const Plc = Plc::FIRST); // Solve the cube by permuting the last 5 edges
        void SearchAPDR(const Plc = Plc::FIRST); // Solve the DR block
		void SearchPLL(const Plc = Plc::FIRST); // Solve the last layer
		void SearchDCAL(const Plc = Plc::FIRST); // Solve the 2 corners of the D layer
		void SearchCDRLL(const Plc = Plc::FIRST); // Orient and permute the U layer corners (like COLL)
		void SearchJTLE(const Plc = Plc::FIRST); // Orient the U layer corners while inserting the DR edge
		void SearchTDR(const Plc = Plc::FIRST); // Solve the DR block
		void SearchZBLL(const Plc = Plc::FIRST); // Solve last layer

        // Search the best first block solve algorithms from an algorithms vector
        void EvaluateFB(const std::vector<Algorithm>&, const uint = 1u);

		// Set regrips
		void SetRegrips();
		
		// If the first block is search externally, use this function to set the first block search time
        void SetTimeFS(double t) { TimeFB = t; }

        // If the first block is search externally, use this function to set the first block search depth
        void SetDepthFS(uint d) { MaxDepthFB = d; }

		Algorithm GetFullSolve(const Spn, const uint) const; // Get the full solve

		// Get search algorithms texts
		std::string GetTextFB(const Spn sp, const uint n) const { return AlgFB[static_cast<int>(sp)][n].ToString(); }
		std::string GetText3QB(const Spn sp, const uint n) const { return Alg3QB[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextEOLE(const Spn sp, const uint n) const { return AlgEOLE[static_cast<int>(sp)][n].ToString(); }
		std::string GetText6CO(const Spn sp, const uint n) const { return Alg6CO[static_cast<int>(sp)][n].ToString(); }
		std::string GetText6CP(const Spn sp, const uint n) const { return Alg6CP[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextL5EP(const Spn sp, const uint n) const { return AlgL5EP[static_cast<int>(sp)][n].ToString(); }
        std::string GetTextAPDR(const Spn sp, const uint n) const { return AlgAPDR[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextDCAL(const Spn sp, const uint n) const { return AlgDCAL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextCDRLL(const Spn sp, const uint n) const { return AlgCDRLL[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextJTLE(const Spn sp, const uint n) const { return AlgJTLE[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextTDR(const Spn sp, const uint n) const { return AlgTDR[static_cast<int>(sp)][n].ToString(); }
		std::string GetTextZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].ToString(); }

        // Get search algorithms lengths
		uint GetLengthFB(const Spn sp, const uint n) const { return AlgFB[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLength3QB(const Spn sp, const uint n) const { return Alg3QB[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthEOLE(const Spn sp, const uint n) const { return AlgEOLE[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLength6CO(const Spn sp, const uint n) const { return Alg6CO[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLength6CP(const Spn sp, const uint n) const { return Alg6CP[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthL5EP(const Spn sp, const uint n) const { return AlgL5EP[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthAPDR(const Spn sp, const uint n) const { return AlgAPDR[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthDCAL(const Spn sp, const uint n) const { return AlgDCAL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthCDRLL(const Spn sp, const uint n) const { return AlgCDRLL[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthJTLE(const Spn sp, const uint n) const { return AlgJTLE[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthTDR(const Spn sp, const uint n) const { return AlgTDR[static_cast<int>(sp)][n].GetNumSteps(); }
		uint GetLengthZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetNumSteps(); }

		// Get metric values
		float GetMetricFB(const Spn sp, const uint n) const { return AlgFB[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetric3QB(const Spn sp, const uint n) const { return Alg3QB[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricEOLE(const Spn sp, const uint n) const { return AlgEOLE[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetric6CO(const Spn sp, const uint n) const { return Alg6CO[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetric6CP(const Spn sp, const uint n) const { return Alg6CP[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricL5EP(const Spn sp, const uint n) const { return AlgL5EP[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricAPDR(const Spn sp, const uint n) const { return AlgAPDR[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricPLL(const Spn sp, const uint n) const { return AlgPLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricDCAL(const Spn sp, const uint n) const { return AlgDCAL[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricCDRLL(const Spn sp, const uint n) const { return AlgCDRLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricJTLE(const Spn sp, const uint n) const { return AlgJTLE[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricTDR(const Spn sp, const uint n) const { return AlgTDR[static_cast<int>(sp)][n].GetMetric(Metric); }
		float GetMetricZBLL(const Spn sp, const uint n) const { return AlgZBLL[static_cast<int>(sp)][n].GetMetric(Metric); }
		
        // Get text for current cases
        std::string GetTextEOLECase(const Spn sp, const uint n) const { return CasesEOLE[static_cast<int>(sp)][n]; }
        std::string GetText6COCase(const Spn sp, const uint n) const { return Cases6CO[static_cast<int>(sp)][n]; }
		std::string GetText6CPCase(const Spn sp, const uint n) const { return Cases6CP[static_cast<int>(sp)][n]; }
		std::string GetTextL5EPCase(const Spn sp, const uint n) const { return CasesL5EP[static_cast<int>(sp)][n]; }
		std::string GetTextAPDRCase(const Spn sp, const uint n) const { return CasesAPDR[static_cast<int>(sp)][n]; }
		std::string GetTextPLLCase(const Spn sp, const uint n) const { return CasesPLL[static_cast<int>(sp)][n]; }
		std::string GetTextDCALCase(const Spn sp, const uint n) const { return CasesDCAL[static_cast<int>(sp)][n]; }
		std::string GetTextCDRLLCase(const Spn sp, const uint n) const { return CasesCDRLL[static_cast<int>(sp)][n]; }
		std::string GetTextJTLECase(const Spn sp, const uint n) const { return CasesJTLE[static_cast<int>(sp)][n]; }
		std::string GetTextTDRCase(const Spn sp, const uint n) const { return CasesTDR[static_cast<int>(sp)][n]; }
		std::string GetTextZBLLCase(const Spn sp, const uint n) const { return CasesZBLL[static_cast<int>(sp)][n]; }
	
        // Get a general solve report (all spins with results)
        std::string GetReport(const bool, bool = false) const; // cancellations, debug
        std::string GetReport(const Spn, const uint) const; // Get a solve report for given spin
        std::string GetTimeReport() const; // Get a solve time report

		// Get the time elapsed searching
        double GetTimeFB() const { return TimeFB; }
        double GetTime3QB() const { return Time3QB; }
        double GetTimeEOLE() const { return TimeEOLE; }
		double GetTime6CO() const { return Time6CO;  }
        double GetTime6CP() const { return Time6CP; }
        double GetTimeL5EP() const { return TimeL5EP; }
        double GetTimeAPDR() const { return TimeAPDR; }
        double GetTimePLL() const { return TimePLL; }
        double GetTimeDCAL() const { return TimeDCAL; }
		double GetTimeCDRLL() const { return TimeCDRLL; }
		double GetTimeJTLE() const { return TimeJTLE; }
		double GetTimeTDR() const { return TimeTDR; }
		double GetTimeZBLL() const { return TimeZBLL; }
		double GetFullTime() const { return GetTimeFB() + GetTime3QB() + GetTimeEOLE() + GetTime6CO() + GetTime6CP() +
			                                GetTimeL5EP() + GetTimeAPDR() + GetTimePLL() + GetTimeCDRLL() + 
			                                GetTimeJTLE() + GetTimeTDR() + GetTimeZBLL(); }

	private:

		std::vector<Algorithm> AlgFB[24],       // First block
							   Alg3QB[24],      // 3 Quarters Belt
							   AlgEOLE[24],     // Edge Orientation + Last Edge
							   Alg6CO[24],      // Orient the 6 remaining corners
							   Alg6CP[24],      // Permute the 6 remaining corners
							   AlgL5EP[24],     // Solve the cube by permuting the last 5 edges
							   AlgAPDR[24],     // Solve the DR block
							   AlgPLL[24],      // Solve the top layer
							   AlgDCAL[24],     // Solve the 2 corners of the D layer
							   AlgCDRLL[24],    // Orient and permute the U layer corners (like COLL)
							   AlgJTLE[24],     // Orient the U layer corners while inserting the DR edge
							   AlgTDR[24],      // Solve the DR block
							   AlgZBLL[24];     // Solve last layer

		// Last used maximum first blocks and belt depths
		uint MaxDepthFB, MaxDepth3QB;
		
		// Cases
		std::vector<std::string> CasesEOLE[24], Cases6CO[24], Cases6CP[24], CasesL5EP[24], CasesAPDR[24], CasesPLL[24],
								 CasesDCAL[24], CasesCDRLL[24], CasesJTLE[24], CasesTDR[24], CasesZBLL[24];

		// Times
        double TimeFB, Time3QB, TimeEOLE, Time6CO, Time6CP, TimeL5EP, TimeAPDR, TimePLL, TimeDCAL, TimeCDRLL, TimeJTLE, TimeTDR, TimeZBLL;

		// Check if the solves for the given spin are consistent (all needed algorithms are present)
		bool CheckSolveConsistency(const Spn) const;
	};
}
