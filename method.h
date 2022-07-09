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
#include "collection.h"

namespace grcube3
{
    // Base class to develop a method to solve the Rubik's cube
    class Method
    {
    public:
        // Constructor with scramble
        Method(const Algorithm& Scr, const int NumCores = 0)
        {
            Scramble = Scr;
            CubeBase = Cube(Scramble);
            Cores = NumCores;
        }

		virtual ~Method() { } // Virtual destructor to avoid warnings

		// Reset the search results
		virtual void Reset() = 0;

        // Set spins for search
		void SetSearchSpins(const std::vector<Spn>&);
		
		// Set the metric for evaluations
		void SetMetric(const Metrics m) { Metric = m; }

        // Set regrips
        virtual void SetRegrips() = 0;
		
		// If the first step is search externally, use this function to set the first step search time
        virtual void SetTimeFS(double) = 0;

        // If the first step is search externally, use this function to set the first step search depth
        virtual void SetDepthFS(uint) = 0;
		
		static uint* GetMaxDepthPointer() { return DS.GetMaxDepthPointer(); } // Get the static deep search maximum depth pointer

		// Get search algorithms texts
        std::string GetTextScramble() const { return Scramble.ToString(); }
		std::string GetTextInspection(const Spn sp, const uint n) const { return Inspections[static_cast<int>(sp)][n].ToString(); }

        // Get search solves lengths
        uint GetLengthScramble() const { return Scramble.GetNumSteps(); }
		uint GetLengthInspection(const Spn sp, const uint n) const { return Inspections[static_cast<int>(sp)][n].GetNumSteps(); }

        // Get metric values
		float GetMetricSolve(const Spn sp, const uint n) const { return GetFullSolve(sp, n).GetMetric(Metric); } // Get the full solve metric
		float GetMetricScramble() const { return Scramble.GetMetric(Metric); }
		Metrics GetMetric() const { return Metric; }

        // Get a general solve report (all spins with results)
        virtual std::string GetReport(const bool, bool = false) const = 0; // cancellations, debug
		virtual std::string GetReport(const Spn, const uint n) const = 0; // Get a solve report for given spin
		virtual std::string GetTimeReport() const = 0; // Get a solve time report
		std::string GetBestReport(const bool = false) const; // Get the best solve report (metric with or without cancellations)

		// Get the full solve
		virtual Algorithm GetFullSolve(const Spn, const uint) const = 0;

        // Get the full solve with cancellations
		Algorithm GetCancellations(const Spn sp, const uint n) const { return GetFullSolve(sp, n).GetCancellations(); }

        // Get the solve with cancellations metric
		float GetMetricCancellations(const Spn spin, const uint n) const { return GetCancellations(spin, n).GetMetric(Metric); }

		// Get used cores in the solve
		int GetUsedCores() const { return Cores; }

        // Check if in the given spin the solve is OK
		bool IsSolved(const Spn sp, const uint n) const { Cube C = CubeBase; C.ApplyAlgorithm(GetFullSolve(sp, n)); return C.IsSolved(); }

		// Check if the CFOP first cross is built
		static bool IsCrossBuilt(const Cube&, const Lyr); 
		static bool IsCrossBuilt(const Cube&, const Spn);
		static bool IsCrossBuilt(const Cube&);
		
		// Check if the CFOP first cross is built + first F2L pair
        static bool IsXCrossBuilt(const Cube&, const Lyr); 
		static bool IsXCrossBuilt(const Cube&, const Spn);
		static bool IsXCrossBuilt(const Cube&);
		
		// Check if all F2L pieces (two layers) are built
        static bool IsF2LBuilt(const Cube&, const Lyr); 
		static bool IsF2LBuilt(const Cube&, const Spn);
		static bool IsF2LBuilt(const Cube&);

		// Check if all F2L pairs are built
        static bool IsF2LPairsBuilt(const Cube&, const Lyr); 
		static bool IsF2LPairsBuilt(const Cube&, const Spn);
		static bool IsF2LPairsBuilt(const Cube&);

		// Check if the last layer is oriented (OLL search completed)
		static bool IsLastLayerOriented(const Cube& C) { return C.IsFaceOriented(Cube::GetUpSliceLayer(C.GetSpin())); }

		// Check if the given F2L pair is adjacent (the corner next to the edge), orientation is not checked
		static bool IsF2LAdjacent(const Pgr, const Cube&);

		// Check if the common colors for F2L pair are in the same face
		static bool IsF2LOriented(const Pgr, const Cube&);

		// Check if the first Roux/Nautilus blocks are built
		static bool IsRouxFBBuilt(const Cube&); // Roux/Nautilus first block (3x2x1)
		static bool IsRouxFBBuilt(const Cube&, const Spn); // Roux/Nautilus first block (3x2x1) with given spin

