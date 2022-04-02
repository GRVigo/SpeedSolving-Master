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

#include <string>
#include <vector>

#include "cube_definitions.h"
#include "algorithm.h"
#include "cube.h"

namespace grcube3
{
	// Supported algorithm sets (collections)
	enum class AlgSets { _1LLL, OLL, PLL, ZBLL, OCLL, CMLL, COLL, EPLL, _2GLL, _6CO, _6CP, APDR, CDRLL, DCAL, EOLE, JTLE, L5EP, TDR };

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
		
		Algorithm& operator[](const uint cn) { return GetAlgorithm(cn); }

		void AddData(const Case& CData) { Data.push_back(CData); }
		
		void Reset() { Name.clear(); Description.clear(); Data.clear(); }
		uint GetCasesNumber() const { return static_cast<uint>(Data.size()); }
		std::string GetCaseName(const uint cp) // Return case name
		{
			if (cp < Data.size()) return Data[cp].Name;
			std::string s_Empty = "";
			return s_Empty;
		}

		// Return first algorithm for the given case index
		Algorithm& GetAlgorithm(const uint cn) { return Data[cn].Algs.front(); }

		// Load a collection of algorithms from an XML file (optionally clean y an U steps from the start/end)
		bool LoadXMLCollectionFile(const std::string&, const bool = false); 
		
		// Save a collection of algorithms to an XML file
		bool SaveXMLCollectionFile(const std::string&) const;

		// Check if two last layer algorithms are equivalent
        static bool EquivalentLLAlgorithms(const Algorithm&, const Algorithm&);

        static void ConvertTXT_To_XML(const std::string&, const std::string&); // Save XML collection file from txt file
		static void SaveVector_To_XML(const std::vector<Algorithm>&, const std::vector<std::string>&, const std::string&, const std::string&); // Save algorithm vector to XML file

		// Get number of collections cases
		static uint GetOLLCases() { return OLL_Algorithms.GetCasesNumber(); }
		static uint GetPLLCases() { return PLL_Algorithms.GetCasesNumber(); }
		static uint Get1LLLCases() { return Algorithms_1LLL.GetCasesNumber(); }
		static uint GetZBLLCases() { return ZBLL_Algorithms.GetCasesNumber(); }
		static uint GetOCLLCases() { return OCLL_Algorithms.GetCasesNumber(); }
		static uint GetCMLLCases() { return CMLL_Algorithms.GetCasesNumber(); }
		static uint GetCOLLCases() { return COLL_Algorithms.GetCasesNumber(); }
		static uint GetEPLLCases() { return EPLL_Algorithms.GetCasesNumber(); }
		static uint Get2GLLCases() { return Algorithms_2GLL.GetCasesNumber(); }
		
		static uint GetEOLECases() { return EOLE_Algorithms.GetCasesNumber(); }
		static uint Get6COCases() { return Algorithms_6CO.GetCasesNumber(); }
		static uint Get6CPCases() { return Algorithms_6CP.GetCasesNumber(); }
		static uint GetAPDRCases() { return APDR_Algorithms.GetCasesNumber(); }
		static uint GetCDRLLCases() { return CDRLL_Algorithms.GetCasesNumber(); }
		static uint GetDCALCases() { return DCAL_Algorithms.GetCasesNumber(); }
		static uint GetJTLECases() { return JTLE_Algorithms.GetCasesNumber(); }
		static uint GetL5EPCases() { return L5EP_Algorithms.GetCasesNumber(); }
		static uint GetTDRCases() { return TDR_Algorithms.GetCasesNumber(); }

		// Algorithm searchs from a collection
		static bool OrientateLL(Algorithm&, std::string&, const AlgSets, const Cube&);
		static bool SolveLL(Algorithm&, std::string&, Stp&, const AlgSets, const Cube&);
		static bool CornersLL(Algorithm&, std::string&, Stp&, const AlgSets, const Cube&);

		// Get algorithms
		static Algorithm GetEOLE(const uint n) { return n < GetEOLECases() ? EOLE_Algorithms.Data[n].Algs[0] : Algorithm(""); }
		static Algorithm Get6CO(const uint n) { return n < Get6COCases() ? Algorithms_6CO.Data[n].Algs[0] : Algorithm(""); }
		static Algorithm Get6CP(const uint n) { return n < Get6CPCases() ? Algorithms_6CP.Data[n].Algs[0] : Algorithm(""); }
		static Algorithm GetAPDR(const uint n) { return n < GetAPDRCases() ? APDR_Algorithms.Data[n].Algs[0] : Algorithm(""); }
		static Algorithm GetCDRLL(const uint n) { return n < GetCDRLLCases() ? CDRLL_Algorithms.Data[n].Algs[0] : Algorithm(""); }
		static Algorithm GetDCAL(const uint n) { return n < GetDCALCases() ? DCAL_Algorithms.Data[n].Algs[0] : Algorithm(""); }
		static Algorithm GetJTLE(const uint n) { return n < GetJTLECases() ? JTLE_Algorithms.Data[n].Algs[0] : Algorithm(""); }
		static Algorithm GetL5EP(const uint n) { return n < GetL5EPCases() ? L5EP_Algorithms.Data[n].Algs[0] : Algorithm(""); }
		static Algorithm GetTDR(const uint n) { return n < GetTDRCases() ? TDR_Algorithms.Data[n].Algs[0] : Algorithm(""); }

		// Get cases
		static std::string GetEOLECase(const uint n) { return n < GetEOLECases() ? EOLE_Algorithms.Data[n].Name : "EOLE error"; }
		static std::string Get6COCase(const uint n) { return n < Get6COCases() ? Algorithms_6CO.Data[n].Name : "6CO error"; }
		static std::string Get6CPCase(const uint n) { return n < Get6CPCases() ? Algorithms_6CP.Data[n].Name : "6CP error"; }
		static std::string GetAPDRCase(const uint n) { return n < GetAPDRCases() ? APDR_Algorithms.Data[n].Name : "APDR error"; }
		static std::string GetCDRLLCase(const uint n) { return n < GetCDRLLCases() ? CDRLL_Algorithms.Data[n].Name : "CDRLL error"; }
		static std::string GetDCALCase(const uint n) { return n < GetDCALCases() ? DCAL_Algorithms.Data[n].Name : "DCAL error"; }
		static std::string GetJTLECase(const uint n) { return n < GetJTLECases() ? JTLE_Algorithms.Data[n].Name : "JTLE error"; }
		static std::string GetL5EPCase(const uint n) { return n < GetL5EPCases() ? L5EP_Algorithms.Data[n].Name : "L5EP error"; }
		static std::string GetTDRCase(const uint n) { return n < GetTDRCases() ? TDR_Algorithms.Data[n].Name : "TDR error"; }

	private:
        std::string Name; // Algorithms collection name
        std::string Description; // Algorithms collection description
        std::vector<Case> Data; // Algorithms collection data (cases)

		static const Collection OLL_Algorithms,
			PLL_Algorithms,
			Algorithms_1LLL,
			ZBLL_Algorithms,
			OCLL_Algorithms,
			CMLL_Algorithms,
			COLL_Algorithms,
			EPLL_Algorithms,
			Algorithms_2GLL,
			Algorithms_6CO,
			Algorithms_6CP,
			APDR_Algorithms,
			CDRLL_Algorithms,
			DCAL_Algorithms,
			EOLE_Algorithms,
			JTLE_Algorithms,
			L5EP_Algorithms,
			TDR_Algorithms;
	};
}
