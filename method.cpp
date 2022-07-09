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

#include "method.h"

#include <chrono>
#include <algorithm>

namespace grcube3
{
    // Load algorithms collections needed
    const Collection Method::Algset_OLL("algsets/OLL.xml", true);
    const Collection Method::Algset_PLL("algsets/PLL.xml", true);
    const Collection Method::Algset_1LLL("algsets/1LLL.xml", true);
    const Collection Method::Algset_ZBLL("algsets/ZBLL.xml", true);
    const Collection Method::Algset_OCLL("algsets/OCLL.xml", true);
    const Collection Method::Algset_CMLL("algsets/CMLL.xml", true);
    const Collection Method::Algset_COLL("algsets/COLL.xml", true);
    const Collection Method::Algset_EPLL("algsets/EPLL.xml", true);
    const Collection Method::Algset_2GLL("algsets/2GLL.xml", true);

    const Collection Method::Algset_EOLE("algsets/EOLE.xml", true);
    const Collection Method::Algset_6CO("algsets/6CO.xml", true);
    const Collection Method::Algset_6CP("algsets/6CP.xml", true);
    const Collection Method::Algset_APDR("algsets/APDR.xml", true);
    const Collection Method::Algset_CDRLL("algsets/CDRLL.xml", true);
    const Collection Method::Algset_DCAL("algsets/DCAL.xml", true);
    const Collection Method::Algset_JTLE("algsets/JTLE.xml", true);
    const Collection Method::Algset_L5EP("algsets/L5EP.xml", true);
    const Collection Method::Algset_TDR("algsets/TDR.xml", true);

    const Collection Method::Algset_NCLL("algsets/NCLL.xml", true);
    const Collection Method::Algset_NCOLL("algsets/NCOLL.xml", true);
    const Collection Method::Algset_TNCLL("algsets/TNCLL.xml", true);
    const Collection Method::Algset_L5E("algsets/L5E.xml", true);
    const Collection Method::Algset_EODF("algsets/EODF.xml", true);
	
	DeepSearch Method::DS(""); // static initialization

