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
#include "collection.h"

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

		DS.Clear();
		DS.SetScramble(Scramble);

        DS.AddToOptionalPieces(Pgr::CROSS_U);
        DS.AddToOptionalPieces(Pgr::CROSS_D);
        DS.AddToOptionalPieces(Pgr::CROSS_F);
        DS.AddToOptionalPieces(Pgr::CROSS_B);
        DS.AddToOptionalPieces(Pgr::CROSS_R);
        DS.AddToOptionalPieces(Pgr::CROSS_L);

        const SearchUnit URoot(SequenceTypes::DOUBLE);
        const SearchUnit U(SequenceTypes::SINGLE);
        
        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(URoot);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U);

        SearchLevel L_NoCheck(SearchCheck::NO_CHECK);
        L_NoCheck.Add(U);
        
        DS.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        DS.AddSearchLevel(L_NoCheck); // Level 2
        for (uint l = 3; l < DepthCrosses; l++) DS.AddSearchLevel(L_Check); // Levels 3 to CrossDepth

        DS.UpdateRootData();

        DS.Run(Cores);

        Cores = DS.GetUsedCores();

        EvaluateCrosses(DS.GetSolves(), MaxSolves);

        const std::chrono::duration<double> crosses_elapsed_seconds = std::chrono::system_clock::now() - time_crosses_start;
        TimeCrosses = crosses_elapsed_seconds.count();

        return !DS.GetSolves().empty();
	}

    // Search the best crosses solve algorithms from an algorithms vector
    void CFOP::EvaluateCrosses(const std::vector<Algorithm>& Solves, const uint MaxSolves)
    {
        // Cross solves evaluation
        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer));
            EvaluateCFOPF2LResult(Crosses[CLI], MaxSolves, Solves, CubeBase, CrossLayer, Plc::BEST);
        }

        // Cross to down layer
        // Inspections must have two movements maximum, and movements must be turns (no parentheses)
        for (int i = 0; i < 6; i++)
        {
            Inspections[i].clear();
            for (auto& c : Crosses[i])
            {
                Inspections[i].push_back(DefaultInspections[i]);
                if (DefaultInspections[i].GetSize() > 0u) c.TransformTurn(DefaultInspections[i].At(0));
                if (DefaultInspections[i].GetSize() > 1u) c.TransformTurn(DefaultInspections[i].At(1));
            }
        }
    }

    // Search the best F2L algorithms for the given cross
    void CFOP::SearchF2L()
    {
        const auto time_F2L_start = std::chrono::system_clock::now();

        bool Skip = false; // Skip the search (for multi threading)

        for (const auto CrossLayer : CrossLayers)
        {
            const int CLI = static_cast<int>(Cube::LayerToFace(CrossLayer)); // Cross layer index

            Algorithm F2L_Aux, // Auxiliary algorithm for F2L search
                      A1_F2L, A2_F2L, A3_F2L, A4_F2L; // Algorithms for F2L

            const SearchUnit U_Up(SequenceTypes::SINGLE, Sst::SINGLE_U);
            const SearchUnit U_Lat(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::LATERAL_UD);

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

                if (Skip) continue;

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

					DS.Clear();
                    DS.SetScramble(AlgSearch);

                    DS.AddSearchLevel(L_F2L_Check);
                    DS.AddSearchLevel(L_F2L_Check);
                    DS.AddSearchLevel(L_F2L_Check);
                    DS.AddSearchLevel(L_F2L_Check);
                    DS.AddSearchLevel(L_F2L_End);
                    DS.UpdateRootData();

                    DS.AddToMandatoryPieces(CROSS);

                    if (F2L1_IsSolved) DS.AddToMandatoryPieces(F2L1);
                    else DS.AddToOptionalPieces(F2L1);

                    if (F2L2_IsSolved) DS.AddToMandatoryPieces(F2L2);
                    else DS.AddToOptionalPieces(F2L2);

                    if (F2L3_IsSolved) DS.AddToMandatoryPieces(F2L3);
                    else DS.AddToOptionalPieces(F2L3);

                    if (F2L4_IsSolved) DS.AddToMandatoryPieces(F2L4);
                    else DS.AddToOptionalPieces(F2L4);

                    DS.Run(Cores);
                    Skip = DS.CheckSkipSearch();

                    F2L_Found = false;

                    std::vector<Algorithm> Solves;
                    EvaluateCFOPF2LResult(Solves, 1u, DS.GetSolves(), CubeF2L, CrossLayer, Plc::SHORT); // Only the shortest F2L solve

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
                } while (F2L_Found && !Skip);
                n++;
            }
        }

        const std::chrono::duration<double> F2L_elapsed_seconds = std::chrono::system_clock::now() - time_F2L_start;
        TimeF2L = F2L_elapsed_seconds.count();
    }

	// Orientation of last layer search
	void CFOP::SearchOLL(const Plc Pol)
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

                OrientateLL(A_OLL[CLI][n], CasesOLL[CLI][n], Algset_OLL, Pol, Metric, CubeF2L);
            }
        }

        const std::chrono::duration<double> OLL_elapsed_seconds = std::chrono::system_clock::now() - time_OLL_start;
        TimeOLL = OLL_elapsed_seconds.count();
	}

	// Permutation of last layer search + AUF
	void CFOP::SearchPLL(const Plc Pol)
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

                SolveLL(A_PLL[CLI][n], CasesPLL[CLI][n], AUFStep, Algset_PLL, Pol, Metric, CubeOLL);

                Algorithm Aux;
                Aux.Append(AUFStep);
                AUF[CLI].push_back(Aux);
            }
        }

        const std::chrono::duration<double> PLL_elapsed_seconds = std::chrono::system_clock::now() - time_PLL_start;
        TimePLL = PLL_elapsed_seconds.count();
	}

    // One-Look Last Layer search (1LLL)
    void CFOP::Search1LLL(const Plc Pol)
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

                SolveLL(A_1LLL[CLI][n], Cases1LLL[CLI][n], AUFStep, Algset_1LLL, Pol, Metric, CubeF2L);

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

        bool Skip = false; // Skip the search (for multi threading)

        const SearchUnit U_UR(SequenceTypes::SINGLE, Sst::SINGLE_UR);
        const SearchUnit U_OR1(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::PETRUS_OR_U);
        const SearchUnit U_OR2(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_R, Sst::PETRUS_OR_R);

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

                if (Skip) continue;

                Algorithm Alg = Scramble;
                Alg += Inspections[CLI][n];
                Alg += Crosses[CLI][n];
                Alg += F2L_1[CLI][n];
                Alg += F2L_2[CLI][n];
                Alg += F2L_3[CLI][n];
                Alg += F2L_4[CLI][n];

                Cube CubeF2L(Alg);

                if (!IsF2LBuilt(CubeF2L, CrossLayer) || CubeF2L.CheckOrientation(Pgr::ALL_EDGES)) continue;

				DS.Clear();
                DS.SetScramble(Alg);
				DS.SetShortPolicy();

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

                DS.AddToMandatoryPieces(FirstLayer);
                DS.AddToMandatoryPieces(MidLayer);
                DS.AddToMandatoryOrientations(LLCross);

                for (int l = 0; l < 5; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

                DS.UpdateRootData();

                DS.Run(Cores);
                Skip = DS.CheckSkipSearch();

                DS.EvaluateShortestResult(EOLL[CLI][n], true);
            }
        }

        const std::chrono::duration<double> EOLL_elapsed_seconds = std::chrono::system_clock::now() - time_EOLL_start;
        TimeEOLL = EOLL_elapsed_seconds.count();
    }

    // ZBLL last layer search + AUF
    void CFOP::SearchZBLL(const Plc Pol)
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

                SolveLL(A_ZBLL[CLI][n], CasesZBLL[CLI][n], AUFStep, Algset_ZBLL, Pol, Metric, CubeEOLL);

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

    // Check if the solves for the given cross layer are consistent (all needed algorithms are present) (spin selection)
    bool CFOP::CheckSolveConsistency(const Spn sp) const
    {
        switch (sp)
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return CheckSolveConsistency(Lyr::D);
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return CheckSolveConsistency(Lyr::U);
        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return CheckSolveConsistency(Lyr::B);
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return CheckSolveConsistency(Lyr::F);
        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return CheckSolveConsistency(Lyr::L);
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return CheckSolveConsistency(Lyr::R);
        default: return false;
        }
    }

    // Get a full solve report (for each cross layer)
    std::string CFOP::GetReport(const bool cancellations, const bool debug) const
    {
        std::string Report, ReportLine;
        // Report = "CFOP: Scramble [" + std::to_string(Scramble.GetNumSteps()) + "] " + GetTextScramble();
        // Report += "\n--------------------------------------------------------------------------------\n";

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

        Report += "Total time: " + std::to_string(GetTime()) + " s\n";
        Report += "Crosses time: " + std::to_string(GetTimeCrosses()) + " s\n";
        Report += "F2L time: " + std::to_string(GetTimeF2L()) + " s\n";
        Report += "Last layer time: " + std::to_string(GetTimeLL()) + " s\n";
        Report += "Threads: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : DeepSearch::GetSystemCores()) +
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

        std::string Report;
        // Report += "CFOP search with cross in layer ";
        // Report += Cube::GetLayerChar(CrossLayer);
        // Report += ":\n---------------------------------\n";
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

        if (!Inspections[CLI][n].Empty()) Report += GetTextInspection(CrossFace, n) + " // Inspection [" + C.GetSpinText() + "]\n";

        std::string CrossType;

        if (!F2L_4[CLI][n].Empty()) CrossType = "Cross";
        else if (!F2L_3[CLI][n].Empty()) CrossType += "XCross";
        else if (!F2L_2[CLI][n].Empty()) CrossType += "XXCross";
        else if (!F2L_1[CLI][n].Empty()) CrossType += "XXXCross";
        else CrossType += "XXXXCross";

        Report += GetTextCross(CrossFace, n) + " // " + CrossType + " (" + Algorithm::GetMetricValue(GetMetricCross(CrossFace, n)) + ")\n";

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
        if (!F2LString.empty()) Report += F2LString + " // F2L 1 (" + Algorithm::GetMetricValue(GetMetricF2L_1(CrossFace, n)) + ")\n";
        F2LString = GetTextF2L_2(CrossFace, n);
        if (!F2LString.empty()) Report += F2LString + " // F2L 2 (" + Algorithm::GetMetricValue(GetMetricF2L_2(CrossFace, n)) + ")\n";
        F2LString = GetTextF2L_3(CrossFace, n);
        if (!F2LString.empty()) Report += F2LString + " // F2L 3 (" + Algorithm::GetMetricValue(GetMetricF2L_3(CrossFace, n)) + ")\n";
        F2LString = GetTextF2L_4(CrossFace, n);
        if (!F2LString.empty()) Report += F2LString + " // F2L 4 (" + Algorithm::GetMetricValue(GetMetricF2L_4(CrossFace, n)) + ")\n";

        if (!A_OLL[CLI].empty() && !A_PLL[CLI].empty()) // OLL + PLL
        {
            // Show OLL solve
            C.ApplyAlgorithm(A_OLL[CLI][n]);
            
            if (!IsLastLayerOriented(C))
            {
                Report += "OLL not solved!\n";
                Report += "Search time: " + std::to_string(GetTime()) + " s\n";
                return Report;
            }

            if (!A_OLL[CLI][n].Empty()) Report += GetTextOLL(CrossFace, n) + " // OLL (" + Algorithm::GetMetricValue(GetMetricOLL(CrossFace, n)) + ")\n";

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
            if (!A_PLL[CLI][n].Empty()) Report += GetTextPLL(CrossFace, n) + " // PLL (" + Algorithm::GetMetricValue(GetMetricPLL(CrossFace, n)) + ")\n";

            // Show AUF solve
            if (!AUF[CLI][n].Empty()) Report += GetTextAUF(CrossFace, n) + " // AUF\n";
        }

        else if (!EOLL[CLI].empty() && !A_ZBLL[CLI].empty()) // LLEO + ZBLL
        {
            // Show edges orientation
            C.ApplyAlgorithm(EOLL[CLI][n]);

            if (!C.EO())
            {
                Report += "Last layer edges not oriented!\n";
                Report += "Search time: " + std::to_string(GetTime()) + " s\n";
                return Report;
            }

            if (!EOLL[CLI][n].Empty()) Report += GetTextEOLL(CrossFace, n) + " // EO (" + Algorithm::GetMetricValue(GetMetricEOLL(CrossFace, n)) + ")\n";

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
            if (!A_ZBLL[CLI][n].Empty()) Report += GetTextZBLL(CrossFace, n) + " // ZBLL (" + Algorithm::GetMetricValue(GetMetricZBLL(CrossFace, n)) + ")\n";

            // Show AUF solve
            if (!AUF[CLI][n].Empty()) Report += GetTextAUF(CrossFace, n) + " // AUF\n";
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

            if (!A_1LLL[CLI][n].Empty()) Report += GetText1LLL(CrossFace, n) + " // 1LLL (" + Algorithm::GetMetricValue(GetMetric1LLL(CrossFace, n)) + ")\n";

            // Show AUF solve
            if (!AUF[CLI][n].Empty()) Report += GetTextAUF(CrossFace, n) + " // AUF\n";
        }

        else
        {
            Report += "Last layer not solved!\n";
            return Report;
        }

        // Show summary
        Report += "\nMetric: " + Algorithm::GetMetricValue(GetMetricSolve(CrossFace, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
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

    // Get a solve report for a cross layer (spin selection)
    std::string CFOP::GetReport(const Spn sp, const uint n) const
    {
        switch (sp)
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return GetReport(Lyr::D, n);
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return GetReport(Lyr::U, n);
        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return GetReport(Lyr::B, n);
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return GetReport(Lyr::F, n);
        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return GetReport(Lyr::L, n);
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return GetReport(Lyr::R, n);
        default: return "No report";
        }
    }

    // Check if in the given cross face the solve is OK
    bool CFOP::IsSolved(const Fce CrossFace, const uint n) const
    {
        Cube C = CubeBase;
        C.ApplyAlgorithm(GetFullSolve(CrossFace, n));
        return C.IsSolved();
    }

    // Check if in the given cross face the solve is OK (spin selection)
    bool CFOP::IsSolved(const Spn sp, const uint n) const
    {
        switch (sp)
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return IsSolved(Fce::D, n);
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return IsSolved(Fce::U, n);
        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return IsSolved(Fce::B, n);
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return IsSolved(Fce::F, n);
        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return IsSolved(Fce::L, n);
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return IsSolved(Fce::R, n);
        default: return false;
        }
    }

    // Get the full solve
    Algorithm CFOP::GetFullSolve(const Spn sp, const uint n) const
    {
        switch (sp)
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return GetFullSolve(Fce::D, n);
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return GetFullSolve(Fce::U, n);
        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return GetFullSolve(Fce::B, n);
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return GetFullSolve(Fce::F, n);
        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return GetFullSolve(Fce::L, n);
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return GetFullSolve(Fce::R, n);
        default: return "";
        }
    }

    // Get the full solve
    Algorithm CFOP::GetFullSolve(const Fce CrossFace, const uint n) const
    {
        const int CLI = static_cast<int>(CrossFace); // Cross layer index

        if (!CheckSolveConsistency(Cube::FaceToLayer(CrossFace)) || n >= Inspections[CLI].size()) return "";

        Algorithm A = Inspections[CLI][n];
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

        return A;
    }

    // Get the solve metric
    float CFOP::GetMetricSolve(const Fce CrossFace, const uint n) const
    {
        return GetFullSolve(CrossFace, n).GetMetric(Metric);
    }

    // Get the solve metric (spin selection)
    float CFOP::GetMetricSolve(const Spn sp, const uint n) const
    {
        switch (sp)
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return GetMetricSolve(Fce::D, n);
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return GetMetricSolve(Fce::U, n);
        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return GetMetricSolve(Fce::B, n);
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return GetMetricSolve(Fce::F, n);
        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return GetMetricSolve(Fce::L, n);
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return GetMetricSolve(Fce::R, n);
        default: return 0.0f;
        }
    }

    // Get the full solve with cancellations
    Algorithm CFOP::GetCancellations(const Lyr CrossLayer, const uint n) const
    {
        return GetFullSolve(Cube::LayerToFace(CrossLayer), n).GetCancellations();
    }

    // Get the full solve with cancellations (spin selection)
    Algorithm CFOP::GetCancellations(const Spn sp, const uint n) const
    {
        switch (sp)
        {
        case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return GetCancellations(Lyr::D, n);
        case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return GetCancellations(Lyr::U, n);
        case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return GetCancellations(Lyr::B, n);
        case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return GetCancellations(Lyr::F, n);
        case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return GetCancellations(Lyr::L, n);
        case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return GetCancellations(Lyr::R, n);
        default: return Algorithm("");
        }
    }

    // Get the best solve report
    std::string CFOP::GetBestReport(bool Cancellations) const
    {
        float M, min_M = 0.0f;
		uint Bestn = 0u;
        Lyr BestLayer = Lyr::NONE;
		
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
