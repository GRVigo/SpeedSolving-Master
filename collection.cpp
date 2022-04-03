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
    // Load algorithms collections needed
    const Collection Collection::OLL_Algorithms("algsets/OLL.xml", true);
    const Collection Collection::PLL_Algorithms("algsets/PLL.xml", true);
    const Collection Collection::Algorithms_1LLL("algsets/1LLL.xml", true);
    const Collection Collection::ZBLL_Algorithms("algsets/ZBLL.xml", true);
    const Collection Collection::OCLL_Algorithms("algsets/OCLL.xml", true);
    const Collection Collection::CMLL_Algorithms("algsets/CMLL.xml", true);
    const Collection Collection::COLL_Algorithms("algsets/COLL.xml", true);
    const Collection Collection::EPLL_Algorithms("algsets/EPLL.xml", true);
    const Collection Collection::Algorithms_2GLL("algsets/2GLL.xml", true);

    const Collection Collection::EOLE_Algorithms("algsets/EOLE.xml", true);
    const Collection Collection::Algorithms_6CO("algsets/6CO.xml", true);
    const Collection Collection::Algorithms_6CP("algsets/6CP.xml", true);
    const Collection Collection::APDR_Algorithms("algsets/APDR.xml", true);
    const Collection Collection::CDRLL_Algorithms("algsets/CDRLL.xml", true);
    const Collection Collection::DCAL_Algorithms("algsets/DCAL.xml", true);
    const Collection Collection::JTLE_Algorithms("algsets/JTLE.xml", true);
    const Collection Collection::L5EP_Algorithms("algsets/L5EP.xml", true);
    const Collection Collection::TDR_Algorithms("algsets/TDR.xml", true);

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
                AuxCase.Name = s_CaseName == nullptr ? "No case name" : s_CaseName;

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
							A = Aux.GetDeveloped(false); // No parentheses
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
    bool Collection::SaveXMLCollectionFile(const std::string& XMLFilename) const
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

    // Search an algorithm from a collection to orientate the last layer
	bool Collection::OrientateLL(Algorithm& LLSolve, std::string &LLCase, const AlgSets AlgSet, const Cube& CubeBase)
	{
		LLSolve.Clear();
		LLCase.clear();	
		
		Collection LL_Algorithms;
		
		switch (AlgSet) // Only orientation algorithm collections
		{
			case AlgSets::OLL: LL_Algorithms = OLL_Algorithms; break;
			case AlgSets::OCLL: LL_Algorithms = OCLL_Algorithms; break;
			default: return false;
		}
		
		const Lyr UpLayer = Cube::GetUpSliceLayer(CubeBase.GetSpin()),
				  MidLayer = Cube::AdjacentLayer(UpLayer),
				  DownLayer = Cube::GetDownSliceLayer(CubeBase.GetSpin());

		bool LL_Found = false;
        Spn Spin1, Spin2; // Cube spins before and after applying algorithm
        Stp StepS1, StepS2; // Steps to return cube to the start spin

        for (uint LL_Index = 0u; LL_Index < LL_Algorithms.GetCasesNumber(); LL_Index++)
        {
            for (const auto yLL: Algorithm::yTurns) // y turn before algorithm
            {
				Cube CubeLL = CubeBase;
				CubeLL.ApplyStep(yLL);
				Spin1 = CubeLL.GetSpin();
				CubeLL.ApplyAlgorithm(LL_Algorithms[LL_Index]);
				Spin2 = CubeLL.GetSpin();
				if (Spin1 != Spin2)
				{
					Cube::GetSpinsSteps(Spin2, Spin1, StepS1, StepS2);
					if (StepS1 != Stp::NONE) CubeLL.ApplyStep(StepS1);
					if (StepS2 != Stp::NONE) CubeLL.ApplyStep(StepS2);
				}
				else StepS1 = StepS2 = Stp::NONE;

				LL_Found = CubeLL.IsSolved(DownLayer) && CubeLL.IsSolved(MidLayer) && CubeLL.IsFaceOriented(UpLayer);

				if (LL_Found)
				{
					LLCase = LL_Algorithms.GetCaseName(LL_Index);

					if (yLL != Stp::NONE)
					{
						// LLSolve.Append(Stp::PARENTHESIS_OPEN);
						LLSolve.Append(yLL);
						// LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
					}
					LLSolve.Append(LL_Algorithms[LL_Index]);
					
					// If the algorithm changes cube orientation, add a turn to recover up layer
					if (Cube::GetUpSliceLayer(Spin1) != Cube::GetUpSliceLayer(Spin2))
					{
						// LLSolve.Append(Stp::PARENTHESIS_OPEN);
						LLSolve.Append(StepS1);
						if (StepS2 != Stp::NONE) LLSolve.Append(StepS2);
						// LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
					}

					break;
				}
            }
            if (LL_Found) break; // Release for loop
        }

        return LL_Found;
	}
	
	// Search an algorithm from a collection to solve the last layer
	bool Collection::SolveLL(Algorithm& LLSolve, std::string &LLCase, Stp& AUFStep, const AlgSets AlgSet, const Cube& CubeBase)
	{	
		LLSolve.Clear();
		LLCase.clear();	
		AUFStep = Stp::NONE;
		
		Collection LL_Algorithms;
		
		switch (AlgSet)
		{
			case AlgSets::_1LLL: LL_Algorithms = Algorithms_1LLL; break;
			case AlgSets::PLL: LL_Algorithms = PLL_Algorithms; break;
			case AlgSets::ZBLL: LL_Algorithms = ZBLL_Algorithms; break;
			case AlgSets::EPLL: LL_Algorithms = EPLL_Algorithms; break;
            case AlgSets::_2GLL: LL_Algorithms = Algorithms_2GLL; break;
			default: return false; 
		}

		bool LL_Found = false;
        Spn Spin1, Spin2; // Cube spins before and after applying algorithm
        Stp StepS1, StepS2; // Steps to return cube to the start spin

        for (uint LL_Index = 0u; LL_Index < LL_Algorithms.GetCasesNumber(); LL_Index++)
        {
            if (AlgSet == AlgSets::_2GLL)
            {
                for (const auto ULL1 : Algorithm::UMovs) // U movement before algorithm
                {
                    for (const auto ULL2 : Algorithm::UMovs) // U movement after algorithm (AUF)
                    {
                        Cube CubeLL = CubeBase;
                        CubeLL.ApplyStep(ULL1);
                        Spin1 = CubeLL.GetSpin();
                        CubeLL.ApplyAlgorithm(LL_Algorithms[LL_Index]);
                        Spin2 = CubeLL.GetSpin();
                        if (Spin1 != Spin2)
                        {
                            Cube::GetSpinsSteps(Spin2, Spin1, StepS1, StepS2);
                            if (StepS1 != Stp::NONE) CubeLL.ApplyStep(StepS1);
                            if (StepS2 != Stp::NONE) CubeLL.ApplyStep(StepS2);
                        }
                        else StepS1 = StepS2 = Stp::NONE;
                        CubeLL.ApplyStep(ULL2);

                        LL_Found = CubeLL.IsSolved();

                        if (LL_Found)
                        {
                            LLCase = LL_Algorithms.GetCaseName(LL_Index);

                            if (ULL1 != Stp::NONE)
                            {
                                // LLSolve.Append(Stp::PARENTHESIS_OPEN);
                                LLSolve.Append(ULL1);
                                // LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
                            }
                            LLSolve.Append(LL_Algorithms[LL_Index]);
                            if (Cube::GetUpSliceLayer(Spin1) != Cube::GetUpSliceLayer(Spin2))
                            {
                                // LLSolve.Append(Stp::PARENTHESIS_OPEN);
                                LLSolve.Append(StepS1);
                                if (StepS2 != Stp::NONE) LLSolve.Append(StepS2);
                                // LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
                            }
                            AUFStep = ULL2;
                            break;
                        }
                    }
                    if (LL_Found) break; // Release for loop
                }
            }
            else
            {
                for (const auto yLL : Algorithm::yTurns) // y turn before algorithm
                {
                    for (const auto ULL : Algorithm::UMovs) // U movement after algorithm (AUF)
                    {
                        Cube CubeLL = CubeBase;
                        CubeLL.ApplyStep(yLL);
                        Spin1 = CubeLL.GetSpin();
                        CubeLL.ApplyAlgorithm(LL_Algorithms[LL_Index]);
                        Spin2 = CubeLL.GetSpin();
                        if (Spin1 != Spin2)
                        {
                            Cube::GetSpinsSteps(Spin2, Spin1, StepS1, StepS2);
                            if (StepS1 != Stp::NONE) CubeLL.ApplyStep(StepS1);
                            if (StepS2 != Stp::NONE) CubeLL.ApplyStep(StepS2);
                        }
                        else StepS1 = StepS2 = Stp::NONE;
                        CubeLL.ApplyStep(ULL);

                        LL_Found = CubeLL.IsSolved();

                        if (LL_Found)
                        {
                            LLCase = LL_Algorithms.GetCaseName(LL_Index);

                            if (yLL != Stp::NONE)
                            {
                                // LLSolve.Append(Stp::PARENTHESIS_OPEN);
                                LLSolve.Append(yLL);
                                // LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
                            }
                            LLSolve.Append(LL_Algorithms[LL_Index]);
                            if (Cube::GetUpSliceLayer(Spin1) != Cube::GetUpSliceLayer(Spin2))
                            {
                                // LLSolve.Append(Stp::PARENTHESIS_OPEN);
                                LLSolve.Append(StepS1);
                                if (StepS2 != Stp::NONE) LLSolve.Append(StepS2);
                                // LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
                            }
                            AUFStep = ULL;
                            break;
                        }
                    }
                    if (LL_Found) break; // Release for loop
                }
            }

            if (LL_Found) break; // Release for loop
        }

        return LL_Found;
	}
	
	// Search an algorithm from a collection (CMLL or COLL) to solve the last layer corners 
	bool Collection::CornersLL(Algorithm& LLSolve, std::string &LLCase, Stp& AUFStep, const AlgSets AlgSet, const Cube& CubeBase)
	{
		LLSolve.Clear();
		LLCase.clear();
        AUFStep = Stp::NONE;
		
		Collection LL_Algorithms;
		
		switch (AlgSet) // Only corners algorithm collections
		{
			case AlgSets::CMLL: LL_Algorithms = CMLL_Algorithms; break;
			case AlgSets::COLL: LL_Algorithms = COLL_Algorithms; break;
			default: return false; 
		}
		
		const Lyr UpLayer = Cube::GetUpSliceLayer(CubeBase.GetSpin()),
				  MidLayer = Cube::AdjacentLayer(UpLayer);
				  
		Pgr UpLayerCorners, DownLayerCorners;
		switch(UpLayer)
		{
        case Lyr::U: UpLayerCorners = Pgr::CORNERS_U; DownLayerCorners = Pgr::CORNERS_D; break;
		case Lyr::D: UpLayerCorners = Pgr::CORNERS_D; DownLayerCorners = Pgr::CORNERS_U; break;
		case Lyr::F: UpLayerCorners = Pgr::CORNERS_F; DownLayerCorners = Pgr::CORNERS_B; break;
		case Lyr::B: UpLayerCorners = Pgr::CORNERS_B; DownLayerCorners = Pgr::CORNERS_F; break;
		case Lyr::R: UpLayerCorners = Pgr::CORNERS_R; DownLayerCorners = Pgr::CORNERS_L; break;
		case Lyr::L: UpLayerCorners = Pgr::CORNERS_L; DownLayerCorners = Pgr::CORNERS_R; break;
		default: return false;
		}

		bool LL_Found = false;
        Spn Spin1, Spin2; // Cube spins before and after applying algorithm
        Stp StepS1, StepS2; // Steps to return cube to the start spin

        for (uint LL_Index = 0u; LL_Index < LL_Algorithms.GetCasesNumber(); LL_Index++)
        {
            for (const auto ULL1: Algorithm::UMovs) // U movement before algorithm
            {
                for (const auto ULL2: Algorithm::UMovs) // U movement after algorithm (AUF)
                {
                    Cube CubeLL = CubeBase;
                    CubeLL.ApplyStep(ULL1);
                    Spin1 = CubeLL.GetSpin();
                    CubeLL.ApplyAlgorithm(LL_Algorithms[LL_Index]);
                    Spin2 = CubeLL.GetSpin();
                    if (Spin1 != Spin2)
                    {
                        Cube::GetSpinsSteps(Spin2, Spin1, StepS1, StepS2);
                        if (StepS1 != Stp::NONE) CubeLL.ApplyStep(StepS1);
                        if (StepS2 != Stp::NONE) CubeLL.ApplyStep(StepS2);
                    }
                    else StepS1 = StepS2 = Stp::NONE;
                    CubeLL.ApplyStep(ULL2);

					LL_Found = CubeLL.IsSolved(DownLayerCorners) && CubeLL.IsSolved(MidLayer) && CubeLL.IsSolved(UpLayerCorners) &&
                               CubeLL.IsSolved(Cube::FromAbsPosition(App::DR, CubeLL.GetSpin())) && CubeLL.IsSolved(Cube::FromAbsPosition(App::DL, CubeLL.GetSpin()));

                    if (LL_Found)
                    {
                        LLCase = LL_Algorithms.GetCaseName(LL_Index);

                        if (ULL1 != Stp::NONE)
                        {
                            // LLSolve.Append(Stp::PARENTHESIS_OPEN);
                            LLSolve.Append(ULL1);
                            // LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
                        }
                        LLSolve.Append(LL_Algorithms[LL_Index]);
						if (Cube::GetUpSliceLayer(Spin1) != Cube::GetUpSliceLayer(Spin2))
						{
							// LLSolve.Append(Stp::PARENTHESIS_OPEN);
							LLSolve.Append(StepS1);
							if (StepS2 != Stp::NONE) LLSolve.Append(StepS2);
							// LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
						}
                        AUFStep = ULL2;
                        break;
                    }
                }
                if (LL_Found) break; // Release for loop
            }
            if (LL_Found) break; // Release for loop
        }

        return LL_Found;
	}
}