    // Search an algorithm from a collection to orientate the last layer
	bool Method::OrientateLL(Algorithm& LLSolve, std::string &LLCase, const Collection& AlgSet,
                             const Plc Pol, const Metrics Met, const Cube& CubeBase)
	{
		LLSolve.Clear();
		LLCase.clear();

        // Only orientation algorithm collections
        if (AlgSet.GetName() != "OLL" && AlgSet.GetName() != "OCLL") return false;
			
		const Lyr UpLayer = Cube::GetUpSliceLayer(CubeBase.GetSpin()),
				  MidLayer = Cube::AdjacentLayer(UpLayer),
				  DownLayer = Cube::GetDownSliceLayer(CubeBase.GetSpin());

		bool LL_Found = false;
        Spn Spin1, Spin2; // Cube spins before and after applying algorithm
        Stp StepS1, StepS2; // Steps to return cube to the start spin

        for (uint LL_Index = 0u; LL_Index < AlgSet.GetCasesNumber(); LL_Index++)
        {
            Algorithm Alg = AlgSet.GetAlgorithm(LL_Index, Pol, Met);

            for (const auto yLL: Algorithm::yTurns) // y turn before algorithm
            {
				Cube CubeLL = CubeBase;
				CubeLL.ApplyStep(yLL);
				Spin1 = CubeLL.GetSpin();
				CubeLL.ApplyAlgorithm(Alg);
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
					LLCase = AlgSet.GetCaseName(LL_Index);

					if (yLL != Stp::NONE)
					{
						// LLSolve.Append(Stp::PARENTHESIS_OPEN);
						LLSolve.Append(yLL);
						// LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
					}

					LLSolve.Append(Alg);
					
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
	bool Method::SolveLL(Algorithm& LLSolve, std::string &LLCase, Stp& AUFStep, const Collection& AlgSet,
                         const Plc Pol, const Metrics Met, const Cube& CubeBase)
	{	
		LLSolve.Clear();
		LLCase.clear();	
		AUFStep = Stp::NONE;

        // Allowed collections
        if (AlgSet.GetName() != "1LLL" && AlgSet.GetName() != "PLL" && AlgSet.GetName() != "ZBLL" && 
            AlgSet.GetName() != "EPLL" && AlgSet.GetName() != "2GLL") return false;

		bool LL_Found = false;
        Spn Spin1, Spin2; // Cube spins before and after applying algorithm
        Stp StepS1, StepS2; // Steps to return cube to the start spin

        for (uint LL_Index = 0u; LL_Index < AlgSet.GetCasesNumber(); LL_Index++)
        {
            Algorithm Alg = AlgSet.GetAlgorithm(LL_Index, Pol, Met);

            if (AlgSet.GetName() == "2GLL")
            {
                for (const auto ULL1 : Algorithm::UMovs) // U movement before algorithm
                {
                    for (const auto ULL2 : Algorithm::UMovs) // U movement after algorithm (AUF)
                    {
                        Cube CubeLL = CubeBase;
                        CubeLL.ApplyStep(ULL1);
                        Spin1 = CubeLL.GetSpin();
                        CubeLL.ApplyAlgorithm(Alg);
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
                            LLCase = AlgSet.GetCaseName(LL_Index);

                            if (ULL1 != Stp::NONE)
                            {
                                // LLSolve.Append(Stp::PARENTHESIS_OPEN);
                                LLSolve.Append(ULL1);
                                // LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
                            }
                            LLSolve.Append(Alg);
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
                        CubeLL.ApplyAlgorithm(Alg);
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
                            LLCase = AlgSet.GetCaseName(LL_Index);

                            if (yLL != Stp::NONE)
                            {
                                // LLSolve.Append(Stp::PARENTHESIS_OPEN);
                                LLSolve.Append(yLL);
                                // LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
                            }
                            LLSolve.Append(Alg);
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
	bool Method::CornersLL(Algorithm& LLSolve, std::string &LLCase, Stp& AUFStep, const Collection& AlgSet,
                           const Plc Pol, const Metrics Met, const Cube& CubeBase, const bool EdgesOriented)
	{
		LLSolve.Clear();
		LLCase.clear();
        AUFStep = Stp::NONE;

        // Allowed algsets
        if (AlgSet.GetName() != "CMLL" && AlgSet.GetName() != "COLL" && AlgSet.GetName() != "NCLL" && 
            AlgSet.GetName() != "NCOLL" && AlgSet.GetName() != "TNCLL") return false;
		
		const Lyr UpLayer = Cube::GetUpSliceLayer(CubeBase.GetSpin()),
				  MidLayer = Cube::AdjacentLayer(UpLayer);
				  
		Pgr UpLayerCorners, UpLayerEdges, DownLayerCorners;
		switch(UpLayer)
		{
        case Lyr::U: UpLayerCorners = Pgr::CORNERS_U; UpLayerEdges = Pgr::CROSS_U; DownLayerCorners = Pgr::CORNERS_D; break;
        case Lyr::D: UpLayerCorners = Pgr::CORNERS_D; UpLayerEdges = Pgr::CROSS_D; DownLayerCorners = Pgr::CORNERS_U; break;
        case Lyr::F: UpLayerCorners = Pgr::CORNERS_F; UpLayerEdges = Pgr::CROSS_F; DownLayerCorners = Pgr::CORNERS_B; break;
        case Lyr::B: UpLayerCorners = Pgr::CORNERS_B; UpLayerEdges = Pgr::CROSS_B; DownLayerCorners = Pgr::CORNERS_F; break;
        case Lyr::R: UpLayerCorners = Pgr::CORNERS_R; UpLayerEdges = Pgr::CROSS_R; DownLayerCorners = Pgr::CORNERS_L; break;
        case Lyr::L: UpLayerCorners = Pgr::CORNERS_L; UpLayerEdges = Pgr::CROSS_L; DownLayerCorners = Pgr::CORNERS_R; break;
		default: return false;
		}

		bool LL_Found = false;
        Spn Spin1, Spin2; // Cube spins before and after applying algorithm
        Stp StepS1, StepS2; // Steps to return cube to the start spin

        for (uint LL_Index = 0u; LL_Index < AlgSet.GetCasesNumber(); LL_Index++)
        {
            Algorithm Alg = AlgSet.GetAlgorithm(LL_Index, Pol, Met);

            for (const auto ULL1: Algorithm::UMovs) // U movement before algorithm
            {
                for (const auto ULL2: Algorithm::UMovs) // U movement after algorithm (AUF)
                {
                    Cube CubeLL = CubeBase;
                    CubeLL.ApplyStep(ULL1);
                    Spin1 = CubeLL.GetSpin();
                    CubeLL.ApplyAlgorithm(Alg);
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
                                CubeLL.IsSolved(Cube::FromAbsPosition(App::DR, CubeLL.GetSpin())) && 
                                CubeLL.IsSolved(Cube::FromAbsPosition(App::DL, CubeLL.GetSpin()));

                    if (LL_Found && EdgesOriented) LL_Found = CubeLL.CheckOrientation(UpLayerEdges);

                    if (LL_Found)
                    {
                        LLCase = AlgSet.GetCaseName(LL_Index);

                        if (ULL1 != Stp::NONE)
                        {
                            // LLSolve.Append(Stp::PARENTHESIS_OPEN);
                            LLSolve.Append(ULL1);
                            // LLSolve.Append(Stp::PARENTHESIS_CLOSE_1_REP);
                        }
                        LLSolve.Append(Alg);
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


    // Common functions for methods

     // Set spins for search
    void Method::SetSearchSpins(const std::vector<Spn>& SS)
    {
        SearchSpins.clear();
        if (SS.empty()) for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s));
        else for (const auto s : SS) SearchSpins.push_back(s);
    }
	
    // Get the best solve report
    std::string Method::GetBestReport(const bool Cancellations) const
    {
        float M, min_M = 0.0f;
		uint Bestn = 0u;
        Spn BestSpin = Spn::Default;

        for (const auto spin : SearchSpins)
        {
            const int sp = static_cast<int>(spin);

            if (!CheckSolveConsistency(spin)) continue;

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                if (!IsSolved(spin, n)) continue;

                if (Cancellations) M = GetMetricCancellations(spin, n);
                else M = GetMetricSolve(spin, n);
				
                if (min_M == 0.0f || M < min_M)
                {
                    min_M = M;
                    BestSpin = spin;
                    Bestn = n;
                }
            }
        }
        if (min_M == 0.0f) return "No solves!\n";

        if (Cancellations)
        {
            Algorithm C = GetCancellations(BestSpin, Bestn);
            return GetReport(BestSpin, Bestn) + "\nCancellations (" + Algorithm::GetMetricValue(C.GetMetric(Metric)) + " " + 
			       Algorithm::GetMetricString(Metric) + "): " + C.ToString() + "\n";
        }

        return GetReport(BestSpin, Bestn);
    }

    // Static functions for check structures solved in the cube
	
	// Check if the CFOP first cross is built for given cross layer
    bool Method::IsCrossBuilt(const Cube& C, const Lyr CrossLayer)
    {
        Cube CAux = C;
		switch (CrossLayer)
		{
		case Lyr::U: CAux.SetSpin(Spn::DF); break;
		case Lyr::D: CAux.SetSpin(Spn::UF); break;
		case Lyr::F: CAux.SetSpin(Spn::BU); break;
		case Lyr::B: CAux.SetSpin(Spn::FU); break;
		case Lyr::R: CAux.SetSpin(Spn::LU); break;
		case Lyr::L: CAux.SetSpin(Spn::RU); break;
		default: return false;
		}
        
        return IsCrossBuilt(CAux);
    }

	// Check if the CFOP first cross is built for given spin
    bool Method::IsCrossBuilt(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
		CAux.SetSpin(spin);
        return IsCrossBuilt(CAux);
    }
	
	// Check if the CFOP first cross is built
    bool Method::IsCrossBuilt(const Cube& C)
    {
        switch (Cube::GetDownSliceLayer(C.GetSpin()))
        {
        case Lyr::U: return C.IsSolved(Pgr::CROSS_U);
        case Lyr::D: return C.IsSolved(Pgr::CROSS_D);
        case Lyr::F: return C.IsSolved(Pgr::CROSS_F);
        case Lyr::B: return C.IsSolved(Pgr::CROSS_B);
        case Lyr::R: return C.IsSolved(Pgr::CROSS_R);
        case Lyr::L: return C.IsSolved(Pgr::CROSS_L);
        default: return false;
        }
    }

    // Check if the CFOP first XCross is built for given cross layer (any XCross)
    bool Method::IsXCrossBuilt(const Cube& C, const Lyr CrossLayer)
    {
        Cube CAux = C;
		switch (CrossLayer)
		{
		case Lyr::U: CAux.SetSpin(Spn::DF); break;
		case Lyr::D: CAux.SetSpin(Spn::UF); break;
		case Lyr::F: CAux.SetSpin(Spn::BU); break;
		case Lyr::B: CAux.SetSpin(Spn::FU); break;
		case Lyr::R: CAux.SetSpin(Spn::LU); break;
		case Lyr::L: CAux.SetSpin(Spn::RU); break;
		default: return false;
		}
        
        return IsXCrossBuilt(CAux);
    }
	
    // Check if the CFOP first XCross is built for given spin (any XCross)
    bool Method::IsXCrossBuilt(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
		CAux.SetSpin(spin);
        return IsXCrossBuilt(CAux);
    }	
	
	// Check if the CFOP first XCross is built
    bool Method::IsXCrossBuilt(const Cube& C)
    {
        switch (Cube::GetDownSliceLayer(C.GetSpin()))
        {
        case Lyr::U: return C.IsSolved(Pgr::CROSS_U) && 
                            (C.IsSolved(Pgr::F2L_U_UFR) || C.IsSolved(Pgr::F2L_U_UFL) || 
                             C.IsSolved(Pgr::F2L_U_UBR) || C.IsSolved(Pgr::F2L_U_UBL));
        case Lyr::D: return C.IsSolved(Pgr::CROSS_D) && 
                            (C.IsSolved(Pgr::F2L_D_DFR) || C.IsSolved(Pgr::F2L_D_DFL) || 
                             C.IsSolved(Pgr::F2L_D_DBR) || C.IsSolved(Pgr::F2L_D_DBL));
        case Lyr::F: return C.IsSolved(Pgr::CROSS_F) && 
                            (C.IsSolved(Pgr::F2L_F_UFR) || C.IsSolved(Pgr::F2L_F_UFL) || 
                             C.IsSolved(Pgr::F2L_F_DFR) || C.IsSolved(Pgr::F2L_F_DFL));
        case Lyr::B: return C.IsSolved(Pgr::CROSS_B) && 
                            (C.IsSolved(Pgr::F2L_B_UBR) || C.IsSolved(Pgr::F2L_B_UBL) || 
                             C.IsSolved(Pgr::F2L_B_DBR) || C.IsSolved(Pgr::F2L_B_DBL));
        case Lyr::R: return C.IsSolved(Pgr::CROSS_R) && 
                            (C.IsSolved(Pgr::F2L_R_UFR) || C.IsSolved(Pgr::F2L_R_UBR) || 
                             C.IsSolved(Pgr::F2L_R_DFR) || C.IsSolved(Pgr::F2L_R_DBR));
        case Lyr::L: return C.IsSolved(Pgr::CROSS_L) && 
                            (C.IsSolved(Pgr::F2L_L_UFL) || C.IsSolved(Pgr::F2L_L_UBL) || 
                             C.IsSolved(Pgr::F2L_L_DFL) || C.IsSolved(Pgr::F2L_L_DBL));
        default: return false;
        }
    }

    // Check if first two layers are built for given cross layer (two layers)
    bool Method::IsF2LBuilt(const Cube& C, const Lyr CrossLayer)
    {
        Cube CAux = C;
		switch (CrossLayer)
		{
		case Lyr::U: CAux.SetSpin(Spn::DF); break;
		case Lyr::D: CAux.SetSpin(Spn::UF); break;
		case Lyr::F: CAux.SetSpin(Spn::BU); break;
		case Lyr::B: CAux.SetSpin(Spn::FU); break;
		case Lyr::R: CAux.SetSpin(Spn::LU); break;
		case Lyr::L: CAux.SetSpin(Spn::RU); break;
		default: return false;
		}
        
        return IsF2LBuilt(CAux);
    }
	
	// Check if first two layers are built for given spin (two layers)
    bool Method::IsF2LBuilt(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
		CAux.SetSpin(spin);
        return IsF2LBuilt(CAux);
    }
	
	// Check if F2L are built (two layers)
    bool Method::IsF2LBuilt(const Cube& C)
    {
        switch (Cube::GetDownSliceLayer(C.GetSpin()))
        {
        case Lyr::U: return C.IsSolved(Pgr::F2L_U);
        case Lyr::D: return C.IsSolved(Pgr::F2L_D);
        case Lyr::F: return C.IsSolved(Pgr::F2L_F);
        case Lyr::B: return C.IsSolved(Pgr::F2L_B);
        case Lyr::R: return C.IsSolved(Pgr::F2L_R);
        case Lyr::L: return C.IsSolved(Pgr::F2L_L);
        default: return false;
        }
    }

    // Check if F2L pairs are built for given cross layer (no cross check)
    bool Method::IsF2LPairsBuilt(const Cube& C, const Lyr CrossLayer)
    {
        Cube CAux = C;
		switch (CrossLayer)
		{
		case Lyr::U: CAux.SetSpin(Spn::DF); break;
		case Lyr::D: CAux.SetSpin(Spn::UF); break;
		case Lyr::F: CAux.SetSpin(Spn::BU); break;
		case Lyr::B: CAux.SetSpin(Spn::FU); break;
		case Lyr::R: CAux.SetSpin(Spn::LU); break;
		case Lyr::L: CAux.SetSpin(Spn::RU); break;
		default: return false;
		}
        
        return IsF2LPairsBuilt(CAux);
    }
	
	// Check if F2L pairs are built for given spin (no cross check)
    bool Method::IsF2LPairsBuilt(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
		CAux.SetSpin(spin);
        return IsF2LPairsBuilt(CAux);
    }
	
	// Check if all four F2L pairs are built (no cross check)
    bool Method::IsF2LPairsBuilt(const Cube& C)
    {
        switch (Cube::GetDownSliceLayer(C.GetSpin()))
        {
        case Lyr::U: return C.IsSolved(Pgr::F2L_U_UFR) && C.IsSolved(Pgr::F2L_U_UFL) && 
                            C.IsSolved(Pgr::F2L_U_UBR) && C.IsSolved(Pgr::F2L_U_UBL);
        case Lyr::D: return C.IsSolved(Pgr::F2L_D_DFR) && C.IsSolved(Pgr::F2L_D_DFL) &&
                            C.IsSolved(Pgr::F2L_D_DBR) && C.IsSolved(Pgr::F2L_D_DBL);
        case Lyr::F: return C.IsSolved(Pgr::F2L_F_UFR) && C.IsSolved(Pgr::F2L_F_UFL) &&
                            C.IsSolved(Pgr::F2L_F_DFR) && C.IsSolved(Pgr::F2L_F_DFL);
        case Lyr::B: return C.IsSolved(Pgr::F2L_B_UBR) && C.IsSolved(Pgr::F2L_B_UBL) &&
                            C.IsSolved(Pgr::F2L_B_DBR) && C.IsSolved(Pgr::F2L_B_DBL);
        case Lyr::R: return C.IsSolved(Pgr::F2L_R_UFR) && C.IsSolved(Pgr::F2L_R_UBR) &&
                            C.IsSolved(Pgr::F2L_R_DFR) && C.IsSolved(Pgr::F2L_R_DBR);
        case Lyr::L: return C.IsSolved(Pgr::F2L_L_UFL) && C.IsSolved(Pgr::F2L_L_UBL) &&
                            C.IsSolved(Pgr::F2L_L_DFL) && C.IsSolved(Pgr::F2L_L_DBL);
        default: return false;
        }
    }

    // Get if the given F2L pair is adjacent (the corner next to the edge), orientation is not checked
    bool Method::IsF2LAdjacent(const Pgr F2L, const Cube& C)
    {
        std::vector<Pcp> F2L_List;
        Cube::AddToPositionsList(F2L, F2L_List);
        if (F2L_List.size() != 2) return false;

        const Pcp F2L_Corner = C.GetPiecePosition(static_cast<Pce>(F2L_List[0])),
            F2L_Edge = C.GetPiecePosition(static_cast<Pce>(F2L_List[1]));

        switch (F2L_Corner)
        {
        case Pcp::UFR: return F2L_Edge == Pcp::UF || F2L_Edge == Pcp::UR || F2L_Edge == Pcp::FR;
        case Pcp::UFL: return F2L_Edge == Pcp::UF || F2L_Edge == Pcp::UL || F2L_Edge == Pcp::FL;
        case Pcp::UBR: return F2L_Edge == Pcp::UB || F2L_Edge == Pcp::UR || F2L_Edge == Pcp::BR;
        case Pcp::UBL: return F2L_Edge == Pcp::UB || F2L_Edge == Pcp::UL || F2L_Edge == Pcp::BL;

        case Pcp::DFR: return F2L_Edge == Pcp::DF || F2L_Edge == Pcp::DR || F2L_Edge == Pcp::FR;
        case Pcp::DFL: return F2L_Edge == Pcp::DF || F2L_Edge == Pcp::DL || F2L_Edge == Pcp::FL;
        case Pcp::DBR: return F2L_Edge == Pcp::DB || F2L_Edge == Pcp::DR || F2L_Edge == Pcp::BR;
        case Pcp::DBL: return F2L_Edge == Pcp::DB || F2L_Edge == Pcp::DL || F2L_Edge == Pcp::BL;

        default: return false;
        }
    }

    // Check if the common colors for F2L pair are in the same face
    bool Method::IsF2LOriented(const Pgr F2L, const Cube& C)
    {
        std::vector<Pcp> F2L_List;
        Cube::AddToPositionsList(F2L, F2L_List);
        if (F2L_List.size() != 2) return false;

        const Pcp F2L_Corner = C.GetPiecePosition(static_cast<Pce>(F2L_List[0])),
            F2L_Edge = C.GetPiecePosition(static_cast<Pce>(F2L_List[1]));

        Sps Edge_Sticker_1, Edge_Sticker_2, Corner_Sticker_1, Corner_Sticker_2, Corner_Sticker_3;
        switch (F2L_Edge)
        {
        case Pcp::UF: Edge_Sticker_1 = Sps::UF_U; Edge_Sticker_2 = Sps::UF_F; break;
        case Pcp::UB: Edge_Sticker_1 = Sps::UB_U; Edge_Sticker_2 = Sps::UB_B; break;
        case Pcp::UR: Edge_Sticker_1 = Sps::UR_U; Edge_Sticker_2 = Sps::UR_R; break;
        case Pcp::UL: Edge_Sticker_1 = Sps::UL_U; Edge_Sticker_2 = Sps::UL_L; break;
        case Pcp::DF: Edge_Sticker_1 = Sps::DF_D; Edge_Sticker_2 = Sps::DF_F; break;
        case Pcp::DB: Edge_Sticker_1 = Sps::DB_D; Edge_Sticker_2 = Sps::DB_B; break;
        case Pcp::DR: Edge_Sticker_1 = Sps::DR_D; Edge_Sticker_2 = Sps::DR_R; break;
        case Pcp::DL: Edge_Sticker_1 = Sps::DL_D; Edge_Sticker_2 = Sps::DL_L; break;
        case Pcp::FR: Edge_Sticker_1 = Sps::FR_F; Edge_Sticker_2 = Sps::FR_R; break;
        case Pcp::FL: Edge_Sticker_1 = Sps::FL_F; Edge_Sticker_2 = Sps::FL_L; break;
        case Pcp::BR: Edge_Sticker_1 = Sps::BR_B; Edge_Sticker_2 = Sps::BR_R; break;
        case Pcp::BL: Edge_Sticker_1 = Sps::BL_B; Edge_Sticker_2 = Sps::BL_L; break;
        default: return false;
        }

        switch (F2L_Corner)
        {
        case Pcp::UFR: Corner_Sticker_1 = Sps::UFR_U; Corner_Sticker_2 = Sps::UFR_F; Corner_Sticker_3 = Sps::UFR_R; break;
        case Pcp::UFL: Corner_Sticker_1 = Sps::UFL_U; Corner_Sticker_2 = Sps::UFL_F; Corner_Sticker_3 = Sps::UFL_L; break;
        case Pcp::UBR: Corner_Sticker_1 = Sps::UBR_U; Corner_Sticker_2 = Sps::UBR_B; Corner_Sticker_3 = Sps::UBR_R; break;
        case Pcp::UBL: Corner_Sticker_1 = Sps::UBL_U; Corner_Sticker_2 = Sps::UBL_B; Corner_Sticker_3 = Sps::UBL_L; break;
        case Pcp::DFR: Corner_Sticker_1 = Sps::DFR_D; Corner_Sticker_2 = Sps::DFR_F; Corner_Sticker_3 = Sps::DFR_R; break;
        case Pcp::DFL: Corner_Sticker_1 = Sps::DFL_D; Corner_Sticker_2 = Sps::DFL_F; Corner_Sticker_3 = Sps::DFL_L; break;
        case Pcp::DBR: Corner_Sticker_1 = Sps::DBR_D; Corner_Sticker_2 = Sps::DBR_B; Corner_Sticker_3 = Sps::DBR_R; break;
        case Pcp::DBL: Corner_Sticker_1 = Sps::DBL_D; Corner_Sticker_2 = Sps::DBL_B; Corner_Sticker_3 = Sps::DBL_L; break;
        default: return false;
        }

        Fce ES1 = C.GetFace(Edge_Sticker_1), ES2 = C.GetFace(Edge_Sticker_2),
            CS1 = C.GetFace(Corner_Sticker_1), CS2 = C.GetFace(Corner_Sticker_2), CS3 = C.GetFace(Corner_Sticker_3);

        return ((ES1 == CS1) && (ES2 == CS2)) || ((ES1 == CS1) && (ES2 == CS1)) || ((ES1 == CS1) && (ES2 == CS3)) ||
            ((ES1 == CS3) && (ES2 == CS1)) || ((ES1 == CS2) && (ES2 == CS3)) || ((ES1 == CS3) && (ES2 == CS2));
    }

    // Check if the left Roux/Nautilus block is built (first block)
    bool Method::IsRouxFBBuilt(const Cube& C)
    {
        // Nautilus cube with desired spin
        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::UF_B1);
        case Spn::UB: return C.IsSolved(Pgr::UB_B1);
        case Spn::UR: return C.IsSolved(Pgr::UR_B1);
        case Spn::UL: return C.IsSolved(Pgr::UL_B1);

        case Spn::DF: return C.IsSolved(Pgr::DF_B1);
        case Spn::DB: return C.IsSolved(Pgr::DB_B1);
        case Spn::DR: return C.IsSolved(Pgr::DR_B1);
        case Spn::DL: return C.IsSolved(Pgr::DL_B1);

        case Spn::FU: return C.IsSolved(Pgr::FU_B1);
        case Spn::FD: return C.IsSolved(Pgr::FD_B1);
        case Spn::FR: return C.IsSolved(Pgr::FR_B1);
        case Spn::FL: return C.IsSolved(Pgr::FL_B1);

        case Spn::BU: return C.IsSolved(Pgr::BU_B1);
        case Spn::BD: return C.IsSolved(Pgr::BD_B1);
        case Spn::BR: return C.IsSolved(Pgr::BR_B1);
        case Spn::BL: return C.IsSolved(Pgr::BL_B1);

        case Spn::RU: return C.IsSolved(Pgr::RU_B1);
        case Spn::RD: return C.IsSolved(Pgr::RD_B1);
        case Spn::RF: return C.IsSolved(Pgr::RF_B1);
        case Spn::RB: return C.IsSolved(Pgr::RB_B1);

        case Spn::LU: return C.IsSolved(Pgr::LU_B1);
        case Spn::LD: return C.IsSolved(Pgr::LD_B1);
        case Spn::LF: return C.IsSolved(Pgr::LF_B1);
        case Spn::LB: return C.IsSolved(Pgr::LB_B1);

        default: return false; // Should not happend
        }
    }

    // Check if the first Roux/Nautilus block is built with the given spin
    bool Method::IsRouxFBBuilt(const Cube& C, const Spn S)
    {
        Cube CRoux = C;
        CRoux.SetSpin(S);
        return IsRouxFBBuilt(CRoux);
    }

    // Check if the first Roux square block is built with the given spin
    bool Method::IsRouxFBSquareBuilt(const Cube& C)
    {
        // Roux cube with desired spin
        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::UF_B1S2);
        case Spn::UB: return C.IsSolved(Pgr::UB_B1S2);
        case Spn::UR: return C.IsSolved(Pgr::UR_B1S2);
        case Spn::UL: return C.IsSolved(Pgr::UL_B1S2);

        case Spn::DF: return C.IsSolved(Pgr::DF_B1S2);
        case Spn::DB: return C.IsSolved(Pgr::DB_B1S2);
        case Spn::DR: return C.IsSolved(Pgr::DR_B1S2);
        case Spn::DL: return C.IsSolved(Pgr::DL_B1S2);

        case Spn::FU: return C.IsSolved(Pgr::FU_B1S2);
        case Spn::FD: return C.IsSolved(Pgr::FD_B1S2);
        case Spn::FR: return C.IsSolved(Pgr::FR_B1S2);
        case Spn::FL: return C.IsSolved(Pgr::FL_B1S2);

        case Spn::BU: return C.IsSolved(Pgr::BU_B1S2);
        case Spn::BD: return C.IsSolved(Pgr::BD_B1S2);
        case Spn::BR: return C.IsSolved(Pgr::BR_B1S2);
        case Spn::BL: return C.IsSolved(Pgr::BL_B1S2);

        case Spn::RU: return C.IsSolved(Pgr::RU_B1S2);
        case Spn::RD: return C.IsSolved(Pgr::RD_B1S2);
        case Spn::RF: return C.IsSolved(Pgr::RF_B1S2);
        case Spn::RB: return C.IsSolved(Pgr::RB_B1S2);

        case Spn::LU: return C.IsSolved(Pgr::LU_B1S2);
        case Spn::LD: return C.IsSolved(Pgr::LD_B1S2);
        case Spn::LF: return C.IsSolved(Pgr::LF_B1S2);
        case Spn::LB: return C.IsSolved(Pgr::LB_B1S2);

        default: return false; // Should not happend
        }
    }

    // Check if the first Roux square block is built with the given spin
    bool Method::IsRouxFBSquareBuilt(const Cube& C, const Spn S)
    {
        Cube CRoux = C;
        CRoux.SetSpin(S);
        return IsRouxFBSquareBuilt(CRoux);
    }

    // Check if the right Roux block is built (second block)
    bool Method::IsRouxSBBuilt(const Cube& C)
    {
        // Roux cube with desired spin
        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::UF_B2);
        case Spn::UB: return C.IsSolved(Pgr::UB_B2);
        case Spn::UR: return C.IsSolved(Pgr::UR_B2);
        case Spn::UL: return C.IsSolved(Pgr::UL_B2);

        case Spn::DF: return C.IsSolved(Pgr::DF_B2);
        case Spn::DB: return C.IsSolved(Pgr::DB_B2);
        case Spn::DR: return C.IsSolved(Pgr::DR_B2);
        case Spn::DL: return C.IsSolved(Pgr::DL_B2);

        case Spn::FU: return C.IsSolved(Pgr::FU_B2);
        case Spn::FD: return C.IsSolved(Pgr::FD_B2);
        case Spn::FR: return C.IsSolved(Pgr::FR_B2);
        case Spn::FL: return C.IsSolved(Pgr::FL_B2);

        case Spn::BU: return C.IsSolved(Pgr::BU_B2);
        case Spn::BD: return C.IsSolved(Pgr::BD_B2);
        case Spn::BR: return C.IsSolved(Pgr::BR_B2);
        case Spn::BL: return C.IsSolved(Pgr::BL_B2);

        case Spn::RU: return C.IsSolved(Pgr::RU_B2);
        case Spn::RD: return C.IsSolved(Pgr::RD_B2);
        case Spn::RF: return C.IsSolved(Pgr::RF_B2);
        case Spn::RB: return C.IsSolved(Pgr::RB_B2);

        case Spn::LU: return C.IsSolved(Pgr::LU_B2);
        case Spn::LD: return C.IsSolved(Pgr::LD_B2);
        case Spn::LF: return C.IsSolved(Pgr::LF_B2);
        case Spn::LB: return C.IsSolved(Pgr::LB_B2);

        default: return false; // Should not happend
        }
    }
    // Check if the right Roux block is built (second block) with the given spin
    bool Method::IsRouxSBBuilt(const Cube& C, const Spn S)
    {
        Cube CRoux = C;
        CRoux.SetSpin(S);
        return IsRouxSBBuilt(CRoux);
    }

    // Check if the left Roux block is built and an square in the right block
    bool Method::IsRouxFBAndSBSquareBuilt(const Cube& C)
    {
        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::UF_B1) &&
            (C.IsSolved(Pgr::UF_B2S1) || C.IsSolved(Pgr::UF_B2S2));
        case Spn::UB: return C.IsSolved(Pgr::UB_B1) &&
            (C.IsSolved(Pgr::UB_B2S1) || C.IsSolved(Pgr::UB_B2S2));
        case Spn::UR: return C.IsSolved(Pgr::UR_B1) &&
            (C.IsSolved(Pgr::UR_B2S1) || C.IsSolved(Pgr::UR_B2S2));
        case Spn::UL: return C.IsSolved(Pgr::UL_B1) &&
            (C.IsSolved(Pgr::UL_B2S1) || C.IsSolved(Pgr::UL_B2S2));

        case Spn::DF: return C.IsSolved(Pgr::DF_B1) &&
            (C.IsSolved(Pgr::DF_B2S1) || C.IsSolved(Pgr::DF_B2S2));
        case Spn::DB: return C.IsSolved(Pgr::DB_B1) &&
            (C.IsSolved(Pgr::DB_B2S1) || C.IsSolved(Pgr::DB_B2S2));
        case Spn::DR: return C.IsSolved(Pgr::DR_B1) &&
            (C.IsSolved(Pgr::DR_B2S1) || C.IsSolved(Pgr::DR_B2S2));
        case Spn::DL: return C.IsSolved(Pgr::DL_B1) &&
            (C.IsSolved(Pgr::DL_B2S1) || C.IsSolved(Pgr::DL_B2S2));

        case Spn::FU: return C.IsSolved(Pgr::FU_B1) &&
            (C.IsSolved(Pgr::FU_B2S1) || C.IsSolved(Pgr::FU_B2S2));
        case Spn::FD: return C.IsSolved(Pgr::FD_B1) &&
            (C.IsSolved(Pgr::FD_B2S1) || C.IsSolved(Pgr::FD_B2S2));
        case Spn::FR: return C.IsSolved(Pgr::FR_B1) &&
            (C.IsSolved(Pgr::FR_B2S1) || C.IsSolved(Pgr::FR_B2S2));
        case Spn::FL: return C.IsSolved(Pgr::FL_B1) &&
            (C.IsSolved(Pgr::FL_B2S1) || C.IsSolved(Pgr::FL_B2S2));

        case Spn::BU: return C.IsSolved(Pgr::BU_B1) &&
            (C.IsSolved(Pgr::BU_B2S1) || C.IsSolved(Pgr::BU_B2S2));
        case Spn::BD: return C.IsSolved(Pgr::BD_B1) &&
            (C.IsSolved(Pgr::BD_B2S1) || C.IsSolved(Pgr::BD_B2S2));
        case Spn::BR: return C.IsSolved(Pgr::BR_B1) &&
            (C.IsSolved(Pgr::BR_B2S1) || C.IsSolved(Pgr::BR_B2S2));
        case Spn::BL: return C.IsSolved(Pgr::BL_B1) &&
            (C.IsSolved(Pgr::BL_B2S1) || C.IsSolved(Pgr::BL_B2S2));

        case Spn::RU: return C.IsSolved(Pgr::RU_B1) &&
            (C.IsSolved(Pgr::RU_B2S1) || C.IsSolved(Pgr::RU_B2S2));
        case Spn::RD: return C.IsSolved(Pgr::RD_B1) &&
            (C.IsSolved(Pgr::RD_B2S1) || C.IsSolved(Pgr::RD_B2S2));
        case Spn::RF: return C.IsSolved(Pgr::RF_B1) &&
            (C.IsSolved(Pgr::RF_B2S1) || C.IsSolved(Pgr::RF_B2S2));
        case Spn::RB: return C.IsSolved(Pgr::RB_B1) &&
            (C.IsSolved(Pgr::RB_B2S1) || C.IsSolved(Pgr::RB_B2S2));

        case Spn::LU: return C.IsSolved(Pgr::LU_B1) &&
            (C.IsSolved(Pgr::LU_B2S1) || C.IsSolved(Pgr::LU_B2S2));
        case Spn::LD: return C.IsSolved(Pgr::LD_B1) &&
            (C.IsSolved(Pgr::LD_B2S1) || C.IsSolved(Pgr::LD_B2S2));
        case Spn::LF: return C.IsSolved(Pgr::LF_B1) &&
            (C.IsSolved(Pgr::LF_B2S1) || C.IsSolved(Pgr::LF_B2S2));
        case Spn::LB: return C.IsSolved(Pgr::LB_B1) &&
            (C.IsSolved(Pgr::LB_B2S1) || C.IsSolved(Pgr::LB_B2S2));

        default: return false; // Should not happend
        }
    }

    // Check if the left Roux block is built and an square in the right block with the given spin
    bool Method::IsRouxFBAndSBSquareBuilt(const Cube& C, const Spn S)
    {
        Cube CRoux = C;
        CRoux.SetSpin(S);
        return IsRouxFBAndSBSquareBuilt(CRoux);
    }

    // Check if the upper layer corners (CMLL) are built
    bool Method::IsCMLLBuilt(const Cube& C)
    {
        switch (C.GetSpin())
        {
        case Spn::UF:
        case Spn::UB:
        case Spn::UR:
        case Spn::UL: return C.IsSolved(Pgr::CORNERS_U);

        case Spn::DF:
        case Spn::DB:
        case Spn::DR:
        case Spn::DL: return C.IsSolved(Pgr::CORNERS_D);

        case Spn::FU:
        case Spn::FD:
        case Spn::FR:
        case Spn::FL: return C.IsSolved(Pgr::CORNERS_F);

        case Spn::BU:
        case Spn::BD:
        case Spn::BR:
        case Spn::BL: return C.IsSolved(Pgr::CORNERS_B);

        case Spn::RU:
        case Spn::RD:
        case Spn::RF:
        case Spn::RB: return C.IsSolved(Pgr::CORNERS_R);

        case Spn::LU:
        case Spn::LD:
        case Spn::LF:
        case Spn::LB: return C.IsSolved(Pgr::CORNERS_L);

        default: return false; // Should not happend
        }
    }

    // Check if the upper layer corners (CMLL) are built
    bool Method::IsCMLLBuilt(const Cube& C, const Spn S)
    {
        Cube CRoux = C;
        CRoux.SetSpin(S);
        return IsCMLLBuilt(CRoux);
    }

    // Check if the last six edges are oriented in roux method
    bool Method::IsRouxL6EOriented(const Cube& c) 
    {
        return AreRouxBlocksBuilt(c) &&
               c.IsFaceOriented2(Cube::GetUpSliceLayer(c.GetSpin())) &&
               c.IsFaceOriented2(Cube::GetDownSliceLayer(c.GetSpin()));
    }

    // Check if the last six edges are oriented in roux method with the given spin
    bool Method::IsRouxL6EOriented(const Cube& C, const Spn S)
    {
        Cube CRoux = C;
        CRoux.SetSpin(S);
        return IsRouxL6EOriented(CRoux);
    }

    // Check if the last six edges oriented and two edges solved in roux method
    bool Method::IsRouxL6EO2E(const Cube& c) 
    {
        return c.IsSolved(Cube::GetLeftSliceLayer(c.GetSpin())) && c.IsSolved(Cube::GetRightSliceLayer(c.GetSpin())) &&
            c.IsFaceOriented2(Cube::GetUpSliceLayer(c.GetSpin())) && c.IsFaceOriented2(Cube::GetDownSliceLayer(c.GetSpin()));
    }

    // Check if the last six edges oriented and two edges solved in roux method with the given spin
    bool Method::IsRouxL6EO2E(const Cube& C, const Spn S)
    {
        Cube CRoux = C;
        CRoux.SetSpin(S);
        return IsRouxL6EO2E(CRoux);
    }

    // Check if Petrus block is built
    bool Method::IsPetrusBlockBuilt(const Cube& C)
    {
        switch (C.GetSpin())
        {
        case Spn::UF:
        case Spn::FR:
        case Spn::RU: return C.IsSolved(Pgr::PETRUS_DBL);

        case Spn::UB:
        case Spn::BL:
        case Spn::LU: return C.IsSolved(Pgr::PETRUS_DFR);

        case Spn::UR:
        case Spn::BU:
        case Spn::RB: return C.IsSolved(Pgr::PETRUS_DFL);

        case Spn::UL:
        case Spn::FU:
        case Spn::LF: return C.IsSolved(Pgr::PETRUS_DBR);

        case Spn::DF:
        case Spn::FL:
        case Spn::LD: return C.IsSolved(Pgr::PETRUS_UBR);

        case Spn::DB:
        case Spn::BR:
        case Spn::RD: return C.IsSolved(Pgr::PETRUS_UFL);

        case Spn::DR:
        case Spn::FD:
        case Spn::RF: return C.IsSolved(Pgr::PETRUS_UBL);

        case Spn::DL:
        case Spn::BD:
        case Spn::LB: return C.IsSolved(Pgr::PETRUS_UFR);

        default: return false; // Should not happend
        }
    }

    // Check if Petrus block is built for given spin
    bool Method::IsPetrusBlockBuilt(const Cube& C, const Spn S)
    {
        Cube CPetrus = C;
        CPetrus.SetSpin(S);
        return IsPetrusBlockBuilt(CPetrus);
    }

    // Check if Petrus expanded block is built
    bool Method::IsPetrusExpandedBlockBuilt(const Cube& C)
    {
        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::PETRUS_DBL_Z);
        case Spn::UB: return C.IsSolved(Pgr::PETRUS_DFR_Z);
        case Spn::UR: return C.IsSolved(Pgr::PETRUS_DFL_X);
        case Spn::UL: return C.IsSolved(Pgr::PETRUS_DBR_X);

        case Spn::DF: return C.IsSolved(Pgr::PETRUS_UBR_Z);
        case Spn::DB: return C.IsSolved(Pgr::PETRUS_UFL_Z);
        case Spn::DR: return C.IsSolved(Pgr::PETRUS_UBL_X);
        case Spn::DL: return C.IsSolved(Pgr::PETRUS_UFR_X);

        case Spn::FU: return C.IsSolved(Pgr::PETRUS_DBR_Y);
        case Spn::FD: return C.IsSolved(Pgr::PETRUS_UBL_Y);
        case Spn::FR: return C.IsSolved(Pgr::PETRUS_DBL_X);
        case Spn::FL: return C.IsSolved(Pgr::PETRUS_UBR_X);

        case Spn::BU: return C.IsSolved(Pgr::PETRUS_DFL_Y);
        case Spn::BD: return C.IsSolved(Pgr::PETRUS_UFR_Y);
        case Spn::BR: return C.IsSolved(Pgr::PETRUS_UFL_X);
        case Spn::BL: return C.IsSolved(Pgr::PETRUS_DFR_X);

        case Spn::RU: return C.IsSolved(Pgr::PETRUS_DBL_Y);
        case Spn::RD: return C.IsSolved(Pgr::PETRUS_UFL_Y);
        case Spn::RF: return C.IsSolved(Pgr::PETRUS_UBL_Z);
        case Spn::RB: return C.IsSolved(Pgr::PETRUS_DFL_Z);

        case Spn::LU: return C.IsSolved(Pgr::PETRUS_DFR_Y);
        case Spn::LD: return C.IsSolved(Pgr::PETRUS_UBR_Y);
        case Spn::LF: return C.IsSolved(Pgr::PETRUS_DBR_Z);
        case Spn::LB: return C.IsSolved(Pgr::PETRUS_UFR_Z);

        default: return false; // Should not happend
        }
    }

