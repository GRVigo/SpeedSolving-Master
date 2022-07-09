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

#include <string>
#include <vector>

#include "cube_definitions.h"
#include "algorithm.h"

namespace grcube3
{
	// Algorithm audit probes
	enum class Probe
	{
		NONE,
		LL_SOLVE, // Solve the last layer
		LL_ORIENT, // Orient the last layer
		CMLL, // Solves the LL corners while preserving the First Two Blocks made in Roux
		COLL, // Solves (orients and permutes) the last layer corners while preserving the last layer edges orientation
		NCLL, // Solves Nautilus NCLL
		NCOLL, // Solves Nautilus NCOLL
		TNCLL, // Solves Nautilus TNCLL
		L5E, // Solves last five edges
		L5EP, // Permute last five edges (already oriented)
	};
	
    struct Case // Case data structure for collections
    {
        std::string Name; // Case name
        std::vector<Algorithm> Algs; // Algorithms that solve the case
		void Reset() { Name.clear(); Algs.clear(); }
    };

    class Collection // Algorithms collection class
	{
	public:
		Collection() { }
        Collection(const std::string& s, bool clean) { LoadXMLCollectionFile(s, clean); } // Initialize algorithms collection
        ~Collection() { } // Destructor
		
		Algorithm& operator[](const uint cn) { return Cases[cn].Algs.front(); }

		void AddData(const Case& CData) { Cases.push_back(CData); }
		
		void Reset() { Name.clear(); Description.clear(); Cases.clear(); }

		std::string GetName() const { return Name; }
		
		std::string GetDescription() const { return Description; }

		uint GetCasesNumber() const { return static_cast<uint>(Cases.size()); }
		
		std::string GetCaseName(const uint cp) const { return cp < Cases.size() ? Cases[cp].Name : ""; }

        uint GetAlgorithmsNumber(const uint cp) const { return cp < Cases.size() ? static_cast<uint>(Cases[cp].Algs.size()) : 0u; }

        Algorithm GetAlgorithm(const uint cp, const uint ap) const { return cp < Cases.size() ? ap < Cases[cp].Algs.size() ? Cases[cp].Algs[ap] : "" : ""; }

        bool SetAlgorithm(const uint cp, const uint ap, const Algorithm alg)
        {
            if (cp >= Cases.size() || ap >= Cases[cp].Algs.size()) return false;
            Cases[cp].Algs[ap] = alg;
            return true;
        }

        bool AddAlgorithm(const uint cp, const Algorithm alg)
        {
            if (cp >= Cases.size()) return false;
            for (const auto& a : Cases[cp].Algs) if (a == alg) return true; // Algortihm already present in collection
            Cases[cp].Algs.push_back(alg);
            return true;
        }

		// Return required algorithm for the given case index
		Algorithm GetAlgorithm(const uint cn, const Plc = Plc::FIRST, const Metrics = Metrics::Movements) const;

		// Returns the same algorithm transformed by y turns for getting most comfortable (subjective) movements
		Algorithm GetSubjectiveBesty(const Algorithm&) const;

		// Load a collection of algorithms from an XML file (optionally clean y an U steps from the start/end)
		bool LoadXMLCollectionFile(const std::string&, const bool = false); 
		
		// Save a collection of algorithms to an XML file
		bool SaveXMLCollectionFile(const std::string&) const;

		// Save txt file from current collection
		void Save_To_TXT(const std::string&) const;

		// Order by length the algorithms for each case
		void OrderAlgorithms();

		// Merge a collection with the current collection
		void MergeCollection(const Collection&, const Probe);

		// Merge a collection with the list of algorithms from a text file
		void MergeTXTFile(const std::string&, const Probe, const bool = false);
		
		// Audit a collection of algorithms, audit results to a TXT file
		void Audit(const Probe, const std::string&) const;
		
		// Check the integrity of the two first layers by the given algorithm
		static bool CheckF2LIntegrity(const Algorithm&);

		// Check the integrity of the two first Roux blocks by the given algorithm
		static bool CheckRouxF2BIntegrity(const Algorithm&);

		// Check the integrity of F2L except DF edge (Nautilus)
		static bool CheckF2LDFIntegrity(const Algorithm&);

		// Check the integrity of F2L except DR edge
		static bool CheckF2LDRIntegrity(const Algorithm&);

		// Check the integrity of F2L except DL edge
		static bool CheckF2LDLIntegrity(const Algorithm&);

		// Check the integrity of F2L except DR edge
		static bool CheckF2LDBIntegrity(const Algorithm&);

		// Check the integrity of F2L except DF edge & DFR corner orientation (Nautilus)
		static bool CheckF2LDFDFRIntegrity(const Algorithm&);

		// Check if two last layer solve algorithms are equivalent
        static bool CheckLLSolveAlgorithms(const Algorithm&, const Algorithm&);
		
		// Check if two last layer orientation algorithms are equivalent
        static bool CheckLLOrientAlgorithms(const Algorithm&, const Algorithm&);

		// Check if two last layer corners solve algorithms are equivalent
		static bool CheckLLCornersAlgorithms(const Algorithm&, const Algorithm&);

        static void ConvertTXT_To_XML(const std::string&, const std::string&); // Save XML collection file from txt file
		static void SaveVector_To_XML(const std::vector<Algorithm>&, const std::vector<std::string>&, const std::string&, const std::string&); // Save algorithm vector to XML file

	private:
        std::string Name; // Algorithms collection name
        std::string Description; // Algorithms collection description
        std::vector<Case> Cases; // Algorithms collection cases
	};
}
