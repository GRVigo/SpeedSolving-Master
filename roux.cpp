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

#include "roux.h"
#include "collection.h"

namespace grcube3
{	
	// Reset the search results
	void Roux::Reset()
	{
        for (int i = 0; i < 24; i++)
        {
			Inspections[i].clear();
            FirstBlocks[i].clear();
            SecondBlocksFS[i].clear();
			SecondBlocksSS[i].clear();
            AlgCMLL[i].clear();
			AlgCOLL[i].clear();
            AlgL6EO[i].clear();
            AlgL6E2E[i].clear();
            AlgL6E[i].clear();
			
            CasesCMLL[i].clear();
            CasesCOLL[i].clear();
        }
		 
        MaxDepthFB = MaxDepthSBFS = MaxDepthSBSS = MaxDepthL6E = MaxDepthL6EO = 0u;

		TimeFB = TimeSBFS = TimeSBSS = TimeCMLL = TimeCOLL = TimeL6EO = TimeL6E2E = TimeL6E = 0.0;
		
		Metric = Metrics::Movements; // Default metric
	}

    // Search the best first blocks solve algorithm with the given search depth
	// Return false if no first blocks found
    bool Roux::SearchFirstBlock(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_fb_start = std::chrono::system_clock::now();

        MaxDepthFB = (MaxDepth <= 4u ? 4u : MaxDepth);

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

        EvaluateFirstBlock(DS.GetSolves(), MaxSolves);

        const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
        TimeFB = fb_elapsed_seconds.count();

        return !DS.GetSolves().empty();
	}
	