    // Check if Petrus expanded block is built for given spin
    bool Method::IsPetrusExpandedBlockBuilt(const Cube& C, const Spn S)
    {
        Cube CPetrus = C;
        CPetrus.SetSpin(S);
        return IsPetrusExpandedBlockBuilt(CPetrus);
    }

    // Check if ZZ EOLine is built
    bool Method::IsEOLineBuilt(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
        CAux.SetSpin(spin);
        return IsEOLineBuilt(CAux);
    }

    // Check if ZZ EOLine is built for given spin
    bool Method::IsEOLineBuilt(const Cube& C)
	{
        if (!C.CheckOrientation(Pgr::ALL_EDGES)) return false;

        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::EOLINE_UF);
        case Spn::UB: return C.IsSolved(Pgr::EOLINE_UB);
        case Spn::UR: return C.IsSolved(Pgr::EOLINE_UR);
        case Spn::UL: return C.IsSolved(Pgr::EOLINE_UL);

        case Spn::DF: return C.IsSolved(Pgr::EOLINE_DF);
        case Spn::DB: return C.IsSolved(Pgr::EOLINE_DB);
        case Spn::DR: return C.IsSolved(Pgr::EOLINE_DR);
        case Spn::DL: return C.IsSolved(Pgr::EOLINE_DL);

