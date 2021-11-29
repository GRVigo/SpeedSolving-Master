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
	<mailto:grvigo@hotmail.com>
*/

#include "cfop.h"

#include <chrono>
#include <algorithm>

namespace grcube3
{
    // Default inspections (first layer down)
	const Algorithm CFOP::DefaultInspections[6] = 
	{
        Algorithm("z2"), // U
        Algorithm(""),   // D
        Algorithm("x'"), // F
        Algorithm("x"),  // B
        Algorithm("z"),  // R
        Algorithm("z'")  // L
	};
	
	// Reset the search results
	void CFOP::Reset()
	{
        for (int i = 0; i < 6; i++)
        {
            Inspections[i].clear();
            Crosses[i].clear();
            F2L_1[i].clear();
            F2L_2[i].clear();
            F2L_3[i].clear();
            F2L_4[i].clear();
            A_OLL[i].clear();
            A_PLL[i].clear();
            A_1LLL[i].clear();
            EOLL[i].clear();
            A_ZBLL[i].clear();
            AUF[i].clear();

            CasesOLL[i].clear();
            CasesPLL[i].clear();
            Cases1LLL[i].clear();
            CasesZBLL[i].clear();
        }

        TimeCrosses = TimeF2L = TimeOLL = TimePLL = Time1LLL = TimeEOLL = TimeZBLL = 0.0f;
        DepthCrosses = 0u;

        CrossLayers.clear();
        for (const auto CL : Cube::ExtLayers) CrossLayers.push_back(CL);
		
		Metric = Metrics::Movements; // Default metric
	}

    // Search the best crosses solve algorithms with the given search depth for the scramble and the maximun number of solves
    // Returns false if no crosses found
    bool CFOP::SearchCrosses(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_crosses_start = std::chrono::system_clock::now();

        DepthCrosses = MaxDepth >= 4 ? MaxDepth : 4u;

        DeepSearch DSCrosses(Scramble);
        DSCrosses.AddToOptionalPieces(Pgr::CROSS_U);
        DSCrosses.AddToOptionalPieces(Pgr::CROSS_D);
        DSCrosses.AddToOptionalPieces(Pgr::CROSS_F);
        DSCrosses.AddToOptionalPieces(Pgr::CROSS_B);
        DSCrosses.AddToOptionalPieces(Pgr::CROSS_R);
        DSCrosses.AddToOptionalPieces(Pgr::CROSS_L);

        const SearchUnit URoot(SequenceType::DOUBLE);
        const SearchUnit U(SequenceType::SINGLE);
        
        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(URoot);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U);

        SearchLevel L_NoCheck(SearchCheck::NO_CHECK);
        L_NoCheck.Add(U);
        
        DSCrosses.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        DSCrosses.AddSearchLevel(L_NoCheck); // Level 2
        for (uint l = 3; l < DepthCrosses; l++) DSCrosses.AddSearchLevel(L_Check); // Levels 3 to CrossDepth

        DSCrosses.UpdateRootData();

        DSCrosses.Run(Cores);

        Cores = DSCrosses.GetCoresUsed();

        EvaluateCrosses(DSCrosses.Solves, MaxSolves);

        const std::chrono::duration<double> crosses_elapsed_seconds = std::chrono::system_clock::now() - time_crosses_start;
        TimeCrosses = crosses_elapsed_seconds.count();

