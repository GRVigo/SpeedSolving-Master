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

#include "petrus.h"
#include "collection.h"

namespace grcube3
{
	// Reset the search results
    void Petrus::Reset()
	{
        for (int i = 0; i < 24; i++)
        {
			Inspections[i].clear();
			Blocks[i].clear();
            ExpandedBlocks[i].clear();
            EO[i].clear();
            F2L[i].clear();
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
		
        MaxDepthBlock = MaxDepthF2L = 0u;

        TimeBlock = TimeExpBlock = TimeEO = TimeF2L = TimeZBLL = TimeOCLL = TimePLL = TimeCOLL = TimeEPLL = 0.0;
		
		Metric = Metrics::Movements; // Default metric
	}

    // Search the best block solve algorithm with the given search deep
    // Return false if no block found
    bool Petrus::SearchBlock(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_block_start = std::chrono::system_clock::now();

        MaxDepthBlock = (MaxDepth <= 4u ? 4u : MaxDepth);

        // Deep search for first block
		DS.Clear();
        DS.SetScramble(Scramble);
		
        DS.AddToOptionalPieces(Pgr::PETRUS_UFR);
        DS.AddToOptionalPieces(Pgr::PETRUS_UFL);
        DS.AddToOptionalPieces(Pgr::PETRUS_UBR);
        DS.AddToOptionalPieces(Pgr::PETRUS_UBL);

        DS.AddToOptionalPieces(Pgr::PETRUS_DFR);
        DS.AddToOptionalPieces(Pgr::PETRUS_DFL);
        DS.AddToOptionalPieces(Pgr::PETRUS_DBR);
        DS.AddToOptionalPieces(Pgr::PETRUS_DBL);

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
        const SearchUnit URoot(SequenceTypes::DOUBLE);
        const SearchUnit U(SequenceTypes::SINGLE);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(URoot);
		
        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U);

        DS.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        for (uint l = 2u; l < MaxDepthBlock; l++) DS.AddSearchLevel(L_Check);

        DS.UpdateRootData();
		// DSFB.SetMinDeep(DSFB.GetMaxDeep() - 2u);

        DS.Run(Cores);

        Cores = DS.GetUsedCores(); // Update to the real number of cores used

        EvaluateBlock(DS.GetSolves(), MaxSolves);

        const std::chrono::duration<double> block_elapsed_seconds = std::chrono::system_clock::now() - time_block_start;
        TimeBlock = block_elapsed_seconds.count();

        return !DS.GetSolves().empty();
	}
	
	// Search the best block solve algorithms from an algorithms vector
	void Petrus::EvaluateBlock(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best block solve algorithms for each corner
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);
			Stp T1, T2;
			Algorithm Insp;

            EvaluatePetrusBlockResult(Blocks[sp], MaxSolves, Solves, CubeBase, spin, Plc::BEST);

			Inspections[sp].clear();

