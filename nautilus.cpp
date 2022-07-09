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

#include "nautilus.h"
#include "collection.h"

namespace grcube3
{	
	// Reset the search results
	void Nautilus::Reset()
	{
        for (int i = 0; i < 24; i++)
        {
			Inspections[i].clear();
            AlgFB[i].clear();
			AlgSB[i].clear();
			AlgdFR[i].clear();
			AlgNCLL[i].clear();
			AlgL5E[i].clear();
			AlgNCOLL[i].clear();
			AlgEODF[i].clear();
			AlgF2L[i].clear();
			AlgZBLL[i].clear();
			AlgOCLL[i].clear();
            AlgPLL[i].clear();
            AlgCOLL[i].clear();
            AlgEPLL[i].clear();

			CasesNCLL[i].clear();
			CasesL5E[i].clear();
			CasesNCOLL[i].clear();
			CasesEODF[i].clear();
			CasesZBLL[i].clear();
			CasesOCLL[i].clear();
            CasesPLL[i].clear();
            CasesCOLL[i].clear();
            CasesEPLL[i].clear();
        }
		 
        MaxDepthFB = MaxDepthSB = MaxDepthdFR = MaxDepthF2L = 0u;
        TimeFB = TimeSB = TimedFR = TimeNCLL = TimeNCOLL = TimeL5E = TimeEODF = TimeF2L = TimeZBLL = TimeOCLL = TimePLL = TimeCOLL = TimeEPLL = 0.0;
		
		Metric = Metrics::Movements; // Default metric
	}

    // Search the best first blocks solve algorithm with the given search depth
	// Return false if no first blocks found
    bool Nautilus::SearchFB(const uint MaxDepth, const uint MaxSolves)
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

        EvaluateFB(DS.GetSolves(), MaxSolves);

        const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
        TimeFB = fb_elapsed_seconds.count();