        return !DSCrosses.Solves.empty();
	}

    // Search the best crosses solve algorithms from an algorithms vector
    void CFOP::EvaluateCrosses(const std::vector<Algorithm>& Solves, const uint MaxSolves)
    {
        // Cross solves evaluation
        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer));
            EvaluateF2LResult(Crosses[CLI], MaxSolves, Solves, CubeBase, CrossLayer, Plc::BEST_SOLVES);
        }

        // Cross to down layer
        // Inspections must have two movements maximum, and movements must be turns (no parentheses)
        for (int i = 0; i < 6; i++)
        {
            for (auto& c : Crosses[i])
            {
                Inspections[i].push_back(DefaultInspections[i]);
                if (DefaultInspections[i].GetSize() > 0u)  c.TransformTurn(DefaultInspections[i].At(0));
                if (DefaultInspections[i].GetSize() > 1u) c.TransformTurn(DefaultInspections[i].At(1));
            }
        }
    }

    // Search the best F2L algorithms for the given cross
    void CFOP::SearchF2L()
    {
        const auto time_F2L_start = std::chrono::system_clock::now();

        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

            Algorithm F2L_Aux, // Auxiliary algorithm for F2L search
                      A1_F2L, A2_F2L, A3_F2L, A4_F2L; // Algorithms for F2L

            const SearchUnit U_Up(SequenceType::SINGLE, Sst::SINGLE_U);
            const SearchUnit U_Lat(SequenceType::RETURN_FIXED_SINGLE, Sst::SINGLE_U, Sst::LATERAL_UD);

            SearchLevel L_F2L_Check(SearchCheck::CHECK);
            L_F2L_Check.Add(U_Up);
            L_F2L_Check.Add(U_Lat);

            SearchLevel L_F2L_End(SearchCheck::CHECK);
            L_F2L_End.Add(U_Lat);

            F2L_1[CLI].clear();
            F2L_2[CLI].clear();
            F2L_3[CLI].clear();
            F2L_4[CLI].clear();

            Pgr F2L1, F2L2, F2L3, F2L4, CROSS;
            switch (CrossLayer)
            {
            case Lyr::U:
                CROSS = Pgr::CROSS_U;
                F2L1 = Pgr::F2L_U_UFR;
                F2L2 = Pgr::F2L_U_UFL;
                F2L3 = Pgr::F2L_U_UBR;
                F2L4 = Pgr::F2L_U_UBL;
                break;
            case Lyr::D:
                CROSS = Pgr::CROSS_D;
                F2L1 = Pgr::F2L_D_DFR;
                F2L2 = Pgr::F2L_D_DFL;
                F2L3 = Pgr::F2L_D_DBR;
                F2L4 = Pgr::F2L_D_DBL;
                break;
            case Lyr::F:
                CROSS = Pgr::CROSS_F;
                F2L1 = Pgr::F2L_F_UFR;
                F2L2 = Pgr::F2L_F_UFL;
                F2L3 = Pgr::F2L_F_DFR;
                F2L4 = Pgr::F2L_F_DFL;
                break;
            case Lyr::B:
                CROSS = Pgr::CROSS_B;
                F2L1 = Pgr::F2L_B_UBR;
                F2L2 = Pgr::F2L_B_UBL;
                F2L3 = Pgr::F2L_B_DBR;
                F2L4 = Pgr::F2L_B_DBL;
                break;
            case Lyr::R:
                CROSS = Pgr::CROSS_R;
                F2L1 = Pgr::F2L_R_UFR;
                F2L2 = Pgr::F2L_R_UBR;
                F2L3 = Pgr::F2L_R_DFR;
                F2L4 = Pgr::F2L_R_DBR;
                break;
            case Lyr::L:
                CROSS = Pgr::CROSS_L;
                F2L1 = Pgr::F2L_L_UFL;
                F2L2 = Pgr::F2L_L_UBL;
                F2L3 = Pgr::F2L_L_DFL;
                F2L4 = Pgr::F2L_L_DBL;
                break;
            default: return;
            }

            bool F2L_Found = false; // F2L found flag
            uint n = 0u;
            for (const auto& Cross : Crosses[CLI])
            {
                const Algorithm AlgBase = Scramble + Inspections[CLI][n] + Cross;
                const Cube C(AlgBase);
                Cube CubeF2L;

                F2L_1[CLI].push_back(Algorithm(""));
                F2L_2[CLI].push_back(Algorithm(""));
                F2L_3[CLI].push_back(Algorithm(""));
                F2L_4[CLI].push_back(Algorithm(""));

                do
                {
                    F2L_Aux = F2L_1[CLI][n] + F2L_2[CLI][n] + F2L_3[CLI][n] + F2L_4[CLI][n];

                    CubeF2L = C;
                    CubeF2L.ApplyAlgorithm(F2L_Aux);

                    bool F2L1_IsSolved = CubeF2L.IsSolved(F2L1);
                    bool F2L2_IsSolved = CubeF2L.IsSolved(F2L2);
                    bool F2L3_IsSolved = CubeF2L.IsSolved(F2L3);
                    bool F2L4_IsSolved = CubeF2L.IsSolved(F2L4);

                    if (F2L1_IsSolved && F2L2_IsSolved && F2L3_IsSolved && F2L4_IsSolved) break;

                    Algorithm AlgSearch = AlgBase;
                    AlgSearch += F2L_Aux;

                    DeepSearch DS_F2L(AlgSearch);

                    DS_F2L.AddSearchLevel(L_F2L_Check);
                    DS_F2L.AddSearchLevel(L_F2L_Check);
                    DS_F2L.AddSearchLevel(L_F2L_Check);
                    DS_F2L.AddSearchLevel(L_F2L_Check);
                    DS_F2L.AddSearchLevel(L_F2L_End);
                    DS_F2L.UpdateRootData();

                    DS_F2L.AddToMandatoryPieces(CROSS);

                    if (F2L1_IsSolved) DS_F2L.AddToMandatoryPieces(F2L1);
                    else DS_F2L.AddToOptionalPieces(F2L1);

                    if (F2L2_IsSolved) DS_F2L.AddToMandatoryPieces(F2L2);
                    else DS_F2L.AddToOptionalPieces(F2L2);

                    if (F2L3_IsSolved) DS_F2L.AddToMandatoryPieces(F2L3);
                    else DS_F2L.AddToOptionalPieces(F2L3);

                    if (F2L4_IsSolved) DS_F2L.AddToMandatoryPieces(F2L4);
                    else DS_F2L.AddToOptionalPieces(F2L4);

                    DS_F2L.Run(Cores);

                    Cores = DS_F2L.GetCoresUsed();

                    F2L_Found = false;

                    std::vector<Algorithm> Solves;
                    EvaluateF2LResult(Solves, 1u, DS_F2L.Solves, CubeF2L, CrossLayer, Plc::SHORT); // Only the shortest F2L solve

                    if (!Solves.empty())
                    {
                        F2L_Aux = Solves[0];

                        CubeF2L.ApplyAlgorithm(F2L_Aux);

                        if ((!F2L1_IsSolved && CubeF2L.IsSolved(F2L1)) || (!F2L2_IsSolved && CubeF2L.IsSolved(F2L2)) ||
                            (!F2L3_IsSolved && CubeF2L.IsSolved(F2L3)) || (!F2L4_IsSolved && CubeF2L.IsSolved(F2L4)))
                        {
                            F2L_Found = true;
                            if (F2L_1[CLI][n].GetSize() == 0u) F2L_1[CLI][n] = F2L_Aux;
                            else if (F2L_2[CLI][n].GetSize() == 0u) F2L_2[CLI][n] = F2L_Aux;
                            else if (F2L_3[CLI][n].GetSize() == 0u) F2L_3[CLI][n] = F2L_Aux;
                            else if (F2L_4[CLI][n].GetSize() == 0u) F2L_4[CLI][n] = F2L_Aux;
                            else F2L_Found = false;
                        }
                    }
                } while (F2L_Found);
                n++;
            }
        }

        const std::chrono::duration<double> F2L_elapsed_seconds = std::chrono::system_clock::now() - time_F2L_start;
        TimeF2L = F2L_elapsed_seconds.count();
    }

	// Orientation of last layer search
	void CFOP::SearchOLL()
	{
        const auto time_OLL_start = std::chrono::system_clock::now();

        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

            A_OLL[CLI].clear();
            CasesOLL[CLI].clear();

            for (uint n = 0u; n < Crosses[CLI].size(); n++)
            {
                A_OLL[CLI].push_back(Algorithm(""));
                CasesOLL[CLI].push_back("");

                Algorithm Alg = Inspections[CLI][n];
                Alg += Crosses[CLI][n];
                Alg += F2L_1[CLI][n];
                Alg += F2L_2[CLI][n];
                Alg += F2L_3[CLI][n];
                Alg += F2L_4[CLI][n];

                Cube CubeF2L = CubeBase;
                CubeF2L.ApplyAlgorithm(Alg);

                Cube::OrientateLL(A_OLL[CLI][n], CasesOLL[CLI][n], AlgSets::OLL, CubeF2L);
            }
        }

        const std::chrono::duration<double> OLL_elapsed_seconds = std::chrono::system_clock::now() - time_OLL_start;
        TimeOLL = OLL_elapsed_seconds.count();
	}

	// Permutation of last layer search + AUF
	void CFOP::SearchPLL()
	{
        const auto time_PLL_start = std::chrono::system_clock::now();

        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

            A_PLL[CLI].clear();
            AUF[CLI].clear();
            CasesPLL[CLI].clear();

            for (uint n = 0u; n < Crosses[CLI].size(); n++)
            {
                A_PLL[CLI].push_back(Algorithm(""));
                CasesPLL[CLI].push_back("");

                Algorithm Alg = Inspections[CLI][n];
                Alg += Crosses[CLI][n];
                Alg += F2L_1[CLI][n];
                Alg += F2L_2[CLI][n];
                Alg += F2L_3[CLI][n];
                Alg += F2L_4[CLI][n];
                Alg += A_OLL[CLI][n];

                Cube CubeOLL = CubeBase;
                CubeOLL.ApplyAlgorithm(Alg);

                Stp AUFStep;

                Cube::SolveLL(A_PLL[CLI][n], CasesPLL[CLI][n], AUFStep, AlgSets::PLL, CubeOLL);

                Algorithm Aux;
                Aux.Append(AUFStep);
                AUF[CLI].push_back(Aux);
            }
        }

        const std::chrono::duration<double> PLL_elapsed_seconds = std::chrono::system_clock::now() - time_PLL_start;
        TimePLL = PLL_elapsed_seconds.count();
	}

    // One-Look Last Layer search (1LLL)
    void CFOP::Search1LLL()
    {
        const auto time_1LLL_start = std::chrono::system_clock::now();

        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

            A_1LLL[CLI].clear();
            AUF[CLI].clear();
            Cases1LLL[CLI].clear();

            for (uint n = 0u; n < Crosses[CLI].size(); n++)
            {
                A_1LLL[CLI].push_back(Algorithm(""));
                Cases1LLL[CLI].push_back("");

                Algorithm Alg = Inspections[CLI][n];
                Alg += Crosses[CLI][n];
                Alg += F2L_1[CLI][n];
                Alg += F2L_2[CLI][n];
                Alg += F2L_3[CLI][n];
                Alg += F2L_4[CLI][n];

                Cube CubeF2L = CubeBase;
                CubeF2L.ApplyAlgorithm(Alg);

                Stp AUFStep;

                Cube::SolveLL(A_1LLL[CLI][n], Cases1LLL[CLI][n], AUFStep, AlgSets::_1LLL, CubeF2L);

                Algorithm Aux;
                Aux.Append(AUFStep);
                AUF[CLI].push_back(Aux);
            }
        }

        const std::chrono::duration<double> elapsed_seconds_1LLL = std::chrono::system_clock::now() - time_1LLL_start;
        Time1LLL = elapsed_seconds_1LLL.count();
    }

    // Last layer edges orientation search
    void CFOP::SearchEOLL()
    {
        const auto time_EOLL_start = std::chrono::system_clock::now();

        const SearchUnit U_UR(SequenceType::SINGLE, Sst::SINGLE_UR);
        const SearchUnit U_OR1(SequenceType::RETURN_FIXED_SINGLE, Sst::SINGLE_U, Sst::PETRUS_OR_U);
        const SearchUnit U_OR2(SequenceType::RETURN_FIXED_SINGLE, Sst::SINGLE_R, Sst::PETRUS_OR_R);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_UR);
        L_Check.Add(U_OR1);
        L_Check.Add(U_OR2);

        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

            EOLL[CLI].clear();

            for (uint n = 0u; n < Crosses[CLI].size(); n++)
            {
                EOLL[CLI].push_back(Algorithm(""));

                Algorithm Alg = Scramble;
                Alg += Inspections[CLI][n];
                Alg += Crosses[CLI][n];
                Alg += F2L_1[CLI][n];
                Alg += F2L_2[CLI][n];
                Alg += F2L_3[CLI][n];
                Alg += F2L_4[CLI][n];

                Cube CubeF2L(Alg);

                if (!IsF2LBuilt(CubeF2L, CrossLayer) || CubeF2L.EO()) continue;

                DeepSearch DSEO(Alg, Plc::SHORT); // Deep search for edges orientation

                Pgr FirstLayer, MidLayer, LLCross;

                switch (CrossLayer)
                {
                case Lyr::U: LLCross = Pgr::CROSS_D; FirstLayer = Pgr::LAYER_U; MidLayer = Pgr::LAYER_E; break;
                case Lyr::D: LLCross = Pgr::CROSS_U; FirstLayer = Pgr::LAYER_D; MidLayer = Pgr::LAYER_E; break;
                case Lyr::F: LLCross = Pgr::CROSS_B; FirstLayer = Pgr::LAYER_F; MidLayer = Pgr::LAYER_S; break;
                case Lyr::B: LLCross = Pgr::CROSS_F; FirstLayer = Pgr::LAYER_B; MidLayer = Pgr::LAYER_S; break;
                case Lyr::R: LLCross = Pgr::CROSS_L; FirstLayer = Pgr::LAYER_R; MidLayer = Pgr::LAYER_M; break;
                case Lyr::L: LLCross = Pgr::CROSS_R; FirstLayer = Pgr::LAYER_L; MidLayer = Pgr::LAYER_M; break;
                default: continue;
                }

                DSEO.AddToMandatoryPieces(FirstLayer);
                DSEO.AddToMandatoryPieces(MidLayer);
                DSEO.AddToMandatoryOrientations(LLCross);

                for (int l = 0; l < 5; l++) DSEO.AddSearchLevel(L_Check); // Add needed search levels

                DSEO.UpdateRootData();

                DSEO.Run(Cores);

                DSEO.EvaluateShortestResult(EOLL[CLI][n], true);
            }
        }

        const std::chrono::duration<double> EOLL_elapsed_seconds = std::chrono::system_clock::now() - time_EOLL_start;
        TimeEOLL = EOLL_elapsed_seconds.count();
    }

    // ZBLL last layer search + AUF
    void CFOP::SearchZBLL()
    {
        const auto time_ZBLL_start = std::chrono::system_clock::now();

        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

            A_ZBLL[CLI].clear();
            AUF[CLI].clear();
            CasesZBLL[CLI].clear();

            for (uint n = 0u; n < Crosses[CLI].size(); n++)
            {
                A_ZBLL[CLI].push_back(Algorithm(""));
                CasesZBLL[CLI].push_back("");

                Algorithm Alg = Inspections[CLI][n];
                Alg += Crosses[CLI][n];
                Alg += F2L_1[CLI][n];
                Alg += F2L_2[CLI][n];
                Alg += F2L_3[CLI][n];
                Alg += F2L_4[CLI][n];
                Alg += EOLL[CLI][n];

                Cube CubeEOLL = CubeBase;
                CubeEOLL.ApplyAlgorithm(Alg);

                Stp AUFStep;

                Cube::SolveLL(A_ZBLL[CLI][n], CasesZBLL[CLI][n], AUFStep, AlgSets::ZBLL, CubeEOLL);

                Algorithm Aux;
                Aux.Append(AUFStep);
                AUF[CLI].push_back(Aux);
            }
        }

        const std::chrono::duration<double> ZBLL_elapsed_seconds = std::chrono::system_clock::now() - time_ZBLL_start;
        TimeZBLL = ZBLL_elapsed_seconds.count();
    }

    // Set regrips
    void CFOP::SetRegrips()
    {
        for (const auto CrossLayer : CrossLayers)
        {
            int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer));

            if (!CheckSolveConsistency(CrossLayer)) continue;

            for (uint n = 0u; n < Crosses[CLI].size(); n++)
            {
                Crosses[CLI][n] = Crosses[CLI][n].GetRegrip();
                F2L_1[CLI][n] = F2L_1[CLI][n].GetRegrip();
                F2L_2[CLI][n] = F2L_2[CLI][n].GetRegrip();
                F2L_3[CLI][n] = F2L_3[CLI][n].GetRegrip();
                F2L_4[CLI][n] = F2L_4[CLI][n].GetRegrip();
                if (!EOLL[CLI].empty()) EOLL[CLI][n] = EOLL[CLI][n].GetRegrip();

                if (Algorithm::IsTurn(Crosses[CLI][n].First()))
                {
                    Inspections[CLI][n].AppendShrink(Crosses[CLI][n].First());
                    if (Inspections[CLI][n].GetSize() == 3u) Inspections[CLI][n] = Inspections[CLI][n].GetCancellations();
                    Crosses[CLI][n].EraseFirst();
                }

                if (Algorithm::IsTurn(Crosses[CLI][n].Last()))
                {
                    F2L_1[CLI][n].Insert(0u, Crosses[CLI][n].Last());
                    while (F2L_1[CLI][n].Shrink());
                    Crosses[CLI][n].EraseLast();
                }

                if (Algorithm::IsTurn(F2L_1[CLI][n].Last()))
                {
                    if (F2L_2[CLI][n].Empty())
                    {
                        if (!A_OLL[CLI].empty())
                        {
                            A_OLL[CLI][n].Insert(0u, F2L_1[CLI][n].Last());
                            while (A_OLL[CLI][n].Shrink());
                            F2L_1[CLI][n].EraseLast();
                        }
                        else if (!A_1LLL[CLI].empty())
                        {
                            A_1LLL[CLI][n].Insert(0u, F2L_1[CLI][n].Last());
                            while (A_1LLL[CLI][n].Shrink());
                            F2L_1[CLI][n].EraseLast();
                        }
                        else if (!EOLL[CLI].empty())
                        {
                            EOLL[CLI][n].Insert(0u, F2L_1[CLI][n].Last());
                            while (EOLL[CLI][n].Shrink());
                            F2L_1[CLI][n].EraseLast();
                        }
                    }
                    else
                    {
                        F2L_2[CLI][n].Insert(0u, F2L_1[CLI][n].Last());
                        while (F2L_2[CLI][n].Shrink());
                        F2L_1[CLI][n].EraseLast();
                    }
                }

                if (Algorithm::IsTurn(F2L_2[CLI][n].Last()))
                {
                    if (F2L_3[CLI][n].Empty())
                    {
                        if (!A_OLL[CLI].empty())
                        {
                            A_OLL[CLI][n].Insert(0u, F2L_2[CLI][n].Last());
                            while (A_OLL[CLI][n].Shrink());
                            F2L_2[CLI][n].EraseLast();
                        }
                        else if (!A_1LLL[CLI].empty())
                        {
                            A_1LLL[CLI][n].Insert(0u, F2L_2[CLI][n].Last());
                            while (A_1LLL[CLI][n].Shrink());
                            F2L_2[CLI][n].EraseLast();
                        }
                        else if (!EOLL[CLI].empty())
                        {
                            EOLL[CLI][n].Insert(0u, F2L_2[CLI][n].Last());
                            while (EOLL[CLI][n].Shrink());
                            F2L_2[CLI][n].EraseLast();
                        }
                    }
                    else
                    {
                        F2L_3[CLI][n].Insert(0u, F2L_2[CLI][n].Last());
                        while (F2L_3[CLI][n].Shrink());
                        F2L_2[CLI][n].EraseLast();
                    }
                }

                if (Algorithm::IsTurn(F2L_3[CLI][n].Last()))
                {
                    if (F2L_4[CLI][n].Empty())
                    {
                        if (!A_OLL[CLI].empty())
                        {
                            A_OLL[CLI][n].Insert(0u, F2L_3[CLI][n].Last());
                            while (A_OLL[CLI][n].Shrink());
                            F2L_3[CLI][n].EraseLast();
                        }
                        else if (!A_1LLL[CLI].empty())
                        {
                            A_1LLL[CLI][n].Insert(0u, F2L_3[CLI][n].Last());
                            while (A_1LLL[CLI][n].Shrink());
                            F2L_3[CLI][n].EraseLast();
                        }
                        else if (!EOLL[CLI].empty())
                        {
                            EOLL[CLI][n].Insert(0u, F2L_3[CLI][n].Last());
                            while (EOLL[CLI][n].Shrink());
                            F2L_3[CLI][n].EraseLast();
                        }
                    }
                    else
                    {
                        F2L_4[CLI][n].Insert(0u, F2L_3[CLI][n].Last());
                        while (F2L_4[CLI][n].Shrink());
                        F2L_3[CLI][n].EraseLast();
                    }
                }

                if (Algorithm::IsTurn(F2L_4[CLI][n].Last()))
                {
                    if (!A_OLL[CLI].empty())
                    {
                        A_OLL[CLI][n].Insert(0u, F2L_4[CLI][n].Last());
                        while (A_OLL[CLI][n].Shrink());
                        F2L_4[CLI][n].EraseLast();
                    }
                    else if (!A_1LLL[CLI].empty())
                    {
                        A_1LLL[CLI][n].Insert(0u, F2L_4[CLI][n].Last());
                        while (A_1LLL[CLI][n].Shrink());
                        F2L_4[CLI][n].EraseLast();
                    }
                    else if (!EOLL[CLI].empty())
                    {
                        EOLL[CLI][n].Insert(0u, F2L_4[CLI][n].Last());
                        while (EOLL[CLI][n].Shrink());
                        F2L_4[CLI][n].EraseLast();
                    }
                }
                if (!EOLL[CLI].empty())
                {
                    if (Algorithm::IsTurn(EOLL[CLI][n].Last()))
                    {
                        A_ZBLL[CLI][n].Insert(0u, EOLL[CLI][n].Last());
                        while (A_ZBLL[CLI][n].Shrink());
                        EOLL[CLI][n].EraseLast();
                    }
                }
            }
        }
    }

    // Get if the given F2L pair is adjacent (the corner next to the edge), orientation is not checked
    bool CFOP::IsF2LAdjacent(const Pgr F2L, const Cube& C)
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
    bool CFOP::IsF2LOriented(const Pgr F2L, const Cube& C)
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
   
    // Returns best F2L solve from the Solves vector class member
    // F2L pieces are used as evaluation condition
    bool CFOP::EvaluateF2LResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves,
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
        case Plc::BEST_SOLVES:
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
            Score += s.GetSubjectiveScore();

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

    // Check if the CFOP first cross is built
    bool CFOP::IsCrossBuilt(const Cube& C, const Lyr CrossLayer)
    {
        switch (CrossLayer)
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

    // Check if first two layers are built
    bool CFOP::IsF2LBuilt(const Cube& C, const Lyr CrossLayer)
    {
        switch (CrossLayer)
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

    // Check if the solves for the given cross layer are consistent (all needed algorithms are present)
    bool CFOP::CheckSolveConsistency(const Lyr CrossLayer) const
    {
        const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

        if (Inspections[CLI].empty()) return false;

        auto n = Inspections[CLI].size();

        return (Crosses[CLI].size() == n) &&
               (F2L_1[CLI].size() == n) && (F2L_2[CLI].size() == n) && (F2L_3[CLI].size() == n) && (F2L_4[CLI].size() == n) &&
               ((A_1LLL[CLI].size() == n) || (A_OLL[CLI].size() == n && A_PLL[CLI].size() == n) || 
               (EOLL[CLI].size() == n && A_ZBLL[CLI].size() == n)) && (AUF[CLI].size() == n);
    }

    // Get a full solve report (for each cross layer)
    std::string CFOP::GetReport(const bool cancellations, const bool debug) const
    {
        std::string Report = "CFOP search for Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble(), ReportLine;
        Report += "\n--------------------------------------------------------------------------------\n";

        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

            if (!CheckSolveConsistency(CrossLayer)) continue;

			for (uint n = 0u; n < Crosses[CLI].size(); n++)
			{
                ReportLine.clear();

				Cube C = CubeBase;
				C.ApplyAlgorithm(Inspections[CLI][n]);
				C.ApplyAlgorithm(Crosses[CLI][n]);

				if (IsCrossBuilt(C, CrossLayer))
				{
					if (!F2L_4[CLI][n].Empty()) ReportLine += "[Cross ";
					else if (!F2L_3[CLI][n].Empty()) ReportLine += "[XCross ";
					else if (!F2L_2[CLI][n].Empty()) ReportLine += "[XXCross ";
					else if (!F2L_1[CLI][n].Empty()) ReportLine += "[XXXCross ";
					else ReportLine += "[XXXXCross ";
					
                    ReportLine.push_back(Cube::GetLayerChar(CrossLayer));
                    ReportLine += "|" + Algorithm::GetMetricValue(GetMetricSolve(Cube::LayerToFace(CrossLayer), n));
                    if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(Cube::LayerToFace(CrossLayer), n)) + ")";
                    ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
					if (!Inspections[CLI][n].Empty()) ReportLine += "(" + Inspections[CLI][n].ToString() + ") ";
                    ReportLine += "(" + Crosses[CLI][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
                        ReportLine += "[Cross ";
                        ReportLine.push_back(Cube::GetLayerChar(CrossLayer));
                        ReportLine += "] Cross not found in " + std::to_string(DepthCrosses) + " or less movements\n";
					}
                    if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(F2L_1[CLI][n]);
				C.ApplyAlgorithm(F2L_2[CLI][n]);
				C.ApplyAlgorithm(F2L_3[CLI][n]);
				C.ApplyAlgorithm(F2L_4[CLI][n]);

                if (!IsF2LBuilt(C, CrossLayer)) ReportLine += " F2L not found:";
                if (!F2L_1[CLI][n].Empty()) ReportLine += " (" + F2L_1[CLI][n].ToString() + ")";
                if (!F2L_2[CLI][n].Empty()) ReportLine += " (" + F2L_2[CLI][n].ToString() + ")";
                if (!F2L_3[CLI][n].Empty()) ReportLine += " (" + F2L_3[CLI][n].ToString() + ")";
                if (!F2L_4[CLI][n].Empty()) ReportLine += " (" + F2L_4[CLI][n].ToString() + ")";

                if (!IsF2LBuilt(C, CrossLayer))
				{
                    ReportLine.push_back('\n');
                    if (debug) Report += ReportLine;
					continue;
				}

                if (!CasesOLL[CLI].empty()) // OLL + PLL
				{
					C.ApplyAlgorithm(A_OLL[CLI][n]);

					if (IsLastLayerOriented(C))
					{
                        if (!A_OLL[CLI][n].Empty())
                        {
                            if (debug) ReportLine += " {OLL: " + CasesOLL[CLI][n] + "}";
                            ReportLine += " (" + A_OLL[CLI][n].ToString() + ")";
                        }
					}
					else
					{
                        ReportLine += " OLL not found\n";
                        if (debug) Report += ReportLine;
						continue;
					}

					C.ApplyAlgorithm(A_PLL[CLI][n]);
					C.ApplyAlgorithm(AUF[CLI][n]);

					if (C.IsSolved())
					{
                        if (!A_PLL[CLI][n].Empty())
                        {
                            if (debug) ReportLine += " {PLL: " + CasesPLL[CLI][n] + "}";
                            ReportLine += " (" + A_PLL[CLI][n].ToString() + ")";
                        }
						if (!AUF[CLI][n].Empty() && AUF[CLI][n].At(0) != Stp::NONE) ReportLine += " (" + AUF[CLI][n].ToString() + ")";
					}
					else
					{
                        ReportLine += " PLL not found\n";
                        if (debug) Report += ReportLine;
						continue;
					}
				}
                else if (!EOLL[CLI].empty()) // EO + ZBLL
                {
                    C.ApplyAlgorithm(EOLL[CLI][n]);

                    if (C.EO())
                    {
                        if (!EOLL[CLI][n].Empty()) ReportLine += " (" + EOLL[CLI][n].ToString() + ")";
                    }
                    else
                    {
                        ReportLine += " last layer edges not oriented\n";
                        if (debug) Report += ReportLine;
                        continue;
                    }

                    C.ApplyAlgorithm(A_ZBLL[CLI][n]);
                    C.ApplyAlgorithm(AUF[CLI][n]);

                    if (C.IsSolved())
                    {
                        if (!A_ZBLL[CLI][n].Empty())
                        {
                            if (debug) ReportLine += " {ZBLL: " + CasesZBLL[CLI][n] + "}";
                            ReportLine += " (" + A_ZBLL[CLI][n].ToString() + ")";
                        }
                        if (!AUF[CLI][n].Empty() && AUF[CLI][n].At(0) != Stp::NONE)
                            ReportLine += " (" + AUF[CLI][n].ToString() + ")";
                    }
                    else
                    {
                        ReportLine += " ZBLL not found\n";
                        if (debug) Report += ReportLine;
                        continue;
                    }
                }
                else if (!Cases1LLL[CLI].empty()) // 1LLL
				{
					C.ApplyAlgorithm(A_1LLL[CLI][n]);
					C.ApplyAlgorithm(AUF[CLI][n]);

					if (C.IsSolved())
					{
                        if (!A_1LLL[CLI][n].Empty())
                        {
                            if (debug) ReportLine += " {1LLL: " + Cases1LLL[CLI][n] + "}";
                            ReportLine += " (" + A_1LLL[CLI][n].ToString() + ")";
                        }
						if (!AUF[CLI][n].Empty() && AUF[CLI][n].At(0) != Stp::NONE)
                            ReportLine += " (" + AUF[CLI][n].ToString() + ")";
					}
					else
					{
                        ReportLine += " 1LLL not found\n";
                        if (debug) Report += ReportLine;
						continue;
					}
				}
                else ReportLine += " last layer not found";

                if (debug)
                {
                    if (C.IsSolved()) ReportLine += " [OK]\n"; // Debug code
                    else ReportLine += " [Not OK]\n"; // Debug code
                }
                else ReportLine.push_back('\n');
                if (debug || C.IsSolved()) Report += ReportLine;
			}
        }

        return Report;
    }

    // Get a solve time report
    std::string CFOP::GetTimeReport() const
    {
        std::string Report;

        Report += "Total search time: " + std::to_string(GetTime()) + " s\n";
        Report += "Crosses search time: " + std::to_string(GetTimeCrosses()) + " s\n";
        Report += "F2L search time: " + std::to_string(GetTimeF2L()) + " s\n";
        Report += "Last layer search time: " + std::to_string(GetTimeLL()) + " s\n";
        Report += "Threads used: " + std::to_string(GetCores() > 0 ? GetCores() : 0) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

    // Get a solve report for a cross layer
    std::string CFOP::GetReport(const Lyr CrossLayer, const uint n) const
    {
        const Fce CrossFace = Cube::LayerToFace(CrossLayer);
        const int CLI = static_cast<int>(CrossFace); // Cross layer index

        if (!CheckSolveConsistency(CrossLayer) || Crosses[CLI].size() <= n)
        {
            std::string FReport = "No solve for cross in layer ";
            FReport.push_back(Cube::GetLayerChar(CrossLayer));
            FReport += ", position " + std::to_string(n) + "\n";
            return FReport;
        }

        std::string Report  = "CFOP search with cross in layer ";
        Report += Cube::GetLayerChar(CrossLayer);
        Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

        Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[CLI][n]);
        C.ApplyAlgorithm(Crosses[CLI][n]);

        if (!IsCrossBuilt(C, CrossLayer))
        {
            Report += "Cross not solved in " + std::to_string(DepthCrosses) + " movements!\n";
            Report += "Search time: " + std::to_string(GetTime()) + " s\n";
            return Report;
        }

        const std::string Inspection = GetTextInspection(CrossFace, n);
        if (!Inspection.empty()) Report +=  "Inspection: " + Inspection + "\n\n";

        std::string CrossType;

        if (!F2L_4[CLI][n].Empty()) CrossType = "Cross";
        else if (!F2L_3[CLI][n].Empty()) CrossType += "XCross";
        else if (!F2L_2[CLI][n].Empty()) CrossType += "XXCross";
        else if (!F2L_1[CLI][n].Empty()) CrossType += "XXXCross";
        else CrossType += "XXXXCross";

        Report += CrossType + " (" + Algorithm::GetMetricValue(GetMetricCross(CrossFace, n)) + "): " + GetTextCross(CrossFace, n) + "\n";

        // Show F2L solves
        C.ApplyAlgorithm(F2L_1[CLI][n]);
        C.ApplyAlgorithm(F2L_2[CLI][n]);
        C.ApplyAlgorithm(F2L_3[CLI][n]);
        C.ApplyAlgorithm(F2L_4[CLI][n]);

        if (!IsF2LBuilt(C, CrossLayer))
        {
            Report += "F2L not solved!\n";
            Report += "Search time: " + std::to_string(GetTime()) + " s\n";
            return Report;
        }

        std::string F2LString = GetTextF2L_1(CrossFace, n);
        if (!F2LString.empty()) Report += "F2L 1 (" + Algorithm::GetMetricValue(GetMetricF2L_1(CrossFace, n)) + "): " + F2LString + "\n";
        F2LString = GetTextF2L_2(CrossFace, n);
        if (!F2LString.empty()) Report += "F2L 2 (" + Algorithm::GetMetricValue(GetMetricF2L_2(CrossFace, n)) + "): " + F2LString + "\n";
        F2LString = GetTextF2L_3(CrossFace, n);
        if (!F2LString.empty()) Report += "F2L 3 (" + Algorithm::GetMetricValue(GetMetricF2L_3(CrossFace, n)) + "): " + F2LString + "\n";
        F2LString = GetTextF2L_4(CrossFace, n);
        if (!F2LString.empty()) Report += "F2L 4 (" + Algorithm::GetMetricValue(GetMetricF2L_4(CrossFace, n)) + "): " + F2LString + "\n";

        if (!A_OLL[CLI].empty()) // OLL + PLL
        {
            // Show OLL solve
            C.ApplyAlgorithm(A_OLL[CLI][n]);
            
            if (!IsLastLayerOriented(C))
            {
                Report += "OLL not solved!\n";
                Report += "Search time: " + std::to_string(GetTime()) + " s\n";
                return Report;
            }

            std::string OLLString = GetTextOLL(CrossFace, n);
            Report += "OLL (" + Algorithm::GetMetricValue(GetMetricOLL(CrossFace, n)) + "): ";
            if (OLLString.size() <= 3u) Report += "SKIP!\n";
            else Report += OLLString + "\n";

            // Check PLL solve
            C.ApplyAlgorithm(A_PLL[CLI][n]);
            C.ApplyAlgorithm(AUF[CLI][n]);
            
            if (!C.IsSolved())
            {
                Report += "PLL not solved!\n";
                Report += "Search time: " + std::to_string(GetTime()) + " s\n";
                return Report;
            }

            // Show PLL solve
            std::string PLLString = GetTextPLL(CrossFace, n);
            Report += "PLL (" + Algorithm::GetMetricValue(GetMetricPLL(CrossFace, n)) + "): ";
            if (PLLString.size() <= 3u) Report += "SKIP!\n";
            else Report += PLLString + "\n";

            // Show AUF solve
            std::string AUFString = GetTextAUF(CrossFace, n);
            if (AUFString.size() > 0u) Report += "AUF: " + AUFString + "\n";
        }

        else if (!EOLL[CLI].empty()) // LLEO + ZBLL
        {
            // Show edges orientation
            C.ApplyAlgorithm(EOLL[CLI][n]);

            if (!C.EO())
            {
                Report += "Last layer edges not oriented!\n";
                Report += "Search time: " + std::to_string(GetTime()) + " s\n";
                return Report;
            }

            std::string EOString = GetTextEOLL(CrossFace, n);
            Report += "EO (" + Algorithm::GetMetricValue(GetMetricEOLL(CrossFace, n)) + "): ";
            if (EOString.size() <= 3u) Report += "SKIP!\n";
            else Report += EOString + "\n";

            // Check ZBLL solve
            C.ApplyAlgorithm(A_ZBLL[CLI][n]);
            C.ApplyAlgorithm(AUF[CLI][n]);

            if (!C.IsSolved())
            {
                Report += "ZBLL not solved!\n";
                Report += "Search time: " + std::to_string(GetTime()) + " s\n";
                return Report;
            }

            // Show ZBLL solve
            std::string ZBLLString = GetTextZBLL(CrossFace, n);
            Report += "ZBLL (" + Algorithm::GetMetricValue(GetMetricZBLL(CrossFace, n)) + "): ";
            if (ZBLLString.size() <= 3u) Report += "SKIP!\n";
            else Report += ZBLLString + "\n";

            // Show AUF solve
            std::string AUFString = GetTextAUF(CrossFace, n);
            if (AUFString.size() > 0u) Report += "AUF: " + AUFString + "\n";
        }

        else if (!A_1LLL[CLI].empty()) // 1LLL
        {
            // Show 1LLL solve
            C.ApplyAlgorithm(A_1LLL[CLI][n]);
            C.ApplyAlgorithm(AUF[CLI][n]);
            
            if (!C.IsSolved())
            {
                Report += "1LLL not solved!\n";
                Report += "Search time: " + std::to_string(GetTime()) + " s\n";
                return Report;
            }
            std::string String1LLL = GetText1LLL(CrossFace, n);
            Report += "1LLL (" + Algorithm::GetMetricValue(GetMetric1LLL(CrossFace, n)) + "): " + String1LLL + "\n";
            if (String1LLL.size() <= 3u) Report += "SKIP!\n";

            // Show AUF solve
            std::string AUFString = GetTextAUF(CrossFace, n);
            if (AUFString.size() > 0u) Report += "AUF: " + AUFString + "\n";
        }

        else
        {
            Report += "Last layer not solved!\n";
            return Report;
        }

        // Show summary
        Report += "\nSolve metric: " + Algorithm::GetMetricValue(GetMetricSolve(CrossFace, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
        Report += CrossType + " in layer ";
		Report.push_back(Cube::GetLayerChar(CrossLayer));
        Report += ", in " + std::to_string(DepthCrosses) + " or less movements\n";
        if (!CasesOLL[CLI].empty()) // OLL + PLL
        {
            Report += "OLL case: " + GetTextOLLCase(CrossFace, n) + "\n";
            Report += "PLL case: " + GetTextPLLCase(CrossFace, n) + "\n";
        }
        else if (!CasesZBLL[CLI].empty()) // EO + ZBLL
            Report += "ZBLL case: " + GetTextZBLLCase(CrossFace, n) + "\n";
        else if (!Cases1LLL[CLI].empty()) // 1LLL
            Report += "1LLL case: " + GetText1LLLCase(CrossFace, n) + "\n";

        return Report;
    };

    // Check if in the given cross face the solve is OK
    bool CFOP::IsSolved(const Fce CrossFace, const uint n) const
    {
        const int CLI = static_cast<int>(CrossFace); // Cross layer index

        if (!CheckSolveConsistency(Cube::FaceToLayer(CrossFace)) || Crosses[CLI].size() <= n) return false;

        Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[CLI][n]);
        C.ApplyAlgorithm(Crosses[CLI][n]);
        C.ApplyAlgorithm(F2L_1[CLI][n]);
        C.ApplyAlgorithm(F2L_2[CLI][n]);
        C.ApplyAlgorithm(F2L_3[CLI][n]);
        C.ApplyAlgorithm(F2L_4[CLI][n]);
        if (!A_OLL[CLI].empty()) C.ApplyAlgorithm(A_OLL[CLI][n]);
        if (!A_PLL[CLI].empty()) C.ApplyAlgorithm(A_PLL[CLI][n]);
        if (!EOLL[CLI].empty()) C.ApplyAlgorithm(EOLL[CLI][n]);
        if (!A_ZBLL[CLI].empty()) C.ApplyAlgorithm(A_ZBLL[CLI][n]);
        if (!A_1LLL[CLI].empty()) C.ApplyAlgorithm(A_1LLL[CLI][n]);
        C.ApplyAlgorithm(AUF[CLI][n]);
        return C.IsSolved();
    }

    // Get the solve metric
    float CFOP::GetMetricSolve(const Fce CrossFace, const uint n) const
    {
        const int CLI = static_cast<int>(CrossFace); // Cross layer index

        if (!CheckSolveConsistency(Cube::FaceToLayer(CrossFace)) || Crosses[CLI].size() <= n) return 0.0f;

        Algorithm A;
        A += Crosses[CLI][n];
        A += F2L_1[CLI][n];
        A += F2L_2[CLI][n];
        A += F2L_3[CLI][n];
        A += F2L_4[CLI][n];
        if (!A_OLL[CLI].empty()) A += A_OLL[CLI][n];
        if (!A_PLL[CLI].empty()) A += A_PLL[CLI][n];
        if (!EOLL[CLI].empty()) A += EOLL[CLI][n];
        if (!A_ZBLL[CLI].empty()) A += A_ZBLL[CLI][n];
        if (!A_1LLL[CLI].empty()) A += A_1LLL[CLI][n];
        A += AUF[CLI][n];
        return A.GetMetric(Metric);
    }

    // Get the full solve with cancellations
    Algorithm CFOP::GetCancellations(const Lyr CrossLayer, const uint n) const
    {
        const Fce CrossFace = Cube::LayerToFace(CrossLayer);
        const int CLI = static_cast<int>(CrossFace); // Cross layer index

        if (!CheckSolveConsistency(CrossLayer) || Crosses[CLI].size() <= n)
        {
            std::string FReport = "No solve with cancellations for cross in layer ";
            FReport.push_back(Cube::GetLayerChar(CrossLayer));
            FReport += ", position " + std::to_string(n) + "\n";
            return FReport;
        }

        Algorithm A;
        A += Inspections[CLI][n];
        A += Crosses[CLI][n];
        A += F2L_1[CLI][n];
        A += F2L_2[CLI][n];
        A += F2L_3[CLI][n];
        A += F2L_4[CLI][n];
        if (!A_OLL[CLI].empty()) A += A_OLL[CLI][n];
        if (!A_PLL[CLI].empty()) A += A_PLL[CLI][n];
        if (!EOLL[CLI].empty()) A += EOLL[CLI][n];
        if (!A_ZBLL[CLI].empty()) A += A_ZBLL[CLI][n];
        if (!A_1LLL[CLI].empty()) A += A_1LLL[CLI][n];
        A += AUF[CLI][n];

        return A.GetCancellations();
    }

    // Get the best solve report
    std::string CFOP::GetBestReport(bool Cancellations) const
    {
        float M, min_M = 0.0f;
		uint Bestn;
        Lyr BestLayer;
		
        for (const auto CrossLayer : CrossLayers)
        {
            Fce face = Cube::LayerToFace(CrossLayer);
            int CLI = static_cast<int>(face);

            if (!CheckSolveConsistency(CrossLayer)) continue;

            for (uint n = 0u; n < Crosses[CLI].size(); n++)
            {
                if (!IsSolved(face, n)) continue;

                if (Cancellations) M = GetMetricCancellations(face, n);
                else M = GetMetricSolve(face, n);
				
                if (min_M == 0.0f || M < min_M)
                {
                    min_M = M;
                    BestLayer = CrossLayer;
                    Bestn = n;
                }
            }
        }
        if (min_M == 0.0f) return "No CFOP solves!\n";

        if (Cancellations)
        {
            Algorithm C = GetCancellations(BestLayer, Bestn);
            return GetReport(BestLayer, Bestn) + "\nCancellations (" + Algorithm::GetMetricValue(C.GetMetric(Metric)) + " " +
			       Algorithm::GetMetricString(Metric) + "): " + C.ToString() + "\n";
        }

        return GetReport(BestLayer, Bestn);
    }

    // Get full F2L text with parentheses
    std::string CFOP::GetTextF2L(const Fce CrossFace, const uint n) const
    {
        std::string F2LString, Aux = GetTextF2L_1(CrossFace, n);
        if (!Aux.empty()) F2LString += "(" + Aux + ")";
        Aux = GetTextF2L_2(CrossFace, n);
        if (!Aux.empty()) F2LString += "(" + Aux + ")";
        Aux = GetTextF2L_3(CrossFace, n);
        if (!Aux.empty()) F2LString += "(" + Aux + ")";
        Aux = GetTextF2L_4(CrossFace, n);
        if (!Aux.empty()) F2LString += "(" + Aux + ")";
        return F2LString;
    }
}
