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

#include "leor.h"
#include "collection.h"

namespace grcube3
{	
	// Reset the search results
	void LEOR::Reset()
	{
        for (int i = 0; i < 24; i++)
        {
			Inspections[i].clear();
            AlgFBA[i].clear();
            AlgEOStripe[i].clear();
			AlgSB[i].clear();
            AlgZBLL[i].clear();
			AlgOCLL[i].clear();
            AlgPLL[i].clear();
            AlgCOLL[i].clear();
            AlgEPLL[i].clear();
			AlgFBB[i].clear();
            AlgFLPair[i].clear();
			
            CasesZBLL[i].clear();
			CasesOCLL[i].clear();
            CasesPLL[i].clear();
            CasesCOLL[i].clear();
            CasesEPLL[i].clear();
        }
		 
        MaxDepthFB = MaxDepthSB = MaxDepthEOStripe = MaxDepthFLPair = 0u;

		TimeFB = TimeEOStripe = TimeSB = TimeZBLL = TimeOCLL = TimePLL = TimeCOLL = TimeEPLL = TimeFLPair = 0.0;
		
		Metric = Metrics::Movements; // Default metric
	}

    // Search the best LEOR-A first blocks solve algorithms with the given search depth
	// Return false if no first blocks found
    bool LEOR::SearchFBA(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_fb_start = std::chrono::system_clock::now();

        MaxDepthFB = (MaxDepth <= 4u ? 4u : MaxDepth);

        // Deep search for first block
		DS.Clear();
        DS.SetScramble(Scramble);
		
        DS.AddToOptionalPieces(Pgr::UF_B1);
        DS.AddToOptionalPieces(Pgr::UB_B1);
        DS.AddToOptionalPieces(Pgr::UR_B1);
        DS.AddToOptionalPieces(Pgr::UL_B1);

        DS.AddToOptionalPieces(Pgr::DF_B1);
        DS.AddToOptionalPieces(Pgr::DB_B1);
        DS.AddToOptionalPieces(Pgr::DR_B1);
        DS.AddToOptionalPieces(Pgr::DL_B1);

        DS.AddToOptionalPieces(Pgr::FU_B1);
        DS.AddToOptionalPieces(Pgr::FD_B1);
        DS.AddToOptionalPieces(Pgr::FR_B1);
        DS.AddToOptionalPieces(Pgr::FL_B1);

        DS.AddToOptionalPieces(Pgr::BU_B1);
        DS.AddToOptionalPieces(Pgr::BD_B1);
        DS.AddToOptionalPieces(Pgr::BR_B1);
        DS.AddToOptionalPieces(Pgr::BL_B1);

        DS.AddToOptionalPieces(Pgr::RU_B1);
        DS.AddToOptionalPieces(Pgr::RD_B1);
        DS.AddToOptionalPieces(Pgr::RF_B1);
        DS.AddToOptionalPieces(Pgr::RB_B1);

        DS.AddToOptionalPieces(Pgr::LU_B1);
        DS.AddToOptionalPieces(Pgr::LD_B1);
        DS.AddToOptionalPieces(Pgr::LF_B1);
        DS.AddToOptionalPieces(Pgr::LB_B1);

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
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
        DS.AddSearchLevel(L_Check); // Level 3
        for (uint l = 4u; l < MaxDepthFB; l++) DS.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

        DS.UpdateRootData();
		// DS.SetMinDepth(DS.GetMaxDepth() - 2u);

        DS.Run(Cores);

        Cores = DS.GetUsedCores(); // Update to the real number of cores used

        EvaluateFBA(DS.GetSolves(), MaxSolves);

        const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
        TimeFB = fb_elapsed_seconds.count();

        return !DS.GetSolves().empty();
	}
	