		// Check if the Roux structures are built
		static bool IsRouxFBSquareBuilt(const Cube&); // First block square
		static bool IsRouxFBSquareBuilt(const Cube&, const Spn); // First block square with given spin
		static bool IsRouxSBBuilt(const Cube&); // Second block (right)
		static bool IsRouxSBBuilt(const Cube&, const Spn); // Second block (right) with the given spin
		static bool IsRouxFBAndSBSquareBuilt(const Cube&);
		static bool IsRouxFBAndSBSquareBuilt(const Cube&, const Spn);
		static bool AreRouxBlocksBuilt(const Cube& c) { return IsRouxFBBuilt(c) && IsRouxSBBuilt(c); }
		static bool IsCMLLBuilt(const Cube&);
		static bool IsCMLLBuilt(const Cube&, const Spn);
		static bool IsCOLLBuilt(const Cube& c) { return IsCMLLBuilt(c); }
		static bool IsCOLLBuilt(const Cube& c, const Spn s) { return IsCMLLBuilt(c, s); }
		static bool IsRouxL6EOriented(const Cube&); // Check if the last six edges are oriented in roux method
		static bool IsRouxL6EOriented(const Cube&, const Spn); // Check if the last six edges are oriented in roux method with given spin
		static bool IsRouxL6EO2E(const Cube&); // Check if the last six edges oriented and two edges solved in roux method
		static bool IsRouxL6EO2E(const Cube&, const Spn); // Check if the last six edges oriented and two edges solved in roux method with the given spin
		
		// Check if Petrus structures are built
		static bool IsPetrusBlockBuilt(const Cube&); // Check if Petrus block is built
		static bool IsPetrusBlockBuilt(const Cube&, const Spn); // Check if Petrus block is built for given spin
		static bool IsPetrusExpandedBlockBuilt(const Cube&); // Check if Petrus expanded block is built
		static bool IsPetrusExpandedBlockBuilt(const Cube&, const Spn);// Check if Petrus expanded block is built for given spin

        // Check if ZZ structures are built
        static bool IsEOLineBuilt(const Cube&, const Spn);
		static bool IsEOLineBuilt(const Cube&);
        static bool IsEOArrowBuilt(const Cube&, const Spn);
		static bool IsEOArrowBuilt(const Cube&);
        static bool IsEOCrossBuilt(const Cube&, const Spn);
		static bool IsEOCrossBuilt(const Cube&);
        static bool IsXEOLineBuilt(const Cube&, const Spn);
		static bool IsXEOLineBuilt(const Cube&);
        static bool IsXEOCrossBuilt(const Cube&, const Spn);
		static bool IsXEOCrossBuilt(const Cube&);
        static bool IsEO223Built(const Cube&, const Spn);
		static bool IsEO223Built(const Cube&);

		// Check if CEOR structures are built
		static bool IsYruRULineBuilt(const Cube&); // CP-Line (only line)
		static bool IsYruRULineBuilt(const Cube&, const Spn); // CP-Line (only line)
		static bool IsYruRUCPBuilt(const Cube&); // Corners permutation (begginers)
		static bool IsYruRUCPBuilt2(const Cube&); // Corners permutation (advanced - don't use, not ready!)
		static bool IsYruRUCPBuilt(const Cube&, const Spn); // Corners permutation

		// Check if Mehta structures are built
		static bool IsMehtaFBBuilt(const Cube&); // Check if the first Mehta blocks are built
		static bool IsMehtaFBBuilt(const Cube&, const Spn); // First block with given spin
		static bool IsMehta3QBBuilt(const Cube&);
		static bool IsMehta3QBBuilt(const Cube&, const Spn); // 3QB with given spin
		static bool IsMehtaEOLEBuilt(const Cube&);
		static bool IsMehtaEOLEBuilt(const Cube&, const Spn); // EOLE with given spin
		static bool IsMehta6COBuilt(const Cube&);
		static bool IsMehta6COBuilt(const Cube&, const Spn); // 6CO with given spin
		static bool IsMehta6CPBuilt(const Cube&);
		static bool IsMehta6CPBuilt(const Cube&, const Spn); // 6CP with given spin
		static bool IsMehtaAPDRBuilt(const Cube&);
		static bool IsMehtaAPDRBuilt(const Cube&, const Spn); // APDR with given spin
		static bool IsMehtaDCALBuilt(const Cube&);
		static bool IsMehtaDCALBuilt(const Cube&, const Spn); // DCAL with given spin
		static bool IsMehtaCDRLLBuilt(const Cube&);
		static bool IsMehtaCDRLLBuilt(const Cube&, const Spn); // CDRLL with given spin
		static bool IsMehtaJTLEBuilt(const Cube&);
		static bool IsMehtaJTLEBuilt(const Cube&, const Spn); // JTLE with given spin
		static bool IsMehtaTDRBuilt(const Cube&);
		static bool IsMehtaTDRBuilt(const Cube&, const Spn); // TDR with given spin

