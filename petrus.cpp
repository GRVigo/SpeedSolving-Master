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
#include "cfop.h"

#include <chrono>
#include <algorithm>

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

        DeepSearch DSBlock(Scramble); // Deep search for first block
		
        DSBlock.AddToOptionalPieces(Pgr::PETRUS_UFR);
        DSBlock.AddToOptionalPieces(Pgr::PETRUS_UFL);
        DSBlock.AddToOptionalPieces(Pgr::PETRUS_UBR);
        DSBlock.AddToOptionalPieces(Pgr::PETRUS_UBL);

        DSBlock.AddToOptionalPieces(Pgr::PETRUS_DFR);
        DSBlock.AddToOptionalPieces(Pgr::PETRUS_DFL);
        DSBlock.AddToOptionalPieces(Pgr::PETRUS_DBR);
        DSBlock.AddToOptionalPieces(Pgr::PETRUS_DBL);

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
        const SearchUnit URoot(SequenceType::DOUBLE);
        const SearchUnit U(SequenceType::SINGLE);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(URoot);
		
        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U);

        DSBlock.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        for (uint l = 2u; l < MaxDepthBlock; l++) DSBlock.AddSearchLevel(L_Check);

        DSBlock.UpdateRootData();
		// DSFB.SetMinDeep(DSFB.GetMaxDeep() - 2u);

        DSBlock.Run(Cores);

        Cores = DSBlock.GetCoresUsed(); // Update to the real number of cores used

        EvaluateBlock(DSBlock.Solves, MaxSolves);

        const std::chrono::duration<double> block_elapsed_seconds = std::chrono::system_clock::now() - time_block_start;
        TimeBlock = block_elapsed_seconds.count();

        return !DSBlock.Solves.empty();
	}
	
	// Search the best block solve algorithms from an algorithms vector
	void Petrus::EvaluateBlock(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best block solve algorithms for each corner
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);
            EvaluateBlockResult(Blocks[sp], MaxSolves, Solves, CubeBase, spin, Plc::BEST_SOLVES);
		}

		for (int sp = 0; sp < 24; sp++)
		{
			Spn spin = static_cast<Spn>(sp);
			Stp T1, T2;
			Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
			Algorithm Insp;
			if (T1 != Stp::NONE) Insp.Append(T1);
			if (T2 != Stp::NONE) Insp.Append(T2);

			for (auto& block : Blocks[sp])
			{
				if (T1 != Stp::NONE) block.TransformTurn(T1);
				if (T2 != Stp::NONE) block.TransformTurn(T2);
				Inspections[sp].push_back(Insp);
			}
		}
	}

    // Search the best expanded block solve algorithm
    void Petrus::SearchExpandedBlock()
    {
        const auto time_eb_start = std::chrono::system_clock::now();

		const SearchUnit U_Root(SequenceType::DOUBLE, Sst::PETRUS_EB);
		const SearchUnit U_EB(SequenceType::SINGLE, Sst::PETRUS_EB);

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
			}

			uint n = 0u;
			for (const auto& Block : Blocks[sp])
			{
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(Block);

				Cube C(AlgStart);

				if (!IsBlockBuilt(C) || IsExpandedBlockBuilt(C)) // Block not solved or expanded block already solved
				{
					ExpandedBlocks[sp].push_back(Algorithm(""));
					continue;
				}

				DeepSearch DSEB(AlgStart, Plc::SHORT); // Deep search for expanded block

				DSEB.AddToMandatoryPieces(EXPBLOCK);

				DSEB.AddSearchLevel(L_Root);
				for (int l = 1; l < 7; l++) DSEB.AddSearchLevel(L_Check); // Add needed search levels

				DSEB.UpdateRootData();

				DSEB.Run(Cores);
				
				Cores = DSEB.GetCoresUsed(); // Update to the real number of cores used

				Algorithm Solve;

				DSEB.EvaluateShortestResult(Solve, true);

				C.ApplyAlgorithm(Solve);

				if (IsExpandedBlockBuilt(C)) ExpandedBlocks[sp].push_back(Solve);
				else ExpandedBlocks[sp].push_back(Algorithm(""));
			}
			n++;
        }
        
        const std::chrono::duration<double> eb_elapsed_seconds = std::chrono::system_clock::now() - time_eb_start;
        TimeExpBlock = eb_elapsed_seconds.count();
    }
	
    // Orientate the other edges outside the block
    void Petrus::SearchEO()
    {
        const auto time_EO_start = std::chrono::system_clock::now();
        
		const SearchUnit U_UR(SequenceType::SINGLE, Sst::SINGLE_UR);
        const SearchUnit U_OR1(SequenceType::RETURN_FIXED_SINGLE, Sst::SINGLE_U, Sst::PETRUS_OR_U);
		const SearchUnit U_OR2(SequenceType::RETURN_FIXED_SINGLE, Sst::SINGLE_R, Sst::PETRUS_OR_R);

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
			}

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(Blocks[sp][n]);
				AlgStart.Append(ExpandedBlocks[sp][n]);
				Cube CubePetrus(AlgStart);

				if (!IsExpandedBlockBuilt(CubePetrus) || CubePetrus.EO()) // Edges already oriented or not expanded block
				{
					EO[sp].push_back(Algorithm(""));
					continue;
				}

				DeepSearch DSEO(AlgStart/*, Plc::SHORT */); // Deep search for edges orientation

				DSEO.AddToMandatoryPieces(ExpBlock);
				DSEO.AddToMandatoryPieces(EO_Edge);
				DSEO.AddToMandatoryOrientations(Pgr::ALL_EDGES);

				for (int l = 0; l < 4; l++) DSEO.AddSearchLevel(L_Check); // Add needed search levels

				DSEO.UpdateRootData();

				DSEO.Run(Cores);

				std::vector<Algorithm> Solves;
				CFOP::EvaluateF2LResult(Solves, 1u, DSEO.Solves, CubePetrus, Cube::GetDownSliceLayer(spin), Plc::BEST_SOLVES);

				if (Solves.empty()) EO[sp].push_back(Algorithm(""));
				else
				{
					CubePetrus.ApplyAlgorithm(Solves[0]);
					if (CubePetrus.EO()) EO[sp].push_back(Solves[0]);
					else EO[sp].push_back(Algorithm(""));
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

		MaxDepthF2L = (MaxDepth <= 4u ? 4u : MaxDepth);

		const SearchUnit U_Root(SequenceType::DOUBLE, Sst::SINGLE_UR);
		const SearchUnit U_UR(SequenceType::SINGLE, Sst::SINGLE_UR);

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
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(Blocks[sp][n]);
				AlgStart.Append(ExpandedBlocks[sp][n]);
				AlgStart.Append(EO[sp][n]);
				const Cube C(AlgStart);

				if (C.EO() && C.IsSolved(DownLayer) && C.IsSolved(MiddleLayer))
				{
					F2L[sp].push_back(Algorithm(""));
					continue; // F2L already solved
				}

				DeepSearch DSF2L(AlgStart, Plc::SHORT); // Deep search for F2L

				DSF2L.AddToMandatoryPieces(LDOWN);
				DSF2L.AddToMandatoryPieces(LMID);
				DSF2L.AddToMandatoryOrientations(CUP);

				DSF2L.AddSearchLevel(L_Root);
				for (uint l = 2; l < MaxDepthF2L; l++) DSF2L.AddSearchLevel(L_Check); // Add needed search levels

				DSF2L.UpdateRootData();

				DSF2L.Run(Cores);

				F2L[sp].push_back(Algorithm(""));
				DSF2L.EvaluateShortestResult(F2L[sp][n], true);
			}
		}

		const std::chrono::duration<double> F2L_elapsed_seconds = std::chrono::system_clock::now() - time_F2L_start;
		TimeF2L = F2L_elapsed_seconds.count();
	}
	
	// Complete the two first layers (F2L) - Alternative version (don't use it, slower and debug needed)
	void Petrus::SearchF2L_Alt(const uint MaxDepth)
	{
		const auto time_F2L_start = std::chrono::system_clock::now();

		MaxDepthF2L = (MaxDepth <= 4u ? 4u : MaxDepth);

		bool F2L_Found = false; // First layer found flag

		Algorithm F2L_Aux, // Auxiliary algorithm for F2L search
			A1_F2L, A2_F2L; // Algorithms for F2L

		std::vector<Algorithm> F2L_Algs;

		const SearchUnit U_Up(SequenceType::SINGLE, Sst::SINGLE_U);
		const SearchUnit U_Lat(SequenceType::RETURN_FIXED_SINGLE, Sst::SINGLE_U, Sst::PETRUS_UD_R);

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
			}

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
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

					DeepSearch DS_F2L(F2L_Aux, Plc::SHORT);

					for (uint l = 0; l < MaxDepthF2L - 1u; l++) DS_F2L.AddSearchLevel(L_F2L_Check);
					DS_F2L.AddSearchLevel(L_F2L_End);
					DS_F2L.UpdateRootData();

					DS_F2L.AddToMandatoryPieces(ExpBlock);
					DS_F2L.AddToMandatoryPieces(F2L_Edge);
					DS_F2L.AddToMandatoryOrientations(LL_Cross);

					if (F2L_Pair_1_IsSolved) DS_F2L.AddToMandatoryPieces(F2L_Pair_1);
					else DS_F2L.AddToOptionalPieces(F2L_Pair_1);

					if (F2L_Pair_2_IsSolved) DS_F2L.AddToMandatoryPieces(F2L_Pair_2);
					else DS_F2L.AddToOptionalPieces(F2L_Pair_2);

					DS_F2L.Run(Cores);

					DS_F2L.EvaluateShortestResult(F2L_Aux, true);

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

				} while (F2L_Found);

				F2L[sp].push_back(Algorithm(""));

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
    void Petrus::SearchZBLL()
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

                Cube::SolveLL(AlgZBLL[sp][n], CasesZBLL[sp][n], AUFStep, AlgSets::ZBLL, CubeZBLL);

				AlgZBLL[sp][n].Append(AUFStep);
			}
		}
		
        const std::chrono::duration<double> ZBLL_elapsed_seconds = std::chrono::system_clock::now() - time_ZBLL_start;
        TimeZBLL = ZBLL_elapsed_seconds.count();
	}

    // OCLL search
    void Petrus::SearchOCLL()
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

                Cube::OrientateLL(AlgOCLL[sp][n], CasesOCLL[sp][n], AlgSets::OCLL, CubeF2L);
			}
		}

        const std::chrono::duration<double> OCLL_elapsed_seconds = std::chrono::system_clock::now() - time_OCLL_start;
        TimeOCLL = OCLL_elapsed_seconds.count();
    }

    // PLL search
    void Petrus::SearchPLL()
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

                Cube::SolveLL(AlgPLL[sp][n], CasesPLL[sp][n], AUFStep, AlgSets::PLL, CubeOCLL);

				AlgPLL[sp][n].Append(AUFStep);
			}
		}

        const std::chrono::duration<double> PLL_elapsed_seconds = std::chrono::system_clock::now() - time_PLL_start;
        TimePLL = PLL_elapsed_seconds.count();
    }

    // COLL search
    void Petrus::SearchCOLL()
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

                Cube::CornersLL(AlgCOLL[sp][n], CasesCOLL[sp][n], LastStep, AlgSets::COLL, CubeF2L);

                AlgCOLL[sp][n].Append(LastStep);
			}
		}

        const std::chrono::duration<double> COLL_elapsed_seconds = std::chrono::system_clock::now() - time_COLL_start;
        TimeCOLL = COLL_elapsed_seconds.count();
    }

    // EPLL search
    void Petrus::SearchEPLL()
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

                Cube::SolveLL(AlgEPLL[sp][n], CasesEPLL[sp][n], AUFStep, AlgSets::EPLL, CubeCOLL);

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
	
	// Check if Petrus block is built
	bool Petrus::IsBlockBuilt(const Cube& C)
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
	bool Petrus::IsBlockBuilt(const Cube& C, const Spn sp)
	{
		switch (sp)
		{
		case Spn::UF:
		case Spn::FR:
		case Spn::RU:	return C.IsSolved(Pgr::PETRUS_DBL);
		
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

	// Check if Petrus expanded block is built
	bool Petrus::IsExpandedBlockBuilt(const Cube& C)
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
	bool Petrus::IsExpandedBlockBuilt(const Cube& C, const Spn spin)
	{
		switch (spin)
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
	
	// Returns best block solve from the Solves vector class member for the given corner position block (Petrus)
	bool Petrus::EvaluateBlockResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn spin, const Plc Policy)
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
        case Plc::BEST_SOLVES:
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
			
			if (!IsBlockBuilt(CheckCube, spin)) continue;

			Score += (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
			Score += s.GetSubjectiveScore();
			
			if (IsExpandedBlockBuilt(CheckCube, spin)) Score += SolveExpandedBlockMagnitude;
			
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

    // Get a solve report
    std::string Petrus::GetReport(const bool cancellations, const bool debug) const
    {
        std::string Report = "Petrus search for Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble(), ReportLine;
        Report += "\n--------------------------------------------------------------------------------\n";

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

				if (IsBlockBuilt(C))
				{
					ReportLine += "[" + C.GetSpinText() + "|" + Algorithm::GetMetricValue(GetMetricSolve(C.GetSpin(), n));
					if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
					ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
					if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
					ReportLine += "(" + Blocks[sp][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
						ReportLine += "[" + C.GetSpinText() + "]: ";
                        if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += "Block not built in " + std::to_string(MaxDepthBlock) + " steps";
                        if (!Blocks[sp][n].Empty()) ReportLine += ": (" + Blocks[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						Report += ReportLine;
					}
					continue;
				}

				C.ApplyAlgorithm(ExpandedBlocks[sp][n]);

				if (IsExpandedBlockBuilt(C))
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

				if (IsExpandedBlockBuilt(C) && C.EO())
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
					C.EO())
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

        Report += "Total search time: " + std::to_string(GetTime()) + " s\n";
        Report += "Blocks search time: " + std::to_string(GetTimeBlock()) + " s\n";
        Report += "Expanded blocks search time: " + std::to_string(GetTimeExpBlock()) + " s\n";
        Report += "Edges orientation search time: " + std::to_string(GetTimeEO()) + " s\n";
        Report += "F2L search time: " + std::to_string(GetTimeF2L()) + " s\n";
        Report += "Last layer search time: " + std::to_string(GetTimeLL()) + " s\n";
        Report += "Threads used: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : 0) +
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

        std::string Report = "Petrus search with orientation " + Cube::GetSpinText(sp);
        Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
		C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += "Inspection [" + C.GetSpinText() + "]: " + Inspections[si][n].ToString() + "\n\n";

		C.ApplyAlgorithm(Blocks[si][n]);
		if (!IsBlockBuilt(C))
		{
			Report += "Block not built in " + std::to_string(MaxDepthBlock) + " movements\n";
			return Report;
		}
        Report += "Block (" + Algorithm::GetMetricValue(GetMetricBlock(sp, n)) + "): " + Blocks[si][n].ToString() + "\n";

		C.ApplyAlgorithm(ExpandedBlocks[si][n]);
		if (!IsExpandedBlockBuilt(C))
		{
			Report += "Expanded block not built!\n";
			return Report;
		}
        if (!ExpandedBlocks[si][n].Empty()) Report += "Expanded block (" + Algorithm::GetMetricValue(GetMetricExpandedBlock(sp, n)) + "): " +
                                                      ExpandedBlocks[si][n].ToString() + "\n";

		C.ApplyAlgorithm(EO[si][n]);
		if (!IsExpandedBlockBuilt(C) || !C.EO())
		{
			Report += "Edges not oriented!\n";
			return Report;
		}
        if (!EO[si][n].Empty()) Report += "EO (" + Algorithm::GetMetricValue(GetMetricEO(sp, n)) + "): " + EO[si][n].ToString() + "\n";

		C.ApplyAlgorithm(F2L[si][n]);
		if (!CFOP::IsF2LBuilt(C, Cube::GetDownSliceLayer(C.GetSpin())))
		{
			Report += "F2L not built!\n";
			return Report;
		}
        if (!F2L[si][n].Empty()) Report += "F2L (" + Algorithm::GetMetricValue(GetMetricF2L(sp, n)) + "): " + F2L[si][n].ToString() + "\n";

		if (!AlgZBLL[si].empty())
		{
			C.ApplyAlgorithm(AlgZBLL[si][n]);
            if (!AlgZBLL[si][n].Empty()) Report += "ZBLL (" + Algorithm::GetMetricValue(GetMetricZBLL(sp, n)) + "): " + AlgZBLL[si][n].ToString() + "\n";
		}
		else if (!AlgOCLL[si].empty() && !AlgPLL[si].empty())
		{
			C.ApplyAlgorithm(AlgOCLL[si][n]);
            if (!AlgOCLL[si][n].Empty()) Report += "OCLL (" + Algorithm::GetMetricValue(GetMetricOCLL(sp, n)) + "): " + AlgOCLL[si][n].ToString() + "\n";

			C.ApplyAlgorithm(AlgPLL[si][n]);
            if (!AlgPLL[si][n].Empty()) Report += "PLL (" + Algorithm::GetMetricValue(GetMetricPLL(sp, n)) + "): " + AlgPLL[si][n].ToString() + "\n";
		}
		else if (!AlgCOLL[si].empty() && !AlgEPLL[si].empty())
		{
			C.ApplyAlgorithm(AlgCOLL[si][n]);
            if (!AlgCOLL[si][n].Empty()) Report += "COLL (" + Algorithm::GetMetricValue(GetMetricCOLL(sp, n)) + "): " + AlgCOLL[si][n].ToString() + "\n";

			C.ApplyAlgorithm(AlgEPLL[si][n]);
            if (!AlgEPLL[si][n].Empty()) Report += "EPLL (" + Algorithm::GetMetricValue(GetMetricEPLL(sp, n)) + "): " + AlgEPLL[si][n].ToString() + "\n";
		}
		else
		{
			Report += "Last layer not solved!";
			return Report;
		}

        // Show summary
        Report += "\nSolve metric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
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

    // Check if in the given spin the solve is OK
    bool Petrus::IsSolved(const Spn CurrentSpin, const uint n) const
    {
        const int Si = static_cast<int>(CurrentSpin); // Spin index
        Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[Si][n]);
        C.ApplyAlgorithm(Blocks[Si][n]);
        C.ApplyAlgorithm(ExpandedBlocks[Si][n]);
        C.ApplyAlgorithm(EO[Si][n]);
        C.ApplyAlgorithm(F2L[Si][n]);
        if (!AlgZBLL[Si].empty()) C.ApplyAlgorithm(AlgZBLL[Si][n]);
		if (!AlgOCLL[Si].empty()) C.ApplyAlgorithm(AlgOCLL[Si][n]);
		if (!AlgPLL[Si].empty()) C.ApplyAlgorithm(AlgPLL[Si][n]);
		if (!AlgCOLL[Si].empty()) C.ApplyAlgorithm(AlgCOLL[Si][n]);
		if (!AlgEPLL[Si].empty()) C.ApplyAlgorithm(AlgEPLL[Si][n]);
        return C.IsSolved();
    }

	// Get the solve metric
    float Petrus::GetMetricSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index
		
		Algorithm A = Blocks[si][n];
		A += ExpandedBlocks[si][n];
		A += EO[si][n];
		A += F2L[si][n];
		if (!AlgZBLL[si].empty()) A += AlgZBLL[si][n];
		if (!AlgOCLL[si].empty()) A += AlgOCLL[si][n];
		if (!AlgPLL[si].empty()) A += AlgPLL[si][n];
		if (!AlgCOLL[si].empty()) A += AlgCOLL[si][n];
		if (!AlgEPLL[si].empty()) A += AlgEPLL[si][n];
		
		return A.GetMetric(Metric);
	}

	// Get the full solve with cancellations
	Algorithm Petrus::GetCancellations(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || Blocks[si].size() <= n)
		{
			std::string FReport = "No solve with cancellations for spin ";
			FReport += Cube::GetSpinText(spin);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

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

		return A.GetCancellations();
	}

    // Get the best solve report
    std::string Petrus::GetBestReport(const bool Cancellations) const
    {
		float M, min_M = 0.0f;
		uint Bestn;
		Spn BestSpin;

		for (int sp = 0; sp < 24; sp++)
		{
			const Spn Spin = static_cast<Spn>(sp);

			if (!CheckSolveConsistency(Spin)) continue;

			for (uint n = 0u; n < Blocks[sp].size(); n++)
			{
				if (!IsSolved(Spin, n)) continue;

				if (Cancellations) M = GetMetricCancellations(Spin, n);
                else M = GetMetricSolve(Spin, n);
				
				if (min_M == 0.0f || M < min_M)
				{
					min_M = M;
					BestSpin = Spin;
					Bestn = n;
				}
			}
		}
		if (min_M == 0.0f) return "No Petrus solves!\n";

		if (Cancellations)
		{
			Algorithm C = GetCancellations(BestSpin, Bestn);
			return GetReport(BestSpin, Bestn) + "\nCancellations (" + Algorithm::GetMetricValue(C.GetMetric(Metric)) + " " + 
			       Algorithm::GetMetricString(Metric) + "): " + C.ToString() + "\n";
		}

		return GetReport(BestSpin, Bestn);
    }
}