        return !DS.GetSolves().empty();
	}
	
	// Search the best first block solve algorithms from an algorithms vector
    void Nautilus::EvaluateFB(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best first block solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			Stp T1, T2;
			Algorithm Insp;

			EvaluateNautilusFBResult(AlgFB[sp], MaxSolves, Solves, CubeBase, spin, Plc::BEST);

			Inspections[sp].clear();

			if (!AlgFB[sp].empty())
			{
				Insp.Clear();
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				if (T1 != Stp::NONE) Insp.Append(T1);
				if (T2 != Stp::NONE) Insp.Append(T2);

				for (auto& fb : AlgFB[sp])
				{
					if (T1 != Stp::NONE) fb.TransformTurn(T1);
					if (T2 != Stp::NONE) fb.TransformTurn(T2);
					Inspections[sp].push_back(Insp);
				}
			}
        }
	}

	// Search the best second block solve algorithm with the given search depth
	void Nautilus::SearchSB(const uint MaxDepth)
    {
        const auto time_sb_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        MaxDepthSB = (MaxDepth <= 4u ? 4u : MaxDepth);
		
        const SearchUnit U_SB(SequenceTypes::SINGLE, Sst::NAUTILUS_rRUM);
        const SearchUnit U_SB_Seq(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_R, Sst::LATERAL_SINGLE_FBp);

		SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_SB);
		L_Check.Add(U_SB_Seq);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgSB[sp].clear();

			Pgr B1, B2S;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; B2S = Pgr::UF_B2S2; break;
			case Spn::UB: B1 = Pgr::UB_B1; B2S = Pgr::UB_B2S2; break;
			case Spn::UR: B1 = Pgr::UR_B1; B2S = Pgr::UR_B2S2; break;
			case Spn::UL: B1 = Pgr::UL_B1; B2S = Pgr::UL_B2S2; break;

			case Spn::DF: B1 = Pgr::DF_B1; B2S = Pgr::DF_B2S2; break;
			case Spn::DB: B1 = Pgr::DB_B1; B2S = Pgr::DB_B2S2; break;
			case Spn::DR: B1 = Pgr::DR_B1; B2S = Pgr::DR_B2S2; break;
			case Spn::DL: B1 = Pgr::DL_B1; B2S = Pgr::DL_B2S2; break;

			case Spn::FU: B1 = Pgr::FU_B1; B2S = Pgr::FU_B2S2; break;
			case Spn::FD: B1 = Pgr::FD_B1; B2S = Pgr::FD_B2S2; break;
			case Spn::FR: B1 = Pgr::FR_B1; B2S = Pgr::FR_B2S2; break;
			case Spn::FL: B1 = Pgr::FL_B1; B2S = Pgr::FL_B2S2; break;

			case Spn::BU: B1 = Pgr::BU_B1; B2S = Pgr::BU_B2S2; break;
			case Spn::BD: B1 = Pgr::BD_B1; B2S = Pgr::BD_B2S2; break;
			case Spn::BR: B1 = Pgr::BR_B1; B2S = Pgr::BR_B2S2; break;
			case Spn::BL: B1 = Pgr::BL_B1; B2S = Pgr::BL_B2S2; break;

			case Spn::RU: B1 = Pgr::RU_B1; B2S = Pgr::RU_B2S2; break;
			case Spn::RD: B1 = Pgr::RD_B1; B2S = Pgr::RD_B2S2; break;
			case Spn::RF: B1 = Pgr::RF_B1; B2S = Pgr::RF_B2S2; break;
			case Spn::RB: B1 = Pgr::RB_B1; B2S = Pgr::RB_B2S2; break;

			case Spn::LU: B1 = Pgr::LU_B1; B2S = Pgr::LU_B2S2; break;
			case Spn::LD: B1 = Pgr::LD_B1; B2S = Pgr::LD_B2S2; break;
			case Spn::LF: B1 = Pgr::LF_B1; B2S = Pgr::LF_B2S2; break;
			case Spn::LB: B1 = Pgr::LB_B1; B2S = Pgr::LB_B2S2; break;

			default: return; // Should not happend
			}

			uint n = 0u;
			for (const auto& FB : AlgFB[sp])
			{
				AlgSB[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FB);

				// Deep search for second block
				DS.Clear();
                DS.SetScramble(AlgStart);

				DS.AddToMandatoryPieces(B1);
				DS.AddToMandatoryPieces(B2S);

				for (uint l = 1u; l < MaxDepthSB; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				Cube CubeNautilus(AlgStart);

                Method::EvaluateNautilusSBResult(AlgSB[sp][n], DS.GetSolves(), CubeNautilus, spin, Plc::BEST);
				n++;
			}
        }

        const std::chrono::duration<double> sb_elapsed_seconds = std::chrono::system_clock::now() - time_sb_start;
        TimeSB = sb_elapsed_seconds.count();
    }

	// Add the pair containing the DFR corner and FR edge
	void Nautilus::SearchPairdFR(bool NoCornerOriented)
	{
		const auto time_dFR_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        const SearchUnit U_Root(SequenceTypes::DOUBLE, Sst::NAUTILUS_rRUM);
        const SearchUnit U_UR(SequenceTypes::SINGLE, Sst::NAUTILUS_rRUM);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(U_Root);

		SearchLevel L_Check(SearchCheck::CHECK);
		L_Check.Add(U_UR);

		MaxDepthdFR = 8u;

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Pgr B1, B2, PAIR;
			Pcp dFR_Edge, dFR_Corner;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; B2 = Pgr::PETRUS_DBR; PAIR = Pgr::F2L_D_DFR; break;
			case Spn::UB: B1 = Pgr::UB_B1; B2 = Pgr::PETRUS_DFL; PAIR = Pgr::F2L_D_DBL; break;
			case Spn::UR: B1 = Pgr::UR_B1; B2 = Pgr::PETRUS_DBL; PAIR = Pgr::F2L_D_DBR; break;
			case Spn::UL: B1 = Pgr::UL_B1; B2 = Pgr::PETRUS_DFR; PAIR = Pgr::F2L_D_DFL; break;

			case Spn::DF: B1 = Pgr::DF_B1; B2 = Pgr::PETRUS_UBL; PAIR = Pgr::F2L_U_UFL; break;
			case Spn::DB: B1 = Pgr::DB_B1; B2 = Pgr::PETRUS_UFR; PAIR = Pgr::F2L_U_UBR; break;
			case Spn::DR: B1 = Pgr::DR_B1; B2 = Pgr::PETRUS_UFL; PAIR = Pgr::F2L_U_UFR; break;
			case Spn::DL: B1 = Pgr::DL_B1; B2 = Pgr::PETRUS_UBR; PAIR = Pgr::F2L_U_UBL; break;

			case Spn::FU: B1 = Pgr::FU_B1; B2 = Pgr::PETRUS_DBL; PAIR = Pgr::F2L_B_UBL; break;
			case Spn::FD: B1 = Pgr::FD_B1; B2 = Pgr::PETRUS_UBR; PAIR = Pgr::F2L_B_DBR; break;
			case Spn::FR: B1 = Pgr::FR_B1; B2 = Pgr::PETRUS_UBL; PAIR = Pgr::F2L_B_UBR; break;
			case Spn::FL: B1 = Pgr::FL_B1; B2 = Pgr::PETRUS_DBR; PAIR = Pgr::F2L_B_DBL; break;

			case Spn::BU: B1 = Pgr::BU_B1; B2 = Pgr::PETRUS_DFR; PAIR = Pgr::F2L_F_UFR; break;
			case Spn::BD: B1 = Pgr::BD_B1; B2 = Pgr::PETRUS_UFL; PAIR = Pgr::F2L_F_DFL; break;
			case Spn::BR: B1 = Pgr::BR_B1; B2 = Pgr::PETRUS_DFL; PAIR = Pgr::F2L_F_DFR; break;
			case Spn::BL: B1 = Pgr::BL_B1; B2 = Pgr::PETRUS_UFR; PAIR = Pgr::F2L_F_UFL; break;

			case Spn::RU: B1 = Pgr::RU_B1; B2 = Pgr::PETRUS_DFL; PAIR = Pgr::F2L_L_UFL; break;
			case Spn::RD: B1 = Pgr::RD_B1; B2 = Pgr::PETRUS_UBL; PAIR = Pgr::F2L_L_DBL; break;
			case Spn::RF: B1 = Pgr::RF_B1; B2 = Pgr::PETRUS_DBL; PAIR = Pgr::F2L_L_DFL; break;
			case Spn::RB: B1 = Pgr::RB_B1; B2 = Pgr::PETRUS_UFL; PAIR = Pgr::F2L_L_UBL; break;

			case Spn::LU: B1 = Pgr::LU_B1; B2 = Pgr::PETRUS_DBR; PAIR = Pgr::F2L_R_UBR; break;
			case Spn::LD: B1 = Pgr::LD_B1; B2 = Pgr::PETRUS_UFR; PAIR = Pgr::F2L_R_DFR; break;
			case Spn::LF: B1 = Pgr::LF_B1; B2 = Pgr::PETRUS_UBR; PAIR = Pgr::F2L_R_UFR; break;
			case Spn::LB: B1 = Pgr::LB_B1; B2 = Pgr::PETRUS_DFR; PAIR = Pgr::F2L_R_DBR; break;

			default: return; // Should not happend
			}

			dFR_Edge = Cube::FromAbsPosition(App::FR, spin);
			dFR_Corner = Cube::FromAbsPosition(App::DFR, spin);

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgdFR[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(AlgFB[sp][n]);
				AlgStart.Append(AlgSB[sp][n]);
				const Cube C(AlgStart);

				if (!C.IsSolved(B1) || !C.IsSolved(B2) || C.IsSolved(PAIR)) continue; // dFR pair already solved or not blocks solved

				// Deep search for dFR pair
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(B1);
				DS.AddToMandatoryPieces(B2);
				if (NoCornerOriented)
				{
					DS.AddToMandatoryPieces(dFR_Edge);
					DS.AddToMandatoryPositions(dFR_Corner);
				}
				else DS.AddToMandatoryPieces(PAIR);

				DS.AddSearchLevel(L_Root);
				for (uint l = 2; l < MaxDepthdFR; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				if (NoCornerOriented) DS.EvaluateShortestResult(AlgdFR[sp][n], true);
                else EvaluateNautilusSBResult(AlgdFR[sp][n], DS.GetSolves(), C, spin, Plc::BEST);
			}
		}

		const std::chrono::duration<double> dFR_elapsed_seconds = std::chrono::system_clock::now() - time_dFR_start;
		TimedFR = dFR_elapsed_seconds.count();
	}

	// NCLL search
    void Nautilus::SearchNCLL(const Plc Pol, const bool AddLastUMovement)
	{
        const auto time_NCLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgNCLL[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgNCLL[sp].push_back(Algorithm(""));
                CasesNCLL[sp].push_back("");

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(AlgFB[sp][n]);
				AlgStart.Append(AlgSB[sp][n]);
				AlgStart.Append(AlgdFR[sp][n]);

				const Cube CubeNautilus(AlgStart);

				if (!IsNautilusSBBuilt(CubeNautilus) || !IsF2LPairsBuilt(CubeNautilus)) continue;

				Stp LastUStep;

                CornersLL(AlgNCLL[sp][n], CasesNCLL[sp][n], LastUStep, Algset_NCLL, Pol, Metric, CubeNautilus);

				if (AddLastUMovement) AlgNCLL[sp][n].Append(LastUStep);
            }
		}

        const std::chrono::duration<double> NCLL_elapsed_seconds = std::chrono::system_clock::now() - time_NCLL_start;
        TimeNCLL = NCLL_elapsed_seconds.count();
	}

	// NCOLL search
	void Nautilus::SearchNCOLL(const Plc Pol, const bool AddLastUMovement)
	{
		const auto time_NCOLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgNCOLL[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgNCOLL[sp].push_back(Algorithm(""));
				CasesNCOLL[sp].push_back("");

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(AlgFB[sp][n]);
				AlgStart.Append(AlgSB[sp][n]);
				AlgStart.Append(AlgdFR[sp][n]);

				const Cube CubeNautilus(AlgStart);

				if (!IsNautilusSBBuilt(CubeNautilus) || !IsF2LPairsBuilt(CubeNautilus)) continue;

				Stp LastUStep;

				CornersLL(AlgNCOLL[sp][n], CasesNCOLL[sp][n], LastUStep, Algset_NCOLL, Pol, Metric, CubeNautilus);

				if (AddLastUMovement) AlgNCOLL[sp][n].Append(LastUStep);
			}
		}

		const std::chrono::duration<double> NCOLL_elapsed_seconds = std::chrono::system_clock::now() - time_NCOLL_start;
		TimeNCOLL = NCOLL_elapsed_seconds.count();
	}

	// TNCLL search
	void Nautilus::SearchTNCLL(const Plc Pol, const bool AddLastUMovement)
	{
		const auto time_TNCLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgTNCLL[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgTNCLL[sp].push_back(Algorithm(""));
				CasesTNCLL[sp].push_back("");

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(AlgFB[sp][n]);
				AlgStart.Append(AlgSB[sp][n]);
				AlgStart.Append(AlgdFR[sp][n]);

				const Cube CubeNautilus(AlgStart);

				if (!IsNautilusSBBuilt(CubeNautilus)) continue;

				Stp LastUStep;

				if (IsF2LPairsBuilt(CubeNautilus)) // If last pair is built, use NCLL algorithms
					CornersLL(AlgTNCLL[sp][n], CasesTNCLL[sp][n], LastUStep, Algset_NCLL, Pol, Metric, CubeNautilus);
				else
					CornersLL(AlgTNCLL[sp][n], CasesTNCLL[sp][n], LastUStep, Algset_TNCLL, Pol, Metric, CubeNautilus);

				if (AddLastUMovement) AlgTNCLL[sp][n].Append(LastUStep);
			}
		}

		const std::chrono::duration<double> TNCLL_elapsed_seconds = std::chrono::system_clock::now() - time_TNCLL_start;
		TimeTNCLL = TNCLL_elapsed_seconds.count();
	}

	// Solve the cube by solving the last 5 edges
    void Nautilus::SearchL5E(const Plc Pol)
	{
		const auto time_l5e_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgL5E[sp].clear();
			CasesL5E[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgL5E[sp].push_back(Algorithm(""));
				CasesL5E[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += AlgSB[sp][n];
				Alg += AlgdFR[sp][n];
				if (!AlgNCLL[sp].empty()) Alg += AlgNCLL[sp][n];
				else if (!AlgNCOLL[sp].empty()) Alg += AlgNCOLL[sp][n];
				else if (!AlgTNCLL[sp].empty()) Alg += AlgTNCLL[sp][n];

				Cube CubeL5E = CubeBase;
				CubeL5E.ApplyAlgorithm(Alg);
				
				if (!IsNautilusNCLLBuilt(CubeL5E, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_L5E.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_L5E.GetAlgorithm(Index, Pol, Metric);

					for (const auto U1Mov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto U2Mov : Algorithm::UMovs) // U movement after algorithm
						{
							Cube CubeNautilus = CubeL5E;
							
							if (U1Mov != Stp::NONE) CubeNautilus.ApplyStep(U1Mov);
							CubeNautilus.ApplyAlgorithm(A);
							if (U2Mov != Stp::NONE) CubeNautilus.ApplyStep(U2Mov);

							if (CubeNautilus.IsSolved())
							{
								Found = true;

								CasesL5E[sp][n] = Algset_L5E.GetCaseName(Index);

								if (U1Mov != Stp::NONE)
								{
									AlgL5E[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgL5E[sp][n].Append(U1Mov);
									AlgL5E[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								AlgL5E[sp][n].Append(A);

								if (U2Mov != Stp::NONE)
								{
									AlgL5E[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgL5E[sp][n].Append(U2Mov);
									AlgL5E[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								break;
							}
						}
						if (Found) break; // Release for loop
					}
					if (Found) break; // Release for loop
				}
			}
		}

		const std::chrono::duration<double> l5e_elapsed_seconds = std::chrono::system_clock::now() - time_l5e_start;
		TimeL5E = l5e_elapsed_seconds.count();
	}

	// Orient the remaining six edges while placing the DF edge
	void Nautilus::SearchEODF(const Plc Pol)
	{
		const auto time_eodf_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgEODF[sp].clear();
			CasesEODF[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgEODF[sp].push_back(Algorithm(""));
				CasesEODF[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += AlgSB[sp][n];

				Cube CubeEODF = CubeBase;
				CubeEODF.ApplyAlgorithm(Alg);
				
				if (!IsNautilusSBBuilt(CubeEODF, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_EODF.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_EODF.GetAlgorithm(Index, Pol, Metric);

					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeNautilus = CubeEODF;

						if (UMov != Stp::NONE) CubeNautilus.ApplyStep(UMov);
						CubeNautilus.ApplyAlgorithm(A);

						if (IsNautilusEODFBuilt(CubeNautilus, spin))
						{
							Found = true;

							CasesEODF[sp][n] = Algset_EODF.GetCaseName(Index);

							if (UMov != Stp::NONE)
							{
								AlgEODF[sp][n].Append(Stp::PARENTHESIS_OPEN);
								AlgEODF[sp][n].Append(UMov);
								AlgEODF[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							AlgEODF[sp][n].Append(A);

							break;
						}

					}
					if (Found) break; // Release for loop
				}
			}
		}

		const std::chrono::duration<double> eodf_elapsed_seconds = std::chrono::system_clock::now() - time_eodf_start;
		TimeEODF = eodf_elapsed_seconds.count();
	}

	// Solve the last F2L pair (complete the two first layers)
    void Nautilus::SearchF2L()
	{
		const auto time_F2L_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthF2L = 12u;

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
				AlgF2L[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(AlgFB[sp][n]);
				AlgStart.Append(AlgSB[sp][n]);
				AlgStart.Append(AlgEODF[sp][n]);
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
				for (uint l = 2u; l < MaxDepthF2L; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();
				
				DS.EvaluateShortestResult(AlgF2L[sp][n], true);
			}
		}

		const std::chrono::duration<double> F2L_elapsed_seconds = std::chrono::system_clock::now() - time_F2L_start;
		TimeF2L = F2L_elapsed_seconds.count();
	}

	// ZBLL search to complete the last layer
    void Nautilus::SearchZBLL(const Plc Pol)
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
				Alg += AlgFB[sp][n];
				Alg += AlgSB[sp][n];
				Alg += AlgEODF[sp][n];
				Alg += AlgF2L[sp][n];

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
    void Nautilus::SearchOCLL(const Plc Pol)
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
				Alg += AlgFB[sp][n];
				Alg += AlgSB[sp][n];
				Alg += AlgEODF[sp][n];
				Alg += AlgF2L[sp][n];

				Cube CubeOCLL = CubeBase;
				CubeOCLL.ApplyAlgorithm(Alg);

                OrientateLL(AlgOCLL[sp][n], CasesOCLL[sp][n], Algset_OCLL, Pol, Metric, CubeOCLL);
			}
		}

        const std::chrono::duration<double> OCLL_elapsed_seconds = std::chrono::system_clock::now() - time_OCLL_start;
        TimeOCLL = OCLL_elapsed_seconds.count();
    }

    // PLL search
    void Nautilus::SearchPLL(const Plc Pol)
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
				Alg += AlgFB[sp][n];
				Alg += AlgSB[sp][n];
				Alg += AlgEODF[sp][n];
				Alg += AlgF2L[sp][n];
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
    void Nautilus::SearchCOLL(const Plc Pol)
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
				Alg += AlgFB[sp][n];
				Alg += AlgSB[sp][n];
				Alg += AlgEODF[sp][n];
				Alg += AlgF2L[sp][n];

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
    void Nautilus::SearchEPLL(const Plc Pol)
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
				Alg += AlgFB[sp][n];
				Alg += AlgSB[sp][n];
				Alg += AlgEODF[sp][n];
				Alg += AlgF2L[sp][n];
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
	void Nautilus::SetRegrips()
	{
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			if (!CheckSolveConsistency(spin)) continue;

			for (uint n = 0u; n < AlgFB[sp].size(); n++)
			{
				AlgFB[sp][n] = AlgFB[sp][n].GetRegrip();

				if (Algorithm::IsTurn(AlgFB[sp][n].First()))
				{
					Inspections[sp][n].AppendShrink(AlgFB[sp][n].First());
					if (Inspections[sp][n].GetSize() == 3u) Inspections[sp][n] = Inspections[sp][n].GetCancellations();
					AlgFB[sp][n].EraseFirst();
				}
			}
		}
	}

    // Get a solve report
    std::string Nautilus::GetReport(const bool cancellations, bool debug) const
    {
        std::string Report, ReportLine;
        // Report = "Nautilus: Scramble [" + std::to_string(Scramble.GetNumSteps()) + "] " + GetTextScramble();
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
				C.ApplyAlgorithm(AlgFB[sp][n]);

				if (IsRouxFBBuilt(C))
				{
					ReportLine += "[" + Cube::GetSpinText(spin) + "|" + Algorithm::GetMetricValue(GetMetricSolve(spin, n));
					if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
					ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
					if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
					ReportLine += "(" + AlgFB[sp][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
						ReportLine += "[" + Cube::GetSpinText(spin) + "]: ";
						if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += " First block not built in " + std::to_string(MaxDepthFB) + " steps";
						if (!AlgFB[sp][n].Empty()) ReportLine += ": (" + AlgFB[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
					}
					if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(AlgSB[sp][n]);

				if (IsNautilusSBBuilt(C)) 
				{
					ReportLine += " (" + AlgSB[sp][n].ToString() + ")";
				}
				else
				{
					ReportLine += " Second block not built in " + std::to_string(MaxDepthSB) + " steps";
					if (!AlgSB[sp][n].Empty()) ReportLine += ": (" + AlgSB[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				if (!AlgL5E[sp].empty()) // L5E variant
				{

					C.ApplyAlgorithm(AlgdFR[sp][n]);

					if (IsNautilusSBBuilt(C) && (IsF2LPairsBuilt(C) || !AlgTNCLL[sp].empty()))
					{
						ReportLine += " (" + AlgdFR[sp][n].ToString() + ")";
					}
					else
					{
						ReportLine += " dFR pair not built in " + std::to_string(MaxDepthdFR) + " steps";
						if (!AlgdFR[sp][n].Empty()) ReportLine += ": (" + AlgdFR[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}

					if (!AlgNCLL[sp].empty())
					{
						C.ApplyAlgorithm(AlgNCLL[sp][n]);
						if (debug) ReportLine += " {NCLL: " + CasesNCLL[sp][n] + "}";
						if (!AlgNCLL[sp][n].Empty()) ReportLine += " (" + AlgNCLL[sp][n].ToString() + ")";
					}
					else if (!AlgNCOLL[sp].empty())
					{
						C.ApplyAlgorithm(AlgNCOLL[sp][n]);
						if (debug) ReportLine += " {NCOLL: " + CasesNCOLL[sp][n] + "}";
						if (!AlgNCOLL[sp][n].Empty()) ReportLine += " (" + AlgNCOLL[sp][n].ToString() + ")";
					}
					else if (!AlgTNCLL[sp].empty())
					{
						if (IsF2LPairsBuilt(C))
						{
							C.ApplyAlgorithm(AlgTNCLL[sp][n]);
							if (debug) ReportLine += " {NCLL: " + CasesTNCLL[sp][n] + "}";
							if (!AlgTNCLL[sp][n].Empty()) ReportLine += " (" + AlgTNCLL[sp][n].ToString() + ")";
						}
						else
						{
							C.ApplyAlgorithm(AlgTNCLL[sp][n]);
							if (debug) ReportLine += " {TNCLL: " + CasesTNCLL[sp][n] + "}";
							if (!AlgTNCLL[sp][n].Empty()) ReportLine += " (" + AlgTNCLL[sp][n].ToString() + ")";
						}
					}

					C.ApplyAlgorithm(AlgL5E[sp][n]);
					if (debug) ReportLine += " {L5E: " + CasesL5E[sp][n] + "}";
					if (!AlgL5E[sp][n].Empty()) ReportLine += " (" + AlgL5E[sp][n].ToString() + ")";
				}
				else if (!AlgEODF[sp].empty()) // LSLL variant
				{
					C.ApplyAlgorithm(AlgEODF[sp][n]);

					if (IsNautilusEODFBuilt(C))
					{
						if (debug) ReportLine += " {EODF: " + CasesEODF[sp][n] + "}";
						ReportLine += " (" + AlgEODF[sp][n].ToString() + ")";
					}
					else
					{
						ReportLine += " EODF not built";
						if (!AlgEODF[sp][n].Empty()) ReportLine += ": (" + AlgEODF[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}

					C.ApplyAlgorithm(AlgF2L[sp][n]);

					if (IsF2LBuilt(C))
					{
						ReportLine += " (" + AlgF2L[sp][n].ToString() + ")";
					}
					else
					{
						ReportLine += " F2L not built in " + std::to_string(MaxDepthF2L) + " steps";
						if (!AlgF2L[sp][n].Empty()) ReportLine += ": (" + AlgF2L[sp][n].ToString() + ")\n";
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
    std::string Nautilus::GetTimeReport() const
    {
        std::string Report;

        Report += "Total time: " + std::to_string(GetFullTime()) + " s\n";
        Report += "First blocks time: " + std::to_string(GetTimeFB()) + " s\n";
        Report += "Second blocks time: " + std::to_string(GetTimeSB()) + " s\n";
		if (GetTimedFR() > 0.0) // L5E variant
		{
            Report += "dFR pair time: " + std::to_string(GetTimedFR()) + " s\n";
            if (GetTimeNCLL() > 0.0) Report += "NCLL time: " + std::to_string(GetTimeNCLL()) + " s\n";
            if (GetTimeNCOLL() > 0.0) Report += "NCOLL time: " + std::to_string(GetTimeNCOLL()) + " s\n";
            if (GetTimeTNCLL() > 0.0) Report += "TNCLL time: " + std::to_string(GetTimeTNCLL()) + " s\n";
            if (GetTimeL5E() > 0.0) Report += "L5E time: " + std::to_string(GetTimeL5E()) + " s\n";
		}
		else if (GetTimeEODF() > 0.0) // LSLL variant
		{
            Report += "EODF time: " + std::to_string(GetTimeEODF()) + " s\n";
            if (GetTimeF2L() > 0.0) Report += "F2L time: " + std::to_string(GetTimeF2L()) + " s\n";
            if (GetTimeLL() > 0.0) Report += "Last layer time: " + std::to_string(GetTimeLL()) + " s\n";
		}

        Report += "Threads used: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : DeepSearch::GetSystemCores()) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string Nautilus::GetReport(const Spn sp, const uint n) const
	{
		const int si = static_cast<int>(sp);

		if (!CheckSolveConsistency(sp) || Inspections[si].size() <= n)
		{
			std::string FReport = "No solve for spin ";
			FReport += Cube::GetSpinText(sp);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

        std::string Report;
        // Report += "Nautilus search with orientation " + Cube::GetSpinText(sp);
        // Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += GetTextInspection(sp,n) + " // Inspection [" + C.GetSpinText() + "]\n";

        C.ApplyAlgorithm(AlgFB[si][n]);
        if (!IsRouxFBBuilt(C))
        {
			Report += "First block not built in " + std::to_string(MaxDepthFB) + " movements\n";
            return Report;
        }
        Report += GetTextFB(sp, n) + " // First block (" + Algorithm::GetMetricValue(GetMetricFB(sp, n)) + ")\n";

        C.ApplyAlgorithm(AlgSB[si][n]);
        if (!IsNautilusSBBuilt(C))
        {
            Report += "Second block not built!\n";
            return Report;
        }
        if (!AlgSB[si][n].Empty()) Report += GetTextSB(sp, n) + " // Second block (" + Algorithm::GetMetricValue(GetMetricSB(sp, n)) + ")\n";

		if (!AlgdFR[si].empty()) // L5E variant
		{
			C.ApplyAlgorithm(AlgdFR[si][n]);
			if (!IsNautilusSBBuilt(C) || !(IsF2LPairsBuilt(C) || !AlgTNCLL[si][n].Empty()))
			{
				Report += "dFR pair not built!\n";
				return Report;
			}
            if (!AlgdFR[si][n].Empty()) Report += GetTextdFR(sp, n) + " // dFR pair (" + Algorithm::GetMetricValue(GetMetricdFR(sp,n)) + ")\n";

			if (!AlgNCLL[si].empty())
			{
                // C.ApplyAlgorithm(AlgNCLL[si][n]);
                if (!AlgNCLL[si][n].Empty()) Report += GetTextNCLL(sp, n) + " // NCLL (" + Algorithm::GetMetricValue(GetMetricNCLL(sp, n)) + ")\n";
			}
			else if (!AlgNCOLL[si].empty())
			{
                // C.ApplyAlgorithm(AlgNCOLL[si][n]);
                if (!AlgNCOLL[si][n].Empty()) Report += GetTextNCOLL(sp, n) + " // NCOLL (" + Algorithm::GetMetricValue(GetMetricNCOLL(sp, n)) + ")\n";
			}
			else if (!AlgTNCLL[si].empty())
			{
				if (IsF2LPairsBuilt(C))
				{
                    // C.ApplyAlgorithm(AlgTNCLL[si][n]);
                    if (!AlgTNCLL[si][n].Empty()) Report += GetTextTNCLL(sp, n) + " // NCLL (" + Algorithm::GetMetricValue(GetMetricTNCLL(sp, n)) + ")\n";
				}
				else
				{
                    // C.ApplyAlgorithm(AlgTNCLL[si][n]);
                    if (!AlgTNCLL[si][n].Empty()) Report += GetTextTNCLL(sp, n) + " // TNCLL (" + Algorithm::GetMetricValue(GetMetricTNCLL(sp, n)) + ")\n";
				}
			}

			if (!AlgL5E[si].empty())
			{
                // C.ApplyAlgorithm(AlgL5E[si][n]);
                if (!AlgL5E[si][n].Empty()) Report += GetTextL5E(sp, n) + " // L5E (" + Algorithm::GetMetricValue(GetMetricL5E(sp, n)) + ")\n";
			}
		}
		else if (!AlgEODF[si].empty()) // LSLL variant
		{
            C.ApplyAlgorithm(AlgEODF[si][n]);
            if (!AlgEODF[si][n].Empty()) Report += GetTextEODF(sp, n) + " // EODF (" + Algorithm::GetMetricValue(GetMetricEODF(sp, n)) + ")\n";

			C.ApplyAlgorithm(AlgF2L[si][n]);
			if (!IsF2LBuilt(C))
			{
				Report += "F2L not built in " + std::to_string(MaxDepthF2L) + " movements\n";
				return Report;
			}
            if (!AlgF2L[si][n].Empty()) Report += GetTextF2L(sp, n) + " // F2L (" + Algorithm::GetMetricValue(GetMetricF2L(sp, n)) + ")\n";

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
		}

        // Show summary
        Report += "\nMetric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
		if (!AlgNCLL[si].empty()) Report += "NCLL case: " + GetTextNCLLCase(sp, n) + "\n";
		if (!AlgNCOLL[si].empty()) Report += "NCOLL case: " + GetTextNCOLLCase(sp, n) + "\n";
		if (!AlgTNCLL[si].empty()) Report += "NCLL/TNCLL case: " + GetTextTNCLLCase(sp, n) + "\n";
		if (!AlgL5E[si].empty()) Report += "L5E case: " + GetTextL5ECase(sp, n) + "\n";
		if (!AlgEODF[si].empty()) Report += "EODF case: " + GetTextEODFCase(sp, n) + "\n";
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
	bool Nautilus::CheckSolveConsistency(const Spn Spin) const
	{
		const int si = static_cast<int>(Spin); // Spin index

		if (Inspections[si].empty()) return false;

		auto n = Inspections[si].size();

		return (AlgFB[si].size() == n) && (AlgSB[si].size() == n) && (
		       (AlgdFR[si].size() == n && AlgNCLL[si].size() == n && AlgL5E[si].size() == n) ||
			   (AlgdFR[si].size() == n && AlgNCOLL[si].size() == n && AlgL5E[si].size() == n) ||
			   (AlgdFR[si].size() == n && AlgTNCLL[si].size() == n && AlgL5E[si].size() == n) ||
			   (AlgEODF[si].size() == n && AlgF2L[si].size() == n && 
			   ((AlgZBLL[si].size() == n) || (AlgOCLL[si].size() == n && AlgPLL[si].size() == n) || (AlgCOLL[si].size() == n && AlgEPLL[si].size() == n))));
	}

	// Get the full solve
    Algorithm Nautilus::GetFullSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || n >= Inspections[si].size()) return 0u;

		Algorithm A = Inspections[si][n];
		A += AlgFB[si][n];
		A += AlgSB[si][n];
		// L5E variant
		if (!AlgdFR[si].empty()) A += AlgdFR[si][n];
		if (!AlgNCLL[si].empty()) A += AlgNCLL[si][n];
		if (!AlgTNCLL[si].empty()) A += AlgTNCLL[si][n];
		if (!AlgNCOLL[si].empty()) A += AlgNCOLL[si][n];
		if (!AlgL5E[si].empty()) A += AlgL5E[si][n];
		// LSLL variant
		if (!AlgEODF[si].empty()) A += AlgEODF[si][n];
		if (!AlgF2L[si].empty()) A += AlgF2L[si][n];
		if (!AlgZBLL[si].empty()) A += AlgZBLL[si][n];
		if (!AlgOCLL[si].empty()) A += AlgOCLL[si][n];
		if (!AlgPLL[si].empty()) A += AlgPLL[si][n];
		if (!AlgCOLL[si].empty()) A += AlgCOLL[si][n];
		if (!AlgEPLL[si].empty()) A += AlgEPLL[si][n];

		return A;
	}
}