	// Search the best LEOR-A first blocks solve algorithms from an algorithms vector
    void LEOR::EvaluateFBA(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best first block solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			Stp T1, T2;
			Algorithm Insp;

			EvaluateNautilusFBResult(AlgFBA[sp], MaxSolves, Solves, CubeBase, spin, Plc::BEST);

			Inspections[sp].clear();

			if (!AlgFBA[sp].empty())
			{
				Insp.Clear();
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				if (T1 != Stp::NONE) Insp.Append(T1);
				if (T2 != Stp::NONE) Insp.Append(T2);

				for (auto& fb : AlgFBA[sp])
				{
					if (T1 != Stp::NONE) fb.TransformTurn(T1);
					if (T2 != Stp::NONE) fb.TransformTurn(T2);
					Inspections[sp].push_back(Insp);
				}
			}
        }
	}

	// Orient all edges while simultaneously solving DF and DB
	void LEOR::SearchEOStripe(const uint MaxDepth)
	{
		const auto time_EO_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthEOStripe = (MaxDepth <= 4u ? 4u : MaxDepth);
        
        const SearchUnit U_SB(SequenceTypes::SINGLE, Sst::NAUTILUS_rRUM);
        const SearchUnit U_SB_Seq(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_R, Sst::LATERAL_SINGLE_FBp);

		SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_SB);
		L_Check.Add(U_SB_Seq);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Pgr ExpBlock;

			switch (spin)
			{
			case Spn::UF: ExpBlock = Pgr::PETRUS_DBL_Z; break;
			case Spn::UB: ExpBlock = Pgr::PETRUS_DFR_Z; break;
			case Spn::UR: ExpBlock = Pgr::PETRUS_DFL_X; break;
			case Spn::UL: ExpBlock = Pgr::PETRUS_DBR_X; break;

			case Spn::DF: ExpBlock = Pgr::PETRUS_UBR_Z; break;
			case Spn::DB: ExpBlock = Pgr::PETRUS_UFL_Z; break;
			case Spn::DR: ExpBlock = Pgr::PETRUS_UBL_X; break;
			case Spn::DL: ExpBlock = Pgr::PETRUS_UFR_X; break;

			case Spn::FU: ExpBlock = Pgr::PETRUS_DBR_Y; break;
			case Spn::FD: ExpBlock = Pgr::PETRUS_UBL_Y; break;
			case Spn::FR: ExpBlock = Pgr::PETRUS_DBL_X; break;
			case Spn::FL: ExpBlock = Pgr::PETRUS_UBR_X; break;

			case Spn::BU: ExpBlock = Pgr::PETRUS_DFL_Y; break;
			case Spn::BD: ExpBlock = Pgr::PETRUS_UFR_Y; break;
			case Spn::BR: ExpBlock = Pgr::PETRUS_UFL_X; break;
			case Spn::BL: ExpBlock = Pgr::PETRUS_DFR_X; break;

			case Spn::RU: ExpBlock = Pgr::PETRUS_DBL_Y; break;
			case Spn::RD: ExpBlock = Pgr::PETRUS_UFL_Y; break;
			case Spn::RF: ExpBlock = Pgr::PETRUS_UBL_Z; break;
			case Spn::RB: ExpBlock = Pgr::PETRUS_DFL_Z; break;

			case Spn::LU: ExpBlock = Pgr::PETRUS_DFR_Y; break;
			case Spn::LD: ExpBlock = Pgr::PETRUS_UBR_Y; break;
			case Spn::LF: ExpBlock = Pgr::PETRUS_DBR_Z; break;
			case Spn::LB: ExpBlock = Pgr::PETRUS_UFR_Z; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgEOStripe[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!AlgFBA[sp].empty()) AlgStart.Append(AlgFBA[sp][n]);
				else if (!AlgFBB[sp].empty()) 
				{
					AlgStart.Append(AlgFBB[sp][n]);
					AlgStart.Append(AlgFLPair[sp][n]);
				}
				Cube CubeLEOR(AlgStart);

				if (!IsRouxFBBuilt(CubeLEOR)) continue;

				// Deep search for edges orientation
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(ExpBlock);
				DS.AddToMandatoryOrientations(Pgr::ALL_EDGES);

				for (uint l = 0u; l < MaxDepthEOStripe; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				DS.EvaluateShortestResult(AlgEOStripe[sp][n], true);
			}
		}
        
        const std::chrono::duration<double> EO_elapsed_seconds = std::chrono::system_clock::now() - time_EO_start;
        TimeEOStripe = EO_elapsed_seconds.count();
	}

	// Search the second block
	void LEOR::SearchSB()
	{
		const auto time_sb_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthSB = 12u;

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

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgSB[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);

				if (!AlgFBA[sp].empty())
				{
					AlgStart += AlgFBA[sp][n];
					AlgStart += AlgEOStripe[sp][n];
				}
				else if (!AlgFBB[sp].empty())
				{
					AlgStart += AlgFBB[sp][n];
					AlgStart += AlgFLPair[sp][n];
					AlgStart += AlgEOStripe[sp][n];
				}

				const Cube C(AlgStart);

				if (C.CheckOrientation(Pgr::ALL_EDGES) && C.IsSolved(DownLayer) && C.IsSolved(MiddleLayer)) continue; // Second block already solved

				// Deep search for second block
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(LDOWN);
				DS.AddToMandatoryPieces(LMID);
				DS.AddToMandatoryOrientations(CUP);

				DS.AddSearchLevel(L_Root);
				for (uint l = 2; l < MaxDepthSB; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				DS.EvaluateShortestResult(AlgSB[sp][n], true);
			}
		}

		const std::chrono::duration<double> sb_elapsed_seconds = std::chrono::system_clock::now() - time_sb_start;
		TimeSB = sb_elapsed_seconds.count();
	}

	// ZBLL search
	void LEOR::SearchZBLL(const Plc Pol)
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

				if (!AlgFBA[sp].empty()) Alg += AlgFBA[sp][n];
				else if (!AlgFBB[sp].empty())
				{
					Alg += AlgFBB[sp][n];
					Alg += AlgFLPair[sp][n];
				}
				else return;
				
				Alg += AlgEOStripe[sp][n];
				Alg += AlgSB[sp][n];

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
    void LEOR::SearchOCLL(const Plc Pol)
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
				
				if (!AlgFBA[sp].empty()) Alg += AlgFBA[sp][n];
				else if (!AlgFBB[sp].empty())
				{
					Alg += AlgFBB[sp][n];
					Alg += AlgFLPair[sp][n];
				}
				else return;
				
				Alg += AlgEOStripe[sp][n];
				Alg += AlgSB[sp][n];

				Cube CubeOCLL = CubeBase;
				CubeOCLL.ApplyAlgorithm(Alg);

                OrientateLL(AlgOCLL[sp][n], CasesOCLL[sp][n], Algset_OCLL, Pol, Metric, CubeOCLL);
			}
		}

        const std::chrono::duration<double> OCLL_elapsed_seconds = std::chrono::system_clock::now() - time_OCLL_start;
        TimeOCLL = OCLL_elapsed_seconds.count();
    }

    // PLL search
    void LEOR::SearchPLL(const Plc Pol)
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
				
				if (!AlgFBA[sp].empty()) Alg += AlgFBA[sp][n];
				else if (!AlgFBB[sp].empty())
				{
					Alg += AlgFBB[sp][n];
					Alg += AlgFLPair[sp][n];
				}
				else return;
				
				Alg += AlgEOStripe[sp][n];
				Alg += AlgSB[sp][n];
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
    void LEOR::SearchCOLL(const Plc Pol)
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
				
				if (!AlgFBA[sp].empty()) Alg += AlgFBA[sp][n];
				else if (!AlgFBB[sp].empty())
				{
					Alg += AlgFBB[sp][n];
					Alg += AlgFLPair[sp][n];
				}
				else return;
				
				Alg += AlgEOStripe[sp][n];
				Alg += AlgSB[sp][n];

				Cube CubeCOLL = CubeBase;
				CubeCOLL.ApplyAlgorithm(Alg);

                Stp LastStep;

                CornersLL(AlgCOLL[sp][n], CasesCOLL[sp][n], LastStep, Algset_COLL, Pol, Metric, CubeCOLL);

                AlgCOLL[sp][n].Append(LastStep);
			}
		}

        const std::chrono::duration<double> COLL_elapsed_seconds = std::chrono::system_clock::now() - time_COLL_start;
        TimeCOLL = COLL_elapsed_seconds.count();
    }

    // EPLL search
    void LEOR::SearchEPLL(const Plc Pol)
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
				
				if (!AlgFBA[sp].empty()) Alg += AlgFBA[sp][n];
				else if (!AlgFBB[sp].empty())
				{
					Alg += AlgFBB[sp][n];
					Alg += AlgFLPair[sp][n];
				}
				else return;
				
				Alg += AlgEOStripe[sp][n];
				Alg += AlgSB[sp][n];
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

	// Search the best LEOR-B first blocks solve algorithms with the given search depth
	// Return false if no first blocks found
	bool LEOR::SearchFBB(const uint MaxDepth, const uint MaxSolves)
	{
		const auto time_fb_start = std::chrono::system_clock::now();

		MaxDepthFB = (MaxDepth <= 4u ? 4u : MaxDepth);

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
		for (uint l = 2u; l < MaxDepthFB; l++) DS.AddSearchLevel(L_Check);

		DS.UpdateRootData();
		// DS.SetMinDepth(DS.GetMaxDepth() - 2u);

		DS.Run(Cores);

        Cores = DS.GetUsedCores(); // Update to the real number of cores used

        EvaluateFBB(DS.GetSolves(), MaxSolves);

		const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
		TimeFB = fb_elapsed_seconds.count();

        return !DS.GetSolves().empty();
	}

	// Search the best LEOR-B first blocks solve algorithms from an algorithms vector
	void LEOR::EvaluateFBB(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best first block solve algorithms adapted to spin
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);
			Stp T1, T2;
			Algorithm Insp;

			EvaluatePetrusBlockResult(AlgFBB[sp], MaxSolves, Solves, CubeBase, spin, Plc::BEST);

			Inspections[sp].clear();

			if (!AlgFBB[sp].empty())
			{
				Insp.Clear();
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				if (T1 != Stp::NONE) Insp.Append(T1);
				if (T2 != Stp::NONE) Insp.Append(T2);

				for (auto& fb : AlgFBB[sp])
				{
					if (T1 != Stp::NONE) fb.TransformTurn(T1);
					if (T2 != Stp::NONE) fb.TransformTurn(T2);
					Inspections[sp].push_back(Insp);
				}
			}
		}
	}

	// Solve the FL pairs (LEOR-B)
	void LEOR::SearchFLPair()
	{
		const auto time_dFL_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        const SearchUnit U_Root(SequenceTypes::DOUBLE, Sst::PETRUS_EB);
        const SearchUnit U_UR(SequenceTypes::SINGLE, Sst::PETRUS_EB);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(U_Root);

		SearchLevel L_Check(SearchCheck::CHECK);
		L_Check.Add(U_UR);

		MaxDepthFLPair = 8u;

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Pgr B1, PAIR;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::PETRUS_DBL; PAIR = Pgr::F2L_D_DFL; break;
			case Spn::UB: B1 = Pgr::PETRUS_DFR; PAIR = Pgr::F2L_D_DBR; break;
			case Spn::UR: B1 = Pgr::PETRUS_DFL; PAIR = Pgr::F2L_D_DFR; break;
			case Spn::UL: B1 = Pgr::PETRUS_DBR; PAIR = Pgr::F2L_D_DBL; break;

			case Spn::DF: B1 = Pgr::PETRUS_UBR; PAIR = Pgr::F2L_U_UFR; break;
			case Spn::DB: B1 = Pgr::PETRUS_UFL; PAIR = Pgr::F2L_U_UBL; break;
			case Spn::DR: B1 = Pgr::PETRUS_UBL; PAIR = Pgr::F2L_U_UBR; break;
			case Spn::DL: B1 = Pgr::PETRUS_UFR; PAIR = Pgr::F2L_U_UFL; break;

			case Spn::FU: B1 = Pgr::PETRUS_DBR; PAIR = Pgr::F2L_B_UBR; break;
			case Spn::FD: B1 = Pgr::PETRUS_UBL; PAIR = Pgr::F2L_B_DBL; break;
			case Spn::FR: B1 = Pgr::PETRUS_DBL; PAIR = Pgr::F2L_B_DBR; break;
			case Spn::FL: B1 = Pgr::PETRUS_UBR; PAIR = Pgr::F2L_B_UBL; break;

			case Spn::BU: B1 = Pgr::PETRUS_DFL; PAIR = Pgr::F2L_F_UFL; break;
			case Spn::BD: B1 = Pgr::PETRUS_UFR; PAIR = Pgr::F2L_F_DFR; break;
			case Spn::BR: B1 = Pgr::PETRUS_UFL; PAIR = Pgr::F2L_F_UFR; break;
			case Spn::BL: B1 = Pgr::PETRUS_DFR; PAIR = Pgr::F2L_F_DFL; break;

			case Spn::RU: B1 = Pgr::PETRUS_DBL; PAIR = Pgr::F2L_L_UBL; break;
			case Spn::RD: B1 = Pgr::PETRUS_UFL; PAIR = Pgr::F2L_L_DFL; break;
			case Spn::RF: B1 = Pgr::PETRUS_UBL; PAIR = Pgr::F2L_L_UFL; break;
			case Spn::RB: B1 = Pgr::PETRUS_DFL; PAIR = Pgr::F2L_L_DBL; break;

			case Spn::LU: B1 = Pgr::PETRUS_DFR; PAIR = Pgr::F2L_R_UFR; break;
			case Spn::LD: B1 = Pgr::PETRUS_UBR; PAIR = Pgr::F2L_R_DBR; break;
			case Spn::LF: B1 = Pgr::PETRUS_DBR; PAIR = Pgr::F2L_R_DFR; break;
			case Spn::LB: B1 = Pgr::PETRUS_UFR; PAIR = Pgr::F2L_R_UBR; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgFLPair[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(AlgFBB[sp][n]);
				const Cube C(AlgStart);

				if (!C.IsSolved(B1) || C.IsSolved(PAIR)) continue; // dFL pair already solved or not block solved

				// Deep search for dFL pair
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(B1);
				DS.AddToMandatoryPieces(PAIR);

				DS.AddSearchLevel(L_Root);
				for (uint l = 2; l < MaxDepthFLPair; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				DS.EvaluateShortestResult(AlgFLPair[sp][n], true); // TODO
			}
		}

		const std::chrono::duration<double> dFL_elapsed_seconds = std::chrono::system_clock::now() - time_dFL_start;
		TimeFLPair = dFL_elapsed_seconds.count();
	}

	// Set regrips
	void LEOR::SetRegrips()
	{
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			if (!CheckSolveConsistency(spin)) continue;

			if (!AlgFBA[sp].empty())
			{
				for (uint n = 0u; n < AlgFBA[sp].size(); n++)
				{
					AlgFBA[sp][n] = AlgFBA[sp][n].GetRegrip();

					if (Algorithm::IsTurn(AlgFBA[sp][n].First()))
					{
						Inspections[sp][n].AppendShrink(AlgFBA[sp][n].First());
						if (Inspections[sp][n].GetSize() == 3u) Inspections[sp][n] = Inspections[sp][n].GetCancellations();
						AlgFBA[sp][n].EraseFirst();
					}
				}
			}
			else if (!AlgFBB[sp].empty())
			{
				for (uint n = 0u; n < AlgFBB[sp].size(); n++)
				{
					AlgFBB[sp][n] = AlgFBB[sp][n].GetRegrip();

					if (Algorithm::IsTurn(AlgFBB[sp][n].First()))
					{
						Inspections[sp][n].AppendShrink(AlgFBB[sp][n].First());
						if (Inspections[sp][n].GetSize() == 3u) Inspections[sp][n] = Inspections[sp][n].GetCancellations();
						AlgFBB[sp][n].EraseFirst();
					}
				}
			}
		}
	}

    // Get a solve report
    std::string LEOR::GetReport(const bool cancellations, bool debug) const
    {
        std::string Report, ReportLine;
        // Report = "LEOR: Scramble [" + std::to_string(Scramble.GetNumSteps()) + "] " + GetTextScramble();
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

				if (!AlgFBA[sp].empty()) // LEOR-A variant
				{
					C.ApplyAlgorithm(AlgFBA[sp][n]);

					if (IsRouxFBBuilt(C))
					{
						ReportLine += "[" + Cube::GetSpinText(spin) + "|" + Algorithm::GetMetricValue(GetMetricSolve(spin, n));
						if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
						ReportLine += " " + Algorithm::GetMetricString(Metric) + "]: ";
						if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += "(" + AlgFBA[sp][n].ToString() + ")";
					}
					else
					{
						if (debug)
						{
							ReportLine += "[" + Cube::GetSpinText(spin) + "]: ";
							if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
							ReportLine += "First block not built in " + std::to_string(MaxDepthFB) + " steps";
							if (!AlgFBA[sp][n].Empty()) ReportLine += ": (" + AlgFBA[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
						}
						if (debug) Report += ReportLine;
						continue;
					}

					C.ApplyAlgorithm(AlgEOStripe[sp][n]);

					if (IsPetrusExpandedBlockBuilt(C) && C.CheckOrientation(Pgr::ALL_EDGES)) ReportLine += " (" + AlgEOStripe[sp][n].ToString() + ")";
					else
					{
						ReportLine += " EO Stripe not built in " + std::to_string(MaxDepthEOStripe) + " steps";
						if (!AlgEOStripe[sp][n].Empty()) ReportLine += ": (" + AlgEOStripe[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}
				}
				else if (!AlgFBB[sp].empty()) // LEOR-B variant
				{
					C.ApplyAlgorithm(AlgFBB[sp][n]);

					if (IsPetrusBlockBuilt(C))
					{
						ReportLine += "[" + Cube::GetSpinText(spin) + "|" + Algorithm::GetMetricValue(GetMetricSolve(spin, n));
						if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
						ReportLine += " " + Algorithm::GetMetricString(Metric) + "]: ";
						if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += "(" + AlgFBB[sp][n].ToString() + ")";
					}
					else
					{
						if (debug)
						{
							ReportLine += "[" + Cube::GetSpinText(spin) + "]: ";
							if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
							ReportLine += "First block not built in " + std::to_string(MaxDepthFB) + " steps";
							if (!AlgFBB[sp][n].Empty()) ReportLine += ": (" + AlgFBB[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
						}
						if (debug) Report += ReportLine;
						continue;
					}

					C.ApplyAlgorithm(AlgFLPair[sp][n]);

					if (IsPetrusBlockBuilt(C) && C.IsSolved(Cube::FromAbsPosition(App::DFL, spin)) && C.IsSolved(Cube::FromAbsPosition(App::FL, spin)))
						ReportLine += " (" + AlgFLPair[sp][n].ToString() + ")";
					else
					{
						ReportLine += " FL Pair not built in " + std::to_string(MaxDepthFLPair) + " steps";
						if (!AlgFLPair[sp][n].Empty()) ReportLine += ": (" + AlgFLPair[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}

					C.ApplyAlgorithm(AlgEOStripe[sp][n]);

					if (IsPetrusExpandedBlockBuilt(C) && C.CheckOrientation(Pgr::ALL_EDGES))
						ReportLine += " (" + AlgEOStripe[sp][n].ToString() + ")";
					else
					{
						ReportLine += " EO DF not built in " + std::to_string(MaxDepthEOStripe) + " steps";
						if (!AlgEOStripe[sp][n].Empty()) ReportLine += ": (" + AlgEOStripe[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}
				}

				C.ApplyAlgorithm(AlgSB[sp][n]);

				if (IsF2LBuilt(C)) ReportLine += " (" + AlgSB[sp][n].ToString() + ")";
				else
				{
					ReportLine += " Second block not built in " + std::to_string(MaxDepthSB) + " steps";
					if (!AlgSB[sp][n].Empty()) ReportLine += ": (" + AlgSB[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
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
    std::string LEOR::GetTimeReport() const
    {
        std::string Report;

        Report += "Total time: " + std::to_string(GetFullTime()) + " s\n";
        Report += "First blocks time: " + std::to_string(GetTimeFB()) + " s\n";
        if (GetTimeFLPair() > 0.0) Report += "FL pair time: " + std::to_string(GetTimeFLPair()) + " s\n";
        if (GetTimeEOStripe() > 0.0) Report += "EO stripe / DF time: " + std::to_string(GetTimeEOStripe()) + " s\n";
        Report += "Second blocks time: " + std::to_string(GetTimeSB()) + " s\n";
        Report += "Last layer time: " + std::to_string(GetTimeLL()) + " s\n";
        Report += "Threads used: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : DeepSearch::GetSystemCores()) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string LEOR::GetReport(const Spn sp, const uint n) const
	{
		const int si = static_cast<int>(sp);

		if (!CheckSolveConsistency(sp) || n >= Inspections[si].size())
		{
			std::string FReport = "No solve for spin ";
			FReport += Cube::GetSpinText(sp);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

        std::string Report;
        // Report += "LEOR search with orientation " + Cube::GetSpinText(sp);
        // Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += GetTextInspection(sp,n) + " // Inspection [" + C.GetSpinText() + "]\n";

		if (!AlgFBA[si].empty()) // LEOR-A variant
		{
			C.ApplyAlgorithm(AlgFBA[si][n]);
			if (!IsRouxFBBuilt(C))
			{
				Report += "First block not built in " + std::to_string(MaxDepthFB) + " movements\n";
				return Report;
			}
            Report += GetTextFBA(sp, n) + " // First block (" + Algorithm::GetMetricValue(GetMetricFBA(sp, n)) + ")\n";

			C.ApplyAlgorithm(AlgEOStripe[si][n]);
			if (!IsPetrusExpandedBlockBuilt(C) || !C.CheckOrientation(Pgr::ALL_EDGES))
			{
				Report += "EO stripe not built in " + std::to_string(MaxDepthEOStripe) + " movements\n";
				return Report;
			}
            if (!AlgEOStripe[si][n].Empty())
                Report += GetTextEOStripe(sp, n) + " // EO stripe (" + Algorithm::GetMetricValue(GetMetricEOStripe(sp,n)) + ")\n";
		}
		else if (!AlgFBB[si].empty()) // LEOR-B variant
		{
			C.ApplyAlgorithm(AlgFBB[si][n]);
			if (!IsPetrusBlockBuilt(C))
			{
				Report += "First block not built in " + std::to_string(MaxDepthFB) + " movements\n";
				return Report;
			}
            Report += GetTextFBB(sp, n) + " // First block (" + Algorithm::GetMetricValue(GetMetricFBB(sp, n)) + ")\n";

			C.ApplyAlgorithm(AlgFLPair[si][n]);
			if (!IsPetrusBlockBuilt(C) || !C.IsSolved(Cube::FromAbsPosition(App::DFL, sp)) || !C.IsSolved(Cube::FromAbsPosition(App::FL, sp)))
			{
				Report += "FL pair not built in " + std::to_string(MaxDepthFLPair) + " movements\n";
				return Report;
			}
            if (!AlgFLPair[si][n].Empty())
                Report += GetTextFLPair(sp, n) + " // FL pair (" + Algorithm::GetMetricValue(GetMetricFLPair(sp,n)) + ")\n";

			C.ApplyAlgorithm(AlgEOStripe[si][n]);
			if (!IsPetrusExpandedBlockBuilt(C) || !C.CheckOrientation(Pgr::ALL_EDGES))
			{
				Report += "EO DF not built in " + std::to_string(MaxDepthEOStripe) + " movements\n";
				return Report;
			}
            if (!AlgEOStripe[si][n].Empty())
                Report += GetTextEOStripe(sp, n) +  " // EO DF (" + Algorithm::GetMetricValue(GetMetricEOStripe(sp,n)) + ")\n";
		}

		C.ApplyAlgorithm(AlgSB[si][n]);
		if (!IsF2LBuilt(C))
		{
			Report += "Second block not built in " + std::to_string(MaxDepthSB) + " movements\n";
			return Report;
		}
        if (!AlgSB[si][n].Empty()) Report += GetTextSB(sp, n) + " // Second block (" + Algorithm::GetMetricValue(GetMetricSB(sp, n)) + ")\n";

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
	bool LEOR::CheckSolveConsistency(const Spn Spin) const
	{
		const int si = static_cast<int>(Spin); // Spin index

		if (Inspections[si].empty()) return false;

		auto n = Inspections[si].size();

		return ((AlgFBA[si].size() == n) || (AlgFBB[si].size() == n && AlgFLPair[si].size() == n)) &&
			   AlgEOStripe[si].size() == n && AlgSB[si].size() == n && 
			   ((AlgZBLL[si].size() == n) || (AlgOCLL[si].size() == n && AlgPLL[si].size() == n) || (AlgCOLL[si].size() == n && AlgEPLL[si].size() == n));
	}

	// Get the full solve
    Algorithm LEOR::GetFullSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || n >= Inspections[si].size()) return 0u;

		Algorithm A = Inspections[si][n];
		if (!AlgFBA[si].empty()) A += AlgFBA[si][n];
		else if (!AlgFBB[si].empty()) 
		{
			A += AlgFBB[si][n];
			if (!AlgFLPair[si].empty()) A += AlgFLPair[si][n];
		}
		if (!AlgEOStripe[si].empty()) A += AlgEOStripe[si][n];
		if (!AlgSB[si].empty()) A += AlgSB[si][n];
		if (!AlgZBLL[si].empty()) A += AlgZBLL[si][n];
		if (!AlgOCLL[si].empty()) A += AlgOCLL[si][n];
		if (!AlgPLL[si].empty()) A += AlgPLL[si][n];
		if (!AlgCOLL[si].empty()) A += AlgCOLL[si][n];
		if (!AlgEPLL[si].empty()) A += AlgEPLL[si][n];
		
		return A;
	}
}
