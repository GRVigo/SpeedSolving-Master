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

namespace grcube3
{
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
        Collection(const std::string& s) { LoadXMLCollectionFile(s, true); } // Initialize algorithms collection
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
		bool SaveXMLCollectionFile(const std::string&);

		// Check if two last layer algorithms are equivalent
        static bool EquivalentLLAlgorithms(const Algorithm&, const Algorithm&);

        static void ConvertTXT_To_XML(const std::string&, const std::string&); // Save XML collection file from txt file
		static void SaveVector_To_XML(const std::vector<Algorithm>&, const std::vector<std::string>&, const std::string&, const std::string&); // Save algorithm vector to XML file
	private:
        std::string Name; // Algorithms collection name
        std::string Description; // Algorithms collection description
        std::vector<Case> Data; // Algorithms collection data (cases)
	};
}