			if (!Blocks[sp].empty())
			{
				Insp.Clear();
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				if (T1 != Stp::NONE) Insp.Append(T1);
				if (T2 != Stp::NONE) Insp.Append(T2);

				for (auto& fb : Blocks[sp])
				{
					if (T1 != Stp::NONE) fb.TransformTurn(T1);
					if (T2 != Stp::NONE) fb.TransformTurn(T2);
					Inspections[sp].push_back(Insp);
				}
			}
		}
	}

    // Search the best expanded block solve algorithm
    void Petrus::SearchExpandedBlock()
    {
        const auto time_eb_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        const SearchUnit U_Root(SequenceTypes::DOUBLE, Sst::PETRUS_EB);
        const SearchUnit U_EB(SequenceTypes::SINGLE, Sst::PETRUS_EB);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(U_Root);
		
		SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_EB);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Pgr EXPBLOCK;

			switch (spin)
			{
			case Spn::UF: EXPBLOCK = Pgr::PETRUS_DBL_Z; break;
			case Spn::UB: EXPBLOCK = Pgr::PETRUS_DFR_Z; break;
			case Spn::UR: EXPBLOCK = Pgr::PETRUS_DFL_X; break;
			case Spn::UL: EXPBLOCK = Pgr::PETRUS_DBR_X; break;

			case Spn::DF: EXPBLOCK = Pgr::PETRUS_UBR_Z; break;
			case Spn::DB: EXPBLOCK = Pgr::PETRUS_UFL_Z; break;
			case Spn::DR: EXPBLOCK = Pgr::PETRUS_UBL_X; break;
			case Spn::DL: EXPBLOCK = Pgr::PETRUS_UFR_X; break;

			case Spn::FU: EXPBLOCK = Pgr::PETRUS_DBR_Y; break;
			case Spn::FD: EXPBLOCK = Pgr::PETRUS_UBL_Y; break;
			case Spn::FR: EXPBLOCK = Pgr::PETRUS_DBL_X; break;
			case Spn::FL: EXPBLOCK = Pgr::PETRUS_UBR_X; break;

			case Spn::BU: EXPBLOCK = Pgr::PETRUS_DFL_Y; break;
			case Spn::BD: EXPBLOCK = Pgr::PETRUS_UFR_Y; break;
			case Spn::BR: EXPBLOCK = Pgr::PETRUS_UFL_X; break;
			case Spn::BL: EXPBLOCK = Pgr::PETRUS_DFR_X; break;

			case Spn::RU: EXPBLOCK = Pgr::PETRUS_DBL_Y; break;
			case Spn::RD: EXPBLOCK = Pgr::PETRUS_UFL_Y; break;
			case Spn::RF: EXPBLOCK = Pgr::PETRUS_UBL_Z; break;
			case Spn::RB: EXPBLOCK = Pgr::PETRUS_DFL_Z; break;

			case Spn::LU: EXPBLOCK = Pgr::PETRUS_DFR_Y; break;
			case Spn::LD: EXPBLOCK = Pgr::PETRUS_UBR_Y; break;
			case Spn::LF: EXPBLOCK = Pgr::PETRUS_DBR_Z; break;
			case Spn::LB: EXPBLOCK = Pgr::PETRUS_UFR_Z; break;

			default: return; // Should not happend
			}

			uint n = 0u;
			for (const auto& Block : Blocks[sp])
			{
                ExpandedBlocks[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(Block);

				Cube C(AlgStart);

                if (!IsPetrusBlockBuilt(C) || IsPetrusExpandedBlockBuilt(C)) continue; // Block not solved or expanded block already solved

				// Deep search for expanded block
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy(); 

				DS.AddToMandatoryPieces(EXPBLOCK);

				DS.AddSearchLevel(L_Root);
                for (int l = 1; l < 8; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				Algorithm Solve;

				DS.EvaluateShortestResult(Solve, true);

				C.ApplyAlgorithm(Solve);

                if (IsPetrusExpandedBlockBuilt(C)) ExpandedBlocks[sp][n] = Solve;

                n++;
			}
        }
        
        const std::chrono::duration<double> eb_elapsed_seconds = std::chrono::system_clock::now() - time_eb_start;
        TimeExpBlock = eb_elapsed_seconds.count();
    }
	
    // Orientate the other edges outside the block
    void Petrus::SearchEO()
    {
        const auto time_EO_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)
        
        const SearchUnit U_UR(SequenceTypes::SINGLE, Sst::SINGLE_UR);
        const SearchUnit U_OR1(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::PETRUS_OR_U);
        const SearchUnit U_OR2(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_R, Sst::PETRUS_OR_R);

        SearchLevel L_Check(SearchCheck::CHECK);
		L_Check.Add(U_UR);
        L_Check.Add(U_OR1);
		L_Check.Add(U_OR2);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Pgr ExpBlock;
			Pcp EO_Edge;

			switch (spin)
			{
			case Spn::UF: ExpBlock = Pgr::PETRUS_DBL_Z; EO_Edge = Pcp::DR; break;
			case Spn::UB: ExpBlock = Pgr::PETRUS_DFR_Z; EO_Edge = Pcp::DL; break;
			case Spn::UR: ExpBlock = Pgr::PETRUS_DFL_X; EO_Edge = Pcp::DB; break;
			case Spn::UL: ExpBlock = Pgr::PETRUS_DBR_X; EO_Edge = Pcp::DF; break;

			case Spn::DF: ExpBlock = Pgr::PETRUS_UBR_Z; EO_Edge = Pcp::UL; break;
			case Spn::DB: ExpBlock = Pgr::PETRUS_UFL_Z; EO_Edge = Pcp::UR; break;
			case Spn::DR: ExpBlock = Pgr::PETRUS_UBL_X; EO_Edge = Pcp::UF; break;
			case Spn::DL: ExpBlock = Pgr::PETRUS_UFR_X; EO_Edge = Pcp::UB; break;

			case Spn::FU: ExpBlock = Pgr::PETRUS_DBR_Y; EO_Edge = Pcp::BL; break;
			case Spn::FD: ExpBlock = Pgr::PETRUS_UBL_Y; EO_Edge = Pcp::BR; break;
			case Spn::FR: ExpBlock = Pgr::PETRUS_DBL_X; EO_Edge = Pcp::UB; break;
			case Spn::FL: ExpBlock = Pgr::PETRUS_UBR_X; EO_Edge = Pcp::DB; break;

			case Spn::BU: ExpBlock = Pgr::PETRUS_DFL_Y; EO_Edge = Pcp::FR; break;
			case Spn::BD: ExpBlock = Pgr::PETRUS_UFR_Y; EO_Edge = Pcp::FL; break;
			case Spn::BR: ExpBlock = Pgr::PETRUS_UFL_X; EO_Edge = Pcp::DF; break;
			case Spn::BL: ExpBlock = Pgr::PETRUS_DFR_X; EO_Edge = Pcp::UF; break;

			case Spn::RU: ExpBlock = Pgr::PETRUS_DBL_Y; EO_Edge = Pcp::FL; break;
			case Spn::RD: ExpBlock = Pgr::PETRUS_UFL_Y; EO_Edge = Pcp::BL; break;
			case Spn::RF: ExpBlock = Pgr::PETRUS_UBL_Z; EO_Edge = Pcp::DL; break;
			case Spn::RB: ExpBlock = Pgr::PETRUS_DFL_Z; EO_Edge = Pcp::UL; break;

			case Spn::LU: ExpBlock = Pgr::PETRUS_DFR_Y; EO_Edge = Pcp::BR; break;
			case Spn::LD: ExpBlock = Pgr::PETRUS_UBR_Y; EO_Edge = Pcp::FR; break;
			case Spn::LF: ExpBlock = Pgr::PETRUS_DBR_Z; EO_Edge = Pcp::UR; break;
			case Spn::LB: ExpBlock = Pgr::PETRUS_UFR_Z; EO_Edge = Pcp::DR; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
                EO[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(Blocks[sp][n]);
				AlgStart.Append(ExpandedBlocks[sp][n]);
				Cube CubePetrus(AlgStart);

				if (!IsPetrusExpandedBlockBuilt(CubePetrus) || CubePetrus.CheckOrientation(Pgr::ALL_EDGES)) continue; // Edges already oriented or not expanded block

				// Deep search for edges orientation
				DS.Clear();
                DS.SetScramble(AlgStart);

				DS.AddToMandatoryPieces(ExpBlock);
				DS.AddToMandatoryPieces(EO_Edge);
				DS.AddToMandatoryOrientations(Pgr::ALL_EDGES);

				for (int l = 0; l < 4; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				std::vector<Algorithm> Solves;
                Method::EvaluateCFOPF2LResult(Solves, 1u, DS.GetSolves(), CubePetrus, Cube::GetDownSliceLayer(spin), Plc::BEST);

				if (!Solves.empty())
				{
					CubePetrus.ApplyAlgorithm(Solves[0]);
					if (CubePetrus.CheckOrientation(Pgr::ALL_EDGES)) EO[sp][n] = Solves[0];
				}
			}
		}
        
        const std::chrono::duration<double> EO_elapsed_seconds = std::chrono::system_clock::now() - time_EO_start;
        TimeEO = EO_elapsed_seconds.count();
    }
	
	// Complete the two first layers (F2L)
    void Petrus::SearchF2L(const uint MaxDepth)
	{
		const auto time_F2L_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthF2L = (MaxDepth <= 4u ? 4u : MaxDepth);

        const SearchUnit U_Root(SequenceTypes::DOUBLE, Sst::SINGLE_UR);
        const SearchUnit U_UR(SequenceTypes::SINGLE, Sst::SINGLE_UR);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(U_Root);

		SearchLevel L_Check(SearchCheck::CHECK);
		L_Check.Add(U_UR);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			const Lyr DownLayer = Cube::GetDownSliceLayer(spin);
			const Lyr MiddleLayer = Cube::AdjacentLayer(DownLayer);

			Pgr LDOWN, LMID, CUP;

			switch (DownLayer)
			{
			case Lyr::U: LDOWN = Pgr::LAYER_U; LMID = Pgr::LAYER_E; CUP = Pgr::CROSS_D; break;
			case Lyr::D: LDOWN = Pgr::LAYER_D; LMID = Pgr::LAYER_E; CUP = Pgr::CROSS_U; break;
			case Lyr::F: LDOWN = Pgr::LAYER_F; LMID = Pgr::LAYER_S; CUP = Pgr::CROSS_B; break;
			case Lyr::B: LDOWN = Pgr::LAYER_B; LMID = Pgr::LAYER_S; CUP = Pgr::CROSS_F; break;
			case Lyr::R: LDOWN = Pgr::LAYER_R; LMID = Pgr::LAYER_M; CUP = Pgr::CROSS_L; break;
			case Lyr::L: LDOWN = Pgr::LAYER_L; LMID = Pgr::LAYER_M; CUP = Pgr::CROSS_R; break;
			default: continue; // Should not happend
			}

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				F2L[sp].push_back(Algorithm(""));
				
				if (Skip) continue;

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(Blocks[sp][n]);
				AlgStart.Append(ExpandedBlocks[sp][n]);
				AlgStart.Append(EO[sp][n]);
				const Cube C(AlgStart);

				if (C.CheckOrientation(Pgr::ALL_EDGES) && C.IsSolved(DownLayer) && C.IsSolved(MiddleLayer)) continue; // F2L already solved

				// Deep search for F2L
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(LDOWN);
				DS.AddToMandatoryPieces(LMID);
				DS.AddToMandatoryOrientations(CUP);

				DS.AddSearchLevel(L_Root);
				for (uint l = 2; l < MaxDepthF2L; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				DS.EvaluateShortestResult(F2L[sp][n], true);
			}
		}

		const std::chrono::duration<double> F2L_elapsed_seconds = std::chrono::system_clock::now() - time_F2L_start;
		TimeF2L = F2L_elapsed_seconds.count();
	}
	
	// Complete the two first layers (F2L) - Alternative version (don't use it, slower and debug needed)
	void Petrus::SearchF2L_Alt(const uint MaxDepth)
	{
		const auto time_F2L_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthF2L = (MaxDepth <= 4u ? 4u : MaxDepth);

		bool F2L_Found = false; // First layer found flag

		Algorithm F2L_Aux, // Auxiliary algorithm for F2L search
			A1_F2L, A2_F2L; // Algorithms for F2L

		std::vector<Algorithm> F2L_Algs;

        const SearchUnit U_Up(SequenceTypes::SINGLE, Sst::SINGLE_U);
        const SearchUnit U_Lat(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::PETRUS_UD_R);

		SearchLevel L_F2L_Check(SearchCheck::CHECK);
		L_F2L_Check.Add(U_Up);
		L_F2L_Check.Add(U_Lat);

		SearchLevel L_F2L_End(SearchCheck::CHECK);
		L_F2L_End.Add(U_Lat);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			F2L_Algs.clear();

			Pgr ExpBlock, F2L_Pair_1, F2L_Pair_2, LL_Cross;
			Pcp F2L_Edge; // Edge to complete F2L

			switch (spin)
			{
			case Spn::UF: ExpBlock = Pgr::PETRUS_DBL_Z;
				LL_Cross = Pgr::LAYER_U;
				F2L_Pair_1 = Pgr::F2L_D_DFR;
				F2L_Pair_2 = Pgr::F2L_D_DBR;
				F2L_Edge = Pcp::DR; break;
			case Spn::UB: ExpBlock = Pgr::PETRUS_DFR_Z;
				LL_Cross = Pgr::LAYER_U;
				F2L_Pair_1 = Pgr::F2L_D_DBL;
				F2L_Pair_2 = Pgr::F2L_D_DFL;
				F2L_Edge = Pcp::DL; break;
			case Spn::UR: ExpBlock = Pgr::PETRUS_DFL_X;
				LL_Cross = Pgr::LAYER_U;
				F2L_Pair_1 = Pgr::F2L_D_DBR;
				F2L_Pair_2 = Pgr::F2L_D_DBL;
				F2L_Edge = Pcp::DB; break;
			case Spn::UL: ExpBlock = Pgr::PETRUS_DBR_X;
				LL_Cross = Pgr::LAYER_U;
				F2L_Pair_1 = Pgr::F2L_D_DFL;
				F2L_Pair_2 = Pgr::F2L_D_DFR;
				F2L_Edge = Pcp::DF; break;

			case Spn::DF: ExpBlock = Pgr::PETRUS_UBR_Z;
				LL_Cross = Pgr::LAYER_D;
				F2L_Pair_1 = Pgr::F2L_U_UFL;
				F2L_Pair_2 = Pgr::F2L_U_UBL;
				F2L_Edge = Pcp::UL;  break;
			case Spn::DB: ExpBlock = Pgr::PETRUS_UFL_Z;
				LL_Cross = Pgr::LAYER_D;
				F2L_Pair_1 = Pgr::F2L_U_UBR;
				F2L_Pair_2 = Pgr::F2L_U_UFR;
				F2L_Edge = Pcp::UR; break;
			case Spn::DR: ExpBlock = Pgr::PETRUS_UBL_X;
				LL_Cross = Pgr::LAYER_D;
				F2L_Pair_1 = Pgr::F2L_U_UFR;
				F2L_Pair_2 = Pgr::F2L_U_UFL;
				F2L_Edge = Pcp::UF; break;
			case Spn::DL: ExpBlock = Pgr::PETRUS_UFR_X;
				LL_Cross = Pgr::LAYER_D;
				F2L_Pair_1 = Pgr::F2L_U_UBR;
				F2L_Pair_2 = Pgr::F2L_U_UBL;
				F2L_Edge = Pcp::UB; break;

			case Spn::FU: ExpBlock = Pgr::PETRUS_DBR_Y;
				LL_Cross = Pgr::LAYER_F;
				F2L_Pair_1 = Pgr::F2L_B_UBL;
				F2L_Pair_2 = Pgr::F2L_B_DBL;
				F2L_Edge = Pcp::BL;  break;
			case Spn::FD: ExpBlock = Pgr::PETRUS_UBL_Y;
				LL_Cross = Pgr::LAYER_F;
				F2L_Pair_1 = Pgr::F2L_B_UBR;
				F2L_Pair_2 = Pgr::F2L_B_DBR;
				F2L_Edge = Pcp::BR; break;
			case Spn::FR: ExpBlock = Pgr::PETRUS_DBL_X;
				LL_Cross = Pgr::LAYER_F;
				F2L_Pair_1 = Pgr::F2L_B_UBR;
				F2L_Pair_2 = Pgr::F2L_B_UBL;
				F2L_Edge = Pcp::UB; break;
			case Spn::FL: ExpBlock = Pgr::PETRUS_UBR_X;
				LL_Cross = Pgr::LAYER_F;
				F2L_Pair_1 = Pgr::F2L_B_DBR;
				F2L_Pair_2 = Pgr::F2L_B_DBL;
				F2L_Edge = Pcp::DB; break;

			case Spn::BU: ExpBlock = Pgr::PETRUS_DFL_Y;
				LL_Cross = Pgr::LAYER_B;
				F2L_Pair_1 = Pgr::F2L_F_UFR;
				F2L_Pair_2 = Pgr::F2L_F_DFR;
				F2L_Edge = Pcp::FR; break;
			case Spn::BD: ExpBlock = Pgr::PETRUS_UFR_Y;
				LL_Cross = Pgr::LAYER_B;
				F2L_Pair_1 = Pgr::F2L_F_UFL;
				F2L_Pair_2 = Pgr::F2L_F_DFL;
				F2L_Edge = Pcp::FL; break;
			case Spn::BR: ExpBlock = Pgr::PETRUS_UFL_X;
				LL_Cross = Pgr::LAYER_B;
				F2L_Pair_1 = Pgr::F2L_F_DFR;
				F2L_Pair_2 = Pgr::F2L_F_DFL;
				F2L_Edge = Pcp::DF; break;
			case Spn::BL: ExpBlock = Pgr::PETRUS_DFR_X;
				LL_Cross = Pgr::LAYER_B;
				F2L_Pair_1 = Pgr::F2L_F_UFR;
				F2L_Pair_2 = Pgr::F2L_F_UFL;
				F2L_Edge = Pcp::UF; break;

			case Spn::RU: ExpBlock = Pgr::PETRUS_DBL_Y;
				LL_Cross = Pgr::LAYER_R;
				F2L_Pair_1 = Pgr::F2L_L_UFL;
				F2L_Pair_2 = Pgr::F2L_L_DFL;
				F2L_Edge = Pcp::FL; break;
			case Spn::RD: ExpBlock = Pgr::PETRUS_UFL_Y;
				LL_Cross = Pgr::LAYER_R;
				F2L_Pair_1 = Pgr::F2L_L_UBL;
				F2L_Pair_2 = Pgr::F2L_L_DBL;
				F2L_Edge = Pcp::BL; break;
			case Spn::RF: ExpBlock = Pgr::PETRUS_UBL_Z;
				LL_Cross = Pgr::LAYER_R;
				F2L_Pair_1 = Pgr::F2L_L_DFL;
				F2L_Pair_2 = Pgr::F2L_L_DBL;
				F2L_Edge = Pcp::DL; break;
			case Spn::RB: ExpBlock = Pgr::PETRUS_DFL_Z;
				LL_Cross = Pgr::LAYER_R;
				F2L_Pair_1 = Pgr::F2L_L_UFL;
				F2L_Pair_2 = Pgr::F2L_L_UBL;
				F2L_Edge = Pcp::UL; break;

			case Spn::LU: ExpBlock = Pgr::PETRUS_DFR_Y;
				LL_Cross = Pgr::LAYER_L;
				F2L_Pair_1 = Pgr::F2L_R_UBR;
				F2L_Pair_2 = Pgr::F2L_R_DBR;
				F2L_Edge = Pcp::BR; break;
			case Spn::LD: ExpBlock = Pgr::PETRUS_UBR_Y;
				LL_Cross = Pgr::LAYER_L;
				F2L_Pair_1 = Pgr::F2L_R_UFR;
				F2L_Pair_2 = Pgr::F2L_R_DFR;
				F2L_Edge = Pcp::FR; break;
			case Spn::LF: ExpBlock = Pgr::PETRUS_DBR_Z;
				LL_Cross = Pgr::LAYER_L;
				F2L_Pair_1 = Pgr::F2L_R_UFR;
				F2L_Pair_2 = Pgr::F2L_R_UBR;
				F2L_Edge = Pcp::UR; break;
			case Spn::LB: ExpBlock = Pgr::PETRUS_UFR_Z;
				LL_Cross = Pgr::LAYER_L;
				F2L_Pair_1 = Pgr::F2L_R_DFR;
				F2L_Pair_2 = Pgr::F2L_R_DBR;
				F2L_Edge = Pcp::DR; break;
			default: return; // Should not happend
			}

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				F2L[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				bool F2L_Pair_1_IsSolved, F2L_Pair_2_IsSolved, F2L_Edge_IsSolved;

				do
				{
					F2L_Aux = Scramble + Inspections[sp][n] + Blocks[sp][n] + ExpandedBlocks[sp][n] + EO[sp][n];
					for (auto& F2L_Alg : F2L_Algs) F2L_Aux += F2L_Alg;

					Cube CubeF2L(F2L_Aux);

					F2L_Pair_1_IsSolved = CubeF2L.IsSolved(F2L_Pair_1);
					F2L_Pair_2_IsSolved = CubeF2L.IsSolved(F2L_Pair_2);
					F2L_Edge_IsSolved = CubeF2L.IsSolved(F2L_Edge);

					if (F2L_Pair_1_IsSolved && F2L_Pair_2_IsSolved && F2L_Edge_IsSolved) break;

					DS.Clear();
                    DS.SetScramble(F2L_Aux);
					DS.SetShortPolicy();

					for (uint l = 0; l < MaxDepthF2L - 1u; l++) DS.AddSearchLevel(L_F2L_Check);
					DS.AddSearchLevel(L_F2L_End);
					DS.UpdateRootData();

					DS.AddToMandatoryPieces(ExpBlock);
					DS.AddToMandatoryPieces(F2L_Edge);
					DS.AddToMandatoryOrientations(LL_Cross);

					if (F2L_Pair_1_IsSolved) DS.AddToMandatoryPieces(F2L_Pair_1);
					else DS.AddToOptionalPieces(F2L_Pair_1);

					if (F2L_Pair_2_IsSolved) DS.AddToMandatoryPieces(F2L_Pair_2);
					else DS.AddToOptionalPieces(F2L_Pair_2);

					DS.Run(Cores);
					Skip = DS.CheckSkipSearch();

					DS.EvaluateShortestResult(F2L_Aux, true);

					CubeF2L.ApplyAlgorithm(F2L_Aux);

					F2L_Found = false;

					if (!F2L_Pair_1_IsSolved && CubeF2L.IsSolved(F2L_Pair_1))
					{
						A1_F2L = F2L_Aux;
						F2L_Algs.push_back(A1_F2L);
						F2L_Found = true;
					}
					else if (!F2L_Pair_2_IsSolved && CubeF2L.IsSolved(F2L_Pair_2))
					{
						A2_F2L = F2L_Aux;
						F2L_Algs.push_back(A2_F2L);
						F2L_Found = true;
					}

				} while (F2L_Found & !Skip);

				if (F2L_Pair_1_IsSolved && F2L_Pair_2_IsSolved && F2L_Edge_IsSolved)
				{
					for (auto& A : F2L_Algs)
					{
						A.InsertParentheses();
						F2L[sp][n] += A;
					}
				}
			}
		}

		const std::chrono::duration<double> F2L_elapsed_seconds = std::chrono::system_clock::now() - time_F2L_start;
		TimeF2L = F2L_elapsed_seconds.count();
	}

    // ZBLL search
    void Petrus::SearchZBLL(const Plc Pol)
	{
        const auto time_ZBLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgZBLL[sp].clear();
            CasesZBLL[sp].clear();

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				AlgZBLL[sp].push_back(Algorithm(""));
                CasesZBLL[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += Blocks[sp][n];
				Alg += ExpandedBlocks[sp][n];
				Alg += EO[sp][n];
				Alg += F2L[sp][n];

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
    void Petrus::SearchOCLL(const Plc Pol)
    {
        const auto time_OCLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgOCLL[sp].clear();
            CasesOCLL[sp].clear();

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				AlgOCLL[sp].push_back(Algorithm(""));
                CasesOCLL[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += Blocks[sp][n];
				Alg += ExpandedBlocks[sp][n];
				Alg += EO[sp][n];
				Alg += F2L[sp][n];

				Cube CubeF2L = CubeBase;
				CubeF2L.ApplyAlgorithm(Alg);

                OrientateLL(AlgOCLL[sp][n], CasesOCLL[sp][n], Algset_OCLL, Pol, Metric, CubeF2L);
			}
		}

        const std::chrono::duration<double> OCLL_elapsed_seconds = std::chrono::system_clock::now() - time_OCLL_start;
        TimeOCLL = OCLL_elapsed_seconds.count();
    }

    // PLL search
    void Petrus::SearchPLL(const Plc Pol)
    {
        const auto time_PLL_start = std::chrono::system_clock::now();
		
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgPLL[sp].clear();
            CasesPLL[sp].clear();

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				AlgPLL[sp].push_back(Algorithm(""));
                CasesPLL[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += Blocks[sp][n];
				Alg += ExpandedBlocks[sp][n];
				Alg += EO[sp][n];
				Alg += F2L[sp][n];
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
    void Petrus::SearchCOLL(const Plc Pol)
    {
        const auto time_COLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgCOLL[sp].clear();
            CasesCOLL[sp].clear();

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				AlgCOLL[sp].push_back(Algorithm(""));
                CasesCOLL[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += Blocks[sp][n];
				Alg += ExpandedBlocks[sp][n];
				Alg += EO[sp][n];
				Alg += F2L[sp][n];

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
    void Petrus::SearchEPLL(const Plc Pol)
    {
        const auto time_EPLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgEPLL[sp].clear();
            CasesEPLL[sp].clear();

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				AlgEPLL[sp].push_back(Algorithm(""));
                CasesEPLL[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += Blocks[sp][n];
				Alg += ExpandedBlocks[sp][n];
				Alg += EO[sp][n];
				Alg += F2L[sp][n];
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
	void Petrus::SetRegrips()
	{
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			if (!CheckSolveConsistency(spin)) continue;

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				Blocks[sp][n] = Blocks[sp][n].GetRegrip();

				if (Algorithm::IsTurn(Blocks[sp][n].First()))
				{
					Inspections[sp][n].AppendShrink(Blocks[sp][n].First());
					if (Inspections[sp][n].GetSize() == 3u) Inspections[sp][n] = Inspections[sp][n].GetCancellations();
					Blocks[sp][n].EraseFirst();
				}
			}
		}
	}

    // Get a solve report
    std::string Petrus::GetReport(const bool cancellations, const bool debug) const
    {
        std::string Report, ReportLine;
        // Report = "Petrus: Scramble [" + std::to_string(Scramble.GetNumSteps()) + "] " + GetTextScramble();
        // Report += "\n--------------------------------------------------------------------------------\n";

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			if (!CheckSolveConsistency(spin)) continue;

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				ReportLine.clear();

				Cube C = CubeBase;
				C.ApplyAlgorithm(Inspections[sp][n]);
				C.ApplyAlgorithm(Blocks[sp][n]);

				if (IsPetrusBlockBuilt(C))
				{
					ReportLine += "[" + Cube::GetSpinText(spin) + "|" + Algorithm::GetMetricValue(GetMetricSolve(spin, n));
					if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
					ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
					if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
					ReportLine += "(" + Blocks[sp][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
						ReportLine += "[" + Cube::GetSpinText(spin) + "]: ";
                        if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += "Block not built in " + std::to_string(MaxDepthBlock) + " steps";
                        if (!Blocks[sp][n].Empty()) ReportLine += ": (" + Blocks[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						Report += ReportLine;
					}
					continue;
				}

				C.ApplyAlgorithm(ExpandedBlocks[sp][n]);

				if (IsPetrusExpandedBlockBuilt(C))
				{
                    if (!ExpandedBlocks[sp][n].Empty()) ReportLine += " (" + ExpandedBlocks[sp][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
						ReportLine += " Expanded block not found: (" + ExpandedBlocks[sp][n].ToString() + ")\n";
						Report += ReportLine;
					}
					continue;
				}

				C.ApplyAlgorithm(EO[sp][n]);

				if (IsPetrusExpandedBlockBuilt(C) && C.CheckOrientation(Pgr::ALL_EDGES))
				{
                    if (!EO[sp][n].Empty()) ReportLine += " (" + EO[sp][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
						ReportLine += " Edges not oriented: (" + EO[sp][n].ToString() + ")\n";
						Report += ReportLine;
					}
					continue;
				}

				C.ApplyAlgorithm(F2L[sp][n]);

				if (C.IsSolved(Cube::GetDownSliceLayer(spin)) &&
					C.IsSolved(Cube::AdjacentLayer(Cube::GetDownSliceLayer(spin))) &&
					C.CheckOrientation(Pgr::ALL_EDGES))
				{
                    if (!F2L[sp][n].Empty()) ReportLine += " (" + F2L[sp][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
						ReportLine += " F2L not solved in " + std::to_string(MaxDepthF2L) + " movements\n";
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
    std::string Petrus::GetTimeReport() const
    {
        std::string Report;

        Report += "Total time: " + std::to_string(GetTime()) + " s\n";
        Report += "Blocks time: " + std::to_string(GetTimeBlock()) + " s\n";
        Report += "Expanded blocks time: " + std::to_string(GetTimeExpBlock()) + " s\n";
        Report += "Edges orientation time: " + std::to_string(GetTimeEO()) + " s\n";
        Report += "F2L time: " + std::to_string(GetTimeF2L()) + " s\n";
        Report += "Last layer time: " + std::to_string(GetTimeLL()) + " s\n";
        Report += "Threads: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : DeepSearch::GetSystemCores()) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string Petrus::GetReport(const Spn sp, const uint n) const
	{
		const int si = static_cast<int>(sp);

		if (!CheckSolveConsistency(sp) || Blocks[si].size() <= n)
		{
			std::string FReport = "No solve for spin ";
			FReport += Cube::GetSpinText(sp);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

        std::string Report;
        // Report = "Petrus search with orientation " + Cube::GetSpinText(sp);
        // Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
		C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += GetTextInspection(sp,n) + " // Inspection [" + C.GetSpinText() + "]\n";

		C.ApplyAlgorithm(Blocks[si][n]);
		if (!IsPetrusBlockBuilt(C))
		{
			Report += "Block not built in " + std::to_string(MaxDepthBlock) + " movements\n";
			return Report;
		}
        Report += GetTextBlock(sp, n) + " // Block (" + Algorithm::GetMetricValue(GetMetricBlock(sp, n)) + ")\n";

		C.ApplyAlgorithm(ExpandedBlocks[si][n]);
		if (!IsPetrusExpandedBlockBuilt(C))
		{
			Report += "Expanded block not built!\n";
			return Report;
		}
        if (!ExpandedBlocks[si][n].Empty())
            Report += GetTextExpandedBlock(sp, n) + " // Expanded block (" + Algorithm::GetMetricValue(GetMetricExpandedBlock(sp, n)) + ")\n";

		C.ApplyAlgorithm(EO[si][n]);
		if (!IsPetrusExpandedBlockBuilt(C) || !C.CheckOrientation(Pgr::ALL_EDGES))
		{
			Report += "Edges not oriented!\n";
			return Report;
		}
        if (!EO[si][n].Empty()) Report += GetTextEO(sp, n) + " // EO (" + Algorithm::GetMetricValue(GetMetricEO(sp, n)) + ")\n";

		C.ApplyAlgorithm(F2L[si][n]);
		if (!Method::IsF2LBuilt(C, Cube::GetDownSliceLayer(C.GetSpin())))
		{
			Report += "F2L not built!\n";
			return Report;
		}
        if (!F2L[si][n].Empty()) Report += GetTextF2L(sp, n) + " // F2L (" + Algorithm::GetMetricValue(GetMetricF2L(sp, n)) + ")\n";

		if (!AlgZBLL[si].empty())
		{
            // C.ApplyAlgorithm(AlgZBLL[si][n]);
            if (!AlgZBLL[si][n].Empty()) Report += GetTextZBLL(sp, n) + " // ZBLL (" + Algorithm::GetMetricValue(GetMetricZBLL(sp, n)) + ")\n";
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
	bool Petrus::CheckSolveConsistency(const Spn Spin) const
	{
		const int si = static_cast<int>(Spin); // Spin index

		if (Inspections[si].empty()) return false;

		auto n = Inspections[si].size();

		return (Blocks[si].size() == n) && (ExpandedBlocks[si].size() == n) && (EO[si].size() == n) && (F2L[si].size() == n) &&
			   ((AlgZBLL[si].size() == n) || 
			   ((AlgOCLL[si].size() == n) && AlgPLL[si].size() == n) || 
			   ((AlgCOLL[si].size() == n) && AlgEPLL[si].size() == n));
	}

	// Get the full solve
    Algorithm Petrus::GetFullSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || n >= Inspections[si].size()) return 0u;
		
		Algorithm A = Inspections[si][n];
		A += Blocks[si][n];
		A += ExpandedBlocks[si][n];
		A += EO[si][n];
		A += F2L[si][n];
		if (!AlgZBLL[si].empty()) A += AlgZBLL[si][n];
		if (!AlgOCLL[si].empty()) A += AlgOCLL[si][n];
		if (!AlgPLL[si].empty()) A += AlgPLL[si][n];
		if (!AlgCOLL[si].empty()) A += AlgCOLL[si][n];
		if (!AlgEPLL[si].empty()) A += AlgEPLL[si][n];
		
		return A;
	}
}