	// Search the best first block solve algorithms from an algorithms vector
    void Roux::EvaluateFirstBlock(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best first block solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			Stp T1, T2;
			Algorithm Insp;

			EvaluateRouxFBResult(FirstBlocks[sp], MaxSolves, Solves, CubeBase, spin, Plc::BEST);

			Inspections[sp].clear();

			if (!FirstBlocks[sp].empty())
			{
				Insp.Clear();
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				if (T1 != Stp::NONE) Insp.Append(T1);
				if (T2 != Stp::NONE) Insp.Append(T2);

				for (auto& fb : FirstBlocks[sp])
				{
					if (T1 != Stp::NONE) fb.TransformTurn(T1);
					if (T2 != Stp::NONE) fb.TransformTurn(T2);
					Inspections[sp].push_back(Insp);
				}
			}
        }
	}

    // Search the best second block first square solve algorithm with the given search depth
	void Roux::SearchSecondBlocksFirstSquare(const uint MaxDepth)
    {
        const auto time_sb_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        MaxDepthSBFS = (MaxDepth <= 4u ? 4u : MaxDepth);
		
        const SearchUnit U_SB(SequenceTypes::SINGLE, Sst::ROUX_SB);
        const SearchUnit U_SB_Seq(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_R, Sst::LATERAL_SINGLE_FBp);

		SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_SB);
		L_Check.Add(U_SB_Seq);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			SecondBlocksFS[sp].clear();

			Pgr B1, B2S1, B2S2;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; B2S1 = Pgr::UF_B2S1; B2S2 = Pgr::UF_B2S2; break;
			case Spn::UB: B1 = Pgr::UB_B1; B2S1 = Pgr::UB_B2S1; B2S2 = Pgr::UB_B2S2; break;
			case Spn::UR: B1 = Pgr::UR_B1; B2S1 = Pgr::UR_B2S1;	B2S2 = Pgr::UR_B2S2; break;
			case Spn::UL: B1 = Pgr::UL_B1; B2S1 = Pgr::UL_B2S1; B2S2 = Pgr::UL_B2S2; break;

			case Spn::DF: B1 = Pgr::DF_B1; B2S1 = Pgr::DF_B2S1; B2S2 = Pgr::DF_B2S2; break;
			case Spn::DB: B1 = Pgr::DB_B1; B2S1 = Pgr::DB_B2S1; B2S2 = Pgr::DB_B2S2; break;
			case Spn::DR: B1 = Pgr::DR_B1; B2S1 = Pgr::DR_B2S1; B2S2 = Pgr::DR_B2S2; break;
			case Spn::DL: B1 = Pgr::DL_B1; B2S1 = Pgr::DL_B2S1; B2S2 = Pgr::DL_B2S2; break;

			case Spn::FU: B1 = Pgr::FU_B1; B2S1 = Pgr::FU_B2S1; B2S2 = Pgr::FU_B2S2; break;
			case Spn::FD: B1 = Pgr::FD_B1; B2S1 = Pgr::FD_B2S1; B2S2 = Pgr::FD_B2S2; break;
			case Spn::FR: B1 = Pgr::FR_B1; B2S1 = Pgr::FR_B2S1; B2S2 = Pgr::FR_B2S2; break;
			case Spn::FL: B1 = Pgr::FL_B1; B2S1 = Pgr::FL_B2S1; B2S2 = Pgr::FL_B2S2; break;

			case Spn::BU: B1 = Pgr::BU_B1; B2S1 = Pgr::BU_B2S1;	B2S2 = Pgr::BU_B2S2; break;
			case Spn::BD: B1 = Pgr::BD_B1; B2S1 = Pgr::BD_B2S1; B2S2 = Pgr::BD_B2S2; break;
			case Spn::BR: B1 = Pgr::BR_B1; B2S1 = Pgr::BR_B2S1; B2S2 = Pgr::BR_B2S2; break;
			case Spn::BL: B1 = Pgr::BL_B1; B2S1 = Pgr::BL_B2S1; B2S2 = Pgr::BL_B2S2; break;

			case Spn::RU: B1 = Pgr::RU_B1; B2S1 = Pgr::RU_B2S1; B2S2 = Pgr::RU_B2S2; break;
			case Spn::RD: B1 = Pgr::RD_B1; B2S1 = Pgr::RD_B2S1; B2S2 = Pgr::RD_B2S2; break;
			case Spn::RF: B1 = Pgr::RF_B1; B2S1 = Pgr::RF_B2S1; B2S2 = Pgr::RF_B2S2; break;
			case Spn::RB: B1 = Pgr::RB_B1; B2S1 = Pgr::RB_B2S1; B2S2 = Pgr::RB_B2S2; break;

			case Spn::LU: B1 = Pgr::LU_B1; B2S1 = Pgr::LU_B2S1; B2S2 = Pgr::LU_B2S2; break;
			case Spn::LD: B1 = Pgr::LD_B1; B2S1 = Pgr::LD_B2S1; B2S2 = Pgr::LD_B2S2; break;
			case Spn::LF: B1 = Pgr::LF_B1; B2S1 = Pgr::LF_B2S1; B2S2 = Pgr::LF_B2S2; break;
			case Spn::LB: B1 = Pgr::LB_B1; B2S1 = Pgr::LB_B2S1; B2S2 = Pgr::LB_B2S2; break;

			default: return; // Should not happend
			}

			uint n = 0u;
			for (const auto& FB : FirstBlocks[sp])
			{
                SecondBlocksFS[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FB);

				DS.Clear();
                DS.SetScramble(AlgStart);

				DS.AddToMandatoryPieces(B1);
				DS.AddToOptionalPieces(B2S1);
				DS.AddToOptionalPieces(B2S2);

				for (uint l = 1u; l < MaxDepthSBFS; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();
				
                Cores = DS.GetUsedCores(); // Update to the real number of cores used

				Cube CubeRoux(AlgStart);

				std::vector<Algorithm> Solves;
				// This function is made for CFOP, but, without the cross check ('false' parameter), is useful here
                Method::EvaluateCFOPF2LResult(Solves, 1u, DS.GetSolves(), CubeRoux, Cube::GetDownSliceLayer(spin), Plc::BEST, false);

				if (!Solves.empty())
				{
					CubeRoux.ApplyAlgorithm(Solves[0]);
					if (IsRouxFBAndSBSquareBuilt(CubeRoux)) SecondBlocksFS[sp][n] = Solves[0];
				}
				n++;
			}
        }

        const std::chrono::duration<double> sb_elapsed_seconds = std::chrono::system_clock::now() - time_sb_start;
        TimeSBFS = sb_elapsed_seconds.count();
    }
	
	// Complete the second square for the second blocks
    void Roux::SearchSecondBlocksSecondSquare(const uint MaxDepth)
    {
        auto time_SBSS_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthSBSS = (MaxDepth <= 4u ? 4u : MaxDepth);

        const SearchUnit U_Up(SequenceTypes::SINGLE, Sst::SINGLE_U);
        const SearchUnit U_Lat(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::LATERAL_UD);

        SearchLevel L_F2L_Check(SearchCheck::CHECK);
        L_F2L_Check.Add(U_Up);
        L_F2L_Check.Add(U_Lat);
		
        SearchLevel L_F2L_End(SearchCheck::CHECK);
        L_F2L_End.Add(U_Lat);

		Pgr B1, B2;

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			SecondBlocksSS[sp].clear();

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; B2 = Pgr::UF_B2; break;
			case Spn::UB: B1 = Pgr::UB_B1; B2 = Pgr::UB_B2; break;
			case Spn::UR: B1 = Pgr::UR_B1; B2 = Pgr::UR_B2; break;
			case Spn::UL: B1 = Pgr::UL_B1; B2 = Pgr::UL_B2; break;

			case Spn::DF: B1 = Pgr::DF_B1; B2 = Pgr::DF_B2; break;
			case Spn::DB: B1 = Pgr::DB_B1; B2 = Pgr::DB_B2; break;
			case Spn::DR: B1 = Pgr::DR_B1; B2 = Pgr::DR_B2; break;
			case Spn::DL: B1 = Pgr::DL_B1; B2 = Pgr::DL_B2; break;

			case Spn::FU: B1 = Pgr::FU_B1; B2 = Pgr::FU_B2; break;
			case Spn::FD: B1 = Pgr::FD_B1; B2 = Pgr::FD_B2; break;
			case Spn::FR: B1 = Pgr::FR_B1; B2 = Pgr::FR_B2; break;
			case Spn::FL: B1 = Pgr::FL_B1; B2 = Pgr::FL_B2; break;

			case Spn::BU: B1 = Pgr::BU_B1; B2 = Pgr::BU_B2; break;
			case Spn::BD: B1 = Pgr::BD_B1; B2 = Pgr::BD_B2; break;
			case Spn::BR: B1 = Pgr::BR_B1; B2 = Pgr::BR_B2; break;
			case Spn::BL: B1 = Pgr::BL_B1; B2 = Pgr::BL_B2; break;

			case Spn::RU: B1 = Pgr::RU_B1; B2 = Pgr::RU_B2; break;
			case Spn::RD: B1 = Pgr::RD_B1; B2 = Pgr::RD_B2; break;
			case Spn::RF: B1 = Pgr::RF_B1; B2 = Pgr::RF_B2; break;
			case Spn::RB: B1 = Pgr::RB_B1; B2 = Pgr::RB_B2; break;

			case Spn::LU: B1 = Pgr::LU_B1; B2 = Pgr::LU_B2; break;
			case Spn::LD: B1 = Pgr::LD_B1; B2 = Pgr::LD_B2; break;
			case Spn::LF: B1 = Pgr::LF_B1; B2 = Pgr::LF_B2; break;
			case Spn::LB: B1 = Pgr::LB_B1; B2 = Pgr::LB_B2; break;

			default: return; // Should not happend
			}
			
			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
                SecondBlocksSS[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);

				Cube CubeRoux(AlgStart);

				// No second block or second block already complete
				if (!IsRouxFBAndSBSquareBuilt(CubeRoux) || AreRouxBlocksBuilt(CubeRoux)) continue; 

				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(B1);
				DS.AddToMandatoryPieces(B2);

				for (uint l = 1u; l < MaxDepthSBSS; l++) DS.AddSearchLevel(L_F2L_Check); // Add needed search levels
				DS.AddSearchLevel(L_F2L_End);

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				std::vector<Algorithm> Solves;
				// This function is made for CFOP, but, without the cross check ('false' parameter), is useful here
                Method::EvaluateCFOPF2LResult(Solves, 1u, DS.GetSolves(), CubeRoux, Cube::GetDownSliceLayer(spin), Plc::SHORT, false);

				if (!Solves.empty())
				{
					CubeRoux.ApplyAlgorithm(Solves[0]);
					if (AreRouxBlocksBuilt(CubeRoux)) SecondBlocksSS[sp][n] = Solves[0];
				}
			}
		}

        const std::chrono::duration<double> SBSS_elapsed_seconds = std::chrono::system_clock::now() - time_SBSS_start;
        TimeSBSS = SBSS_elapsed_seconds.count();
    }
	
	// CMLL search
    void Roux::SearchCMLL(const Plc Pol, const bool AddLastUMovement)
	{
        const auto time_CMLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgCMLL[sp].clear();

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				AlgCMLL[sp].push_back(Algorithm(""));
                CasesCMLL[sp].push_back("");

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);

				const Cube CubeRoux(AlgStart);

				if (!AreRouxBlocksBuilt(CubeRoux)) continue;

				Stp LastUStep;

                CornersLL(AlgCMLL[sp][n], CasesCMLL[sp][n], LastUStep, Algset_CMLL, Pol, Metric, CubeRoux);

				if (AddLastUMovement) AlgCMLL[sp][n].Append(LastUStep);
            }
		}

        const std::chrono::duration<double> CMLL_elapsed_seconds = std::chrono::system_clock::now() - time_CMLL_start;
        TimeCMLL = CMLL_elapsed_seconds.count();
	}
    
    // COLL search
    void Roux::SearchCOLL(const Plc Pol, const bool AddLastUMovement)
	{
        const auto time_COLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgCOLL[sp].clear();

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				AlgCOLL[sp].push_back(Algorithm(""));
                CasesCOLL[sp].push_back("");

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);

				const Cube CubeRoux(AlgStart);

				if (!AreRouxBlocksBuilt(CubeRoux)) continue;

				Stp LastUStep;

                CornersLL(AlgCOLL[sp][n], CasesCOLL[sp][n], LastUStep, Algset_COLL, Pol, Metric, CubeRoux);

				if (AddLastUMovement) AlgCOLL[sp][n].Append(LastUStep);
			}
		}

        const std::chrono::duration<double> COLL_elapsed_seconds = std::chrono::system_clock::now() - time_COLL_start;
        TimeCOLL = COLL_elapsed_seconds.count();
	}

    // Search the last six edges (U & M layer movements)
    void Roux::SearchL6E(const uint MaxDepth)
    {
        const auto time_L6E_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        MaxDepthL6E = (MaxDepth <= 4u ? 4u : MaxDepth);

        // First level is extended in the search to improve the multithreading
        const SearchUnit U_Root(SequenceTypes::TRIPLE, Sst::SINGLE_UM);
        const SearchUnit U_Level(SequenceTypes::SINGLE, Sst::SINGLE_UM);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(U_Root);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_Level);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgL6E[sp].clear();

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				AlgL6E[sp].push_back(Algorithm(""));
				
				if (Skip) continue;

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);
				if (!AlgCMLL[sp].empty()) AlgStart.Append(AlgCMLL[sp][n]);
				if (!AlgCOLL[sp].empty()) AlgStart.Append(AlgCOLL[sp][n]);
                if (!AlgL6EO[sp].empty()) AlgStart.Append(AlgL6EO[sp][n]);
                if (!AlgL6E2E[sp].empty()) AlgStart.Append(AlgL6E2E[sp][n]);
				
				Cube CubeRoux(AlgStart);

				if (!AreRouxBlocksBuilt(CubeRoux) || CubeRoux.IsSolved()) continue;

				// Auxiliar deep search for last six edges in 3 movements (very fast)
				DeepSearch PreDSL6E(AlgStart, Plc::SHORT);
				PreDSL6E.AddToMandatoryPieces(Pgr::ALL); // Search the full cube
				PreDSL6E.AddSearchLevel(L_Check);
				PreDSL6E.AddSearchLevel(L_Check);
				PreDSL6E.AddSearchLevel(L_Check);
				PreDSL6E.UpdateRootData();
				PreDSL6E.Run(Cores);
                if (!PreDSL6E.GetSolves().empty())
				{
					PreDSL6E.EvaluateShortestResult(AlgL6E[sp][n], true);
					continue; // L6E solve in three movements found
				}
				
				// Full deep search for last six edges
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(Pgr::ALL); // Search the full cube

				DS.AddSearchLevel(L_Root); // Level 1 (three steps -TRIPLE- root algorithms
				for (uint l = 2; l < MaxDepthL6E; l++) DS.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

				DS.UpdateRootData();
                DS.SetMinDepth(DS.GetMaxDepth() - 4u);

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				DS.EvaluateShortestResult(AlgL6E[sp][n], true);
			}
        }

        const std::chrono::duration<double> L6E_elapsed_seconds = std::chrono::system_clock::now() - time_L6E_start;
        TimeL6E = L6E_elapsed_seconds.count();
    }
	
	// Search the last six edges orientated (U & M layer movements) with the given search deep
    void Roux::SearchL6EO(const uint MaxDepth)
    {
        const auto time_L6EO_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        MaxDepthL6EO = (MaxDepth <= 4u ? 4u : MaxDepth);

        // First level is extended in the search to improve the multithreading - first three levels will not be checked
        // (it's supose that the last six edges will not be solved in three movements)
        const SearchUnit U_Root(SequenceTypes::TRIPLE, Sst::SINGLE_UM);
        const SearchUnit U_Level(SequenceTypes::SINGLE, Sst::SINGLE_UM);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(U_Root);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_Level);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

            AlgL6EO[sp].clear();

			Pgr B1, B2, CORNERS, EDGES;

			switch (static_cast<Spn>(sp))
			{
			case Spn::UF: B1 = Pgr::UF_B1; B2 = Pgr::UF_B2;	CORNERS = Pgr::CORNERS_U; EDGES = Pgr::ROUX_L6E_UF; break;
			case Spn::UB: B1 = Pgr::UB_B1; B2 = Pgr::UB_B2;	CORNERS = Pgr::CORNERS_U; EDGES = Pgr::ROUX_L6E_UB; break;
			case Spn::UR: B1 = Pgr::UR_B1; B2 = Pgr::UR_B2; CORNERS = Pgr::CORNERS_U; EDGES = Pgr::ROUX_L6E_UR; break;
			case Spn::UL: B1 = Pgr::UL_B1; B2 = Pgr::UL_B2; CORNERS = Pgr::CORNERS_U; EDGES = Pgr::ROUX_L6E_UL; break;

			case Spn::DF: B1 = Pgr::DF_B1; B2 = Pgr::DF_B2; CORNERS = Pgr::CORNERS_D; EDGES = Pgr::ROUX_L6E_DF; break;
			case Spn::DB: B1 = Pgr::DB_B1; B2 = Pgr::DB_B2; CORNERS = Pgr::CORNERS_D; EDGES = Pgr::ROUX_L6E_DB; break;
			case Spn::DR: B1 = Pgr::DR_B1; B2 = Pgr::DR_B2; CORNERS = Pgr::CORNERS_D; EDGES = Pgr::ROUX_L6E_DR; break;
			case Spn::DL: B1 = Pgr::DL_B1; B2 = Pgr::DL_B2; CORNERS = Pgr::CORNERS_D; EDGES = Pgr::ROUX_L6E_DL; break;

			case Spn::FU: B1 = Pgr::FU_B1; B2 = Pgr::FU_B2; CORNERS = Pgr::CORNERS_F; EDGES = Pgr::ROUX_L6E_FU; break;
			case Spn::FD: B1 = Pgr::FD_B1; B2 = Pgr::FD_B2; CORNERS = Pgr::CORNERS_F; EDGES = Pgr::ROUX_L6E_FD; break;
			case Spn::FR: B1 = Pgr::FR_B1; B2 = Pgr::FR_B2; CORNERS = Pgr::CORNERS_F; EDGES = Pgr::ROUX_L6E_FR; break;
			case Spn::FL: B1 = Pgr::FL_B1; B2 = Pgr::FL_B2; CORNERS = Pgr::CORNERS_F; EDGES = Pgr::ROUX_L6E_FL; break;

			case Spn::BU: B1 = Pgr::BU_B1; B2 = Pgr::BU_B2; CORNERS = Pgr::CORNERS_B; EDGES = Pgr::ROUX_L6E_BU; break;
			case Spn::BD: B1 = Pgr::BD_B1; B2 = Pgr::BD_B2; CORNERS = Pgr::CORNERS_B; EDGES = Pgr::ROUX_L6E_BD; break;
			case Spn::BR: B1 = Pgr::BR_B1; B2 = Pgr::BR_B2; CORNERS = Pgr::CORNERS_B; EDGES = Pgr::ROUX_L6E_BR; break;
			case Spn::BL: B1 = Pgr::BL_B1; B2 = Pgr::BL_B2; CORNERS = Pgr::CORNERS_B; EDGES = Pgr::ROUX_L6E_BL; break;

			case Spn::RU: B1 = Pgr::RU_B1; B2 = Pgr::RU_B2; CORNERS = Pgr::CORNERS_R; EDGES = Pgr::ROUX_L6E_RU; break;
			case Spn::RD: B1 = Pgr::RD_B1; B2 = Pgr::RD_B2; CORNERS = Pgr::CORNERS_R; EDGES = Pgr::ROUX_L6E_RD; break;
			case Spn::RF: B1 = Pgr::RF_B1; B2 = Pgr::RF_B2; CORNERS = Pgr::CORNERS_R; EDGES = Pgr::ROUX_L6E_RF; break;
			case Spn::RB: B1 = Pgr::RB_B1; B2 = Pgr::RB_B2; CORNERS = Pgr::CORNERS_R; EDGES = Pgr::ROUX_L6E_RB; break;

			case Spn::LU: B1 = Pgr::LU_B1; B2 = Pgr::LU_B2; CORNERS = Pgr::CORNERS_L; EDGES = Pgr::ROUX_L6E_LU; break;
			case Spn::LD: B1 = Pgr::LD_B1; B2 = Pgr::LD_B2; CORNERS = Pgr::CORNERS_L; EDGES = Pgr::ROUX_L6E_LD; break;
			case Spn::LF: B1 = Pgr::LF_B1; B2 = Pgr::LF_B2; CORNERS = Pgr::CORNERS_L; EDGES = Pgr::ROUX_L6E_LF; break;
			case Spn::LB: B1 = Pgr::LB_B1; B2 = Pgr::LB_B2; CORNERS = Pgr::CORNERS_L; EDGES = Pgr::ROUX_L6E_LB; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
                AlgL6EO[sp].push_back(Algorithm(""));
				
				if (Skip) continue;

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);
				if (!AlgCMLL[sp].empty()) AlgStart.Append(AlgCMLL[sp][n]);
				if (!AlgCOLL[sp].empty()) AlgStart.Append(AlgCOLL[sp][n]);

				Cube CubeRoux(AlgStart);

				if (!AreRouxBlocksBuilt(CubeRoux) || IsRouxL6EOriented(CubeRoux)) continue; // Last six edges already oriented or no solve

                DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

                DS.AddToMandatoryPieces(B1);
                DS.AddToMandatoryPieces(B2);
                DS.AddToMandatoryPieces(CORNERS);
                DS.AddToMandatoryOrientations(EDGES);

                DS.AddSearchLevel(L_Root); // Level 1 (three steps -TRIPLE- root algorithms
                for (uint l = 2; l < MaxDepthL6EO; l++) DS.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

                DS.UpdateRootData();

                DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

                DS.EvaluateShortestResult(AlgL6EO[sp][n], true);
			}
        }

        const std::chrono::duration<double> L6EO_elapsed_seconds = std::chrono::system_clock::now() - time_L6EO_start;
        TimeL6EO = L6EO_elapsed_seconds.count();
    }

	// Search the UR & UL edges (L6E)
    void Roux::SearchL6E2E(const uint MaxDepth)
    {
        const auto time_L6E2E_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

        MaxDepthL6E2E = (MaxDepth <= 4u ? 4u : MaxDepth);

        // First level is extended in the search to improve the multithreading - first two levels will not be checked
        // (it's supose that the last six edges will not be solved in two movements)
        SearchUnit U_Root(SequenceTypes::DOUBLE, Sst::SINGLE_UM);
        SearchUnit U_Level(SequenceTypes::SINGLE, Sst::SINGLE_UM);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(U_Root);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_Level);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

            AlgL6E2E[sp].clear();

			Pgr LR, LL, LM;

			switch (static_cast<Spn>(sp))
			{
			case Spn::UF:
			case Spn::UB:
			case Spn::DF:
			case Spn::DB:
			case Spn::FU:
			case Spn::FD:
			case Spn::BU:
			case Spn::BD: LR = Pgr::LAYER_R; LL = Pgr::LAYER_L; LM = Pgr::LAYER_M; break;

			case Spn::UR:
			case Spn::UL:
			case Spn::DR:
			case Spn::DL:
			case Spn::RU:
			case Spn::RD:
			case Spn::LU:
			case Spn::LD: LR = Pgr::LAYER_F; LL = Pgr::LAYER_B; LM = Pgr::LAYER_S; break;

			case Spn::FR:
			case Spn::FL:
			case Spn::BR:
			case Spn::BL:
			case Spn::RF:
			case Spn::RB:
			case Spn::LF:
			case Spn::LB: LR = Pgr::LAYER_U; LL = Pgr::LAYER_D; LM = Pgr::LAYER_E; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
                AlgL6E2E[sp].push_back(Algorithm(""));
				
				if (Skip) continue;

				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FirstBlocks[sp][n]);
				AlgStart.Append(SecondBlocksFS[sp][n]);
				AlgStart.Append(SecondBlocksSS[sp][n]);
				if (!AlgCMLL[sp].empty()) AlgStart.Append(AlgCMLL[sp][n]);
				if (!AlgCOLL[sp].empty()) AlgStart.Append(AlgCOLL[sp][n]);
                AlgStart.Append(AlgL6EO[sp][n]);

				Cube CubeRoux(AlgStart);

                if (!IsRouxL6EOriented(CubeRoux) || IsRouxL6EO2E(CubeRoux)) continue; // L6E two edges solved or no solve

                // Deep search for last six edges (UR & UL edges)
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

                DS.AddToMandatoryPieces(LR);
                DS.AddToMandatoryPieces(LL);
                DS.AddToMandatoryOrientations(LM);

                DS.AddSearchLevel(L_Root); // Level 1 (three steps -TRIPLE- root algorithms
                for (uint l = 2; l < MaxDepthL6E2E; l++) DS.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

                DS.UpdateRootData();

                DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

                DS.EvaluateShortestResult(AlgL6E2E[sp][n], true);
			}
		}

        const std::chrono::duration<double> L6E2E_elapsed_seconds = std::chrono::system_clock::now() - time_L6E2E_start;
        TimeL6E2E = L6E2E_elapsed_seconds.count();
    }

	// Set regrips
	void Roux::SetRegrips()
	{
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			if (!CheckSolveConsistency(spin)) continue;

			for (uint n = 0u; n < FirstBlocks[sp].size(); n++)
			{
				FirstBlocks[sp][n] = FirstBlocks[sp][n].GetRegrip();

				if (Algorithm::IsTurn(FirstBlocks[sp][n].First()))
				{
					Inspections[sp][n].AppendShrink(FirstBlocks[sp][n].First());
					if (Inspections[sp][n].GetSize() == 3u) Inspections[sp][n] = Inspections[sp][n].GetCancellations();
					FirstBlocks[sp][n].EraseFirst();
				}
			}
		}
	}

    // Get a solve report
    std::string Roux::GetReport(const bool cancellations, bool debug) const
    {
        std::string Report, ReportLine;
        // Report = "Roux: Scramble [" + std::to_string(Scramble.GetNumSteps()) + "] " + GetTextScramble();
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
				C.ApplyAlgorithm(FirstBlocks[sp][n]);

				if (IsRouxFBBuilt(C))
				{
					ReportLine += "[" + Cube::GetSpinText(spin) + "|" + Algorithm::GetMetricValue(GetMetricSolve(spin, n));
					if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
					ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
					if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
					ReportLine += "(" + FirstBlocks[sp][n].ToString() + ")";
				}
				else
				{
					if (debug)
					{
						ReportLine += "[" + Cube::GetSpinText(spin) + "]: ";
						if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += "First block not built in " + std::to_string(MaxDepthFB) + " steps";
						if (!FirstBlocks[sp][n].Empty()) ReportLine += ": (" + FirstBlocks[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
					}
					if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(SecondBlocksFS[sp][n]);

				if (IsRouxFBAndSBSquareBuilt(C)) ReportLine += " (" + SecondBlocksFS[sp][n].ToString() + ")";
				else
				{
					ReportLine += "First square for second block not built in " + std::to_string(MaxDepthSBFS) + " steps";
					if (!SecondBlocksFS[sp][n].Empty()) ReportLine += ": (" + SecondBlocksFS[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				if (!AreRouxBlocksBuilt(C))
				{
					C.ApplyAlgorithm(SecondBlocksSS[sp][n]);

					if (AreRouxBlocksBuilt(C)) ReportLine += " (" + SecondBlocksSS[sp][n].ToString() + ")";
					else
					{
						ReportLine += "Second square for second block not built in " + std::to_string(MaxDepthSBSS) + " steps";
						if (!SecondBlocksSS[sp][n].Empty()) ReportLine += ": (" + SecondBlocksSS[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}
				}

				if (!AlgCMLL[sp].empty())
				{
					C.ApplyAlgorithm(AlgCMLL[sp][n]);
                    if (debug) ReportLine += " {CMLL: " + CasesCMLL[sp][n] + "}";
					if (!AlgCMLL[sp][n].Empty()) ReportLine += " (" + AlgCMLL[sp][n].ToString() + ")";
				}

				if (!AlgCOLL[sp].empty())
				{
					C.ApplyAlgorithm(AlgCOLL[sp][n]);
                    if (debug) ReportLine += " {COLL: " + CasesCOLL[sp][n] + "}";
					if (!AlgCOLL[sp][n].Empty()) ReportLine += " (" + AlgCOLL[sp][n].ToString() + ")";
				}

                if (!AlgL6EO[sp].empty())
				{
                    C.ApplyAlgorithm(AlgL6EO[sp][n]);
                    if (!AlgL6EO[sp][n].Empty()) ReportLine += " (" + AlgL6EO[sp][n].ToString() + ")";
				}
				else if (debug) ReportLine += " [No L6E orient.]"; // Debug code

                if (!AlgL6E2E[sp].empty())
				{
                    C.ApplyAlgorithm(AlgL6E2E[sp][n]);
                    if (!AlgL6E2E[sp][n].Empty()) ReportLine += " (" + AlgL6E2E[sp][n].ToString() + ")";
				}
				else if (debug) ReportLine += " [No L6E URUL]"; // Debug code

				C.ApplyAlgorithm(AlgL6E[sp][n]);
				if (!AlgL6E[sp][n].Empty()) ReportLine += " (" + AlgL6E[sp][n].ToString() + ")";

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
    std::string Roux::GetTimeReport() const
    {
        std::string Report;

        Report += "Total time: " + std::to_string(GetFullTime()) + " s\n";
        Report += "First blocks time: " + std::to_string(GetTimeFB()) + " s\n";
        Report += "Second blocks time: " + std::to_string(GetTimeSB()) + " s\n";
        if (GetTimeCMLL() > 0.0) Report += "CMLL time: " + std::to_string(GetTimeCMLL()) + " s\n";
        if (GetTimeCOLL() > 0.0) Report += "COLL time: " + std::to_string(GetTimeCOLL()) + " s\n";
        Report += "L6E time: " + std::to_string(GetTimeL6E() + GetTimeL6EO() + GetTimeL6E2E()) + " s\n";
        Report += "Threads: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : DeepSearch::GetSystemCores()) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string Roux::GetReport(const Spn sp, const uint n) const
	{
		const int si = static_cast<int>(sp);

		if (!CheckSolveConsistency(sp) || FirstBlocks[si].size() <= n)
		{
			std::string FReport = "No solve for spin ";
			FReport += Cube::GetSpinText(sp);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

        std::string Report;
        // Report += "Roux search with orientation " + Cube::GetSpinText(sp);
        // Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += GetTextInspection(sp,n) + " // Inspection [" + C.GetSpinText() + "]\n";

        C.ApplyAlgorithm(FirstBlocks[si][n]);
        if (!IsRouxFBBuilt(C))
        {
			Report += "First block not built in " + std::to_string(MaxDepthFB) + " movements\n";
            return Report;
        }
        Report += GetTextFirstBlock(sp, n) + " // First block (" + Algorithm::GetMetricValue(GetMetricFirstBlock(sp, n)) + ")\n";

        C.ApplyAlgorithm(SecondBlocksFS[si][n]);
        C.ApplyAlgorithm(SecondBlocksSS[si][n]);
        if (!AreRouxBlocksBuilt(C))
        {
            Report += "Second block not built!\n";
            return Report;
        }
        if (GetLengthSecondBlockSS(sp, n) == 0u)
            Report += GetTextSecondBlockFS(sp, n) +  " // Second block (" + Algorithm::GetMetricValue(GetMetricSecondBlock(sp,n)) + ")\n";
        else
        {
            Report += GetTextSecondBlockFS(sp, n) + " // Second block - square (" + Algorithm::GetMetricValue(GetMetricSecondBlockFS(sp, n)) + ")\n";
            Report += GetTextSecondBlockSS(sp, n) + " // Second block (" + Algorithm::GetMetricValue(GetMetricSecondBlockSS(sp, n)) + ")\n";
        }

		if (!AlgCMLL[si].empty())
        {
            C.ApplyAlgorithm(AlgCMLL[si][n]);
            if (!AlgCMLL[si][n].Empty()) Report += GetTextCMLL(sp, n) + " // CMLL (" + Algorithm::GetMetricValue(GetMetricCMLL(sp, n)) + ")\n";
        }
        else if (!AlgCOLL[si].empty())
        {
            C.ApplyAlgorithm(AlgCOLL[si][n]);
            if (!AlgCOLL[si][n].Empty()) Report += GetTextCOLL(sp, n) + " // COLL (" + Algorithm::GetMetricValue(GetMetricCOLL(sp, n)) + ")\n";
        }
        else
        {
            Report += "CMLL/COLL not solved!";
            return Report;
        }

        if (!AlgL6EO[si].empty())
        {
            if (!AlgL6EO[si][n].Empty())
            {
                C.ApplyAlgorithm(AlgL6EO[si][n]);

                if (!C.IsFaceOriented2(Cube::GetUpSliceLayer(sp)))
                {
                    Report += "L6E not orientated: " + GetTextL6EO(sp, n) + "\n";
                    return Report;
                }
                else if (!AlgL6EO[si][n].Empty())
                    Report += GetTextL6EO(sp, n) + " // L6E orientation (" + Algorithm::GetMetricValue(GetMetricL6EO(sp, n)) + ")\n";
            }
        }
        if (!AlgL6E2E[si].empty())
		{
            if (!AlgL6E2E[si][n].Empty())
            {
                C.ApplyAlgorithm(AlgL6E2E[si][n]);
                if (!AlgL6E2E[si][n].Empty())
                    Report += GetTextL6E2E(sp, n) + " // UR & UL edges (" + Algorithm::GetMetricValue(GetMetricL6E2E(sp, n)) + ")\n";
            }
		}

		C.ApplyAlgorithm(AlgL6E[si][n]);
		if (C.IsSolved())
		{
            if (!AlgL6E[si][n].Empty()) Report += GetTextL6E(sp, n) + " // L6E (" + Algorithm::GetMetricValue(GetMetricL6E(sp, n)) + ")\n";
		}
        else Report += "L6E not solved: " + GetTextL6E(sp, n) + "\n";

        // Show summary
        Report += "\nMetric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
		if (AlgCMLL[si].empty()) // COLL
			Report += "COLL case: " + GetTextCOLLCase(sp, n) + "\n";
		else // CMLL 
			Report += "CMLL case: " + GetTextCMLLCase(sp, n) + "\n";

		return Report;
	}

	// Check if the solves for the given cross layer are consistent (all needed algorithms are present)
	bool Roux::CheckSolveConsistency(const Spn Spin) const
	{
		const int si = static_cast<int>(Spin); // Spin index

		if (Inspections[si].empty()) return false;

		auto n = Inspections[si].size();

		return (FirstBlocks[si].size() == n) &&
			   (SecondBlocksFS[si].size() == n) && (SecondBlocksSS[si].size() == n) && ((AlgCMLL[si].size() == n) || (AlgCOLL[si].size() == n)) &&
			   (AlgL6E[si].size() == n); // && (AlgOL6E[si].size() == n) && (AlgL6E_URUL[si].size() == n);
	}

	// Get the full solve
    Algorithm Roux::GetFullSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || n >= Inspections[si].size()) return 0u;

		Algorithm A = Inspections[si][n];
		A += FirstBlocks[si][n];
		A += SecondBlocksFS[si][n];
		A += SecondBlocksSS[si][n];
		if (!AlgCMLL[si].empty()) A += AlgCMLL[si][n];
		if (!AlgCOLL[si].empty()) A += AlgCOLL[si][n];
        if (!AlgL6EO[si].empty()) A += AlgL6EO[si][n];
        if (!AlgL6E2E[si].empty()) A += AlgL6E2E[si][n];
		A += AlgL6E[si][n];
		
		return A;
	}
}
