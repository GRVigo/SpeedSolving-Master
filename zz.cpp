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

#include "zz.h"
#include "collection.h"

namespace grcube3
{
	// Reset the search results
    void ZZ::Reset()
	{
        for (int i = 0; i < 24; i++)
        {
            Inspections[i].clear();
            EOX[i].clear();
            F2L_1[i].clear();
            F2L_2[i].clear();
            F2L_3[i].clear();
            F2L_4[i].clear();
            AlgZBLL[i].clear();
            AlgOCLL[i].clear();
            AlgPLL[i].clear();
            AlgCOLL[i].clear();
            AlgEPLL[i].clear();
			
            CasesZBLL[i].clear();
            CasesOCLL[i].clear();
            CasesPLL[i].clear();
            CasesCOLL[i].clear();
            CasesEPLL[i].clear();
        }
		
        MaxDepthEOX = 0u;

        TimeEOX = TimeF2L = TimeZBLL = TimeOCLL = TimePLL = TimeCOLL = TimeEPLL = 0.0;
		
		Metric = Metrics::Movements; // Default metric
	}

    // Search the best EOX solve with the given search deep
    // Return false if no EOX found
    bool ZZ::SearchEOX(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_EOX_start = std::chrono::system_clock::now();

        MaxDepthEOX = (MaxDepth <= 4u ? 4u : MaxDepth);

        // Deep search for EOLine
		DS.Clear();
        DS.SetScramble(Scramble);
		
        DS.AddToOptionalPieces(Pgr::EOLINE_UF);
        DS.AddToOptionalPieces(Pgr::EOLINE_UR);
        DS.AddToOptionalPieces(Pgr::EOLINE_DF);
        DS.AddToOptionalPieces(Pgr::EOLINE_DR);

        DS.AddToOptionalPieces(Pgr::EOLINE_FU);
        DS.AddToOptionalPieces(Pgr::EOLINE_FR);
        DS.AddToOptionalPieces(Pgr::EOLINE_BU);
        DS.AddToOptionalPieces(Pgr::EOLINE_BR);
        
        DS.AddToOptionalPieces(Pgr::EOLINE_RU);
        DS.AddToOptionalPieces(Pgr::EOLINE_RF);
        DS.AddToOptionalPieces(Pgr::EOLINE_LU);
        DS.AddToOptionalPieces(Pgr::EOLINE_LF);
        
        DS.AddToMandatoryOrientations(Pgr::ALL_EDGES);

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the EO Line not will be solved in a single movement)
        const SearchUnit URoot(SequenceTypes::DOUBLE);
        const SearchUnit U(SequenceTypes::SINGLE);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(URoot);
		
        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U);

        DS.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        for (uint l = 2u; l < MaxDepthEOX; l++) DS.AddSearchLevel(L_Check);

        DS.UpdateRootData();
        // DS.SetMinDeep(DS.GetMaxDeep() - 2u);

        DS.Run(Cores);

        Cores = DS.GetUsedCores(); // Update to the real number of cores used

        EvaluateEOX(DS.GetSolves(), MaxSolves);

        const std::chrono::duration<double> EOX_elapsed_seconds = std::chrono::system_clock::now() - time_EOX_start;
        TimeEOX = EOX_elapsed_seconds.count();

        for (auto spin : SearchSpins) if (!EOX[static_cast<int>(spin)].empty()) return true;
        return false;
	}
	
    // Search the best EOLine solves from an algorithms vector
    void ZZ::EvaluateEOX(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
        // Best EOX solves for each spin
        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);
            Stp T1, T2;
            Algorithm Insp;

            EvaluateZZEOXResult(EOX[sp], MaxSolves, Solves, CubeBase, spin, Plc::BEST);

            Inspections[sp].clear();

            if (!EOX[sp].empty())
            {
                Insp.Clear();
                Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
                if (T1 != Stp::NONE) Insp.Append(T1);
                if (T2 != Stp::NONE) Insp.Append(T2);

                for (auto& eox : EOX[sp])
                {
                    if (T1 != Stp::NONE) eox.TransformTurn(T1);
                    if (T2 != Stp::NONE) eox.TransformTurn(T2);
                    Inspections[sp].push_back(Insp);
                }
            }
        }
	}
	
    // Complete the two first layers (F2L)
    void ZZ::SearchF2L()
    {
        const auto time_F2L_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                F2L_1[sp].push_back(Algorithm(""));
                F2L_2[sp].push_back(Algorithm(""));
                F2L_3[sp].push_back(Algorithm(""));
                F2L_4[sp].push_back(Algorithm(""));
				
				if (Skip) continue;

                Cube C = CubeBase;
                C.ApplyAlgorithm(Inspections[sp][n]);
                C.ApplyAlgorithm(EOX[sp][n]);

                if (IsEOCrossBuilt(C, spin)) Skip = SearchF2L_EOCross(spin, n);
                else if (IsEOLineBuilt(C, spin)) Skip = SearchF2L_EOLine(spin, n);
            }
        }

        const std::chrono::duration<double> F2L_elapsed_seconds = std::chrono::system_clock::now() - time_F2L_start;
        TimeF2L = F2L_elapsed_seconds.count();
    }

	// Complete the two first layers (F2L) after EO Line
    bool ZZ::SearchF2L_EOLine(const Spn spin, const uint n)
    {
        const int sp = static_cast<int>(spin);
		
		bool Skip = false; // Skip the search (for multi threading)

        bool F2L_Found = false; // F2L found flag

        Algorithm F2L_Aux, // Auxiliary algorithm for F2L search
                  A1_F2L, A2_F2L, A3_F2L, A4_F2L; // Algorithms for F2L

        const SearchUnit U_Up(SequenceTypes::SINGLE, Sst::SINGLE_U);
        const SearchUnit U_Lat(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::ZZ_LATERAL2);

        SearchLevel L_F2L_Check(SearchCheck::CHECK);
        L_F2L_Check.Add(U_Up);
        L_F2L_Check.Add(U_Lat);
            
        SearchLevel L_F2L_End(SearchCheck::CHECK);
        L_F2L_End.Add(U_Lat);

        Pgr LINE, SQUARE_L1, SQUARE_L2, SQUARE_R1, SQUARE_R2;
        switch (spin)
        {
        case Spn::UF: LINE = Pgr::EOLINE_UF;
            SQUARE_L1 = Pgr::UF_B1S1; SQUARE_L2 = Pgr::UF_B1S2; SQUARE_R1 = Pgr::UF_B2S1; SQUARE_R2 = Pgr::UF_B2S2;
            break;
        case Spn::UB: LINE = Pgr::EOLINE_UB;
            SQUARE_L1 = Pgr::UB_B1S1; SQUARE_L2 = Pgr::UB_B1S2; SQUARE_R1 = Pgr::UB_B2S1; SQUARE_R2 = Pgr::UB_B2S2;
            break;
        case Spn::UR: LINE = Pgr::EOLINE_UR;
            SQUARE_L1 = Pgr::UR_B1S1; SQUARE_L2 = Pgr::UR_B1S2; SQUARE_R1 = Pgr::UR_B2S1; SQUARE_R2 = Pgr::UR_B2S2;
            break;
        case Spn::UL: LINE = Pgr::EOLINE_UL;
            SQUARE_L1 = Pgr::UL_B1S1; SQUARE_L2 = Pgr::UL_B1S2; SQUARE_R1 = Pgr::UL_B2S1; SQUARE_R2 = Pgr::UL_B2S2;
            break;

        case Spn::DF: LINE = Pgr::EOLINE_DF;
            SQUARE_L1 = Pgr::DF_B1S1; SQUARE_L2 = Pgr::DF_B1S2; SQUARE_R1 = Pgr::DF_B2S1; SQUARE_R2 = Pgr::DF_B2S2;
            break;
        case Spn::DB: LINE = Pgr::EOLINE_DB;
            SQUARE_L1 = Pgr::DB_B1S1; SQUARE_L2 = Pgr::DB_B1S2; SQUARE_R1 = Pgr::DB_B2S1; SQUARE_R2 = Pgr::DB_B2S2;
            break;
        case Spn::DR: LINE = Pgr::EOLINE_DR;
            SQUARE_L1 = Pgr::DR_B1S1; SQUARE_L2 = Pgr::DR_B1S2; SQUARE_R1 = Pgr::DR_B2S1; SQUARE_R2 = Pgr::DR_B2S2;
            break;
        case Spn::DL: LINE = Pgr::EOLINE_DL;
            SQUARE_L1 = Pgr::DL_B1S1; SQUARE_L2 = Pgr::DL_B1S2; SQUARE_R1 = Pgr::DL_B2S1; SQUARE_R2 = Pgr::DL_B2S2;
            break;

        case Spn::FU: LINE = Pgr::EOLINE_FU;
            SQUARE_L1 = Pgr::FU_B1S1; SQUARE_L2 = Pgr::FU_B1S2; SQUARE_R1 = Pgr::FU_B2S1; SQUARE_R2 = Pgr::FU_B2S2;
            break;
        case Spn::FD: LINE = Pgr::EOLINE_FD;
            SQUARE_L1 = Pgr::FD_B1S1; SQUARE_L2 = Pgr::FD_B1S2; SQUARE_R1 = Pgr::FD_B2S1; SQUARE_R2 = Pgr::FD_B2S2;
            break;
        case Spn::FR: LINE = Pgr::EOLINE_FR;
            SQUARE_L1 = Pgr::FR_B1S1; SQUARE_L2 = Pgr::FR_B1S2; SQUARE_R1 = Pgr::FR_B2S1; SQUARE_R2 = Pgr::FR_B2S2;
            break;
        case Spn::FL: LINE = Pgr::EOLINE_FL;
            SQUARE_L1 = Pgr::FL_B1S1; SQUARE_L2 = Pgr::FL_B1S2; SQUARE_R1 = Pgr::FL_B2S1; SQUARE_R2 = Pgr::FL_B2S2;
            break;
				
		case Spn::BU: LINE = Pgr::EOLINE_BU;
            SQUARE_L1 = Pgr::BU_B1S1; SQUARE_L2 = Pgr::BU_B1S2; SQUARE_R1 = Pgr::BU_B2S1; SQUARE_R2 = Pgr::BU_B2S2;
            break;
        case Spn::BD: LINE = Pgr::EOLINE_BD;
            SQUARE_L1 = Pgr::BD_B1S1; SQUARE_L2 = Pgr::BD_B1S2; SQUARE_R1 = Pgr::BD_B2S1; SQUARE_R2 = Pgr::BD_B2S2;
            break;
        case Spn::BR: LINE = Pgr::EOLINE_BR;
            SQUARE_L1 = Pgr::BR_B1S1; SQUARE_L2 = Pgr::BR_B1S2; SQUARE_R1 = Pgr::BR_B2S1; SQUARE_R2 = Pgr::BR_B2S2;
            break;
        case Spn::BL: LINE = Pgr::EOLINE_BL;
            SQUARE_L1 = Pgr::BL_B1S1; SQUARE_L2 = Pgr::BL_B1S2; SQUARE_R1 = Pgr::BL_B2S1; SQUARE_R2 = Pgr::BL_B2S2;
            break;
				
		case Spn::RU: LINE = Pgr::EOLINE_RU;
            SQUARE_L1 = Pgr::RU_B1S1; SQUARE_L2 = Pgr::RU_B1S2; SQUARE_R1 = Pgr::RU_B2S1; SQUARE_R2 = Pgr::RU_B2S2;
            break;
        case Spn::RD: LINE = Pgr::EOLINE_RD;
            SQUARE_L1 = Pgr::RD_B1S1; SQUARE_L2 = Pgr::RD_B1S2; SQUARE_R1 = Pgr::RD_B2S1; SQUARE_R2 = Pgr::RD_B2S2;
            break;
        case Spn::RF: LINE = Pgr::EOLINE_RF;
            SQUARE_L1 = Pgr::RF_B1S1; SQUARE_L2 = Pgr::RF_B1S2; SQUARE_R1 = Pgr::RF_B2S1; SQUARE_R2 = Pgr::RF_B2S2;
            break;
        case Spn::RB: LINE = Pgr::EOLINE_RB;
            SQUARE_L1 = Pgr::RB_B1S1; SQUARE_L2 = Pgr::RB_B1S2; SQUARE_R1 = Pgr::RB_B2S1; SQUARE_R2 = Pgr::RB_B2S2;
            break;
				
		case Spn::LU: LINE = Pgr::EOLINE_LU;
            SQUARE_L1 = Pgr::LU_B1S1; SQUARE_L2 = Pgr::LU_B1S2; SQUARE_R1 = Pgr::LU_B2S1; SQUARE_R2 = Pgr::LU_B2S2;
            break;
        case Spn::LD: LINE = Pgr::EOLINE_LD;
            SQUARE_L1 = Pgr::LD_B1S1; SQUARE_L2 = Pgr::LD_B1S2; SQUARE_R1 = Pgr::LD_B2S1; SQUARE_R2 = Pgr::LD_B2S2;
            break;
        case Spn::LF: LINE = Pgr::EOLINE_LF;
            SQUARE_L1 = Pgr::LF_B1S1; SQUARE_L2 = Pgr::LF_B1S2; SQUARE_R1 = Pgr::LF_B2S1; SQUARE_R2 = Pgr::LF_B2S2;
            break;
        case Spn::LB: LINE = Pgr::EOLINE_LB;
            SQUARE_L1 = Pgr::LB_B1S1; SQUARE_L2 = Pgr::LB_B1S2; SQUARE_R1 = Pgr::LB_B2S1; SQUARE_R2 = Pgr::LB_B2S2;
            break;
				
        default: return false;
        }
			
        const Algorithm AlgBase = Scramble + Inspections[sp][n] + EOX[sp][n];
        const Cube C(AlgBase);

        int control = 0;
        do
        {
            if (control++ > 4) break; // To avoid deadlock

            F2L_Aux = AlgBase;
            F2L_Aux += F2L_1[sp][n];
            F2L_Aux += F2L_2[sp][n];
            F2L_Aux += F2L_3[sp][n];
            F2L_Aux += F2L_4[sp][n];

            Cube CubeF2L(F2L_Aux);

            const bool SQUARE_L1_IsSolved = CubeF2L.IsSolved(SQUARE_L1),
					   SQUARE_L2_IsSolved = CubeF2L.IsSolved(SQUARE_L2),
					   SQUARE_R1_IsSolved = CubeF2L.IsSolved(SQUARE_R1),
					   SQUARE_R2_IsSolved = CubeF2L.IsSolved(SQUARE_R2);

            if (SQUARE_L1_IsSolved && SQUARE_L2_IsSolved && SQUARE_R1_IsSolved && SQUARE_R2_IsSolved) break;

			DS.Clear();
            DS.SetScramble(F2L_Aux);

            DS.AddSearchLevel(L_F2L_Check);
            DS.AddSearchLevel(L_F2L_Check);
            DS.AddSearchLevel(L_F2L_Check);
            DS.AddSearchLevel(L_F2L_Check);
            DS.AddSearchLevel(L_F2L_End);
            DS.UpdateRootData();

            DS.AddToMandatoryPieces(LINE);

			if (SQUARE_L1_IsSolved) DS.AddToMandatoryPieces(SQUARE_L1);
            else DS.AddToOptionalPieces(SQUARE_L1);

            if (SQUARE_L2_IsSolved) DS.AddToMandatoryPieces(SQUARE_L2);
            else DS.AddToOptionalPieces(SQUARE_L2);

            if (SQUARE_R1_IsSolved) DS.AddToMandatoryPieces(SQUARE_R1);
            else DS.AddToOptionalPieces(SQUARE_R1);

            if (SQUARE_R2_IsSolved) DS.AddToMandatoryPieces(SQUARE_R2);
            else DS.AddToOptionalPieces(SQUARE_R2);

            DS.Run(Cores);
			Skip = DS.CheckSkipSearch();

            F2L_Found = false;

            std::vector<Algorithm> Solves;
            EvaluateZZF2LResult(Solves, 1u, DS.GetSolves(), CubeF2L, spin, Plc::SHORT);

            if (!Solves.empty())
            {
                F2L_Aux = Solves[0];

                CubeF2L.ApplyAlgorithm(F2L_Aux);

                if ((!SQUARE_L1_IsSolved && CubeF2L.IsSolved(SQUARE_L1)) || (!SQUARE_L2_IsSolved && CubeF2L.IsSolved(SQUARE_L2)) ||
                    (!SQUARE_R1_IsSolved && CubeF2L.IsSolved(SQUARE_R1)) || (!SQUARE_R2_IsSolved && CubeF2L.IsSolved(SQUARE_R2)))
                {
                    F2L_Found = true;
                    if (F2L_1[sp][n].GetSize() == 0u) F2L_1[sp][n] = F2L_Aux;
                    else if (F2L_2[sp][n].GetSize() == 0u) F2L_2[sp][n] = F2L_Aux;
                    else if (F2L_3[sp][n].GetSize() == 0u) F2L_3[sp][n] = F2L_Aux;
                    else if (F2L_4[sp][n].GetSize() == 0u) F2L_4[sp][n] = F2L_Aux;
                    else F2L_Found = false;
                }
            }
        } while (F2L_Found && !Skip);
		
		return Skip;
    }

	// Complete the two first layers (F2L) after EO Cross (similar to CFOP F2L)
    bool ZZ::SearchF2L_EOCross(const Spn spin, const uint n)
    {
        const int sp = static_cast<int>(spin);
		
		bool Skip = false; // Skip the search (for multi threading)

        const Lyr CrossLayer = Cube::GetDownSliceLayer(spin);

        bool F2L_Found = false; // F2L found flag
			
        const Algorithm AlgBase = Scramble + Inspections[sp][n] + EOX[sp][n];
        const Cube C(AlgBase);
			
        if (!IsEOCrossBuilt(C, spin)) return false; // It's necessary to have the cross built

        Algorithm F2L_Aux, // Auxiliary algorithm for F2L search
                  A1_F2L, A2_F2L, A3_F2L, A4_F2L; // Algorithms for F2L

        const SearchUnit U_Up(SequenceTypes::SINGLE, Sst::SINGLE_U);
        const SearchUnit U_Lat(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::ZZ_LATERAL);

        SearchLevel L_F2L_Check(SearchCheck::CHECK);
        L_F2L_Check.Add(U_Up);
        L_F2L_Check.Add(U_Lat);
            
        SearchLevel L_F2L_End(SearchCheck::CHECK);
        L_F2L_End.Add(U_Lat);

        Pgr F2L1, F2L2, F2L3, F2L4, CROSS;

        switch (CrossLayer)
        {
        case Lyr::U: CROSS = Pgr::CROSS_U; F2L1 = Pgr::F2L_U_UFR; F2L2 = Pgr::F2L_U_UFL; F2L3 = Pgr::F2L_U_UBR; F2L4 = Pgr::F2L_U_UBL; break;
        case Lyr::D: CROSS = Pgr::CROSS_D; F2L1 = Pgr::F2L_D_DFR; F2L2 = Pgr::F2L_D_DFL; F2L3 = Pgr::F2L_D_DBR; F2L4 = Pgr::F2L_D_DBL; break;
        case Lyr::F: CROSS = Pgr::CROSS_F; F2L1 = Pgr::F2L_F_UFR; F2L2 = Pgr::F2L_F_UFL; F2L3 = Pgr::F2L_F_DFR; F2L4 = Pgr::F2L_F_DFL; break;
        case Lyr::B: CROSS = Pgr::CROSS_B; F2L1 = Pgr::F2L_B_UBR; F2L2 = Pgr::F2L_B_UBL; F2L3 = Pgr::F2L_B_DBR; F2L4 = Pgr::F2L_B_DBL; break;
        case Lyr::R: CROSS = Pgr::CROSS_R; F2L1 = Pgr::F2L_R_UFR; F2L2 = Pgr::F2L_R_UBR; F2L3 = Pgr::F2L_R_DFR; F2L4 = Pgr::F2L_R_DBR; break;
        case Lyr::L: CROSS = Pgr::CROSS_L; F2L1 = Pgr::F2L_L_UFL; F2L2 = Pgr::F2L_L_UBL; F2L3 = Pgr::F2L_L_DFL; F2L4 = Pgr::F2L_L_DBL; break;
        default: return false;
        }

        int control = 0; 
        do
        {
            if (control++ > 4) break; // To avoid a deadlock

            F2L_Aux = AlgBase;
            F2L_Aux += F2L_1[sp][n];
            F2L_Aux += F2L_2[sp][n];
            F2L_Aux += F2L_3[sp][n];
            F2L_Aux += F2L_4[sp][n];

            Cube CubeF2L(F2L_Aux);

            const bool F2L_1_IsSolved = CubeF2L.IsSolved(F2L1),
                       F2L_2_IsSolved = CubeF2L.IsSolved(F2L2),
                       F2L_3_IsSolved = CubeF2L.IsSolved(F2L3),
                       F2L_4_IsSolved = CubeF2L.IsSolved(F2L4);

            if (F2L_1_IsSolved && F2L_2_IsSolved && F2L_3_IsSolved && F2L_4_IsSolved) break;

			DS.Clear();
            DS.SetScramble(F2L_Aux);

            DS.AddSearchLevel(L_F2L_Check);
            DS.AddSearchLevel(L_F2L_Check);
            DS.AddSearchLevel(L_F2L_Check);
            DS.AddSearchLevel(L_F2L_Check);
            DS.AddSearchLevel(L_F2L_End);
            DS.UpdateRootData();

            DS.AddToMandatoryPieces(CROSS);

            if (F2L_1_IsSolved) DS.AddToMandatoryPieces(F2L1);
            else DS.AddToOptionalPieces(F2L1);

            if (F2L_2_IsSolved) DS.AddToMandatoryPieces(F2L2);
            else DS.AddToOptionalPieces(F2L2);

            if (F2L_3_IsSolved) DS.AddToMandatoryPieces(F2L3);
            else DS.AddToOptionalPieces(F2L3);

            if (F2L_4_IsSolved) DS.AddToMandatoryPieces(F2L4);
            else DS.AddToOptionalPieces(F2L4);

            DS.Run(Cores);
			Skip = DS.CheckSkipSearch();
			
            Cores = DS.GetUsedCores(); // Update to the real number of cores used

            std::vector<Algorithm> Solves;
            EvaluateCFOPF2LResult(Solves, 1u, DS.GetSolves(), CubeF2L, CrossLayer, Plc::SHORT);

            if (!Solves.empty())
            {
                F2L_Aux = Solves[0];

                CubeF2L.ApplyAlgorithm(F2L_Aux);

                if ((!F2L_1_IsSolved && CubeF2L.IsSolved(F2L1)) || (!F2L_2_IsSolved && CubeF2L.IsSolved(F2L2)) ||
                    (!F2L_3_IsSolved && CubeF2L.IsSolved(F2L3)) || (!F2L_4_IsSolved && CubeF2L.IsSolved(F2L4)))
                {
                    F2L_Found = true;
                    if (F2L_1[sp][n].GetSize() == 0u) F2L_1[sp][n] = F2L_Aux;
                    else if (F2L_2[sp][n].GetSize() == 0u) F2L_2[sp][n] = F2L_Aux;
                    else if (F2L_3[sp][n].GetSize() == 0u) F2L_3[sp][n] = F2L_Aux;
                    else if (F2L_4[sp][n].GetSize() == 0u) F2L_4[sp][n] = F2L_Aux;
                    else F2L_Found = false;
                }
            }
        } while (F2L_Found && !Skip);
		
		return Skip;
    }

    // ZBLL search
    void ZZ::SearchZBLL(const Plc Pol)
	{
        const auto time_ZBLL_start = std::chrono::system_clock::now();

        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);

            AlgZBLL[sp].clear();
            CasesZBLL[sp].clear();

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                AlgZBLL[sp].push_back(Algorithm(""));
                CasesZBLL[sp].push_back("");

                Algorithm Alg = Inspections[sp][n];
                Alg += EOX[sp][n];
                Alg += F2L_1[sp][n];
                Alg += F2L_2[sp][n];
                Alg += F2L_3[sp][n];
                Alg += F2L_4[sp][n];

                Cube CubeZBLL = CubeBase;
                CubeZBLL.ApplyAlgorithm(Alg);

                Stp AUFStep;

                SolveLL(AlgZBLL[sp][n], CasesZBLL[sp][n], AUFStep, Algset_ZBLL, Pol, Metric, CubeZBLL);

                AlgZBLL[sp][n].Append(AUFStep);
            }
        }
		
        const std::chrono::duration<double> ZBLL_elapsed_seconds = std::chrono::system_clock::now() - time_ZBLL_start;
        TimeZBLL = ZBLL_elapsed_seconds.count();
	}

    // OCLL search
    void ZZ::SearchOCLL(const Plc Pol)
    {
        const auto time_OCLL_start = std::chrono::system_clock::now();

        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);

            AlgOCLL[sp].clear();
            CasesOCLL[sp].clear();

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                AlgOCLL[sp].push_back(Algorithm(""));
                CasesOCLL[sp].push_back("");

                Algorithm Alg = Inspections[sp][n];
                Alg += EOX[sp][n];
                Alg += F2L_1[sp][n];
                Alg += F2L_2[sp][n];
                Alg += F2L_3[sp][n];
                Alg += F2L_4[sp][n];

                Cube CubeF2L = CubeBase;
                CubeF2L.ApplyAlgorithm(Alg);

                OrientateLL(AlgOCLL[sp][n], CasesOCLL[sp][n], Algset_OCLL, Pol, Metric, CubeF2L);
            }
        }

        const std::chrono::duration<double> OCLL_elapsed_seconds = std::chrono::system_clock::now() - time_OCLL_start;
        TimeOCLL = OCLL_elapsed_seconds.count();
    }

    // PLL search
    void ZZ::SearchPLL(const Plc Pol)
    {
        const auto time_PLL_start = std::chrono::system_clock::now();
		
        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);

            AlgPLL[sp].clear();
            CasesPLL[sp].clear();

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                AlgPLL[sp].push_back(Algorithm(""));
                CasesPLL[sp].push_back("");

                Algorithm Alg = Inspections[sp][n];
                Alg += EOX[sp][n];
                Alg += F2L_1[sp][n];
                Alg += F2L_2[sp][n];
                Alg += F2L_3[sp][n];
                Alg += F2L_4[sp][n];
                Alg += AlgOCLL[sp][n];

                Cube CubeOCLL = CubeBase;
                CubeOCLL.ApplyAlgorithm(Alg);

                Stp AUFStep;

                SolveLL(AlgPLL[sp][n], CasesPLL[sp][n], AUFStep, Algset_PLL, Pol, Metric, CubeOCLL);

                AlgPLL[sp][n].Append(AUFStep);
            }
        }

        const std::chrono::duration<double> PLL_elapsed_seconds = std::chrono::system_clock::now() - time_PLL_start;
        TimePLL = PLL_elapsed_seconds.count();
    }

    // COLL search
    void ZZ::SearchCOLL(const Plc Pol)
    {
        const auto time_COLL_start = std::chrono::system_clock::now();

        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);

            AlgCOLL[sp].clear();
            CasesCOLL[sp].clear();

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                AlgCOLL[sp].push_back(Algorithm(""));
                CasesCOLL[sp].push_back("");

                Algorithm Alg = Inspections[sp][n];
                Alg += EOX[sp][n];
                Alg += F2L_1[sp][n];
                Alg += F2L_2[sp][n];
                Alg += F2L_3[sp][n];
                Alg += F2L_4[sp][n];

                Cube CubeF2L = CubeBase;
                CubeF2L.ApplyAlgorithm(Alg);

                Stp LastStep;

                CornersLL(AlgCOLL[sp][n], CasesCOLL[sp][n], LastStep, Algset_COLL, Pol, Metric, CubeF2L);

                AlgCOLL[sp][n].Append(LastStep);
            }
        }

        const std::chrono::duration<double> COLL_elapsed_seconds = std::chrono::system_clock::now() - time_COLL_start;
        TimeCOLL = COLL_elapsed_seconds.count();
    }

    // EPLL search
    void ZZ::SearchEPLL(const Plc Pol)
    {
        const auto time_EPLL_start = std::chrono::system_clock::now();

        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);

            AlgEPLL[sp].clear();
            CasesEPLL[sp].clear();

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                AlgEPLL[sp].push_back(Algorithm(""));
                CasesEPLL[sp].push_back("");

                Algorithm Alg = Inspections[sp][n];
                Alg += EOX[sp][n];
                Alg += F2L_1[sp][n];
                Alg += F2L_2[sp][n];
                Alg += F2L_3[sp][n];
                Alg += F2L_4[sp][n];
                Alg += AlgCOLL[sp][n];

                Cube CubeCOLL = CubeBase;
                CubeCOLL.ApplyAlgorithm(Alg);

                Stp AUFStep;

                SolveLL(AlgEPLL[sp][n], CasesEPLL[sp][n], AUFStep, Algset_EPLL, Pol, Metric, CubeCOLL);

                AlgEPLL[sp][n].Append(AUFStep);
            }
        }

        const std::chrono::duration<double> EPLL_elapsed_seconds = std::chrono::system_clock::now() - time_EPLL_start;
        TimeEPLL = EPLL_elapsed_seconds.count();
    }

    // Set regrips
    void ZZ::SetRegrips()
    {
        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);

            if (!CheckSolveConsistency(spin)) continue;

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                EOX[sp][n] = EOX[sp][n].GetRegrip();
                F2L_1[sp][n] = F2L_1[sp][n].GetRegrip();
                F2L_2[sp][n] = F2L_2[sp][n].GetRegrip();
                F2L_3[sp][n] = F2L_3[sp][n].GetRegrip();
                F2L_4[sp][n] = F2L_4[sp][n].GetRegrip();
                
                if (Algorithm::IsTurn(EOX[sp][n].First()))
                {
                    Stp FS = EOX[sp][n].First();
                    Inspections[sp][n].AppendShrink(FS);
                    if (Inspections[sp][n].GetSize() == 3u)
                    {
                        Algorithm A = Inspections[sp][n].GetCancellations();
                        Inspections[sp][n] = A;
                    }
                    EOX[sp][n].EraseFirst();
                }

                if (Algorithm::IsTurn(EOX[sp][n].Last()))
                {
                    F2L_1[sp][n].Insert(0u, EOX[sp][n].Last());
                    while (F2L_1[sp][n].Shrink());
                    EOX[sp][n].EraseLast();
                }

                if (Algorithm::IsTurn(F2L_1[sp][n].Last()))
                {
                    if (F2L_2[sp][n].Empty())
                    {
                        if (!AlgZBLL[sp].empty())
                        {
                            AlgZBLL[sp][n].Insert(0u, F2L_1[sp][n].Last());
                            while (AlgZBLL[sp][n].Shrink());
                            F2L_1[sp][n].EraseLast();
                        }
                        else if (!AlgOCLL[sp].empty())
                        {
                            AlgOCLL[sp][n].Insert(0u, F2L_1[sp][n].Last());
                            while (AlgOCLL[sp][n].Shrink());
                            F2L_1[sp][n].EraseLast();
                        }
                        else if (!AlgCOLL[sp].empty())
                        {
                            AlgCOLL[sp][n].Insert(0u, F2L_1[sp][n].Last());
                            while (AlgCOLL[sp][n].Shrink());
                            F2L_1[sp][n].EraseLast();
                        }
                    }
                    else
                    {
                        F2L_2[sp][n].Insert(0u, F2L_1[sp][n].Last());
                        while (F2L_2[sp][n].Shrink());
                        F2L_1[sp][n].EraseLast();
                    }
                }

                if (Algorithm::IsTurn(F2L_2[sp][n].Last()))
                {
                    if (F2L_3[sp][n].Empty())
                    {
                        if (!AlgZBLL[sp].empty())
                        {
                            AlgZBLL[sp][n].Insert(0u, F2L_2[sp][n].Last());
                            while (AlgZBLL[sp][n].Shrink());
                            F2L_2[sp][n].EraseLast();
                        }
                        else if (!AlgOCLL[sp].empty())
                        {
                            AlgOCLL[sp][n].Insert(0u, F2L_2[sp][n].Last());
                            while (AlgOCLL[sp][n].Shrink());
                            F2L_2[sp][n].EraseLast();
                        }
                        else if (!AlgCOLL[sp].empty())
                        {
                            AlgCOLL[sp][n].Insert(0u, F2L_2[sp][n].Last());
                            while (AlgCOLL[sp][n].Shrink());
                            F2L_2[sp][n].EraseLast();
                        }
                    }
                    else
                    {
                        F2L_3[sp][n].Insert(0u, F2L_2[sp][n].Last());
                        while (F2L_3[sp][n].Shrink());
                        F2L_2[sp][n].EraseLast();
                    }
                }

                if (Algorithm::IsTurn(F2L_3[sp][n].Last()))
                {
                    if (F2L_4[sp][n].Empty())
                    {
                        if (!AlgZBLL[sp].empty())
                        {
                            AlgZBLL[sp][n].Insert(0u, F2L_3[sp][n].Last());
                            while (AlgZBLL[sp][n].Shrink());
                            F2L_3[sp][n].EraseLast();
                        }
                        else if (!AlgOCLL[sp].empty())
                        {
                            AlgOCLL[sp][n].Insert(0u, F2L_3[sp][n].Last());
                            while (AlgOCLL[sp][n].Shrink());
                            F2L_3[sp][n].EraseLast();
                        }
                        else if (!AlgCOLL[sp].empty())
                        {
                            AlgCOLL[sp][n].Insert(0u, F2L_3[sp][n].Last());
                            while (AlgCOLL[sp][n].Shrink());
                            F2L_3[sp][n].EraseLast();
                        }
                    }
                    else
                    {
                        F2L_4[sp][n].Insert(0u, F2L_3[sp][n].Last());
                        while (F2L_4[sp][n].Shrink());
                        F2L_3[sp][n].EraseLast();
                    }
                }
                
                if (Algorithm::IsTurn(F2L_4[sp][n].Last()))
                {
                    if (!AlgZBLL[sp].empty())
                    {
                        AlgZBLL[sp][n].Insert(0u, F2L_4[sp][n].Last());
                        while (AlgZBLL[sp][n].Shrink());
                        F2L_4[sp][n].EraseLast();
                    }
                    else if (!AlgOCLL[sp].empty())
                    {
                        AlgOCLL[sp][n].Insert(0u, F2L_4[sp][n].Last());
                        while (AlgOCLL[sp][n].Shrink());
                        F2L_4[sp][n].EraseLast();
                    }
                    else if (!AlgCOLL[sp].empty())
                    {
                        AlgCOLL[sp][n].Insert(0u, F2L_4[sp][n].Last());
                        while (AlgCOLL[sp][n].Shrink());
                        F2L_4[sp][n].EraseLast();
                    }
                }
            }
        }
    }

    // Get a solve report
    std::string ZZ::GetReport(const bool cancellations, const bool debug) const
    {
        std::string Report, ReportLine;
        // Report = "ZZ: Scramble [" + std::to_string(Scramble.GetNumSteps()) + "] " + GetTextScramble();
        // Report += "\n--------------------------------------------------------------------------------\n";

        for (const auto spin : SearchSpins)
        {
            int sp = static_cast<int>(spin);

            if (!CheckSolveConsistency(spin)) continue;

            for (uint n = 0u; n < Inspections[sp].size(); n++)
            {
                ReportLine.clear();

                Cube C = CubeBase;
                C.ApplyAlgorithm(Inspections[sp][n]);
                C.ApplyAlgorithm(EOX[sp][n]);

                if (IsEOLineBuilt(C, spin))
                {
                    ReportLine += "[" + Cube::GetSpinText(spin) + "|";
                    if (IsEO223Built(C, spin)) ReportLine += "EO223 ";
                    else if (IsXEOCrossBuilt(C, spin)) ReportLine += "XEOCross ";
                    else if (IsXEOLineBuilt(C, spin)) ReportLine += "XEOLine ";
                    else if (IsEOCrossBuilt(C, spin)) ReportLine += "EOCross ";
                    else if (IsEOArrowBuilt(C, spin)) ReportLine += "EOArrow ";
                    else ReportLine += "EOLine ";
                    ReportLine += Algorithm::GetMetricValue(GetMetricSolve(spin, n));
                    if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
                    ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
                    if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
                    ReportLine += "(" + EOX[sp][n].ToString() + ")";
                }
                else
                {
                    if (debug)
                    {
                        ReportLine += "[" + Cube::GetSpinText(spin) + "|No EOLine]: ";
                        if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
                        if (!EOX[sp][n].Empty()) ReportLine += "(" + EOX[sp][n].ToString() + ")\n";
                        Report += ReportLine;
                    }
                    continue;
                }

				C.ApplyAlgorithm(F2L_1[sp][n]);
				C.ApplyAlgorithm(F2L_2[sp][n]);
				C.ApplyAlgorithm(F2L_3[sp][n]);
				C.ApplyAlgorithm(F2L_4[sp][n]);

                if (IsF2LBuilt(C, Cube::GetDownSliceLayer(spin))) ReportLine += GetTextF2L(spin, n);

                else
                {
                    if (debug) 
					{
						ReportLine += " F2L not found: " + GetTextF2L(spin, n) + "\n";
                        Report += ReportLine;
					}
                    continue;
                }

                if (!AlgZBLL[sp].empty())
                {
                    C.ApplyAlgorithm(AlgZBLL[sp][n]);
                    if (debug) ReportLine += " {ZBLL: " + CasesZBLL[sp][n] + "}";
                    if (!AlgZBLL[sp][n].Empty()) ReportLine += " (" + AlgZBLL[sp][n].ToString() + ")";
                }

                if (!AlgOCLL[sp].empty())
                {
                    C.ApplyAlgorithm(AlgOCLL[sp][n]);
                    if (debug) ReportLine += " {OCLL: " + CasesOCLL[sp][n] + "}";
                    if (!AlgOCLL[sp][n].Empty()) ReportLine += " (" + AlgOCLL[sp][n].ToString() + ")";
                }
                if (!AlgPLL[sp].empty())
                {
                    C.ApplyAlgorithm(AlgPLL[sp][n]);
                    if (debug) ReportLine += " {PLL: " + CasesPLL[sp][n] + "}";
                    if (!AlgPLL[sp][n].Empty()) ReportLine += " (" + AlgPLL[sp][n].ToString() + ")";
                }

                if (!AlgCOLL[sp].empty())
                {
                    C.ApplyAlgorithm(AlgCOLL[sp][n]);
                    if (debug) ReportLine += " {COLL: " + CasesCOLL[sp][n] + "}";
                    if (!AlgCOLL[sp][n].Empty()) ReportLine += " (" + AlgCOLL[sp][n].ToString() + ")";
                }
                if (!AlgEPLL[sp].empty())
                {
                    C.ApplyAlgorithm(AlgEPLL[sp][n]);
                    if (debug) ReportLine += " {EPLL: " + CasesEPLL[sp][n] + "}";
                    if (!AlgEPLL[sp][n].Empty()) ReportLine += " (" + AlgEPLL[sp][n].ToString() + ")";
                }

                if (debug)
                {
                    if (C.IsSolved()) ReportLine += " [OK]\n";
                    else ReportLine += " [Not OK]\n";
                }
                else ReportLine.push_back('\n');

                Report += ReportLine;
            }
        }

        return Report;
    }

    // Get a solve time report
    std::string ZZ::GetTimeReport() const
    {
        std::string Report;

        Report += "Total time: " + std::to_string(GetTime()) + " s\n";
        Report += "EO X time: " + std::to_string(GetTimeEOX()) + " s\n";
        Report += "F2L time: " + std::to_string(GetTimeF2L()) + " s\n";
        Report += "Last layer time: " + std::to_string(GetTimeLL()) + " s\n";
        Report += "Threads: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : DeepSearch::GetSystemCores()) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string ZZ::GetReport(const Spn sp, const uint n) const
	{
		const int si = static_cast<int>(sp);
		
        if (!CheckSolveConsistency(sp) || EOX[si].size() <= n)
		{
			std::string FReport = "No solve for spin ";
			FReport += Cube::GetSpinText(sp);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

        std::string Report;
        // Report += "ZZ search with cube in orientation " + Cube::GetSpinText(sp);
        // Report += ":\n---------------------------------\n";
        // Report = "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
		C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += GetTextInspection(sp,n) + " // Inspection [" + C.GetSpinText() + "]\n";

        C.ApplyAlgorithm(EOX[si][n]);
		if (!IsEOLineBuilt(C, sp))
		{
            Report += "EO X not built in " + std::to_string(MaxDepthEOX) + " movements\n";
			return Report;
		}

        Report += EOX[si][n].ToString() + " // ";
        if (IsEO223Built(C, sp)) Report += "EO223 (";
        else if (IsXEOCrossBuilt(C, sp)) Report += "XEOCross (";
        else if (IsXEOLineBuilt(C, sp)) Report += "XEOLine (";
        else if (IsEOCrossBuilt(C, sp)) Report += "EOCross (";
        else if (IsEOArrowBuilt(C, sp)) Report += "EOArrow (";
        else Report += "EOLine (";
        Report += Algorithm::GetMetricValue(GetMetricEOX(sp, n)) + ")\n";

        // Show F2L solves
        C.ApplyAlgorithm(F2L_1[si][n]);
        C.ApplyAlgorithm(F2L_2[si][n]);
        C.ApplyAlgorithm(F2L_3[si][n]);
        C.ApplyAlgorithm(F2L_4[si][n]);

        if (!IsF2LBuilt(C, Cube::GetDownSliceLayer(sp)))
        {
            Report += "F2L not solved!\n";
            return Report;
        }

        std::string F2LString = GetTextF2L_1(sp, n);
        if (!F2LString.empty()) Report += F2LString + " // F2L 1 (" + Algorithm::GetMetricValue(GetMetricF2L_1(sp, n)) + ")\n";
        F2LString = GetTextF2L_2(sp, n);
        if (!F2LString.empty()) Report += F2LString + " // F2L 2 (" + Algorithm::GetMetricValue(GetMetricF2L_2(sp, n)) + ")\n";
        F2LString = GetTextF2L_3(sp, n);
        if (!F2LString.empty()) Report += F2LString + " // F2L 3 (" + Algorithm::GetMetricValue(GetMetricF2L_3(sp, n)) + ")\n";
        F2LString = GetTextF2L_4(sp, n);
        if (!F2LString.empty()) Report += F2LString + " // F2L 4 (" + Algorithm::GetMetricValue(GetMetricF2L_4(sp, n)) + ")\n";

		if (!AlgZBLL[si].empty())
		{
            // C.ApplyAlgorithm(AlgZBLL[si][n]);
            Report += GetTextZBLL(sp, n) + " // ZBLL (" + Algorithm::GetMetricValue(GetMetricZBLL(sp, n)) + ")\n";
		}
		else if (!AlgOCLL[si].empty() && !AlgPLL[si].empty())
		{
            // C.ApplyAlgorithm(AlgOCLL[si][n]);
            if (!AlgOCLL[si][n].Empty()) Report += GetTextOCLL(sp, n) + " // OCLL (" + Algorithm::GetMetricValue(GetMetricOCLL(sp, n)) + ")\n";

            // C.ApplyAlgorithm(AlgPLL[si][n]);
            if (!AlgPLL[si][n].Empty()) Report += GetTextPLL(sp, n) + " // PLL (" + Algorithm::GetMetricValue(GetMetricPLL(sp, n)) + ")\n";
		}
		else if (!AlgCOLL[si].empty() && !AlgEPLL[si].empty())
		{
            // C.ApplyAlgorithm(AlgCOLL[si][n]);
            if (!AlgCOLL[si][n].Empty()) Report += GetTextCOLL(sp, n) + " // COLL (" + Algorithm::GetMetricValue(GetMetricCOLL(sp, n)) + ")\n";

            // C.ApplyAlgorithm(AlgEPLL[si][n]);
            if (!AlgEPLL[si][n].Empty()) Report += GetTextEPLL(sp, n) + " // EPLL (" + Algorithm::GetMetricValue(GetMetricEPLL(sp, n)) + ")\n";
		}
		else
		{
			Report += "Last layer not solved!";
			return Report;
		}

        // Show summary
        Report += "\nMetric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
        if (!CasesOCLL[si].empty() && !CasesPLL[si].empty()) // OCLL + PLL
        {
			Report += "OCLL case: " + GetTextOCLLCase(sp, n) + "\n";
			Report += "PLL case: " + GetTextPLLCase(sp, n) + "\n";
		}
        else if (!CasesZBLL[si].empty()) // ZBLL
			Report += "ZBLL case: " + GetTextZBLLCase(sp, n) + "\n";
        else if (!CasesCOLL[si].empty() && !CasesEPLL[si].empty()) // COLL + EPLL
		{
			Report += "COLL case: " + GetTextCOLLCase(sp, n) + "\n";
			Report += "EPLL case: " + GetTextEPLLCase(sp, n) + "\n";
		}

		return Report;
	}

    // Check if the solves for the given cross layer are consistent (all needed algorithms are present)
    bool ZZ::CheckSolveConsistency(const Spn Spin) const
    {
        const int si = static_cast<int>(Spin); // Spin index

        if (Inspections[si].empty()) return false;

        auto n = Inspections[si].size();

        return (EOX[si].size() == n) &&
               (F2L_1[si].size() == n) && (F2L_2[si].size() == n) && (F2L_3[si].size() == n) && (F2L_4[si].size() == n) &&
               ((AlgZBLL[si].size() == n) ||
               ((AlgOCLL[si].size() == n) && AlgPLL[si].size() == n) ||
               ((AlgCOLL[si].size() == n) && AlgEPLL[si].size() == n));
    }

    // Get the full solve
    Algorithm ZZ::GetFullSolve(const Spn spin, const uint n) const
    {
        const int si = static_cast<int>(spin); // Spin index

        if (!CheckSolveConsistency(spin) || n >= Inspections[si].size()) return "";

        Algorithm A = Inspections[si][n];
        A += EOX[si][n];
        A += F2L_1[si][n];
        A += F2L_2[si][n];
        A += F2L_3[si][n];
        A += F2L_4[si][n];
        if (!AlgZBLL[si].empty()) A += AlgZBLL[si][n];
        if (!AlgOCLL[si].empty()) A += AlgOCLL[si][n];
        if (!AlgPLL[si].empty()) A += AlgPLL[si][n];
        if (!AlgCOLL[si].empty()) A += AlgCOLL[si][n];
        if (!AlgEPLL[si].empty()) A += AlgEPLL[si][n];

        return A;
    }

    // Get full F2L string
    std::string ZZ::GetTextF2L(const Spn sp, const uint n) const
    {
        std::string F2LString, Aux = GetTextF2L_1(sp, n);
        if (!Aux.empty()) F2LString += " (" + Aux + ")";
        Aux = GetTextF2L_2(sp, n);
        if (!Aux.empty()) F2LString += " (" + Aux + ")";
        Aux = GetTextF2L_3(sp, n);
        if (!Aux.empty()) F2LString += " (" + Aux + ")";
        Aux = GetTextF2L_4(sp, n);
        if (!Aux.empty()) F2LString += " (" + Aux + ")";
        return F2LString;
    }
}