        case Spn::FU: return C.IsSolved(Pgr::EOLINE_FU);
        case Spn::FD: return C.IsSolved(Pgr::EOLINE_FD);
        case Spn::FR: return C.IsSolved(Pgr::EOLINE_FR);
        case Spn::FL: return C.IsSolved(Pgr::EOLINE_FL);

        case Spn::BU: return C.IsSolved(Pgr::EOLINE_BU);
        case Spn::BD: return C.IsSolved(Pgr::EOLINE_BD);
        case Spn::BR: return C.IsSolved(Pgr::EOLINE_BR);
        case Spn::BL: return C.IsSolved(Pgr::EOLINE_BL);

        case Spn::RU: return C.IsSolved(Pgr::EOLINE_RU);
        case Spn::RD: return C.IsSolved(Pgr::EOLINE_RD);
        case Spn::RF: return C.IsSolved(Pgr::EOLINE_RF);
        case Spn::RB: return C.IsSolved(Pgr::EOLINE_RB);

        case Spn::LU: return C.IsSolved(Pgr::EOLINE_LU);
        case Spn::LD: return C.IsSolved(Pgr::EOLINE_LD);
        case Spn::LF: return C.IsSolved(Pgr::EOLINE_LF);
        case Spn::LB: return C.IsSolved(Pgr::EOLINE_LB);