		// Check if the second Nautilus blocks are built
		static bool IsNautilusSBSBuilt(const Cube&); // Second block square (1x2x2)
		static bool IsNautilusSBSBuilt(const Cube&, const Spn); // Second block square with given spin
		static bool IsNautilusSBBuilt(const Cube&); // Second block (2x2x2)
		static bool IsNautilusSBBuilt(const Cube&, const Spn); // Second block with given spin

		static bool IsNautilusNCLLBuilt(const Cube&); // NCLL
		static bool IsNautilusNCLLBuilt(const Cube&, const Spn); // NCLL with given spin

		static bool IsNautilusEODFBuilt(const Cube&); // EODF
		static bool IsNautilusEODFBuilt(const Cube&, const Spn); // EODF with given spin

		// Solves evaluation

		// Returns best CFOP F2L solve from the Solves vector class member
		static bool EvaluateCFOPF2LResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Lyr, const Plc, const bool = true);
		
		// Returns best solve algorithm from the Solves vector class member and his score for the given spin
		// F2L pieces are used as evaluation condition for the Roux first blocks (first block in the left side)
		static bool EvaluateRouxFBResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);

		// Returns best block solves from the Solves vector class member for the given corner position block (Petrus)
		static bool EvaluatePetrusBlockResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);

        // Returns best ZZ EOX/F2L solve from the Solves vector class member and his score
        static bool EvaluateZZEOXResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);
        static bool EvaluateZZF2LResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);

		// Returns best solve algorithms from the Solves vector class member and his score for the given spin
		static bool EvaluateYruRULinesResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn); // CP not solved
		static bool EvaluateYruRUCPLinesResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn); // CP solved
		// Returns best solve for pEO extension from the Solves vector class member and his score for the given spin
		static bool EvaluateCEORpEOResult(Algorithm&, const std::vector<Algorithm>&, const Cube&, const Spn);

		// Returns best Mehta solve algorithm from the Solves vector class member and his score for the given spin
		static bool EvaluateMehtaFBResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);
		static bool EvaluateMehta3QBResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);

		// Returns best solve algorithm from the Solves vector class member and his score for the given spin
		// Second block pieces are used as evaluation condition for the Nautilus first blocks (first block in the left side)
		static bool EvaluateNautilusFBResult(std::vector<Algorithm>&, const uint, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc);
		static bool EvaluateNautilusSBResult(Algorithm&, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc); // Full second block
		static bool EvaluateNautilusSBSResult(Algorithm&, const std::vector<Algorithm>&, const Cube&, const Spn, const Plc); // Second block square

		// Algorithm searchs from a collection
		static bool OrientateLL(Algorithm&, std::string&, const Collection&, const Plc, const Metrics, const Cube&);
		static bool SolveLL(Algorithm&, std::string&, Stp&, const Collection&, const Plc, const Metrics, const Cube&);
		static bool CornersLL(Algorithm&, std::string&, Stp&, const Collection&, const Plc, const Metrics, const Cube&, const bool = false);

		// Supported algsets
		static const Collection Algset_OLL,
								Algset_PLL,
								Algset_1LLL,
								Algset_ZBLL,
								Algset_OCLL,
								Algset_CMLL,
								Algset_COLL,
								Algset_EPLL,
								Algset_2GLL,
								Algset_6CO,
								Algset_6CP,
								Algset_APDR,
								Algset_CDRLL,
								Algset_DCAL,
								Algset_EOLE,
								Algset_JTLE,
								Algset_L5EP,
								Algset_TDR,
								Algset_NCLL,
								Algset_NCOLL,
								Algset_TNCLL,
								Algset_L5E,
								Algset_EODF;
	protected:

        Algorithm Scramble; // Cube scramble
		Cube CubeBase; // Cube with current scramble
        std::vector<Spn> SearchSpins; // Spins to search
        std::vector<Algorithm> Inspections[24]; // Algorithms for inspections
		Metrics Metric; // Metric for measures
       
		// Cores to use in the search: 
		//    -1 = no multithreading
		//    0 = all avaliable cores
		//    other = use this amount of cores
		int Cores;
		
		static DeepSearch DS; // Deep search for methods

		// Check if the solves for the given spin are consistent (all needed algorithms are present)
		virtual bool CheckSolveConsistency(const Spn) const = 0;
	};
}
