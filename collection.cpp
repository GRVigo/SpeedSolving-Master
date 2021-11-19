/*  This file is part of "GR Cube"

	Copyright (C) 2020 German Ramos Rodriguez

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
	<mailto:grvigo@hotmail.com>
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#include "collection.h"
#include "cube.h"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace grcube3
{
	// Load a collection of algorithms from an XML file
    bool Collection::LoadXMLCollectionFile(const std::string& XMLFilename, const bool clean)
    {
        Reset(); // Clear current collection

        XMLDocument col_xml;

        if (col_xml.LoadFile(XMLFilename.c_str()) != XML_SUCCESS) return false; // Read existing file

        XMLNode* pRoot = col_xml.FirstChild();
        if (pRoot != nullptr)
        {            
            XMLElement* pElementRoot = pRoot->ToElement();
            const char* s_Name;
            s_Name = pElementRoot->Attribute("Name");
            Name = s_Name;

            const char* s_Description = pElementRoot->Attribute("Description");
            Description = s_Description;

            XMLElement* pElementCase = pRoot->FirstChildElement("Case");
            while (pElementCase != nullptr)
            {
                Case AuxCase;

                const char* s_CaseName = pElementCase->GetText();
                AuxCase.Name = s_CaseName;

                XMLElement* pElementAlg = pElementCase->FirstChildElement("Algorithm");
                while (pElementAlg != nullptr)
                {
                    const char* s_Algorithm = pElementAlg->GetText();
                    Algorithm A;
                    if (s_Algorithm != NULL)
                    {
                        const Algorithm Aux(s_Algorithm);
						if(clean)
						{
							A = Aux.GetDeveloped();
							while (A.GetSize() > 2u && (Algorithm::IsyTurn(A.First()) || Algorithm::IsUMov(A.First()))) A.EraseFirst();
							while (A.GetSize() > 2u && (Algorithm::IsyTurn(A.Last()) || Algorithm::IsUMov(A.Last()))) A.EraseLast();
						}
						else A.Append(Aux);
                    }
                    AuxCase.Algs.push_back(A);
                    pElementAlg = pElementAlg->NextSiblingElement("Algorithm");
                }
				
				Data.push_back(AuxCase);
 
                pElementCase = pElementCase->NextSiblingElement("Case");
            }
        }
        return !Data.empty();
    }

    // Save a collection of algorithms to an XML file
    bool Collection::SaveXMLCollectionFile(const std::string& XMLFilename)
    {
        XMLDocument col_xml;

        XMLNode* pCollection = col_xml.NewElement("Collection"); // Collection root node
        XMLElement* pElementRoot = pCollection->ToElement();
        pElementRoot->SetAttribute("Name", Name.c_str());
        pElementRoot->SetAttribute("Description", "Algorithms collection");
        pElementRoot->SetAttribute("CasesNumber", std::to_string(Data.size()).c_str());
        col_xml.InsertFirstChild(pCollection);

        uint n_case = 0u;
        for (const auto& Case : Data)
        {
            XMLElement* pElementCase = col_xml.NewElement("Case");
            pElementCase->SetText(Case.Name.c_str());
            pElementCase->SetAttribute("Index", std::to_string(n_case).c_str());

            for (const auto& Alg : Case.Algs)
            {
                XMLElement* pElementAlg = col_xml.NewElement("Algorithm");
                pElementAlg->SetText(Alg.ToString().c_str());
                pElementCase->InsertEndChild(pElementAlg);
            }

            pCollection->InsertEndChild(pElementCase);
            n_case++;
        }

        return col_xml.SaveFile(XMLFilename.c_str()) == XML_SUCCESS;
    }

    // Check if two last layer algorithms are equivalent
    bool Collection::EquivalentLLAlgorithms(const Algorithm& A, const Algorithm& B)
    {
        const Cube C(A);
        Algorithm Aux = B;
        if (!Aux.TransformReverseInvert()) return false;

        for (const auto UMov1 : Algorithm::UMovs)
        {
            for (const auto UMov2 : Algorithm::UMovs)
            {
                Cube CubeAux = C;
                CubeAux.ApplyStep(UMov1);
                CubeAux.ApplyAlgorithm(Aux);
                CubeAux.ApplyStep(UMov2);
                if (CubeAux.IsSolved()) return true;
            }
        }
        return false;
    }

    // Save XML collection file from TXT file (copy & paste from algdb.com)
    void Collection::ConvertTXT_To_XML(const std::string& TXTFilename, const std::string& XMLFilename)
    {
        std::ifstream f_txt;
        f_txt.open(TXTFilename.c_str());

        if (!f_txt.is_open()) return;

        std::string ColName;
        std::vector<Case> DataCases;

        Case CurrentCase;
        CurrentCase.Name = "Skip";
        CurrentCase.Algs.push_back(Algorithm(""));
        DataCases.push_back(CurrentCase);

        CurrentCase.Reset();

        uint n_case = 0u;
        const std::string delimiter = " ";
        std::string line, startword;

        while (std::getline(f_txt, line))
        {
            if (line.empty()) continue;

            startword = line.substr(0, line.find(delimiter));

            if (ColName.empty()) ColName = startword;
			
			bool NewCaseFound = ColName.compare(startword) == 0;
			
			if (NewCaseFound) n_case++;
			
            if (n_case == 1u && NewCaseFound)
            {
                CurrentCase.Name = line.substr(line.find(delimiter) + 1, line.length());
            }
            else if (NewCaseFound)
            {
                DataCases.push_back(CurrentCase);
                CurrentCase.Reset();
                CurrentCase.Name = line.substr(line.find(delimiter) + 1, line.length());
            }
            else // Add algorithm
            {
                Algorithm A(line.c_str());
                CurrentCase.Algs.push_back(A);
            }
        }

        DataCases.push_back(CurrentCase);

        f_txt.close();

        XMLDocument col_xml;
		
        XMLNode* pCollection = col_xml.NewElement("Collection"); // Collection root node
        XMLElement* pElementRoot = pCollection->ToElement();
        pElementRoot->SetAttribute("Name", ColName.c_str());
        pElementRoot->SetAttribute("Description", "Algorithms collection");
        pElementRoot->SetAttribute("CasesNumber", std::to_string(DataCases.size()).c_str());
        col_xml.InsertFirstChild(pCollection);

		n_case = 0u;
        for (const auto& Case: DataCases)
        {
            XMLElement* pElementCase = col_xml.NewElement("Case");
            pElementCase->SetText(Case.Name.c_str());
            pElementCase->SetAttribute("Index", std::to_string(n_case).c_str());

            for (const auto& Alg : Case.Algs)
            {
                XMLElement* pElementAlg = col_xml.NewElement("Algorithm");
                pElementAlg->SetText(Alg.ToString().c_str());
                pElementCase->InsertEndChild(pElementAlg);
            }

            pCollection->InsertEndChild(pElementCase);
			n_case++;
        }

        col_xml.SaveFile(XMLFilename.c_str());
    }

    // Save algorithm vector to XML file
    void Collection::SaveVector_To_XML(const std::vector<Algorithm>& Algs, const std::vector<std::string>& Cases, const std::string& Name, const std::string& XMLFilename)
    {
        XMLDocument col_xml;
		
        XMLNode* pCollection = col_xml.NewElement("Collection"); // Collection root node
        XMLElement* pElementRoot = pCollection->ToElement();
        pElementRoot->SetAttribute("Name", Name.c_str());
        pElementRoot->SetAttribute("Description", "Algorithms collection");
        pElementRoot->SetAttribute("CasesNumber", std::to_string(Cases.size()).c_str());
        col_xml.InsertFirstChild(pCollection);

        uint n_case = 0u;
        for (const auto& Case : Cases)
        {
            XMLElement* pElementCase = col_xml.NewElement("Case");
            pElementCase->SetText(Case.c_str());
            pElementCase->SetAttribute("Index", std::to_string(n_case).c_str());

			// Only one algorithm per case
            XMLElement* pElementAlg = col_xml.NewElement("Algorithm");
            pElementAlg->SetText(Algs[n_case].ToString().c_str());
            pElementCase->InsertEndChild(pElementAlg);

            pCollection->InsertEndChild(pElementCase);

            n_case++;
        }

        col_xml.SaveFile(XMLFilename.c_str());
    }
}