        default: return false; // Should not happend
        }
	}

	// Check if ZZ EOArrow is built for given spin
	bool Method::IsEOArrowBuilt(const Cube& C, const Spn spin)
	{
        Cube CAux = C;
        CAux.SetSpin(spin);
		if (!IsEOLineBuilt(CAux)) return false;
        return CAux.IsSolved(Cube::FromAbsPosition(App::DL, spin));
	}

	// Check if ZZ EOArrow is built
	bool Method::IsEOArrowBuilt(const Cube& C)
	{
		if (!IsEOLineBuilt(C)) return false;
        return C.IsSolved(Cube::FromAbsPosition(App::DL, C.GetSpin()));
	}

    // Check if ZZ EOCross is built for given spin
    bool Method::IsEOCrossBuilt(const Cube& C, const Spn spin)
	{
        Cube CAux = C;
        CAux.SetSpin(spin);
		return IsEOCrossBuilt(CAux);
	}

	// Check if ZZ EOCross is built
    bool Method::IsEOCrossBuilt(const Cube& C)
	{
        if (!C.CheckOrientation(Pgr::ALL_EDGES)) return false;

        switch (Cube::GetDownSliceLayer(C.GetSpin()))
        {
        case Lyr::U: return C.IsSolved(Pgr::CROSS_U);
        case Lyr::D: return C.IsSolved(Pgr::CROSS_D);
        case Lyr::F: return C.IsSolved(Pgr::CROSS_F);
        case Lyr::B: return C.IsSolved(Pgr::CROSS_B);
        case Lyr::R: return C.IsSolved(Pgr::CROSS_R);
        case Lyr::L: return C.IsSolved(Pgr::CROSS_L);

        default: return false; // Should not happend
        }
	}

    // Check if ZZ XEOLine is built for given spin
    bool Method::IsXEOLineBuilt(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
        CAux.SetSpin(spin);
		return IsXEOLineBuilt(CAux);
    }

	// Check if ZZ XEOLine is built
    bool Method::IsXEOLineBuilt(const Cube& C)
    {
        if (!IsEOLineBuilt(C)) return false;
        return C.IsSolved(Cube::FromAbsPosition(App::DL, C.GetSpin())) && 
               C.IsSolved(Cube::FromAbsPosition(App::BL, C.GetSpin())) &&
               C.IsSolved(Cube::FromAbsPosition(App::DBL, C.GetSpin()));
    }

    // Check if ZZ XEOCross is built for given spin
    bool Method::IsXEOCrossBuilt(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
        CAux.SetSpin(spin);
		return IsXEOCrossBuilt(CAux);
    }

	// Check if ZZ XEOCross is built
    bool Method::IsXEOCrossBuilt(const Cube& C)
    {
        if (!IsEOCrossBuilt(C)) return false;
        return C.IsSolved(Cube::FromAbsPosition(App::BL, C.GetSpin())) &&
               C.IsSolved(Cube::FromAbsPosition(App::DBL, C.GetSpin()));
    }
 
    // Check if ZZ EO223 is built for given spin
    bool Method::IsEO223Built(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
        CAux.SetSpin(spin);
		return IsEO223Built(CAux);
    }

	// Check if ZZ EO223 is built
    bool Method::IsEO223Built(const Cube& C)
    {
        if (!IsEOArrowBuilt(C)) return false;
        return C.IsSolved(Cube::FromAbsPosition(App::BL, C.GetSpin())) &&
               C.IsSolved(Cube::FromAbsPosition(App::FL, C.GetSpin())) &&
               C.IsSolved(Cube::FromAbsPosition(App::DBL, C.GetSpin())) &&
               C.IsSolved(Cube::FromAbsPosition(App::DFL, C.GetSpin()));
    }

    // Check if the YruRU CP-Line is built (only line)
    bool Method::IsYruRULineBuilt(const Cube& C)
    {
        switch (C.GetSpin())
        {
        case Spn::UF: case Spn::RB: return C.IsSolved(Pgr::CPLINE_UF); // == CPLINE_RB
        case Spn::UB: case Spn::LF: return C.IsSolved(Pgr::CPLINE_UB); // == CPLINE_LF
        case Spn::UR: case Spn::BL: return C.IsSolved(Pgr::CPLINE_UR); // == CPLINE_BL
        case Spn::UL: case Spn::FR: return C.IsSolved(Pgr::CPLINE_UL); // == CPLINE_FR

        case Spn::DF: case Spn::LB: return C.IsSolved(Pgr::CPLINE_DF); // == CPLINE_LB
        case Spn::DB: case Spn::RF: return C.IsSolved(Pgr::CPLINE_DB); // == CPLINE_RF
        case Spn::DR: case Spn::FL: return C.IsSolved(Pgr::CPLINE_DR); // == CPLINE_FL
        case Spn::DL: case Spn::BR: return C.IsSolved(Pgr::CPLINE_DL); // == CPLINE_BR

        case Spn::FU: case Spn::LD: return C.IsSolved(Pgr::CPLINE_FU); // == CPLINE_LD
        case Spn::FD: case Spn::RU: return C.IsSolved(Pgr::CPLINE_FD); // == CPLINE_RU
        case Spn::BU: case Spn::RD: return C.IsSolved(Pgr::CPLINE_BU); // == CPLINE_RD
        case Spn::BD: case Spn::LU: return C.IsSolved(Pgr::CPLINE_BD); // == CPLINE_LU

        default: return false; // Should not happend
        }
    }

    // Check if the CP-Line is built (only line)
    bool Method::IsYruRULineBuilt(const Cube& C, const Spn sp)
    {
        Cube CAux = C;
        CAux.SetSpin(sp);
        return IsYruRULineBuilt(CAux);
    }

    // Corners permutation (begginers version)
    bool Method::IsYruRUCPBuilt(const Cube& C)
    {
        if (!IsYruRULineBuilt(C)) return false; // Line for CP (1x1x3 block) must be solved

        // Get the corner positions
        const Pcp Place1 = Cube::FromAbsPosition(App::UFL, C.GetSpin()), // Odd place / Couple 1
            Place2 = Cube::FromAbsPosition(App::UBL, C.GetSpin()), // Even place / Couple 1
            Place3 = Cube::FromAbsPosition(App::UBR, C.GetSpin()), // Odd place / Couple 2
            Place4 = Cube::FromAbsPosition(App::UFR, C.GetSpin()), // Even place / Couple 2
            Place5 = Cube::FromAbsPosition(App::DFR, C.GetSpin()), // Odd place / Couple 3
            Place6 = Cube::FromAbsPosition(App::DBR, C.GetSpin()); // Even place / Couple 3

  // Get the corners
        const Pce Corner1 = static_cast<Pce>(Place1),
            Corner2 = static_cast<Pce>(Place2),
            Corner3 = static_cast<Pce>(Place3),
            Corner4 = static_cast<Pce>(Place4),
            Corner5 = static_cast<Pce>(Place5),
            Corner6 = static_cast<Pce>(Place6);

        // Get the positions for each corner
        const Pcp PlaceForCorner1 = C.GetPiecePosition(Corner1),
            PlaceForCorner2 = C.GetPiecePosition(Corner2),
            PlaceForCorner3 = C.GetPiecePosition(Corner3),
            PlaceForCorner4 = C.GetPiecePosition(Corner4),
            PlaceForCorner5 = C.GetPiecePosition(Corner5),
            PlaceForCorner6 = C.GetPiecePosition(Corner6);

        // Check if the corner 5 is in an odd place
        bool Corner5OddLocation = PlaceForCorner5 == Place1 || PlaceForCorner5 == Place3 || PlaceForCorner5 == Place5;

        // Check if the corners 5 & 6 are in the same couple
        bool Corners5_6SameCouple = (PlaceForCorner5 == Place1 && PlaceForCorner6 == Place2) ||
            (PlaceForCorner5 == Place2 && PlaceForCorner6 == Place1) ||
            (PlaceForCorner5 == Place3 && PlaceForCorner6 == Place4) ||
            (PlaceForCorner5 == Place4 && PlaceForCorner6 == Place3) ||
            (PlaceForCorner5 == Place5 && PlaceForCorner6 == Place6) ||
            (PlaceForCorner5 == Place6 && PlaceForCorner6 == Place5);

        // Check the parity for each corner (true = odd)
        const bool ParityCorner1 = PlaceForCorner1 == Place1 || PlaceForCorner1 == Place3 || PlaceForCorner1 == Place5,
            ParityCorner2 = PlaceForCorner2 == Place1 || PlaceForCorner2 == Place3 || PlaceForCorner2 == Place5,
            ParityCorner3 = PlaceForCorner3 == Place1 || PlaceForCorner3 == Place3 || PlaceForCorner3 == Place5,
            ParityCorner4 = PlaceForCorner4 == Place1 || PlaceForCorner4 == Place3 || PlaceForCorner4 == Place5,
            ParityCorner5 = PlaceForCorner5 == Place1 || PlaceForCorner5 == Place3 || PlaceForCorner5 == Place5,
            ParityCorner6 = PlaceForCorner6 == Place1 || PlaceForCorner6 == Place3 || PlaceForCorner6 == Place5;

        // Read the sequence or corners
        std::vector<Pce> Sequence;
        if (Corner5OddLocation) // If Corner 5 is in an odd location, we will "read" the thread from UFL to DBR
        {
            Sequence.push_back(C.GetPiece(Place1));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place6));
        }
        else // If corner 5 is in an even location, we will read from DBR to UFL
        {
            Sequence.push_back(C.GetPiece(Place6));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place1));
        }

        if (!Corners5_6SameCouple) // If corner 6 is in the couple of location of 5, do nothing.
        {
            int PlaceCorner6Index = -1, PlaceForCorner6FriendIndex = -1, PlaceCorner5Index = -1, PlaceForCorner5FriendIndex = -1;

            // Get the index for corners 5 & 6
            for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
            {
                if (Sequence[Index] == Corner5) PlaceCorner5Index = Index;
                else if (Sequence[Index] == Corner6) PlaceCorner6Index = Index;
            }

            // Get the index for corner 5 friend
            if (PlaceCorner5Index % 2 == 0) PlaceForCorner5FriendIndex = PlaceCorner5Index + 1;
            else PlaceForCorner5FriendIndex = PlaceCorner5Index - 1;

            // Get the index for corner 6 friend
            if (PlaceCorner6Index % 2 == 0) PlaceForCorner6FriendIndex = PlaceCorner6Index + 1;
            else PlaceForCorner6FriendIndex = PlaceCorner6Index - 1;

            // The first swap is always done between "6" and "the friend of 5", so that now 5 and 6 are friends
            Pce Buffer = Sequence[PlaceCorner6Index];
            Sequence[PlaceCorner6Index] = Sequence[PlaceForCorner5FriendIndex];
            Sequence[PlaceForCorner5FriendIndex] = Buffer;

            // Second swap
            if (ParityCorner5 == ParityCorner6)
            {
                // Search the third corner with the same parity than corners 5 & 6
                int ThirdCornerSameParityIndex = -1;
                if (ParityCorner1 == ParityCorner5)
                {
                    for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
                    {
                        if (Sequence[Index] == Corner1)
                        {
                            ThirdCornerSameParityIndex = Index;
                            break;
                        }
                    }
                }
                else if (ParityCorner2 == ParityCorner5)
                {
                    for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
                    {
                        if (Sequence[Index] == Corner2)
                        {
                            ThirdCornerSameParityIndex = Index;
                            break;
                        }
                    }
                }
                else if (ParityCorner3 == ParityCorner5)
                {
                    for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
                    {
                        if (Sequence[Index] == Corner3)
                        {
                            ThirdCornerSameParityIndex = Index;
                            break;
                        }
                    }
                }
                else if (ParityCorner4 == ParityCorner5)
                {
                    for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
                    {
                        if (Sequence[Index] == Corner4)
                        {
                            ThirdCornerSameParityIndex = Index;
                            break;
                        }
                    }
                }
                else return false; // Should not happend

                // If 5 and 6 initially have the same parity, swap the third corner with the same parity with the friend of 6
                Pce Buffer = Sequence[PlaceForCorner6FriendIndex];
                Sequence[PlaceForCorner6FriendIndex] = Sequence[ThirdCornerSameParityIndex];
                Sequence[ThirdCornerSameParityIndex] = Buffer;
            }
            else // ParityCorner5 != ParityCorner6
            {
                // Get the indexes for the third couple or corners
                int ThirdCoupleCorner1Index = -1, ThirdCoupleCorner2Index = -1;
                for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
                {
                    if (Index != PlaceCorner5Index && Index != PlaceForCorner5FriendIndex &&
                        Index != PlaceCorner6Index && Index != PlaceForCorner6FriendIndex)
                    {
                        ThirdCoupleCorner1Index = Index;
                        break;
                    }
                }
                for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
                {
                    if (Index != PlaceCorner5Index && Index != PlaceForCorner5FriendIndex &&
                        Index != PlaceCorner6Index && Index != PlaceForCorner6FriendIndex && Index != ThirdCoupleCorner1Index)
                    {
                        ThirdCoupleCorner2Index = Index;
                        break;
                    }
                }

                // Swap the third pair of friends (i.e. the pair of friends that neither 5 nor 6 is a part of) with each other.
                Pce Buffer = Sequence[ThirdCoupleCorner1Index];
                Sequence[ThirdCoupleCorner1Index] = Sequence[ThirdCoupleCorner2Index];
                Sequence[ThirdCoupleCorner2Index] = Buffer;
            }
        }

        // Remove from the sequence corners 5 & 6
        std::vector<Pce> Seq4;
        for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
            if (Sequence[Index] != Corner5 && Sequence[Index] != Corner6) Seq4.push_back(Sequence[Index]);

        // Get the index for corner 4
        int Corner4Index = -1;
        for (int Index = 0; Index < static_cast<int>(Seq4.size()); Index++)
            if (Seq4[Index] == Corner4)
            {
                Corner4Index = Index;
                break;
            }

        // Check if, in the sequence, the two corners next to corner 4 (cyclic) are corners 1 & 2
        switch (Corner4Index)
        {
        case 0: return Seq4[1] == Corner1 && Seq4[2] == Corner2;
        case 1: return Seq4[2] == Corner1 && Seq4[3] == Corner2;
        case 2: return Seq4[3] == Corner1 && Seq4[0] == Corner2;
        case 3: return Seq4[0] == Corner1 && Seq4[1] == Corner2;
        default: return false;
        }
    }
    
    // Corners permutation (advanced version) - TODO
    bool Method::IsYruRUCPBuilt2(const Cube& C) // TODO - not tested, needs some work
    {
        if (!IsYruRULineBuilt(C)) return false; // Line for CP (1x1x3 block) must be solved

        // Get the corner positions
        const Pcp Place1 = Cube::FromAbsPosition(App::UFL, C.GetSpin()), // Odd place / Couple 1
            Place2 = Cube::FromAbsPosition(App::UBL, C.GetSpin()), // Even place / Couple 1
            Place3 = Cube::FromAbsPosition(App::UBR, C.GetSpin()), // Odd place / Couple 2
            Place4 = Cube::FromAbsPosition(App::UFR, C.GetSpin()), // Even place / Couple 2
            Place5 = Cube::FromAbsPosition(App::DFR, C.GetSpin()), // Odd place / Couple 3
            Place6 = Cube::FromAbsPosition(App::DBR, C.GetSpin()); // Even place / Couple 3

        // Get the corners
        const Pce Corner1 = static_cast<Pce>(Place1),
            Corner2 = static_cast<Pce>(Place2),
            // Corner3 = static_cast<Pce>(Place3),
            Corner4 = static_cast<Pce>(Place4),
            Corner5 = static_cast<Pce>(Place5),
            Corner6 = static_cast<Pce>(Place6);

        // Locate corners 5 and 6
        const Pcp PlaceForCorner5 = C.GetPiecePosition(Corner5), PlaceForCorner6 = C.GetPiecePosition(Corner6);

        std::vector<Pce> Sequence;

        // Corners 5 and 6 are already friends
        if (PlaceForCorner5 == Place1 && PlaceForCorner6 == Place2)
        {
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place6));
        }
        else if (PlaceForCorner5 == Place2 && PlaceForCorner6 == Place1)
        {
            Sequence.push_back(C.GetPiece(Place6));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place3));
        }
        else if (PlaceForCorner5 == Place3 && PlaceForCorner6 == Place4)
        {
            Sequence.push_back(C.GetPiece(Place1));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place6));
        }
        else if (PlaceForCorner5 == Place4 && PlaceForCorner6 == Place3)
        {
            Sequence.push_back(C.GetPiece(Place6));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place1));
        }
        else if (PlaceForCorner5 == Place5 && PlaceForCorner6 == Place6)
        {
            Sequence.push_back(C.GetPiece(Place1));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place4));
        }
        else if (PlaceForCorner5 == Place6 && PlaceForCorner6 == Place5)
        {
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place1));
        }
        // Corners 5 and 6 are almost friends (5 and 6 will become friends with a single U move or a single R move)
        else if (PlaceForCorner5 == Place1 && PlaceForCorner6 == Place4)
        {
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place6));
        }
        else if (PlaceForCorner5 == Place4 && PlaceForCorner6 == Place1)
        {
            Sequence.push_back(C.GetPiece(Place6));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place2));
        }
        else if (PlaceForCorner5 == Place2 && PlaceForCorner6 == Place3)
        {
            Sequence.push_back(C.GetPiece(Place6));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place1));
            Sequence.push_back(C.GetPiece(Place4));
        }
        else if (PlaceForCorner5 == Place3 && PlaceForCorner6 == Place2)
        {
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place1));
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place6));
        }
        else if (PlaceForCorner5 == Place4 && PlaceForCorner6 == Place5)
        {
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place6));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place1));
        }
        else if (PlaceForCorner5 == Place5 && PlaceForCorner6 == Place4)
        {
            Sequence.push_back(C.GetPiece(Place1));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place3));
            Sequence.push_back(C.GetPiece(Place6));
        }
        else if (PlaceForCorner5 == Place3 && PlaceForCorner6 == Place6)
        {
            Sequence.push_back(C.GetPiece(Place1));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place5));
        }
        else if (PlaceForCorner5 == Place6 && PlaceForCorner6 == Place3)
        {
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place1));
        }
        // Letter N
        else if (PlaceForCorner5 == Place1 && PlaceForCorner6 == Place3)
        {
            Sequence.push_back(C.GetPiece(Place2));
            Sequence.push_back(C.GetPiece(Place6));
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place5));
        }
        else if (PlaceForCorner5 == Place3 && PlaceForCorner6 == Place1)
        {
            Sequence.push_back(C.GetPiece(Place5));
            Sequence.push_back(C.GetPiece(Place4));
            Sequence.push_back(C.GetPiece(Place6));
            Sequence.push_back(C.GetPiece(Place2));
        }
        // Inverted Letter N
        else if (PlaceForCorner5 == Place3 && PlaceForCorner6 == Place5)
        {
        Sequence.push_back(C.GetPiece(Place1));
        Sequence.push_back(C.GetPiece(Place4));
        Sequence.push_back(C.GetPiece(Place2));
        Sequence.push_back(C.GetPiece(Place6));
        }
        else if (PlaceForCorner5 == Place5 && PlaceForCorner6 == Place3)
        {
        Sequence.push_back(C.GetPiece(Place6));
        Sequence.push_back(C.GetPiece(Place2));
        Sequence.push_back(C.GetPiece(Place4));
        Sequence.push_back(C.GetPiece(Place1));
        }
        // Diamond
        else if (PlaceForCorner5 == Place1 && PlaceForCorner6 == Place5)
        {
        Sequence.push_back(C.GetPiece(Place2));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place6));
        Sequence.push_back(C.GetPiece(Place4));
        }
        else if (PlaceForCorner5 == Place5 && PlaceForCorner6 == Place1)
        {
        Sequence.push_back(C.GetPiece(Place4));
        Sequence.push_back(C.GetPiece(Place6));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place2));
        }
        // Arrow head
        else if (PlaceForCorner5 == Place2 && PlaceForCorner6 == Place6)
        {
        Sequence.push_back(C.GetPiece(Place1));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place5));
        Sequence.push_back(C.GetPiece(Place4));
        }
        else if (PlaceForCorner5 == Place6 && PlaceForCorner6 == Place2)
        {
        Sequence.push_back(C.GetPiece(Place4));
        Sequence.push_back(C.GetPiece(Place5));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place1));
        }
        // Lightning Bolt
        else if (PlaceForCorner5 == Place1 && PlaceForCorner6 == Place6)
        {
        Sequence.push_back(C.GetPiece(Place2));
        Sequence.push_back(C.GetPiece(Place4));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place5));
        }
        else if (PlaceForCorner5 == Place6 && PlaceForCorner6 == Place1)
        {
        Sequence.push_back(C.GetPiece(Place5));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place4));
        Sequence.push_back(C.GetPiece(Place2));
        }
        // Inverted Lightning Bolt
        else if (PlaceForCorner5 == Place2 && PlaceForCorner6 == Place5)
        {
        Sequence.push_back(C.GetPiece(Place1));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place4));
        Sequence.push_back(C.GetPiece(Place6));
        }
        else if (PlaceForCorner5 == Place5 && PlaceForCorner6 == Place2)
        {
        Sequence.push_back(C.GetPiece(Place6));
        Sequence.push_back(C.GetPiece(Place4));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place1));
        }
        // Letter V (with an arm)
        else if (PlaceForCorner5 == Place2 && PlaceForCorner6 == Place4)
        {
        Sequence.push_back(C.GetPiece(Place1));
        Sequence.push_back(C.GetPiece(Place5));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place6));
        }
        else if (PlaceForCorner5 == Place4 && PlaceForCorner6 == Place2)
        {
        Sequence.push_back(C.GetPiece(Place6));
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place5));
        Sequence.push_back(C.GetPiece(Place1));
        }
        // Inverted Letter V (with an arm)
        else if (PlaceForCorner5 == Place4 && PlaceForCorner6 == Place6)
        {
        Sequence.push_back(C.GetPiece(Place1));
        Sequence.push_back(C.GetPiece(Place5));
        Sequence.push_back(C.GetPiece(Place2));
        Sequence.push_back(C.GetPiece(Place3));
        }
        else if (PlaceForCorner5 == Place6 && PlaceForCorner6 == Place4)
        {
        Sequence.push_back(C.GetPiece(Place3));
        Sequence.push_back(C.GetPiece(Place2));
        Sequence.push_back(C.GetPiece(Place5));
        Sequence.push_back(C.GetPiece(Place1));
        }

        // Get the index for corner 4 into the sequence
        int Corner4Index = -1;
        for (int Index = 0; Index < static_cast<int>(Sequence.size()); Index++)
            if (Sequence[Index] == Corner4)
            {
                Corner4Index = Index;
                break;
            }

        // Check if, in the sequence, the two corners next to corner 4 (cyclic) are corners 1 & 2
        switch (Corner4Index)
        {
        case 0: return Sequence[1] == Corner1 && Sequence[2] == Corner2;
        case 1: return Sequence[2] == Corner1 && Sequence[3] == Corner2;
        case 2: return Sequence[3] == Corner1 && Sequence[0] == Corner2;
        case 3: return Sequence[0] == Corner1 && Sequence[1] == Corner2;
        default: return false;
        }
    }

    // Corners permutation
    bool Method::IsYruRUCPBuilt(const Cube& C, const Spn spin)
    {
        Cube CAux = C;
        CAux.SetSpin(spin);
        return IsYruRUCPBuilt(CAux);
    }

    // Check if the Mehta first block is built
    bool Method::IsMehtaFBBuilt(const Cube& C)
    {
        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::RB_B1);
        case Spn::UB: return C.IsSolved(Pgr::LF_B1);
        case Spn::UR: return C.IsSolved(Pgr::BL_B1);
        case Spn::UL: return C.IsSolved(Pgr::FR_B1);

        case Spn::DF: return C.IsSolved(Pgr::LB_B1);
        case Spn::DB: return C.IsSolved(Pgr::RF_B1);
        case Spn::DR: return C.IsSolved(Pgr::FL_B1);
        case Spn::DL: return C.IsSolved(Pgr::BR_B1);

        case Spn::FU: return C.IsSolved(Pgr::LD_B1);
        case Spn::FD: return C.IsSolved(Pgr::RU_B1);
        case Spn::FR: return C.IsSolved(Pgr::UL_B1);
        case Spn::FL: return C.IsSolved(Pgr::DR_B1);

        case Spn::BU: return C.IsSolved(Pgr::RD_B1);
        case Spn::BD: return C.IsSolved(Pgr::LU_B1);
        case Spn::BR: return C.IsSolved(Pgr::DL_B1);
        case Spn::BL: return C.IsSolved(Pgr::UR_B1);

        case Spn::RU: return C.IsSolved(Pgr::FD_B1);
        case Spn::RD: return C.IsSolved(Pgr::BU_B1);
        case Spn::RF: return C.IsSolved(Pgr::DB_B1);
        case Spn::RB: return C.IsSolved(Pgr::UF_B1);

        case Spn::LU: return C.IsSolved(Pgr::BD_B1);
        case Spn::LD: return C.IsSolved(Pgr::FU_B1);
        case Spn::LF: return C.IsSolved(Pgr::UB_B1);
        case Spn::LB: return C.IsSolved(Pgr::DF_B1);

        default: return false; // Should not happend
        }
    }

    // Check if the first Mehta block is built with the given spin
    bool Method::IsMehtaFBBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehtaFBBuilt(CubeMehta);
    }

    // Check if the Mehta 3QB is built (also first block)
    bool Method::IsMehta3QBBuilt(const Cube& C)
    {
        if (!IsMehtaFBBuilt(C)) return false;

        // Pieces to search
        Pce E_1, E_2, E_3, E_4;

        // Get the required pieces based on the spin
        switch (C.GetSpin())
        {
        case Spn::UF:
        case Spn::UB:
        case Spn::UR:
        case Spn::UL:
        case Spn::DF:
        case Spn::DB:
        case Spn::DR:
        case Spn::DL: E_1 = Pce::FR; E_2 = Pce::FL; E_3 = Pce::BR; E_4 = Pce::BL; break;

        case Spn::FU:
        case Spn::FD:
        case Spn::FR:
        case Spn::FL:
        case Spn::BU:
        case Spn::BD:
        case Spn::BR:
        case Spn::BL: E_1 = Pce::UR; E_2 = Pce::UL; E_3 = Pce::DR; E_4 = Pce::DL; break;

        case Spn::RU:
        case Spn::RD:
        case Spn::RF:
        case Spn::RB:
        case Spn::LU:
        case Spn::LD:
        case Spn::LF:
        case Spn::LB: E_1 = Pce::UF; E_2 = Pce::UB; E_3 = Pce::DF; E_4 = Pce::DB; break;

        default: return false; // Invalid spin (should not happend)
        }

        uint score = 0u;

        if (C.IsSolved(E_1)) score++;
        if (C.IsSolved(E_2)) score++;
        if (C.IsSolved(E_3)) score++;
        if (C.IsSolved(E_4)) score++;

        return score >= 3u;
    }

    // Check if Mehta 3QB is built with the given spin
    bool Method::IsMehta3QBBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehta3QBBuilt(CubeMehta);
    }

    // Check if the Mehta EOLE is built (also first block)
    bool Method::IsMehtaEOLEBuilt(const Cube& C)
    {
        if (!IsMehtaFBBuilt(C) || !C.CheckOrientation(Pgr::ALL_EDGES)) return false;

        // Get the required pieces based on the spin
        switch (C.GetSpin())
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL:
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return C.IsSolved(Lyr::E);

        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL:
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return C.IsSolved(Lyr::S);

        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB:
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return C.IsSolved(Lyr::M);

        default: return false; // Invalid spin (should not happend)
        }
    }

    // Check if Mehta EOLE is built with the given spin
    bool Method::IsMehtaEOLEBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehtaEOLEBuilt(CubeMehta);
    }

    // Check if the Mehta 6CO is built
    bool Method::IsMehta6COBuilt(const Cube& C)
    {
        return IsMehtaEOLEBuilt(C) && C.CheckOrientation(Pgr::ALL_CORNERS);
    }

    // Check if Mehta 6CO is built with the given spin
    bool Method::IsMehta6COBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehta6COBuilt(CubeMehta);
    }

    // Check if the Mehta 6CP is built
    bool Method::IsMehta6CPBuilt(const Cube& C)
    {
        return IsMehtaEOLEBuilt(C) && C.IsSolved(Pgr::ALL_CORNERS);
    }

    // Check if Mehta 6CP is built with the given spin
    bool Method::IsMehta6CPBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehta6CPBuilt(CubeMehta);
    }

    // Check if the Mehta APDR is built
    bool Method::IsMehtaAPDRBuilt(const Cube& C)
    {
        // Get the required pieces based on the spin
        switch (C.GetSpin())
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return C.IsSolved(Lyr::D) && C.IsSolved(Lyr::E) && C.CheckOrientation(Pgr::LAYER_U);
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return C.IsSolved(Lyr::U) && C.IsSolved(Lyr::E) && C.CheckOrientation(Pgr::LAYER_D);

        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return C.IsSolved(Lyr::B) && C.IsSolved(Lyr::S) && C.CheckOrientation(Pgr::LAYER_F);
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return C.IsSolved(Lyr::F) && C.IsSolved(Lyr::S) && C.CheckOrientation(Pgr::LAYER_B);

        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return C.IsSolved(Lyr::L) && C.IsSolved(Lyr::M) && C.CheckOrientation(Pgr::LAYER_R);
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return C.IsSolved(Lyr::R) && C.IsSolved(Lyr::M) && C.CheckOrientation(Pgr::LAYER_L);

        default: return false; // Invalid spin (should not happend)
        }
    }

    // Check if Mehta APDR is built with the given spin
    bool Method::IsMehtaAPDRBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehtaAPDRBuilt(CubeMehta);
    }

    // Check if the Mehta DCAL is built
    bool Method::IsMehtaDCALBuilt(const Cube& C)
    {
        if (!IsMehtaEOLEBuilt(C)) return false;
        return C.IsSolved(Cube::FromAbsPosition(App::DFR, C.GetSpin())) && C.IsSolved(Cube::FromAbsPosition(App::DBR, C.GetSpin()));
    }

    // Check if Mehta DCAL is built with the given spin
    bool Method::IsMehtaDCALBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehtaDCALBuilt(CubeMehta);
    }

    // Check if the Mehta CDRLL is built
    bool Method::IsMehtaCDRLLBuilt(const Cube& C)
    {
        if (!IsMehtaDCALBuilt(C)) return false;

        switch (C.GetSpin())
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return C.IsSolved(Pgr::CORNERS_U);
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return C.IsSolved(Pgr::CORNERS_D);

        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return C.IsSolved(Pgr::CORNERS_F);
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return C.IsSolved(Pgr::CORNERS_B);

        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return C.IsSolved(Pgr::CORNERS_R);
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return C.IsSolved(Pgr::CORNERS_L);

        default: return false;
        }
    }

    // Check if Mehta CDRLL is built with the given spin
    bool Method::IsMehtaCDRLLBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehtaCDRLLBuilt(CubeMehta);
    }

    // Check if the Mehta JTLE is built (same conditions than APDR)
    bool Method::IsMehtaJTLEBuilt(const Cube& C)
    {
        return IsMehtaAPDRBuilt(C);
    }

    // Check if Mehta JTLE is built with the given spin
    bool Method::IsMehtaJTLEBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehtaJTLEBuilt(CubeMehta);
    }

    // Check if the Mehta TDR is built
    bool Method::IsMehtaTDRBuilt(const Cube& C)
    {
        if (!C.CheckOrientation(Pgr::ALL_EDGES)) return false;
        switch (Cube::GetUpSliceLayer(C.GetSpin()))
        {
        case Lyr::U: return C.IsSolved(Pgr::LAYER_D) && C.IsSolved(Pgr::LAYER_E);
        case Lyr::D: return C.IsSolved(Pgr::LAYER_U) && C.IsSolved(Pgr::LAYER_E);
        case Lyr::F: return C.IsSolved(Pgr::LAYER_B) && C.IsSolved(Pgr::LAYER_S);
        case Lyr::B: return C.IsSolved(Pgr::LAYER_F) && C.IsSolved(Pgr::LAYER_S);
        case Lyr::R: return C.IsSolved(Pgr::LAYER_L) && C.IsSolved(Pgr::LAYER_M);
        case Lyr::L: return C.IsSolved(Pgr::LAYER_R) && C.IsSolved(Pgr::LAYER_M);
        default: return false;
        }
    }

    // Check if Mehta TDR is built with the given spin
    bool Method::IsMehtaTDRBuilt(const Cube& C, const Spn S)
    {
        Cube CubeMehta = C;
        CubeMehta.SetSpin(S); // Mehta cube with desired spin
        return IsMehtaTDRBuilt(CubeMehta);
    }

    // Check if the right Nautilus block is built (second block)
    bool Method::IsNautilusSBBuilt(const Cube& C)
    {
        if (!IsRouxFBBuilt(C)) return false;

        // Nautilus cube with desired spin
        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::PETRUS_DBR);
        case Spn::UB: return C.IsSolved(Pgr::PETRUS_DFL);
        case Spn::UR: return C.IsSolved(Pgr::PETRUS_DBL);
        case Spn::UL: return C.IsSolved(Pgr::PETRUS_DFR);

        case Spn::DF: return C.IsSolved(Pgr::PETRUS_UBL);
        case Spn::DB: return C.IsSolved(Pgr::PETRUS_UFR);
        case Spn::DR: return C.IsSolved(Pgr::PETRUS_UFL);
        case Spn::DL: return C.IsSolved(Pgr::PETRUS_UBR);

        case Spn::FU: return C.IsSolved(Pgr::PETRUS_DBL);
        case Spn::FD: return C.IsSolved(Pgr::PETRUS_UBR);
        case Spn::FR: return C.IsSolved(Pgr::PETRUS_UBL);
        case Spn::FL: return C.IsSolved(Pgr::PETRUS_DBR);

        case Spn::BU: return C.IsSolved(Pgr::PETRUS_DFR);
        case Spn::BD: return C.IsSolved(Pgr::PETRUS_UFL);
        case Spn::BR: return C.IsSolved(Pgr::PETRUS_DFL);
        case Spn::BL: return C.IsSolved(Pgr::PETRUS_UFR);

        case Spn::RU: return C.IsSolved(Pgr::PETRUS_DFL);
        case Spn::RD: return C.IsSolved(Pgr::PETRUS_UBL);
        case Spn::RF: return C.IsSolved(Pgr::PETRUS_DBL);
        case Spn::RB: return C.IsSolved(Pgr::PETRUS_UFL);

        case Spn::LU: return C.IsSolved(Pgr::PETRUS_DBR);
        case Spn::LD: return C.IsSolved(Pgr::PETRUS_UFR);
        case Spn::LF: return C.IsSolved(Pgr::PETRUS_UBR);
        case Spn::LB: return C.IsSolved(Pgr::PETRUS_DFR);

        default: return false; // Should not happend
        }
    }

    // Check if the second Nautilus block is built with the given spin
    bool Method::IsNautilusSBBuilt(const Cube& C, const Spn S)
    {
        Cube CNautilus = C;
        CNautilus.SetSpin(S);
        return IsNautilusSBBuilt(CNautilus);
    }

    // Check if the Nautilus second block square is built
    bool Method::IsNautilusSBSBuilt(const Cube& C)
    {
        if (!IsRouxFBBuilt(C)) return false;

        switch (C.GetSpin())
        {
        case Spn::UF: return C.IsSolved(Pgr::UF_B1) && C.IsSolved(Pgr::UF_B2S2);
        case Spn::UB: return C.IsSolved(Pgr::UB_B1) && C.IsSolved(Pgr::UB_B2S2);
        case Spn::UR: return C.IsSolved(Pgr::UR_B1) && C.IsSolved(Pgr::UR_B2S2);
        case Spn::UL: return C.IsSolved(Pgr::UL_B1) && C.IsSolved(Pgr::UL_B2S2);

        case Spn::DF: return C.IsSolved(Pgr::DF_B1) && C.IsSolved(Pgr::DF_B2S2);
        case Spn::DB: return C.IsSolved(Pgr::DB_B1) && C.IsSolved(Pgr::DB_B2S2);
        case Spn::DR: return C.IsSolved(Pgr::DR_B1) && C.IsSolved(Pgr::DR_B2S2);
        case Spn::DL: return C.IsSolved(Pgr::DL_B1) && C.IsSolved(Pgr::DL_B2S2);

        case Spn::FU: return C.IsSolved(Pgr::FU_B1) && C.IsSolved(Pgr::FU_B2S2);
        case Spn::FD: return C.IsSolved(Pgr::FD_B1) && C.IsSolved(Pgr::FD_B2S2);
        case Spn::FR: return C.IsSolved(Pgr::FR_B1) && C.IsSolved(Pgr::FR_B2S2);
        case Spn::FL: return C.IsSolved(Pgr::FL_B1) && C.IsSolved(Pgr::FL_B2S2);

        case Spn::BU: return C.IsSolved(Pgr::BU_B1) && C.IsSolved(Pgr::BU_B2S2);
        case Spn::BD: return C.IsSolved(Pgr::BD_B1) && C.IsSolved(Pgr::BD_B2S2);
        case Spn::BR: return C.IsSolved(Pgr::BR_B1) && C.IsSolved(Pgr::BR_B2S2);
        case Spn::BL: return C.IsSolved(Pgr::BL_B1) && C.IsSolved(Pgr::BL_B2S2);

        case Spn::RU: return C.IsSolved(Pgr::RU_B1) && C.IsSolved(Pgr::RU_B2S2);
        case Spn::RD: return C.IsSolved(Pgr::RD_B1) && C.IsSolved(Pgr::RD_B2S2);
        case Spn::RF: return C.IsSolved(Pgr::RF_B1) && C.IsSolved(Pgr::RF_B2S2);
        case Spn::RB: return C.IsSolved(Pgr::RB_B1) && C.IsSolved(Pgr::RB_B2S2);

        case Spn::LU: return C.IsSolved(Pgr::LU_B1) && C.IsSolved(Pgr::LU_B2S2);
        case Spn::LD: return C.IsSolved(Pgr::LD_B1) && C.IsSolved(Pgr::LD_B2S2);
        case Spn::LF: return C.IsSolved(Pgr::LF_B1) && C.IsSolved(Pgr::LF_B2S2);
        case Spn::LB: return C.IsSolved(Pgr::LB_B1) && C.IsSolved(Pgr::LB_B2S2);

        default: return false; // Should not happend
        }
    }

    // Check if the Nautilus second block square is built with the given spin
    bool Method::IsNautilusSBSBuilt(const Cube& C, const Spn S)
    {
        Cube CNautilus = C;
        CNautilus.SetSpin(S);
        return IsNautilusSBBuilt(CNautilus);
    }

    // Check if NCLL Nautilus is built (for L5E)
    bool Method::IsNautilusNCLLBuilt(const Cube& C)
    {
        if (!IsNautilusSBBuilt(C) || !IsF2LPairsBuilt(C)) return false;
        return C.CheckOrientation(Pgr::ALL_CORNERS);
    }

    // Check if NCLL Nautilus is built (for L5E) with the given spin
    bool Method::IsNautilusNCLLBuilt(const Cube& C, const Spn S)
    {
        Cube CNautilus = C;
        CNautilus.SetSpin(S);
        return IsNautilusNCLLBuilt(CNautilus);
    }

    // Check if EODF Nautilus is built
    bool Method::IsNautilusEODFBuilt(const Cube& C)
    {
        if (!IsNautilusSBBuilt(C) || !C.IsSolved(Cube::FromAbsPosition(App::DF, C.GetSpin()))) return false;
        return C.CheckOrientation(Pgr::ALL_EDGES);
    }

    // Check if NCLL Nautilus is built (for L5EP) with the given spin
    bool Method::IsNautilusEODFBuilt(const Cube& C, const Spn S)
    {
        Cube CNautilus = C;
        CNautilus.SetSpin(S);
        return IsNautilusEODFBuilt(CNautilus);
    }

	// Static functions for solves evaluation
	
	// Returns best CFOP F2L solve from the Solves vector class member
    bool Method::EvaluateCFOPF2LResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves,
                                       const std::vector<Algorithm>& Solves, const Cube& CBase, const Lyr StartLayer, 
                                       const Plc Policy, const bool CrossMandatory)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        // Pieces to search
        Pgr F2L_1, F2L_2, F2L_3, F2L_4, CrossPG;

        // Get the required cross and F2L pieces groups based on the start layer
        switch (StartLayer)
        {
        case Lyr::U:
            CrossPG = Pgr::CROSS_U;
            F2L_1 = Pgr::F2L_U_UFR;
            F2L_2 = Pgr::F2L_U_UFL;
            F2L_3 = Pgr::F2L_U_UBR;
            F2L_4 = Pgr::F2L_U_UBL;
            break;
        case Lyr::D:
            CrossPG = Pgr::CROSS_D;
            F2L_1 = Pgr::F2L_D_DFR;
            F2L_2 = Pgr::F2L_D_DFL;
            F2L_3 = Pgr::F2L_D_DBR;
            F2L_4 = Pgr::F2L_D_DBL;
            break;
        case Lyr::F:
            CrossPG = Pgr::CROSS_F;
            F2L_1 = Pgr::F2L_F_UFR;
            F2L_2 = Pgr::F2L_F_UFL;
            F2L_3 = Pgr::F2L_F_DFR;
            F2L_4 = Pgr::F2L_F_DFL;
            break;
        case Lyr::B:
            CrossPG = Pgr::CROSS_B;
            F2L_1 = Pgr::F2L_B_UBR;
            F2L_2 = Pgr::F2L_B_UBL;
            F2L_3 = Pgr::F2L_B_DBR;
            F2L_4 = Pgr::F2L_B_DBL;
            break;
        case Lyr::R:
            CrossPG = Pgr::CROSS_R;
            F2L_1 = Pgr::F2L_R_UFR;
            F2L_2 = Pgr::F2L_R_UBR;
            F2L_3 = Pgr::F2L_R_DFR;
            F2L_4 = Pgr::F2L_R_DBR;
            break;
        case Lyr::L:
            CrossPG = Pgr::CROSS_L;
            F2L_1 = Pgr::F2L_L_UFL;
            F2L_2 = Pgr::F2L_L_UBL;
            F2L_3 = Pgr::F2L_L_DFL;
            F2L_4 = Pgr::F2L_L_DBL;
            break;
        default: BestSolves.clear(); return false; // Invalid cross layer
        }

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveF2LMagnitude, SolveOppositeLayerMagnitude, SolveOrientedMagnitude, SolveNotAdjacentMagnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 1000u;
            SolveF2LMagnitude = 100u;
            SolveOppositeLayerMagnitude = 10u;
            SolveOrientedMagnitude = 50u;
            SolveNotAdjacentMagnitude = 10u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 100u;
            SolveF2LMagnitude = 1000u;
            SolveOppositeLayerMagnitude = 10u;
            SolveOrientedMagnitude = 50u;
            SolveNotAdjacentMagnitude = 10u;
            break;
        }

        uint Score;
        
        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (CrossMandatory && !CheckCube.IsSolved(CrossPG)) continue;

            Score += (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            // Check F2L solves
            if (CheckCube.IsSolved(F2L_1)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L_2)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L_3)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L_4)) Score += SolveF2LMagnitude;

            // Check both F2L pieces condition in the opposite layer
            const Lyr EndLayer = Cube::OppositeLayer(StartLayer);

            if (IsF2LOriented(F2L_1, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L_1, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L_1, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L_2, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L_2, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L_2, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L_3, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L_3, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L_3, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L_4, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L_4, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L_4, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }
	
    // Returns best solve algorithm from the Solves vector class member and his score for the given spin
    // F2L pieces are used as evaluation condition for the Roux first blocks (first block in the left side)
    bool Method::EvaluateRouxFBResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        // Pieces to search
        Pgr F2L_1, F2L_2, F2L_3, F2L_4;

        // Get the required F2L pieces groups based on the spin
        switch (Sp)
        {
        case Spn::DF:
        case Spn::DB:
        case Spn::DR:
        case Spn::DL:
            F2L_1 = Pgr::F2L_U_UFR;
            F2L_2 = Pgr::F2L_U_UFL;
            F2L_3 = Pgr::F2L_U_UBR;
            F2L_4 = Pgr::F2L_U_UBL;
            break;
        case Spn::UF:
        case Spn::UB:
        case Spn::UR:
        case Spn::UL:
            F2L_1 = Pgr::F2L_D_DFR;
            F2L_2 = Pgr::F2L_D_DFL;
            F2L_3 = Pgr::F2L_D_DBR;
            F2L_4 = Pgr::F2L_D_DBL;
            break;
        case Spn::BU:
        case Spn::BD:
        case Spn::BR:
        case Spn::BL:
            F2L_1 = Pgr::F2L_F_UFR;
            F2L_2 = Pgr::F2L_F_UFL;
            F2L_3 = Pgr::F2L_F_DFR;
            F2L_4 = Pgr::F2L_F_DFL;
            break;
        case Spn::FU:
        case Spn::FD:
        case Spn::FR:
        case Spn::FL:
            F2L_1 = Pgr::F2L_B_UBR;
            F2L_2 = Pgr::F2L_B_UBL;
            F2L_3 = Pgr::F2L_B_DBR;
            F2L_4 = Pgr::F2L_B_DBL;
            break;
        case Spn::LU:
        case Spn::LD:
        case Spn::LF:
        case Spn::LB:
            F2L_1 = Pgr::F2L_R_UFR;
            F2L_2 = Pgr::F2L_R_UBR;
            F2L_3 = Pgr::F2L_R_DFR;
            F2L_4 = Pgr::F2L_R_DBR;
            break;
        case Spn::RU:
        case Spn::RD:
        case Spn::RF:
        case Spn::RB:
            F2L_1 = Pgr::F2L_L_UFL;
            F2L_2 = Pgr::F2L_L_UBL;
            F2L_3 = Pgr::F2L_L_DFL;
            F2L_4 = Pgr::F2L_L_DBL;
            break;
        default: return 0u; // Invalid spin (should not happend)
        }

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveF2LMagnitude, SolveOppositeLayerMagnitude, SolveOrientedMagnitude, SolveNotAdjacentMagnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 1000u;
            SolveF2LMagnitude = 100u;
            SolveOppositeLayerMagnitude = 10u;
            SolveOrientedMagnitude = 50u;
            SolveNotAdjacentMagnitude = 10u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 100u;
            SolveF2LMagnitude = 1000u;
            SolveOppositeLayerMagnitude = 10u;
            SolveOrientedMagnitude = 50u;
            SolveNotAdjacentMagnitude = 10u;
            break;
        }

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsRouxFBBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

            // Evaluate solve size
            Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            // Check F2L solves
            if (CheckCube.IsSolved(F2L_1)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L_2)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L_3)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L_4)) Score += SolveF2LMagnitude;

            // Check both F2L pieces condition in the opposite layer
            const Lyr EndLayer = Cube::GetUpSliceLayer(Sp);

            if (IsF2LOriented(F2L_1, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L_1, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L_1, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L_2, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L_2, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L_2, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L_3, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L_3, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L_3, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L_4, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L_4, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L_4, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }

    // Returns best block solve from the Solves vector class member for the given corner position block (Petrus)
    bool Method::EvaluatePetrusBlockResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn spin, const Plc Policy)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveExpandedBlockMagnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 1000u;
            SolveExpandedBlockMagnitude = 100u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 1u;
            SolveExpandedBlockMagnitude = 1000u;
            break;
        }

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves)
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsPetrusBlockBuilt(CheckCube, spin)) continue;

            Score += (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            if (IsPetrusExpandedBlockBuilt(CheckCube, spin)) Score += SolveExpandedBlockMagnitude;

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }

	// Returns best solve EO X from the Solves vector class member and his score for ZZ
    bool Method::EvaluateZZEOXResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn spin, const Plc Policy)
	{
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves
		
		// Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveEOArrowMagnitude, SolveEOCrossMagnitude, SolveXEOLineMagnitude, SolveXEOCrossMagnitude, SolveEO223Magnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 1000u;
            SolveEOArrowMagnitude = 100u;
            SolveEOCrossMagnitude = 150u;
            SolveXEOLineMagnitude = 200u;
            SolveXEOCrossMagnitude = 250u;
            SolveEO223Magnitude = 500u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 40u;
            SolveEOArrowMagnitude = 1000u;
            SolveEOCrossMagnitude = 1500u;
            SolveXEOLineMagnitude = 2000u;
            SolveXEOCrossMagnitude = 2500u;
            SolveEO223Magnitude = 5000u;
            break;
        }
		
        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

		for (const auto& s : Solves)
		{
            Score = 0u;

			Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);
			
            if (!IsEOLineBuilt(CheckCube, spin)) continue;

			Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();
			
            if (IsEO223Built(CheckCube, spin)) Score += SolveEO223Magnitude;
            else if (IsXEOCrossBuilt(CheckCube, spin)) Score += SolveXEOCrossMagnitude;
            else if (IsXEOLineBuilt(CheckCube, spin)) Score += SolveXEOLineMagnitude;
            else if (IsEOCrossBuilt(CheckCube, spin)) Score += SolveEOCrossMagnitude;
            else if (IsEOArrowBuilt(CheckCube, spin)) Score += SolveEOArrowMagnitude;
			
            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
		}
		
        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
	}

	// Returns best solve F2L from the Solves vector class member and his score for ZZ
    bool Method::EvaluateZZF2LResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn spin, const Plc Policy)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        // Pieces to search
        Pgr F2L1, F2L2, F2L3, F2L4;

        switch (Cube::GetDownSliceLayer(spin))
        {
        case Lyr::U: F2L1 = Pgr::UF_B1; F2L2 = Pgr::UB_B1; F2L3 = Pgr::UR_B1; F2L4 = Pgr::UL_B1; break;
        case Lyr::D: F2L1 = Pgr::DF_B1; F2L2 = Pgr::DB_B1; F2L3 = Pgr::DR_B1; F2L4 = Pgr::DL_B1; break;
        case Lyr::F: F2L1 = Pgr::FU_B1; F2L2 = Pgr::FD_B1; F2L3 = Pgr::FR_B1; F2L4 = Pgr::FL_B1; break;
        case Lyr::B: F2L1 = Pgr::BU_B1; F2L2 = Pgr::BD_B1; F2L3 = Pgr::BR_B1; F2L4 = Pgr::BL_B1; break;
        case Lyr::R: F2L1 = Pgr::RU_B1; F2L2 = Pgr::RD_B1; F2L3 = Pgr::RF_B1; F2L4 = Pgr::RB_B1; break;
        case Lyr::L: F2L1 = Pgr::LU_B1; F2L2 = Pgr::LD_B1; F2L3 = Pgr::LF_B1; F2L4 = Pgr::LB_B1; break;
        default: return 0u; // Invalid cross layer
        }

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveF2LMagnitude, SolveOppositeLayerMagnitude, SolveOrientedMagnitude, SolveNotAdjacentMagnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 1000u;
            SolveF2LMagnitude = 100u;
            SolveOppositeLayerMagnitude = 10u;
            SolveOrientedMagnitude = 50u;
            SolveNotAdjacentMagnitude = 10u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 1000u;
            SolveF2LMagnitude = 5000u;
            SolveOppositeLayerMagnitude = 100u;
            SolveOrientedMagnitude = 50u;
            SolveNotAdjacentMagnitude = 100u;
            break;
        }

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            // Check F2L solves
            if (CheckCube.IsSolved(F2L1)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L2)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L3)) Score += SolveF2LMagnitude;
            if (CheckCube.IsSolved(F2L4)) Score += SolveF2LMagnitude;

            // Check both F2L pieces conditionvin the opposite layer
            const Lyr EndLayer = Cube::GetUpSliceLayer(spin);

            if (IsF2LOriented(F2L1, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L1, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L1, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L2, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L2, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L2, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L3, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L3, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L3, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (IsF2LOriented(F2L4, CheckCube)) Score += SolveOrientedMagnitude;
            else
            {
                std::vector<Pcp> AuxF2LPos;
                Cube::AddToPositionsList(F2L4, AuxF2LPos);
                std::vector<Pce> AuxF2L;
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[0]));
                AuxF2L.push_back(static_cast<Pce>(AuxF2LPos[1]));

                if (CheckCube.ArePiecesInLayer(AuxF2L, EndLayer))
                {
                    Score += SolveOppositeLayerMagnitude;
                    if (IsF2LAdjacent(F2L4, CheckCube)) Score += SolveNotAdjacentMagnitude;
                }
            }

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }

    // Returns best solve algorithm from the Solves vector class member and his score for the given spin (CP not solved)
    bool Method::EvaluateYruRULinesResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves)
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsYruRULineBuilt(CheckCube, Sp) || IsYruRUCPBuilt(CheckCube, Sp)) continue; // Discard CP skip solves

            Score += 50u * (s.GetSize() > 50u ? 0u : 50u - s.GetSize());
            Score -= s.GetSubjectiveScore();

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }

    // Returns best solve algorithm from the Solves vector class member and his score for the given spin (CP solved)
    bool Method::EvaluateYruRUCPLinesResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves)
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsYruRUCPBuilt(CheckCube, Sp)) continue;

            Score += 50u * (s.GetSize() > 50u ? 0u : 50u - s.GetSize());
            Score -= s.GetSubjectiveScore();

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }

    // Returns best solve for pEO extension from the Solves vector class member and his score for the given spin
    bool Method::EvaluateCEORpEOResult(Algorithm& BestSolve, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp)
    {
        BestSolve.Clear();

        if (Solves.empty()) return false; // No solves

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves)
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!CheckCube.CheckOrientation(Cube::FromAbsPosition(App::DB, Sp))) continue;
            if (!IsRouxFBBuilt(CheckCube, Sp)) continue;

            Score += 50u * (s.GetSize() > 50u ? 0u : 50u - s.GetSize());
            Score -= s.GetSubjectiveScore();

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        BestSolve = ScoredSolves[0].second;

        return true;
    }

    // Returns best Mehta first block solve algorithm from the Solves vector class member and his score for the given spin
    bool Method::EvaluateMehtaFBResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        // Pieces to search
        Pce E_1, E_2, E_3, E_4;

        // Get the required pieces based on the spin
        switch (Sp)
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL:
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL:
            E_1 = Pce::FR; E_2 = Pce::FL; E_3 = Pce::BR; E_4 = Pce::BL; break;

        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL:
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL:
            E_1 = Pce::UR; E_2 = Pce::UL; E_3 = Pce::DR; E_4 = Pce::DL; break;

        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB:
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB:
            E_1 = Pce::UF; E_2 = Pce::UB; E_3 = Pce::DF; E_4 = Pce::DB; break;

        default: return 0u; // Invalid spin (should not happend)
        }

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveBeltMagnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 1000u;
            SolveBeltMagnitude = 100u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 100u;
            SolveBeltMagnitude = 1000u;
            break;
        }

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsMehtaFBBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

            // Evaluate solve size
            Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            // Check belt edges solves
            if (CheckCube.IsSolved(E_1)) Score += SolveBeltMagnitude;
            if (CheckCube.IsSolved(E_2)) Score += SolveBeltMagnitude;
            if (CheckCube.IsSolved(E_3)) Score += SolveBeltMagnitude;
            if (CheckCube.IsSolved(E_4)) Score += SolveBeltMagnitude;

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }

    // Returns best 3QB Mehta solve algorithm from the Solves vector class member and his score for the given spin
    bool Method::EvaluateMehta3QBResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        Pce E_1, E_2, E_3, E_4; // Pieces to search
        Pcp EP_1, EP_2, EP_3, EP_4; // Edge positions to check orientation

        // Get the required pieces based on the spin
        switch (Sp)
        {
        case Spn::UF:
        case Spn::UB:
        case Spn::UR:
        case Spn::UL: E_1 = Pce::FR; E_2 = Pce::FL; E_3 = Pce::BR; E_4 = Pce::BL; EP_1 = Pcp::UF; EP_2 = Pcp::UB; EP_3 = Pcp::UR; EP_4 = Pcp::UL; break;

        case Spn::DF:
        case Spn::DB:
        case Spn::DR:
        case Spn::DL: E_1 = Pce::FR; E_2 = Pce::FL; E_3 = Pce::BR; E_4 = Pce::BL; EP_1 = Pcp::DF; EP_2 = Pcp::DB; EP_3 = Pcp::DR; EP_4 = Pcp::DL; break;

        case Spn::FU:
        case Spn::FD:
        case Spn::FR:
        case Spn::FL: E_1 = Pce::UR; E_2 = Pce::UL; E_3 = Pce::DR; E_4 = Pce::DL; EP_1 = Pcp::UF; EP_2 = Pcp::DF; EP_3 = Pcp::FR; EP_4 = Pcp::FL; break;

        case Spn::BU:
        case Spn::BD:
        case Spn::BR:
        case Spn::BL: E_1 = Pce::UR; E_2 = Pce::UL; E_3 = Pce::DR; E_4 = Pce::DL; EP_1 = Pcp::UB; EP_2 = Pcp::DB; EP_3 = Pcp::BR; EP_4 = Pcp::BL; break;

        case Spn::RU:
        case Spn::RD:
        case Spn::RF:
        case Spn::RB: E_1 = Pce::UF; E_2 = Pce::UB; E_3 = Pce::DF; E_4 = Pce::DB; EP_1 = Pcp::UR; EP_2 = Pcp::DR; EP_3 = Pcp::FR; EP_4 = Pcp::FL; break;

        case Spn::LU:
        case Spn::LD:
        case Spn::LF:
        case Spn::LB: E_1 = Pce::UF; E_2 = Pce::UB; E_3 = Pce::DF; E_4 = Pce::DB; EP_1 = Pcp::UL; EP_2 = Pcp::DL; EP_3 = Pcp::FL; EP_4 = Pcp::BL; break;

        default: return 0u; // Invalid spin (should not happend)
        }

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveBeltMagnitude, SolveEOMagnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 1000u;
            SolveBeltMagnitude = 100u;
            SolveEOMagnitude = 10u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 10u;
            SolveBeltMagnitude = 1000u;
            SolveEOMagnitude = 100u;
            break;
        }

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsMehta3QBBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

            // Evaluate solve size
            Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            // Check belt edges solves
            if (CheckCube.IsSolved(E_1)) Score += SolveBeltMagnitude;
            if (CheckCube.IsSolved(E_2)) Score += SolveBeltMagnitude;
            if (CheckCube.IsSolved(E_3)) Score += SolveBeltMagnitude;
            if (CheckCube.IsSolved(E_4)) Score += SolveBeltMagnitude;

            // Give preference to belt with unsolved edge in FR absolut position
            if (!CheckCube.IsSolved(Cube::FromAbsPosition(App::FR, Sp))) Score += SolveBeltMagnitude / 2u;

            // Check up layer edges orientation
            if (CheckCube.CheckOrientation(EP_1)) Score += SolveEOMagnitude;
            if (CheckCube.CheckOrientation(EP_2)) Score += SolveEOMagnitude;
            if (CheckCube.CheckOrientation(EP_3)) Score += SolveEOMagnitude;
            if (CheckCube.CheckOrientation(EP_4)) Score += SolveEOMagnitude;

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }

    // Returns best solve algorithm from the Solves vector class member and his score for the given spin
    bool Method::EvaluateNautilusFBResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
    {
        BestSolves.clear();

        if (Solves.empty()) return false; // No solves

        // Pieces to search
        Pgr F2L_BR;
        Pce E_DB, E_DR;

        // Get the required pieces based on the spin
        switch (Sp)
        {
        case Spn::DF: F2L_BR = Pgr::F2L_U_UBL; E_DB = Pce::UB; E_DR = Pce::UL; break;
        case Spn::DB: F2L_BR = Pgr::F2L_U_UFR; E_DB = Pce::UF; E_DR = Pce::UR; break;
        case Spn::DR: F2L_BR = Pgr::F2L_U_UFL; E_DB = Pce::UL; E_DR = Pce::UF; break;
        case Spn::DL: F2L_BR = Pgr::F2L_U_UBR; E_DB = Pce::UR; E_DR = Pce::UB; break;

        case Spn::UF: F2L_BR = Pgr::F2L_D_DBR; E_DB = Pce::DB; E_DR = Pce::DR; break;
        case Spn::UB: F2L_BR = Pgr::F2L_D_DFL; E_DB = Pce::DF; E_DR = Pce::DL; break;
        case Spn::UR: F2L_BR = Pgr::F2L_D_DBL; E_DB = Pce::DL; E_DR = Pce::DB; break;
        case Spn::UL: F2L_BR = Pgr::F2L_D_DFR; E_DB = Pce::DR; E_DR = Pce::DF; break;

        case Spn::BU: F2L_BR = Pgr::F2L_F_DFR; E_DB = Pce::DF; E_DR = Pce::FR; break;
        case Spn::BD: F2L_BR = Pgr::F2L_F_UFL; E_DB = Pce::UF; E_DR = Pce::FL; break;
        case Spn::BR: F2L_BR = Pgr::F2L_F_DFL; E_DB = Pce::FL; E_DR = Pce::DF; break;
        case Spn::BL: F2L_BR = Pgr::F2L_F_UFR; E_DB = Pce::FR; E_DR = Pce::UF; break;

        case Spn::FU: F2L_BR = Pgr::F2L_B_DBL; E_DB = Pce::DB; E_DR = Pce::BL; break;
        case Spn::FD: F2L_BR = Pgr::F2L_B_UBR; E_DB = Pce::UB; E_DR = Pce::BR; break;
        case Spn::FR: F2L_BR = Pgr::F2L_B_UBL; E_DB = Pce::BL; E_DR = Pce::UB; break;
        case Spn::FL: F2L_BR = Pgr::F2L_B_DBR; E_DB = Pce::BR; E_DR = Pce::DB; break;

        case Spn::LU: F2L_BR = Pgr::F2L_R_DBR; E_DB = Pce::DR; E_DR = Pce::BR; break;
        case Spn::LD: F2L_BR = Pgr::F2L_R_UFR; E_DB = Pce::UR; E_DR = Pce::FR; break;
        case Spn::LF: F2L_BR = Pgr::F2L_R_UBR; E_DB = Pce::BR; E_DR = Pce::UR; break;
        case Spn::LB: F2L_BR = Pgr::F2L_R_DFR; E_DB = Pce::FR; E_DR = Pce::DR; break;

        case Spn::RU: F2L_BR = Pgr::F2L_L_DFL; E_DB = Pce::DL; E_DR = Pce::FL; break;
        case Spn::RD: F2L_BR = Pgr::F2L_L_UBL; E_DB = Pce::UL; E_DR = Pce::BL; break;
        case Spn::RF: F2L_BR = Pgr::F2L_L_DBL; E_DB = Pce::BL; E_DR = Pce::DL; break;
        case Spn::RB: F2L_BR = Pgr::F2L_L_UFL; E_DB = Pce::FL; E_DR = Pce::UL; break;

        default: return 0u; // Invalid spin (should not happend)
        }

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveF2LMagnitude, SolveEdge1Magnitude, SolveEdge2Magnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 10000u;
            SolveF2LMagnitude = 1000u;
            SolveEdge1Magnitude = 100u;
            SolveEdge2Magnitude = 50u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 10u;
            SolveF2LMagnitude = 10000u;
            SolveEdge1Magnitude = 1000u;
            SolveEdge2Magnitude = 500u;
            break;
        }

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsRouxFBBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

            // Evaluate solve size
            Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            // Check F2L solves
            if (CheckCube.IsSolved(F2L_BR)) Score += SolveF2LMagnitude;

            // Check edge solves
            if (CheckCube.IsSolved(E_DB)) Score += SolveEdge1Magnitude;
            if (CheckCube.IsSolved(E_DR)) Score += SolveEdge2Magnitude;

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        const uint ssize = MaxSolves < ScoredSolves.size() ? MaxSolves : static_cast<uint>(ScoredSolves.size());

        for (uint n = 0u; n < ssize; n++) BestSolves.push_back(ScoredSolves[n].second);
        return true;
    }

    // Returns best solve algorithm from the Solves vector class member and his score for the given spin
    bool Method::EvaluateNautilusSBResult(Algorithm& Solve, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
    {
        Solve.Clear();

        if (Solves.empty()) return false; // No solves

        // Pieces to search
        Pgr F2L_FR;
        Pce E_FD;

        // Get the required pieces based on the spin
        switch (Sp)
        {
        case Spn::DF: F2L_FR = Pgr::F2L_U_UFL; E_FD = Pce::UF; break;
        case Spn::DB: F2L_FR = Pgr::F2L_U_UBR; E_FD = Pce::UB; break;
        case Spn::DR: F2L_FR = Pgr::F2L_U_UFR; E_FD = Pce::UR; break;
        case Spn::DL: F2L_FR = Pgr::F2L_U_UBL; E_FD = Pce::UL; break;

        case Spn::UF: F2L_FR = Pgr::F2L_D_DFR; E_FD = Pce::DF; break;
        case Spn::UB: F2L_FR = Pgr::F2L_D_DBL; E_FD = Pce::DB; break;
        case Spn::UR: F2L_FR = Pgr::F2L_D_DBR; E_FD = Pce::DR; break;
        case Spn::UL: F2L_FR = Pgr::F2L_D_DFL; E_FD = Pce::DL; break;

        case Spn::BU: F2L_FR = Pgr::F2L_F_UFR; E_FD = Pce::UF; break;
        case Spn::BD: F2L_FR = Pgr::F2L_F_DFL; E_FD = Pce::DF; break;
        case Spn::BR: F2L_FR = Pgr::F2L_F_DFR; E_FD = Pce::FR; break;
        case Spn::BL: F2L_FR = Pgr::F2L_F_UFL; E_FD = Pce::FL; break;

        case Spn::FU: F2L_FR = Pgr::F2L_B_UBL; E_FD = Pce::UB; break;
        case Spn::FD: F2L_FR = Pgr::F2L_B_DBR; E_FD = Pce::DB; break;
        case Spn::FR: F2L_FR = Pgr::F2L_B_UBR; E_FD = Pce::BR; break;
        case Spn::FL: F2L_FR = Pgr::F2L_B_DBL; E_FD = Pce::BL; break;

        case Spn::LU: F2L_FR = Pgr::F2L_R_UBR; E_FD = Pce::UR; break;
        case Spn::LD: F2L_FR = Pgr::F2L_R_DFR; E_FD = Pce::DR; break;
        case Spn::LF: F2L_FR = Pgr::F2L_R_UFR; E_FD = Pce::FR; break;
        case Spn::LB: F2L_FR = Pgr::F2L_R_DBR; E_FD = Pce::BR; break;

        case Spn::RU: F2L_FR = Pgr::F2L_L_UFL; E_FD = Pce::UL; break;
        case Spn::RD: F2L_FR = Pgr::F2L_L_DBL; E_FD = Pce::DL; break;
        case Spn::RF: F2L_FR = Pgr::F2L_L_DFL; E_FD = Pce::FL; break;
        case Spn::RB: F2L_FR = Pgr::F2L_L_UBL; E_FD = Pce::BL; break;

        default: return 0u; // Invalid spin (should not happend)
        }

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveF2LMagnitude, SolveEdgeMagnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 1000u;
            SolveF2LMagnitude = 100u;
            SolveEdgeMagnitude = 10u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 10u;
            SolveF2LMagnitude = 1000u;
            SolveEdgeMagnitude = 100u;
            break;
        }

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsNautilusSBBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

            // Evaluate solve size
            Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            // Check F2L solves
            if (CheckCube.IsSolved(F2L_FR)) Score += SolveF2LMagnitude;

            // Check edge solves
            if (CheckCube.IsSolved(E_FD)) Score += SolveEdgeMagnitude;

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        Solve = ScoredSolves[0].second;

        return true;
    }

    bool Method::EvaluateNautilusSBSResult(Algorithm& Solve, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
    {
        Solve.Clear();

        if (Solves.empty()) return false; // No solves

        // Pieces to search
        Pgr F2L_FR;
        Pce E_FD, E_DB;

        // Get the required pieces based on the spin
        switch (Sp)
        {
        case Spn::DF: F2L_FR = Pgr::F2L_U_UFL; E_FD = Pce::UF; E_DB = Pce::UB; break;
        case Spn::DB: F2L_FR = Pgr::F2L_U_UBR; E_FD = Pce::UB; E_DB = Pce::UF; break;
        case Spn::DR: F2L_FR = Pgr::F2L_U_UFR; E_FD = Pce::UR; E_DB = Pce::UL; break;
        case Spn::DL: F2L_FR = Pgr::F2L_U_UBL; E_FD = Pce::UL; E_DB = Pce::UR; break;

        case Spn::UF: F2L_FR = Pgr::F2L_D_DFR; E_FD = Pce::DF; E_DB = Pce::DB; break;
        case Spn::UB: F2L_FR = Pgr::F2L_D_DBL; E_FD = Pce::DB; E_DB = Pce::DF; break;
        case Spn::UR: F2L_FR = Pgr::F2L_D_DBR; E_FD = Pce::DR; E_DB = Pce::DL; break;
        case Spn::UL: F2L_FR = Pgr::F2L_D_DFL; E_FD = Pce::DL; E_DB = Pce::DR; break;

        case Spn::BU: F2L_FR = Pgr::F2L_F_UFR; E_FD = Pce::UF; E_DB = Pce::DF; break;
        case Spn::BD: F2L_FR = Pgr::F2L_F_DFL; E_FD = Pce::DF; E_DB = Pce::UF; break;
        case Spn::BR: F2L_FR = Pgr::F2L_F_DFR; E_FD = Pce::FR; E_DB = Pce::FL; break;
        case Spn::BL: F2L_FR = Pgr::F2L_F_UFL; E_FD = Pce::FL; E_DB = Pce::FR; break;

        case Spn::FU: F2L_FR = Pgr::F2L_B_UBL; E_FD = Pce::UB; E_DB = Pce::DB; break;
        case Spn::FD: F2L_FR = Pgr::F2L_B_DBR; E_FD = Pce::DB; E_DB = Pce::UB; break;
        case Spn::FR: F2L_FR = Pgr::F2L_B_UBR; E_FD = Pce::BR; E_DB = Pce::BL; break;
        case Spn::FL: F2L_FR = Pgr::F2L_B_DBL; E_FD = Pce::BL; E_DB = Pce::BR; break;

        case Spn::LU: F2L_FR = Pgr::F2L_R_UBR; E_FD = Pce::UR; E_DB = Pce::DR; break;
        case Spn::LD: F2L_FR = Pgr::F2L_R_DFR; E_FD = Pce::DR; E_DB = Pce::UR; break;
        case Spn::LF: F2L_FR = Pgr::F2L_R_UFR; E_FD = Pce::FR; E_DB = Pce::BR; break;
        case Spn::LB: F2L_FR = Pgr::F2L_R_DBR; E_FD = Pce::BR; E_DB = Pce::FR; break;

        case Spn::RU: F2L_FR = Pgr::F2L_L_UFL; E_FD = Pce::UL; E_DB = Pce::DL; break;
        case Spn::RD: F2L_FR = Pgr::F2L_L_DBL; E_FD = Pce::DL; E_DB = Pce::UL; break;
        case Spn::RF: F2L_FR = Pgr::F2L_L_DFL; E_FD = Pce::FL; E_DB = Pce::BL; break;
        case Spn::RB: F2L_FR = Pgr::F2L_L_UBL; E_FD = Pce::BL; E_DB = Pce::FL; break;

        default: return 0u; // Invalid spin (should not happend)
        }

        // Based on the search policy, different conditions have different weights in the final score
        uint SolveSizeMagnitude, SolveBlockMagnitude, SolveF2LMagnitude, SolveEdgeMagnitude;
        switch (Policy)
        {
        case Plc::SHORT:
            SolveSizeMagnitude = 10000u;
            SolveBlockMagnitude = 1000u;
            SolveF2LMagnitude = 100u;
            SolveEdgeMagnitude = 10u;
            break;
        case Plc::BEST:
        default:
            SolveSizeMagnitude = 10u;
            SolveBlockMagnitude = 10000u;
            SolveF2LMagnitude = 1000u;
            SolveEdgeMagnitude = 100u;
            break;
        }

        uint Score;

        std::vector<std::pair<uint, Algorithm>> ScoredSolves;

        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = 0u;

            Cube CheckCube = CBase;
            CheckCube.ApplyAlgorithm(s);

            if (!IsNautilusSBSBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

            // Evaluate solve size
            Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
            Score -= s.GetSubjectiveScore();

            // Check last second block edge solves
            if (CheckCube.IsSolved(E_DB)) Score += SolveBlockMagnitude;

            // Check F2L solves
            if (CheckCube.IsSolved(F2L_FR)) Score += SolveF2LMagnitude;

            // Check edge solves
            if (CheckCube.IsSolved(E_FD)) Score += SolveEdgeMagnitude;

            if (Score > 0u)
            {
                std::pair<uint, Algorithm> ScoredSolve;
                ScoredSolve.first = Score;
                ScoredSolve.second = s;
                ScoredSolves.push_back(ScoredSolve);
            }
        }

        if (ScoredSolves.empty()) return false;

        std::sort(ScoredSolves.begin(), ScoredSolves.end(), [](auto a, auto b) { return a.first > b.first; });

        Solve = ScoredSolves[0].second;

        return true;
    }
}
