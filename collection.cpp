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
	<mailto:grvigo@hotmail.com>
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

#include "collection.h"
#include "cube.h"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace grcube3
{
	// Return required algorithm for the given case index
	Algorithm Collection::GetAlgorithm(const uint CaseIndex, const Plc Pol, const Metrics Met) const 
	{
		if (CaseIndex >= Cases.size() || Cases[CaseIndex].Algs.empty()) return "";
		
		// If there is only one algorithm or first demanded, nothing to choose
		if (Cases[CaseIndex].Algs.size() == 1u || Pol == Plc::FIRST) return Cases[CaseIndex].Algs.front(); 
		
		uint pos = 0u;
		if (Pol == Plc::SHORT) // Shortest algorithm for current case
		{
            float min_size = 0.0f;
            uint n = 0u;
			for (const auto& A : Cases[CaseIndex].Algs)
			{
				if (n == 0u || A.GetMetric(Met) < min_size) 
				{
					min_size = A.GetMetric(Met);
					pos = n;
				}
				n++;
			}
		}
		else if (Pol == Plc::BEST) // Algorithm with best (lower) score
		{
			uint min_score = 0u, score, n = 0u;

			for (const auto& A : Cases[CaseIndex].Algs)
			{
				score = A.GetSubjectiveScore();
				if (n == 0u || score < min_score) 
				{
					min_score = score;
					pos = n;
				}
				n++;
			}
		}
		else if (Pol == Plc::RANDOM) // Random algorithm from current case
		{
			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution<std::mt19937::result_type> distribution(0u ,static_cast<uint>(Cases[CaseIndex].Algs.size()) - 1u);

			pos = distribution(rng);
		}

		
		return Cases[CaseIndex].Algs[pos];
	}
	
	// Returns the same algorithm transformed by y turns for getting most comfortable (subjective) movements
	Algorithm Collection::GetSubjectiveBesty(const Algorithm& A) const
	{
		uint MinScore = A.GetSubjectiveScore(), MinScoreIndex = static_cast<uint>(Algorithm::yTurns.size());

		if (MinScore == 0u) return Algorithm("");

		for (uint i = 0u; i < static_cast<uint>(Algorithm::yTurns.size()); i++)
		{
			Algorithm BestAlg = A;
			BestAlg.TransformTurn(Algorithm::yTurns[i]); // No turns added
			uint BestScore = BestAlg.GetSubjectiveScore();

			if (BestScore < MinScore)
			{
				MinScore = BestScore;
				MinScoreIndex = i;
			}
		}

		Algorithm BestAlg = A;
		if (MinScoreIndex < static_cast<uint>(Algorithm::yTurns.size()))
			BestAlg.TransformTurn(Algorithm::yTurns[MinScoreIndex]); // No turns added

		return BestAlg;
	}

 	// Load a collection of algorithms (algset) from an XML file
    bool Collection::LoadXMLCollectionFile(const std::string& XMLFilename, const bool clean)
    {
        Reset(); // Clear current collection

        XMLDocument col_xml;

        if (col_xml.LoadFile(XMLFilename.c_str()) != XML_SUCCESS) return false; // Read existing file

        XMLNode* pRoot = col_xml.FirstChild();
        if (pRoot != nullptr)
        {   
			XMLElement* pElementRoot = pRoot->ToElement();
			
			// Read algset name
            const char* s_Name = pElementRoot->Attribute("Name");
            Name = s_Name;

			// Read algset description
            const char* s_Description = pElementRoot->Attribute("Description");
            Description = s_Description;

			// Read cases
            XMLElement* pElementCase = pRoot->FirstChildElement("Case");
            while (pElementCase != nullptr)
            {
                Case CaseAux;

                const char* s_CaseName = pElementCase->GetText();
                CaseAux.Name = s_CaseName == nullptr ? "No case name" : s_CaseName;

                XMLElement* pElementAlg = pElementCase->FirstChildElement("Algorithm");
                while (pElementAlg != nullptr)
                {
                    const char* s_Algorithm = pElementAlg->GetText();
                    Algorithm A;
                    if (s_Algorithm != NULL)
                    {
						if(clean) // Clean <U, y> movsets from algorithm start & end
						{
							const Algorithm Aux(s_Algorithm);
							A = Aux.GetDeveloped(false); // No parentheses
							while (A.GetSize() > 2u && (Algorithm::IsyTurn(A.First()) || Algorithm::IsUMov(A.First()))) A.EraseFirst();
							while (A.GetSize() > 2u && (Algorithm::IsyTurn(A.Last()) || Algorithm::IsUMov(A.Last()))) A.EraseLast();
						}
						else A = Algorithm(s_Algorithm); // Algorithm as read
                    }
                    CaseAux.Algs.push_back(A);
                    pElementAlg = pElementAlg->NextSiblingElement("Algorithm");
                }
				
				Cases.push_back(CaseAux);
 
                pElementCase = pElementCase->NextSiblingElement("Case");
            }
        }
        return !Cases.empty();
    }

    // Save a collection of algorithms to an XML file
    bool Collection::SaveXMLCollectionFile(const std::string& XMLFilename) const
    {
        XMLDocument col_xml;

        XMLNode* pCollection = col_xml.NewElement("Collection"); // Collection root node
        XMLElement* pElementRoot = pCollection->ToElement();
        pElementRoot->SetAttribute("Name", Name.c_str());
        pElementRoot->SetAttribute("Description", Description.c_str());
        pElementRoot->SetAttribute("CasesNumber", std::to_string(Cases.size()).c_str());
        col_xml.InsertFirstChild(pCollection);

        uint n_case = 0u;
        for (const auto& Case : Cases)
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

	// Save txt file from collection
	void Collection::Save_To_TXT(const std::string& TXTFilename) const
	{
		std::ofstream f_txt;
		f_txt.open(TXTFilename.c_str());

		if (!f_txt.is_open()) return;

		for (const auto& Case : Cases) for (auto Alg : Case.Algs) f_txt << Alg.ToString() << "\n";

		f_txt.close();
	}

	// Order by length the algorithms for each case
	void Collection::OrderAlgorithms()
	{
		for (auto& Case : Cases)
			std::sort(Case.Algs.begin(), Case.Algs.end(), [](auto a, auto b) { return a.GetSize() < b.GetSize(); });
	}

	// Merge a collection with the current collection
	void Collection::MergeCollection(const Collection& MC, const Probe MergeProbe)
	{
		for (auto& Case : Cases)
		{
			if (!Case.Algs.empty())
			{
				Algorithm BaseAlg = Case.Algs.front();
				Cube CubeAux(BaseAlg);
				if (CubeAux.GetSpin() != Spn::Default)
				{
					Stp T1, T2;
					Cube::GetSpinsSteps(CubeAux.GetSpin(), Spn::Default, T1, T2);
					if (T1 != Stp::NONE) BaseAlg.Append(T1);
					if (T2 != Stp::NONE) BaseAlg.Append(T2);
				}

				for (const auto& MergeCase : MC.Cases)
				{
					for (auto MergeAlg : MergeCase.Algs)
					{
						CubeAux.Reset();
						CubeAux.ApplyAlgorithm(MergeAlg);
						if (CubeAux.GetSpin() != Spn::Default)
						{
							Stp T1, T2;
							Cube::GetSpinsSteps(CubeAux.GetSpin(), Spn::Default, T1, T2);
							if (T1 != Stp::NONE) MergeAlg.Append(T1);
							if (T2 != Stp::NONE) MergeAlg.Append(T2);
						}

						if (BaseAlg.GetSize() <= 1u) continue; // Skip case
						if (BaseAlg == MergeAlg) continue; // Same algorithms

						// Check if F2L is invariant
						if (MergeProbe == Probe::LL_SOLVE || MergeProbe == Probe::LL_ORIENT || MergeProbe == Probe::COLL)
						{
							if (!CheckF2LIntegrity(BaseAlg) || !CheckF2LIntegrity(MergeAlg)) continue;
						}

						// Check if first Roux blocks are invariant
						if (MergeProbe == Probe::CMLL)
						{
							if (!CheckRouxF2BIntegrity(BaseAlg) || !CheckRouxF2BIntegrity(MergeAlg)) continue;
						}

						// Check the integrity of F2L except DF edge
						if (MergeProbe == Probe::NCLL || MergeProbe == Probe::NCOLL ||
							MergeProbe == Probe::L5E)
						{
							if (!CheckF2LDFIntegrity(BaseAlg) || !CheckF2LDFIntegrity(MergeAlg)) continue;
						}

						// Check the integrity of F2L except DR or DF or DL or DB edge (Mehta)
						if (MergeProbe == Probe::L5EP)
						{
							if ((!CheckF2LDRIntegrity(BaseAlg) && !CheckF2LDFIntegrity(BaseAlg) &&
								!CheckF2LDLIntegrity(BaseAlg) && !CheckF2LDBIntegrity(BaseAlg)) ||
								(!CheckF2LDRIntegrity(MergeAlg) && !CheckF2LDFIntegrity(MergeAlg) &&
								!CheckF2LDLIntegrity(MergeAlg) && !CheckF2LDBIntegrity(MergeAlg))) continue;
						}

						// Check the integrity of F2L except DF edge & DFR corner orientation
						if (MergeProbe == Probe::TNCLL)
						{
							if (!CheckF2LDFDFRIntegrity(BaseAlg) || !CheckF2LDFDFRIntegrity(MergeAlg)) continue;
						}

						// Equivalent algorithms that solves the last layer
						if (MergeProbe == Probe::LL_SOLVE || MergeProbe == Probe::L5E || MergeProbe == Probe::L5EP)
						{	
							if (!CheckLLSolveAlgorithms(BaseAlg, MergeAlg)) continue;
						}

						// Equivalent algorithms that orients the last layer
						if (MergeProbe == Probe::LL_ORIENT || MergeProbe == Probe::COLL || MergeProbe == Probe::NCOLL)
						{	
							if (!CheckLLOrientAlgorithms(BaseAlg, MergeAlg)) continue;
						}

						// Equivalent algorithms that solves the last layer corners
						if (MergeProbe == Probe::CMLL || MergeProbe == Probe::COLL ||
							MergeProbe == Probe::NCLL || MergeProbe == Probe::NCOLL || MergeProbe == Probe::TNCLL)
						{
							if (!CheckLLCornersAlgorithms(BaseAlg, MergeAlg)) continue;
						}

						// The merge algorithm is valid, add to this case
						Case.Algs.push_back(MergeAlg);
					}
				}
			}
		}	
	}

	// Merge a collection with the list of algorithms from a text file
	void Collection::MergeTXTFile(const std::string& MF, const Probe MergeProbe, const bool clean)
	{
		std::ifstream f_txt;
        f_txt.open(MF.c_str());

        if (!f_txt.is_open()) return;

        std::vector<Algorithm> TxtAlgs;
		std::string line;

        while (std::getline(f_txt, line))
        {
            if (line.empty()) continue;

            Algorithm A;

			if(clean) // Clean <U, y> movsets from algorithm start & end
			{
				const Algorithm Aux(line.c_str());
				A = Aux.GetDeveloped(false); // No parentheses
				while (A.GetSize() > 2u && (Algorithm::IsyTurn(A.First()) || Algorithm::IsUMov(A.First()))) A.EraseFirst();
				while (A.GetSize() > 2u && (Algorithm::IsyTurn(A.Last()) || Algorithm::IsUMov(A.Last()))) A.EraseLast();
			}
			else A = Algorithm(line.c_str()); // Algorithm as read

            TxtAlgs.push_back(A);
        }

        f_txt.close();

		for (auto& Case : Cases)
		{
			if (!Case.Algs.empty())
			{
				Algorithm BaseAlg = Case.Algs.front();
				Cube CubeAux(BaseAlg);
				if (CubeAux.GetSpin() != Spn::Default)
				{
					Stp T1, T2;
					Cube::GetSpinsSteps(CubeAux.GetSpin(), Spn::Default, T1, T2);
					if (T1 != Stp::NONE) BaseAlg.Append(T1);
					if (T2 != Stp::NONE) BaseAlg.Append(T2);
				}

				if (BaseAlg.GetSize() <= 1u) continue; // Skip case

				for (auto MergeAlg : TxtAlgs)
				{
					CubeAux.Reset();
					CubeAux.ApplyAlgorithm(MergeAlg);
					if (CubeAux.GetSpin() != Spn::Default)
					{
						Stp T1, T2;
						Cube::GetSpinsSteps(CubeAux.GetSpin(), Spn::Default, T1, T2);
						if (T1 != Stp::NONE) MergeAlg.Append(T1);
						if (T2 != Stp::NONE) MergeAlg.Append(T2);
					}

					if (BaseAlg == MergeAlg) continue; // Same algorithms

					// Check if F2L is invariant
					if (MergeProbe == Probe::LL_SOLVE || MergeProbe == Probe::LL_ORIENT || MergeProbe == Probe::COLL)
					{
						if (!CheckF2LIntegrity(BaseAlg) || !CheckF2LIntegrity(MergeAlg)) continue;
					}

					// Check if first Roux blocks are invariant
					if (MergeProbe == Probe::CMLL)
					{
						if (!CheckRouxF2BIntegrity(BaseAlg) || !CheckRouxF2BIntegrity(MergeAlg)) continue;
					}

					// Check the integrity of F2L except DF edge
					if (MergeProbe == Probe::NCLL || MergeProbe == Probe::NCOLL ||
						MergeProbe == Probe::L5E)
					{
						if (!CheckF2LDFIntegrity(BaseAlg) || !CheckF2LDFIntegrity(MergeAlg)) continue;
					}

					// Check the integrity of F2L except DR or DF or DL or DB edge (Mehta)
					if (MergeProbe == Probe::L5EP)
					{
						if ((!CheckF2LDRIntegrity(BaseAlg) && !CheckF2LDFIntegrity(BaseAlg) &&
							!CheckF2LDLIntegrity(BaseAlg) && !CheckF2LDBIntegrity(BaseAlg)) ||
							(!CheckF2LDRIntegrity(MergeAlg) && !CheckF2LDFIntegrity(MergeAlg) &&
							!CheckF2LDLIntegrity(MergeAlg) && !CheckF2LDBIntegrity(MergeAlg))) continue;
					}

					// Check the integrity of F2L except DF edge & DFR corner orientation
					if (MergeProbe == Probe::TNCLL)
					{
						if (!CheckF2LDFDFRIntegrity(BaseAlg) || !CheckF2LDFDFRIntegrity(MergeAlg)) continue;
					}

					// Equivalent algorithms that solves the last layer
					if (MergeProbe == Probe::LL_SOLVE || MergeProbe == Probe::L5E || MergeProbe == Probe::L5EP)
					{	
						if (!CheckLLSolveAlgorithms(BaseAlg, MergeAlg)) continue;
					}

					// Equivalent algorithms that orients the last layer
					if (MergeProbe == Probe::LL_ORIENT || MergeProbe == Probe::COLL || MergeProbe == Probe::NCOLL)
					{	
						if (!CheckLLOrientAlgorithms(BaseAlg, MergeAlg)) continue;
					}

					// Equivalent algorithms that solves the last layer corners
					if (MergeProbe == Probe::CMLL || MergeProbe == Probe::COLL ||
						MergeProbe == Probe::NCLL || MergeProbe == Probe::NCOLL || MergeProbe == Probe::TNCLL)
					{
						if (!CheckLLCornersAlgorithms(BaseAlg, MergeAlg)) continue;
					}

					// The merge algorithm is valid, add to this case
					Case.Algs.push_back(MergeAlg);
				}
			}
		}	
	}

	// Check the integrity of the two first layers by the given algorithm
	bool Collection::CheckF2LIntegrity(const Algorithm& Alg)
	{
		const Cube C(Alg);
		return C.IsSolved(Pgr::F2L_D); // First two layers must be invariant
	}

	// Check the integrity of the two first Roux blocks by the given algorithm
	bool Collection::CheckRouxF2BIntegrity(const Algorithm& Alg)
	{
		const Cube C(Alg);
		return C.IsSolved(Pgr::UF_B1) && C.IsSolved(Pgr::UF_B2);
	}

	// Check the integrity of F2L except DF edge
	bool Collection::CheckF2LDFIntegrity(const Algorithm& Alg)
	{
		const Cube C(Alg);
		return C.IsSolved(Pgr::UF_B1) && C.IsSolved(Pgr::UF_B2) && C.IsSolved(Pce::DB);
	}

	// Check the integrity of F2L except DB edge
	bool Collection::CheckF2LDBIntegrity(const Algorithm& Alg)
	{
		const Cube C(Alg);
		return C.IsSolved(Pgr::UF_B1) && C.IsSolved(Pgr::UF_B2) && C.IsSolved(Pce::DF);
	}

	// Check the integrity of F2L except DR edge
	bool Collection::CheckF2LDRIntegrity(const Algorithm& Alg)
	{
		const Cube C(Alg);
		return C.IsSolved(Pgr::UF_B1) && C.IsSolved(Pgr::F2L_D_DFR) && C.IsSolved(Pgr::F2L_D_DBR) &&
			   C.IsSolved(Pce::DF) && C.IsSolved(Pce::DB);
	}

	// Check the integrity of F2L except DL edge
	bool Collection::CheckF2LDLIntegrity(const Algorithm& Alg)
	{
		const Cube C(Alg);
		return C.IsSolved(Pgr::UF_B2) && C.IsSolved(Pgr::F2L_D_DFL) && C.IsSolved(Pgr::F2L_D_DBL) &&
			   C.IsSolved(Pce::DF) && C.IsSolved(Pce::DB);
	}

	// Check the integrity of F2L except DF edge & DFR corner
	bool Collection::CheckF2LDFDFRIntegrity(const Algorithm& Alg)
	{
		const Cube C(Alg);
		return C.IsSolved(Pgr::UF_B1) && C.IsSolved(Pgr::UF_B2S2) && C.IsSolved(Pce::DB) && 
			   C.IsSolved(Pce::FR) && (C.GetPiecePosition(Pce::DFR) == Pcp::DFR);
	}
	
    // Check if two last layer (layer in up position) solve algorithms are equivalent
    bool Collection::CheckLLSolveAlgorithms(const Algorithm& AlgA, const Algorithm& AlgB)
    {
		Algorithm AlgAux = AlgA;
		if (!AlgAux.TransformReverseInvert()) return false;
		const Cube CubeA(AlgAux);

        for (const auto UMov1 : Algorithm::UMovs)
        {
            for (const auto UMov2 : Algorithm::UMovs)
            {
                Cube CubeAux = CubeA;
                CubeAux.ApplyStep(UMov1);
                CubeAux.ApplyAlgorithm(AlgB);
                CubeAux.ApplyStep(UMov2);
                if (CubeAux.IsSolved()) return true;
            }
        }
        return false;
    }
	
	// Check if two last layer corners (layer in up position) solve algorithms are equivalent
	bool Collection::CheckLLCornersAlgorithms(const Algorithm& AlgA, const Algorithm& AlgB)
	{
		Algorithm AlgAux = AlgA;
		if (!AlgAux.TransformReverseInvert()) return false;
		const Cube CubeA(AlgAux);

		for (const auto UMov1 : Algorithm::UMovs)
		{
			for (const auto UMov2 : Algorithm::UMovs)
			{
				Cube CubeAux = CubeA;
				CubeAux.ApplyStep(UMov1);
				CubeAux.ApplyAlgorithm(AlgB);
				CubeAux.ApplyStep(UMov2);
				if (CubeAux.IsSolved(Pgr::CORNERS_U)) return true;
			}
		}
		return false;
	}

	// Check if two last layer orientation algorithms are equivalent
    bool Collection::CheckLLOrientAlgorithms(const Algorithm& AlgA, const Algorithm& AlgB)
    {
        Algorithm AlgAux = AlgA;
        if (!AlgAux.TransformReverseInvert()) return false;
		const Cube CubeA(AlgAux);

        for (const auto UMov : Algorithm::UMovs)
        {
			Cube CubeAux = CubeA;
			CubeAux.ApplyStep(UMov);
			CubeAux.ApplyAlgorithm(AlgB);
			if (CubeAux.CheckOrientation(Pgr::LAYER_U)) return true;
        }
        return false;
    }

	// Audit a collection of algorithms, audit results to a TXT file
	void Collection::Audit(const Probe AuditProbe, const std::string& txt_filename) const
	{
		std::ofstream faud;
        faud.open(txt_filename.c_str());

        if (!faud.is_open()) return;
		
		faud <<"# Algorithms collection audit file\n# ------------------------------------------\n";
		faud << "Name: " << Name << "\n";
		faud << "Description: " << Description << "\n";
		faud << "Number of cases: " << std::to_string(Cases.size()) << "\n";
		
		uint nalgs = 0u;
		for (const auto& Case : Cases) nalgs += static_cast<uint>(Case.Algs.size());
		faud << "Number of algorithms: " << std::to_string(nalgs) << "\n";
		
		uint nc = 0u; // case number
		for (const auto& Case : Cases)
		{
			faud << "Case #" << std::to_string(nc) << ": " << Case.Name << " [" << std::to_string(Case.Algs.size()) << " alg(s)]\n";
			
			uint na = 0u; // algorithm number
			for (const auto& Alg : Case.Algs)
			{
                faud << "\tAlgorithm #" << std::to_string(nc) << "." << std::to_string(na);
                faud << " [" << std::to_string(Alg.GetNumSteps()) << " mov(s)]: " << Alg.ToString() << "\n";

				Algorithm AuxAlg = Alg;
				Cube CubeAux(AuxAlg);
				if (CubeAux.GetSpin() != Spn::Default)
				{
					faud << "\t\tAlg changes orientation: " << CubeAux.GetSpinText() << "\n";
					Stp T1, T2;
					Cube::GetSpinsSteps(CubeAux.GetSpin(), Spn::Default, T1, T2);
					if (T1 != Stp::NONE) AuxAlg.Append(T1);
					if (T2 != Stp::NONE) AuxAlg.Append(T2);
				}
				
				// Check if F2L is invariant
				if (AuditProbe == Probe::LL_SOLVE || AuditProbe == Probe::LL_ORIENT || AuditProbe == Probe::COLL)
				{
					if (!CheckF2LIntegrity(AuxAlg)) faud << "\t\t ***** F2L integrity NOT OK! *****\n";
				}

				// Check if first Roux blocks are invariant
				if (AuditProbe == Probe::CMLL)
				{
					if (!CheckRouxF2BIntegrity(AuxAlg)) faud << "\t\t ***** Roux F2B integrity NOT OK! *****\n";
				}

				// Check the integrity of F2L except DF edge
				if (AuditProbe == Probe::NCLL || AuditProbe == Probe::NCOLL ||
					AuditProbe == Probe::L5E)
				{
					if (!CheckF2LDFIntegrity(AuxAlg)) faud << "\t\t ***** F2L except DF integrity NOT OK! *****\n";
				}

				// Check the integrity of F2L except DR or DF or DL or DB edge (Mehta)
				if (AuditProbe == Probe::L5EP)
				{
					if (!CheckF2LDRIntegrity(AuxAlg) && !CheckF2LDFIntegrity(AuxAlg) &&
						!CheckF2LDLIntegrity(AuxAlg) && !CheckF2LDBIntegrity(AuxAlg))
						faud << "\t\t ***** F2L except DR or DF or DL or DB integrity NOT OK! *****\n";
				}

				// Check the integrity of F2L except DF edge & DFR corner orientation
				if (AuditProbe == Probe::TNCLL)
				{
					if (!CheckF2LDFDFRIntegrity(AuxAlg)) faud << "\t\t ***** F2L except DB/DFR integrity NOT OK! *****\n";
				}
				
				// Equivalent algorithms that solves the last layer
				if (AuditProbe == Probe::LL_SOLVE || AuditProbe == Probe::L5E || AuditProbe == Probe::L5EP)
				{	
					// Compare with same case algorithms
					uint nab = 0u; // b algorithm number
					for (const auto& AlgB : Case.Algs)
					{
						Algorithm AuxAlgB = AlgB;
						Cube CubeAuxB(AuxAlgB);
						if (CubeAuxB.GetSpin() != Spn::Default)
						{
							Stp T1, T2;
							Cube::GetSpinsSteps(CubeAuxB.GetSpin(), Spn::Default, T1, T2);
							if (T1 != Stp::NONE) AuxAlgB.Append(T1);
							if (T2 != Stp::NONE) AuxAlgB.Append(T2);
						}

						if (&Alg != &AlgB && !CheckLLSolveAlgorithms(AuxAlg, AuxAlgB)) 
							faud << "\t\tNot equivalent solve alg #" << nc << "." << nab << ": " << AlgB.ToString() << "\n";
						nab++;
					}

					// Compare with other cases algorithms
					uint noc = 0u; // other case number
					for (const auto& OtherCase : Cases)
					{
						if (&Case != &OtherCase)
						{
							uint noa = 0u; // other algorithm number
							for (const auto& OtherAlg : OtherCase.Algs)
							{
								Algorithm AuxAlgB = OtherAlg;
								Cube CubeAuxB(AuxAlgB);
								if (CubeAuxB.GetSpin() != Spn::Default)
								{
									Stp T1, T2;
									Cube::GetSpinsSteps(CubeAuxB.GetSpin(), Spn::Default, T1, T2);
									if (T1 != Stp::NONE) AuxAlgB.Append(T1);
									if (T2 != Stp::NONE) AuxAlgB.Append(T2);
								}

								if (CheckLLSolveAlgorithms(AuxAlg, AuxAlgB))
									faud << "\t\tEquivalent solve alg #" << noc << "." << noa << ": " << OtherAlg.ToString() << "\n";
								noa++;
							}
						}
						noc++;
					}
				}
				
				// Equivalent algorithms that orients the last layer
				if (AuditProbe == Probe::LL_ORIENT || AuditProbe == Probe::COLL || AuditProbe == Probe::NCOLL)
				{	
					// Compare orientation with same case algorithms
					uint nab = 0u; // b algorithm number
					for (const auto& AlgB : Case.Algs)
					{
						Algorithm AuxAlgB = AlgB;
						Cube CubeAuxB(AuxAlgB);
						if (CubeAuxB.GetSpin() != Spn::Default)
						{
							Stp T1, T2;
							Cube::GetSpinsSteps(CubeAuxB.GetSpin(), Spn::Default, T1, T2);
							if (T1 != Stp::NONE) AuxAlgB.Append(T1);
							if (T2 != Stp::NONE) AuxAlgB.Append(T2);
						}

						if (&Alg != &AlgB && !CheckLLOrientAlgorithms(AuxAlg, AuxAlgB))
							faud << "\t\tNot equivalent orient alg #" << nc << "." << nab << ": " << AlgB.ToString() << "\n";
						nab++;
					}

					// Compare orientation with other cases algorithms
					uint noc = 0u; // other case number
					for (const auto& OtherCase : Cases)
					{
						if (&Case != &OtherCase)
						{
							uint noa = 0u; // other algorithm number
							for (const auto& OtherAlg : OtherCase.Algs)
							{
								Algorithm AuxAlgB = OtherAlg;
								Cube CubeAuxB(AuxAlgB);
								if (CubeAuxB.GetSpin() != Spn::Default)
								{
									Stp T1, T2;
									Cube::GetSpinsSteps(CubeAuxB.GetSpin(), Spn::Default, T1, T2);
									if (T1 != Stp::NONE) AuxAlgB.Append(T1);
									if (T2 != Stp::NONE) AuxAlgB.Append(T2);
								}

								if (CheckLLOrientAlgorithms(AuxAlg, AuxAlgB))
									faud << "\t\tEquivalent orient alg #" << noc << "." << noa << ": " << OtherAlg.ToString() << "\n";
								noa++;
							}
						}
						noc++;
					}
				}

				// Equivalent algorithms that solves the last layer corners
				if (AuditProbe == Probe::CMLL || AuditProbe == Probe::COLL ||
					AuditProbe == Probe::NCLL || AuditProbe == Probe::NCOLL || AuditProbe == Probe::TNCLL)
				{
					// Compare with same case algorithms
					uint nab = 0u; // b algorithm number
					for (const auto& AlgB : Case.Algs)
					{
						Algorithm AuxAlgB = AlgB;
						Cube CubeAuxB(AuxAlgB);
						if (CubeAuxB.GetSpin() != Spn::Default)
						{
							Stp T1, T2;
							Cube::GetSpinsSteps(CubeAuxB.GetSpin(), Spn::Default, T1, T2);
							if (T1 != Stp::NONE) AuxAlgB.Append(T1);
							if (T2 != Stp::NONE) AuxAlgB.Append(T2);
						}

						if (&Alg != &AlgB && !CheckLLCornersAlgorithms(AuxAlg, AuxAlgB))
							faud << "\t\tNot equivalent corners solve alg #" << nc << "." << nab << ": " << AlgB.ToString() << "\n";
						nab++;
					}

					// Compare with other cases algorithms
					uint noc = 0u; // other case number
					for (const auto& OtherCase : Cases)
					{
						if (&Case != &OtherCase)
						{
							uint noa = 0u; // other algorithm number
							for (const auto& OtherAlg : OtherCase.Algs)
							{
								Algorithm AuxAlgB = OtherAlg;
								Cube CubeAuxB(AuxAlgB);
								if (CubeAuxB.GetSpin() != Spn::Default)
								{
									Stp T1, T2;
									Cube::GetSpinsSteps(CubeAuxB.GetSpin(), Spn::Default, T1, T2);
									if (T1 != Stp::NONE) AuxAlgB.Append(T1);
									if (T2 != Stp::NONE) AuxAlgB.Append(T2);
								}

								if (CheckLLCornersAlgorithms(AuxAlg, AuxAlgB))
									faud << "\t\tEquivalent corners solve alg #" << noc << "." << noa << ": " << OtherAlg.ToString() << "\n";
								noa++;
							}
						}
						noc++;
					}
				}
				na++;
			}
			nc++;
		}
		
		faud.close();
	}

    // Save XML collection file from TXT file (copy & paste from algdb.net)
    void Collection::ConvertTXT_To_XML(const std::string& TXTFilename, const std::string& XMLFilename)
    {
        std::ifstream f_txt;
        f_txt.open(TXTFilename.c_str());

        if (!f_txt.is_open()) return;

        std::string ColName;
        std::vector<Case> CasesCases;

        Case CurrentCase;
        CurrentCase.Name = "Skip";
        CurrentCase.Algs.push_back(Algorithm(""));
        CasesCases.push_back(CurrentCase);

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
                CasesCases.push_back(CurrentCase);
                CurrentCase.Reset();
                CurrentCase.Name = line.substr(line.find(delimiter) + 1, line.length());
            }
            else // Add algorithm
            {
                Algorithm A(line.c_str());
                CurrentCase.Algs.push_back(A);
            }
        }

        CasesCases.push_back(CurrentCase);

        f_txt.close();

        XMLDocument col_xml;
		
        XMLNode* pCollection = col_xml.NewElement("Collection"); // Collection root node
        XMLElement* pElementRoot = pCollection->ToElement();
        pElementRoot->SetAttribute("Name", ColName.c_str());
        pElementRoot->SetAttribute("Description", "Algorithms collection");
        pElementRoot->SetAttribute("CasesNumber", std::to_string(CasesCases.size()).c_str());
        col_xml.InsertFirstChild(pCollection);

		n_case = 0u;
        for (const auto& Case: CasesCases)
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
