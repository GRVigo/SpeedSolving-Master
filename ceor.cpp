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

#include "ceor.h"
#include "collection.h"

namespace grcube3
{	
	// Reset the search results
	void CEOR::Reset()
	{
        for (int i = 0; i < 24; i++)
        {
			Inspections[i].clear();
			Lines[i].clear();
            CPLines[i].clear();
            pEO[i].clear();
			EO[i].clear();
			EOBF[i].clear();
            F2L[i].clear();
			Alg2GLL[i].clear();
			
            Cases2GLL[i].clear();
        }
		 
        MaxDepthCPLines = MaxDepthpEO = MaxDepthEO = MaxDepthEOBF = MaxDepthF2L = 0u;

        TimeLines = TimeCPLines = TimepEO = TimeEO = TimeEOBF = TimeF2L = Time2GLL = 0.0;

		Metric = Metrics::Movements; // Default metric
	}
	
	// Search the best Lines solve algorithms with the given search depth - CP solves discarded
	// Return false if no Lines found
    bool CEOR::SearchLines(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_lines_start = std::chrono::system_clock::now();

        MaxDepthCPLines = (MaxDepth < 4u ? 4u : MaxDepth);

        // Deep search for Lines
		DS.Clear();
        DS.SetScramble(Scramble);
		
        DS.AddToOptionalPieces(Pgr::CPLINE_UF); // CPLINE_RB
		DS.AddToOptionalPieces(Pgr::CPLINE_UB); // CPLINE_LF
		DS.AddToOptionalPieces(Pgr::CPLINE_UR); // CPLINE_BL
		DS.AddToOptionalPieces(Pgr::CPLINE_UL); // CPLINE_FR

		DS.AddToOptionalPieces(Pgr::CPLINE_DF); // CPLINE_LB
		DS.AddToOptionalPieces(Pgr::CPLINE_DB); // CPLINE_RF
		DS.AddToOptionalPieces(Pgr::CPLINE_DR); // CPLINE_FL
		DS.AddToOptionalPieces(Pgr::CPLINE_DL); // CPLINE_BR

		DS.AddToOptionalPieces(Pgr::CPLINE_FU); // CPLINE_LD
		DS.AddToOptionalPieces(Pgr::CPLINE_FD); // CPLINE_RU
		DS.AddToOptionalPieces(Pgr::CPLINE_BU); // CPLINE_RD
		DS.AddToOptionalPieces(Pgr::CPLINE_BD); // CPLINE_LU

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
        DS.AddSearchLevel(L_Check); // Level 2
        DS.AddSearchLevel(L_Check); // Level 3
        for (uint l = 4u; l < MaxDepthCPLines; l++) DS.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

        DS.UpdateRootData();
		// DS.SetMinDeep(DSLine.GetMaxDepth() - 2u);

        DS.Run(Cores);

        Cores = DS.GetUsedCores(); // Update to the real number of cores used

        EvaluateLines(DS.GetSolves(), MaxSolves);

        const std::chrono::duration<double> lines_elapsed_seconds = std::chrono::system_clock::now() - time_lines_start;
        TimeLines = lines_elapsed_seconds.count();

        return !DS.GetSolves().empty();
	}
	
	// Search the best CP solve algorithms
	void CEOR::SearchCP()
    {
        const auto time_cp_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

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

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			CPLines[sp].clear();

			Pgr LINE;

			switch (spin)
			{
			case Spn::UF: LINE = Pgr::CPLINE_UF; break;
			case Spn::UB: LINE = Pgr::CPLINE_UB; break;
			case Spn::UR: LINE = Pgr::CPLINE_UR; break;
			case Spn::UL: LINE = Pgr::CPLINE_UL; break;

			case Spn::DF: LINE = Pgr::CPLINE_DF; break;
			case Spn::DB: LINE = Pgr::CPLINE_DB; break;
			case Spn::DR: LINE = Pgr::CPLINE_DR; break;
			case Spn::DL: LINE = Pgr::CPLINE_DL; break;

			case Spn::FU: LINE = Pgr::CPLINE_FU; break;
			case Spn::FD: LINE = Pgr::CPLINE_FD; break;
			case Spn::FR: LINE = Pgr::CPLINE_FR; break;
			case Spn::FL: LINE = Pgr::CPLINE_FL; break;
						  
			case Spn::BU: LINE = Pgr::CPLINE_BU; break;
			case Spn::BD: LINE = Pgr::CPLINE_BD; break;
			case Spn::BR: LINE = Pgr::CPLINE_BR; break;
			case Spn::BL: LINE = Pgr::CPLINE_BL; break;
						  
			case Spn::RU: LINE = Pgr::CPLINE_RU; break;
			case Spn::RD: LINE = Pgr::CPLINE_RD; break;
			case Spn::RF: LINE = Pgr::CPLINE_RF; break;
			case Spn::RB: LINE = Pgr::CPLINE_RB; break;
						  
			case Spn::LU: LINE = Pgr::CPLINE_LU; break;
			case Spn::LD: LINE = Pgr::CPLINE_LD; break;
			case Spn::LF: LINE = Pgr::CPLINE_LF; break;
			case Spn::LB: LINE = Pgr::CPLINE_LB; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < Lines[sp].size(); n++)
			{
                CPLines[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(Lines[sp][n]);
				Cube CubeYruRU(AlgStart);

				if (!IsYruRULineBuilt(CubeYruRU, spin)) continue;

				// Deep search for CP
				DS.Clear();
                DS.SetScramble(AlgStart);

				DS.AddToMandatoryPieces(LINE);

				DS.AddSearchLevel(L_Root);
				for (uint l = 2u; l < 4u; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

                Cores = DS.GetUsedCores(); // Update to the real number of cores used

				std::vector<Algorithm> Solves;
			
                EvaluateYruRUCPLinesResult(Solves, 1u, DS.GetSolves(), CubeYruRU, spin);

				if (!Solves.empty()) CPLines[sp][n] = Solves[0];
			}
        }

        const std::chrono::duration<double> cp_elapsed_seconds = std::chrono::system_clock::now() - time_cp_start;
        TimeCPLines = cp_elapsed_seconds.count();
    }

    // Search the best CP-Lines solve algorithms with the given search depth
	// Return false if no CP-Lines found
    bool CEOR::SearchCPLines(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_cplines_start = std::chrono::system_clock::now();

        MaxDepthCPLines = (MaxDepth < 4u ? 4u : MaxDepth);

        // Deep search for CP-Line
		DS.Clear();
        DS.SetScramble(Scramble);
		
        DS.AddToOptionalPieces(Pgr::CPLINE_UF); // CPLINE_RB
		DS.AddToOptionalPieces(Pgr::CPLINE_UB); // CPLINE_LF
		DS.AddToOptionalPieces(Pgr::CPLINE_UR); // CPLINE_BL
		DS.AddToOptionalPieces(Pgr::CPLINE_UL); // CPLINE_FR

		DS.AddToOptionalPieces(Pgr::CPLINE_DF); // CPLINE_LB
		DS.AddToOptionalPieces(Pgr::CPLINE_DB); // CPLINE_RF
		DS.AddToOptionalPieces(Pgr::CPLINE_DR); // CPLINE_FL
		DS.AddToOptionalPieces(Pgr::CPLINE_DL); // CPLINE_BR

		DS.AddToOptionalPieces(Pgr::CPLINE_FU); // CPLINE_LD
		DS.AddToOptionalPieces(Pgr::CPLINE_FD); // CPLINE_RU
		DS.AddToOptionalPieces(Pgr::CPLINE_BU); // CPLINE_RD
		DS.AddToOptionalPieces(Pgr::CPLINE_BD); // CPLINE_LU

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
        DS.AddSearchLevel(L_Check); // Level 2
        DS.AddSearchLevel(L_Check); // Level 3
        for (uint l = 4u; l < MaxDepthCPLines; l++) DS.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

        DS.UpdateRootData();
		// DS.SetMinDeep(DSCPLine.GetMaxDepth() - 2u);

        DS.Run(Cores);

        Cores = DS.GetUsedCores(); // Update to the real number of cores used

        EvaluateCPLines(DS.GetSolves(), MaxSolves);

        const std::chrono::duration<double> cplines_elapsed_seconds = std::chrono::system_clock::now() - time_cplines_start;
        TimeCPLines = cplines_elapsed_seconds.count();

        return !DS.GetSolves().empty();
	}
	
	// Search the best lines solve algorithms from an algorithms vector
    void CEOR::EvaluateLines(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best line solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			Stp T1, T2;
			Algorithm Insp;

			EvaluateYruRULinesResult(Lines[sp], MaxSolves, Solves, CubeBase, spin);

			Inspections[sp].clear();

			if (!Lines[sp].empty())
			{
				Insp.Clear();
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				if (T1 != Stp::NONE) Insp.Append(T1);
				if (T2 != Stp::NONE) Insp.Append(T2);

				for (auto& line : Lines[sp])
				{
					if (T1 != Stp::NONE) line.TransformTurn(T1);
					if (T2 != Stp::NONE) line.TransformTurn(T2);
					Inspections[sp].push_back(Insp);
				}
			}
        }
	}
	
	// Search the best CP-Lines solve algorithms from an algorithms vector
    void CEOR::EvaluateCPLines(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best CP-Line solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			Stp T1, T2;
			Algorithm Insp;

			EvaluateYruRUCPLinesResult(CPLines[sp], MaxSolves, Solves, CubeBase, spin);

			Inspections[sp].clear();

			if (!CPLines[sp].empty())
			{
				Insp.Clear();
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				if (T1 != Stp::NONE) Insp.Append(T1);
				if (T2 != Stp::NONE) Insp.Append(T2);

				for (auto& cpline : CPLines[sp])
				{
					if (T1 != Stp::NONE) cpline.TransformTurn(T1);
					if (T2 != Stp::NONE) cpline.TransformTurn(T2);
					Inspections[sp].push_back(Insp);
				}
			}
        }
	}

    // Search the best pEO extension solve algorithm
	void CEOR::SearchpEO(const uint MaxDepth)
    {
        const auto time_peo_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthpEO = (MaxDepth <= 4u ? 4u : MaxDepth);

        const SearchUnit URoot(SequenceTypes::DOUBLE, Sst::YRURU_urUR);
        const SearchUnit U_pEO(SequenceTypes::SINGLE, Sst::YRURU_urUR);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(URoot);

		SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U_pEO);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			pEO[sp].clear();

			Pgr B1;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; break;
			case Spn::UB: B1 = Pgr::UB_B1; break;
			case Spn::UR: B1 = Pgr::UR_B1; break;
			case Spn::UL: B1 = Pgr::UL_B1; break;

			case Spn::DF: B1 = Pgr::DF_B1; break;
			case Spn::DB: B1 = Pgr::DB_B1; break;
			case Spn::DR: B1 = Pgr::DR_B1; break;
			case Spn::DL: B1 = Pgr::DL_B1; break;

			case Spn::FU: B1 = Pgr::FU_B1; break;
			case Spn::FD: B1 = Pgr::FD_B1; break;
			case Spn::FR: B1 = Pgr::FR_B1; break;
			case Spn::FL: B1 = Pgr::FL_B1; break;

			case Spn::BU: B1 = Pgr::BU_B1; break;
			case Spn::BD: B1 = Pgr::BD_B1; break;
			case Spn::BR: B1 = Pgr::BR_B1; break;
			case Spn::BL: B1 = Pgr::BL_B1; break;

			case Spn::RU: B1 = Pgr::RU_B1; break;
			case Spn::RD: B1 = Pgr::RD_B1; break;
			case Spn::RF: B1 = Pgr::RF_B1; break;
			case Spn::RB: B1 = Pgr::RB_B1; break;

			case Spn::LU: B1 = Pgr::LU_B1; break;
			case Spn::LD: B1 = Pgr::LD_B1; break;
			case Spn::LF: B1 = Pgr::LF_B1; break;
			case Spn::LB: B1 = Pgr::LB_B1; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < CPLines[sp].size(); n++)
			{
                pEO[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!Lines[sp].empty()) AlgStart.Append(Lines[sp][n]);
				AlgStart.Append(CPLines[sp][n]);
				Cube CubeYruRU(AlgStart);

				if (!IsYruRUCPBuilt(CubeYruRU))
				{
					pEO[sp].push_back(Algorithm(""));
					continue;
				}

				// Deep search for pEO extension
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(B1);

				DS.AddSearchLevel(L_Root);
				for (uint l = 2u; l < MaxDepthpEO; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();
			
                EvaluateCEORpEOResult(pEO[sp][n], DS.GetSolves(), CubeYruRU, spin);
			}
        }

        const std::chrono::duration<double> peo_elapsed_seconds = std::chrono::system_clock::now() - time_peo_start;
        TimepEO = peo_elapsed_seconds.count();
    }
	
	// Complete the EO-BF in two steps - EO first
	void CEOR::SearchEO(const uint MaxDepth)
	{
		auto time_eo_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthEO = (MaxDepth <= 4u ? 4u : MaxDepth);

        const SearchUnit URoot(SequenceTypes::DOUBLE, Sst::YRURU_rUR);
        const SearchUnit U_U(SequenceTypes::SINGLE, Sst::YRURU_rUR);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(URoot);

		SearchLevel L_EO_Check(SearchCheck::CHECK);
		L_EO_Check.Add(U_U);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			EO[sp].clear();

			Pgr B1;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; break;
			case Spn::UB: B1 = Pgr::UB_B1; break;
			case Spn::UR: B1 = Pgr::UR_B1; break;
			case Spn::UL: B1 = Pgr::UL_B1; break;

			case Spn::DF: B1 = Pgr::DF_B1; break;
			case Spn::DB: B1 = Pgr::DB_B1; break;
			case Spn::DR: B1 = Pgr::DR_B1; break;
			case Spn::DL: B1 = Pgr::DL_B1; break;

			case Spn::FU: B1 = Pgr::FU_B1; break;
			case Spn::FD: B1 = Pgr::FD_B1; break;
			case Spn::FR: B1 = Pgr::FR_B1; break;
			case Spn::FL: B1 = Pgr::FL_B1; break;

			case Spn::BU: B1 = Pgr::BU_B1; break;
			case Spn::BD: B1 = Pgr::BD_B1; break;
			case Spn::BR: B1 = Pgr::BR_B1; break;
			case Spn::BL: B1 = Pgr::BL_B1; break;

			case Spn::RU: B1 = Pgr::RU_B1; break;
			case Spn::RD: B1 = Pgr::RD_B1; break;
			case Spn::RF: B1 = Pgr::RF_B1; break;
			case Spn::RB: B1 = Pgr::RB_B1; break;

			case Spn::LU: B1 = Pgr::LU_B1; break;
			case Spn::LD: B1 = Pgr::LD_B1; break;
			case Spn::LF: B1 = Pgr::LF_B1; break;
			case Spn::LB: B1 = Pgr::LB_B1; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < CPLines[sp].size(); n++)
			{
				EO[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!Lines[sp].empty()) AlgStart.Append(Lines[sp][n]);
				AlgStart.Append(CPLines[sp][n]);
				AlgStart.Append(pEO[sp][n]);
				Cube CubeYruRU(AlgStart);
				
				if (!IsRouxFBBuilt(CubeYruRU)) continue;
				
				// Deep search for EO
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(B1);
				DS.AddToMandatoryOrientations(Pgr::ALL_EDGES);

				DS.AddSearchLevel(L_Root);
				for (uint l = 2u; l < MaxDepthEO; l++) DS.AddSearchLevel(L_EO_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				Algorithm Solve;
				DS.EvaluateShortestResult(Solve, true);

				CubeYruRU.ApplyAlgorithm(Solve);
				if (CubeYruRU.CheckOrientation(Pgr::ALL_EDGES)) EO[sp][n] = Solve;
			}
		}

		const std::chrono::duration<double> eo_elapsed_seconds = std::chrono::system_clock::now() - time_eo_start;
		TimeEO = eo_elapsed_seconds.count();
	}

	// Complete the EO-BF in a single step or two steps as second step
    void CEOR::SearchEOBF(const uint MaxDepth)
    {
        auto time_eobf_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthEOBF = (MaxDepth <= 4u ? 4u : MaxDepth);

        const SearchUnit URoot(SequenceTypes::DOUBLE, Sst::YRURU_rUR);
        const SearchUnit U_U(SequenceTypes::SINGLE, Sst::YRURU_rUR);
		
		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(URoot);

        SearchLevel L_EOBF_Check(SearchCheck::CHECK);
        L_EOBF_Check.Add(U_U);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			EOBF[sp].clear();

			Pgr B1, BF;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::UF_B1; BF = Pgr::EOLINE_UF; break;
			case Spn::UB: B1 = Pgr::UB_B1; BF = Pgr::EOLINE_UB; break;
			case Spn::UR: B1 = Pgr::UR_B1; BF = Pgr::EOLINE_UR; break;
			case Spn::UL: B1 = Pgr::UL_B1; BF = Pgr::EOLINE_UL; break;

			case Spn::DF: B1 = Pgr::DF_B1; BF = Pgr::EOLINE_DF; break;
			case Spn::DB: B1 = Pgr::DB_B1; BF = Pgr::EOLINE_DB; break;
			case Spn::DR: B1 = Pgr::DR_B1; BF = Pgr::EOLINE_DR; break;
			case Spn::DL: B1 = Pgr::DL_B1; BF = Pgr::EOLINE_DL; break;

			case Spn::FU: B1 = Pgr::FU_B1; BF = Pgr::EOLINE_FU; break;
			case Spn::FD: B1 = Pgr::FD_B1; BF = Pgr::EOLINE_FD; break;
			case Spn::FR: B1 = Pgr::FR_B1; BF = Pgr::EOLINE_FR; break;
			case Spn::FL: B1 = Pgr::FL_B1; BF = Pgr::EOLINE_FL; break;

			case Spn::BU: B1 = Pgr::BU_B1; BF = Pgr::EOLINE_BU; break;
			case Spn::BD: B1 = Pgr::BD_B1; BF = Pgr::EOLINE_BD; break;
			case Spn::BR: B1 = Pgr::BR_B1; BF = Pgr::EOLINE_BR; break;
			case Spn::BL: B1 = Pgr::BL_B1; BF = Pgr::EOLINE_BL; break;

			case Spn::RU: B1 = Pgr::RU_B1; BF = Pgr::EOLINE_RU; break;
			case Spn::RD: B1 = Pgr::RD_B1; BF = Pgr::EOLINE_RD; break;
			case Spn::RF: B1 = Pgr::RF_B1; BF = Pgr::EOLINE_RF; break;
			case Spn::RB: B1 = Pgr::RB_B1; BF = Pgr::EOLINE_RB; break;

			case Spn::LU: B1 = Pgr::LU_B1; BF = Pgr::EOLINE_LU; break;
			case Spn::LD: B1 = Pgr::LD_B1; BF = Pgr::EOLINE_LD; break;
			case Spn::LF: B1 = Pgr::LF_B1; BF = Pgr::EOLINE_LF; break;
			case Spn::LB: B1 = Pgr::LB_B1; BF = Pgr::EOLINE_LB; break;

			default: return; // Should not happend
			}

			for (uint n = 0u; n < CPLines[sp].size(); n++)
			{
				EOBF[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!Lines[sp].empty()) AlgStart.Append(Lines[sp][n]);
				AlgStart.Append(CPLines[sp][n]);
				AlgStart.Append(pEO[sp][n]);
				if (!EO[sp].empty()) AlgStart.Append(EO[sp][n]);
				Cube CubeYruRU(AlgStart);

				if (!IsRouxFBBuilt(CubeYruRU) || (!EO[sp].empty() && !CubeYruRU.CheckOrientation(Pgr::ALL_EDGES))) continue;

				// Deep search for EO-BF
				DS.Clear();
                DS.SetScramble(AlgStart);
				DS.SetShortPolicy();

				DS.AddToMandatoryPieces(B1);
				DS.AddToMandatoryPieces(BF);
				DS.AddToMandatoryOrientations(Pgr::ALL_EDGES);

				DS.AddSearchLevel(L_Root);
				for (uint l = 2u; l < MaxDepthEOBF; l++) DS.AddSearchLevel(L_EOBF_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				std::vector<Algorithm> Solves;
                EvaluateCFOPF2LResult(Solves, 1u, DS.GetSolves(), CubeYruRU, Cube::GetDownSliceLayer(spin), Plc::BEST);

				if (!Solves.empty())
				{
					CubeYruRU.ApplyAlgorithm(Solves[0]);
					if (CubeYruRU.CheckOrientation(Pgr::ALL_EDGES)) EOBF[sp][n] = Solves[0];
				}
			}
		}
		
        const std::chrono::duration<double> eobf_elapsed_seconds = std::chrono::system_clock::now() - time_eobf_start;
        TimeEOBF = eobf_elapsed_seconds.count();
    }
	
	// F2L search
    void CEOR::SearchF2L(const uint MaxDepth)
	{
		const auto time_F2L_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepthF2L = (MaxDepth <= 4u ? 4u : MaxDepth);

        const SearchUnit U_Root(SequenceTypes::DOUBLE, Sst::YRURU_UR);
        const SearchUnit U_UR(SequenceTypes::SINGLE, Sst::YRURU_UR);

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

			for (uint n = 0u; n < CPLines[sp].size(); n++)
			{
				F2L[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!Lines[sp].empty()) AlgStart.Append(Lines[sp][n]);
				AlgStart.Append(CPLines[sp][n]);
				AlgStart.Append(pEO[sp][n]);
				if (!EO[sp].empty()) AlgStart.Append(EO[sp][n]);
				AlgStart.Append(EOBF[sp][n]);
				const Cube CubeYruRU(AlgStart);

				if (!IsRouxFBBuilt(CubeYruRU) || !CubeYruRU.CheckOrientation(Pgr::ALL_EDGES)) continue;

				if (CubeYruRU.IsSolved(DownLayer) && CubeYruRU.IsSolved(MiddleLayer) && CubeYruRU.CheckOrientation(Pgr::ALL_EDGES)) continue; // F2L already solved

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
    
    // 2GLL search
    void CEOR::Search2GLL(const Plc Pol)
	{
        const auto time_2GLL_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Alg2GLL[sp].clear();
			Cases2GLL[sp].clear();

			for (uint n = 0u; n < CPLines[sp].size(); n++)
			{
				Alg2GLL[sp].push_back(Algorithm(""));
				Cases2GLL[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				if (!Lines[sp].empty()) Alg += Lines[sp][n];
				Alg += CPLines[sp][n];
				Alg += pEO[sp][n];
				if (!EO[sp].empty()) Alg.Append(EO[sp][n]);
				Alg += EOBF[sp][n];
				Alg += F2L[sp][n];

				Cube CubeYruRU = CubeBase;
				CubeYruRU.ApplyAlgorithm(Alg);

				const Lyr DownLayer = Cube::GetDownSliceLayer(spin);

				if (!CubeYruRU.IsSolved(DownLayer) || 
					!CubeYruRU.IsSolved(Cube::AdjacentLayer(DownLayer)) ||
					!CubeYruRU.CheckOrientation(Pgr::ALL_EDGES)) continue;

				Stp AUFStep;

				SolveLL(Alg2GLL[sp][n], Cases2GLL[sp][n], AUFStep, Algset_2GLL, Pol, Metric, CubeYruRU);

				Alg2GLL[sp][n].Append(AUFStep);
			}
		}

        const std::chrono::duration<double> _2GLL_elapsed_seconds = std::chrono::system_clock::now() - time_2GLL_start;
        Time2GLL = _2GLL_elapsed_seconds.count();
	}

	// Set regrips
	void CEOR::SetRegrips()
	{
		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			if (!CheckSolveConsistency(spin)) continue;

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				if (Lines[sp].empty())
				{
					CPLines[sp][n] = CPLines[sp][n].GetRegrip();

					if (Algorithm::IsTurn(CPLines[sp][n].First()))
					{
						Inspections[sp][n].AppendShrink(CPLines[sp][n].First());
						if (Inspections[sp][n].GetSize() == 3u) Inspections[sp][n] = Inspections[sp][n].GetCancellations();
						CPLines[sp][n].EraseFirst();
					}
				}
				else
				{
					Lines[sp][n] = Lines[sp][n].GetRegrip();
					// CPLines[sp][n] = CPLines[sp][n].GetRegrip();

					if (Algorithm::IsTurn(Lines[sp][n].First()))
					{
						Inspections[sp][n].AppendShrink(Lines[sp][n].First());
						if (Inspections[sp][n].GetSize() == 3u) Inspections[sp][n] = Inspections[sp][n].GetCancellations();
						Lines[sp][n].EraseFirst();
					}

					if (Algorithm::IsTurn(Lines[sp][n].Last()))
					{
						CPLines[sp][n].Insert(0u, Lines[sp][n].Last());
						while (CPLines[sp][n].Shrink());
						Lines[sp][n].EraseLast();
					}
				}
			}
		}
	}

    // Get a solve report
    std::string CEOR::GetReport(const bool cancellations, bool debug) const
    {
        std::string Report, ReportLine;
        // Report = "CEOR (YruRU): Scramble [" + std::to_string(Scramble.GetNumSteps()) + "] " + GetTextScramble();
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
				
				if (!Lines[sp].empty()) // Line + CP in two steps
				{
					C.ApplyAlgorithm(Lines[sp][n]);
					
					if (IsYruRULineBuilt(C, spin))
					{
						ReportLine += "[" + Cube::GetSpinText(spin) + "|" + Algorithm::GetMetricValue(GetMetricSolve(spin, n));
						if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
						ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
						if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += "(" + Lines[sp][n].ToString() + ")";
					}
					else
					{
						if (debug)
						{
							ReportLine += "[" + Cube::GetSpinText(spin) + "]: ";
							if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
							ReportLine += "Line not built in " + std::to_string(MaxDepthCPLines) + " steps";
							if (!Lines[sp][n].Empty()) ReportLine += ": (" + Lines[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
						}
						if (debug) Report += ReportLine;
						continue;
					}
					
					C.ApplyAlgorithm(CPLines[sp][n]);

					if (IsYruRUCPBuilt(C)) ReportLine += "(" + CPLines[sp][n].ToString() + ")";
					else
					{
						if (debug)
						{
							ReportLine += "CP not built in " + std::to_string(4u) + " steps";
							if (!CPLines[sp][n].Empty()) ReportLine += ": (" + CPLines[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
						}
						if (debug) Report += ReportLine;
						continue;
					}
				}
				else // Line + CP in a single step
				{
					C.ApplyAlgorithm(CPLines[sp][n]);

					if (IsYruRUCPBuilt(C))
					{
						ReportLine += "[" + Cube::GetSpinText(spin) + "|" + Algorithm::GetMetricValue(GetMetricSolve(spin, n));
						if (cancellations) ReportLine += "(" + Algorithm::GetMetricValue(GetMetricCancellations(spin, n)) + ")";
						ReportLine += " " + Algorithm::GetMetricString(Metric) +  "]: ";
						if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
						ReportLine += "(" + CPLines[sp][n].ToString() + ")";
					}
					else
					{
						if (debug)
						{
							ReportLine += "[" + Cube::GetSpinText(spin) + "]: ";
							if (!Inspections[sp][n].Empty()) ReportLine += "(" + Inspections[sp][n].ToString() + ") ";
							ReportLine += "CP-Line not built in " + std::to_string(MaxDepthCPLines) + " steps";
							if (!CPLines[sp][n].Empty()) ReportLine += ": (" + CPLines[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
						}
						if (debug) Report += ReportLine;
						continue;
					}
				}
				
				C.ApplyAlgorithm(pEO[sp][n]);

				if (IsRouxFBBuilt(C)) ReportLine += " (" + pEO[sp][n].ToString() + ")";
				else
				{
					ReportLine += "pEO-extension not built in " + std::to_string(MaxDepthpEO) + " steps";
					if (!pEO[sp][n].Empty()) ReportLine += ": (" + pEO[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				if (!EO[sp].empty())
				{
					C.ApplyAlgorithm(EO[sp][n]);
					if (C.CheckOrientation(Pgr::ALL_EDGES)) ReportLine += " (" + EO[sp][n].ToString() + ")";
					else
					{
						ReportLine += "EO not built in " + std::to_string(MaxDepthEO) + " steps";
						if (!EO[sp][n].Empty()) ReportLine += ": (" + EO[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}
				}

				C.ApplyAlgorithm(EOBF[sp][n]);

				if (C.CheckOrientation(Pgr::ALL_EDGES)) ReportLine += " (" + EOBF[sp][n].ToString() + ")";
				else
				{
					if (EO[sp].empty()) ReportLine += "EO-BF not built in " + std::to_string(MaxDepthEOBF) + " steps";
					else ReportLine += "BF not built in " + std::to_string(MaxDepthEOBF) + " steps";
					if (!EOBF[sp][n].Empty()) ReportLine += ": (" + EOBF[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(F2L[sp][n]);

				if (IsF2LBuilt(C, Cube::GetDownSliceLayer(spin))) ReportLine += " (" + F2L[sp][n].ToString() + ")";
				else
				{
					ReportLine += "F2L not built in " + std::to_string(MaxDepthF2L) + " steps";
					if (!F2L[sp][n].Empty()) ReportLine += ": (" + F2L[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(Alg2GLL[sp][n]);
                if (debug) ReportLine += " {2GLL: " + Cases2GLL[sp][n] + "}";
				if (!Alg2GLL[sp][n].Empty()) ReportLine += " (" + Alg2GLL[sp][n].ToString() + ")";

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
    std::string CEOR::GetTimeReport() const
    {
        std::string Report;

        Report += "Total time: " + std::to_string(GetFullTime()) + " s\n";
        if (TimeLines == 0.0) Report += "CP-Lines time: " + std::to_string(GetTimeCPLines()) + " s\n";
		else
		{
            Report += "Lines time: " + std::to_string(GetTimeLines()) + " s\n";
            Report += "CP time: " + std::to_string(GetTimeCPLines()) + " s\n";
		}
        Report += "pEO-extension time: " + std::to_string(GetTimepEO()) + " s\n";
        if (TimeEO == 0.0) Report += "EO-BF time: " + std::to_string(GetTimeEOBF()) + " s\n";
		else
		{
            Report += "EO time: " + std::to_string(GetTimeEO()) + " s\n";
            Report += "BF time: " + std::to_string(GetTimeEOBF()) + " s\n";
		}
        Report += "F2L time: " + std::to_string(GetTimeF2L()) + " s\n";
        Report += "2GLL time: " + std::to_string(GetTime2GLL()) + " s\n";
        Report += "Threads: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : DeepSearch::GetSystemCores()) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string CEOR::GetReport(const Spn sp, const uint n) const
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
        // Report  = "CEOR (YruRU) search with orientation " + Cube::GetSpinText(sp);
        // Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += GetTextInspection(sp,n) + " // Inspection [" + C.GetSpinText() + "]\n";

		if (!Lines[si].empty()) // Lines + CP
		{
			C.ApplyAlgorithm(Lines[si][n]);
			if (!IsYruRULineBuilt(C, sp))
			{
				Report += "Line not built in " + std::to_string(MaxDepthCPLines) + " movements\n";
				return Report;
			}
            Report += GetTextLines(sp, n) + " // Line (" + Algorithm::GetMetricValue(GetMetricLines(sp, n)) + ")\n";
			
			C.ApplyAlgorithm(CPLines[si][n]);
			if (!IsYruRUCPBuilt(C))
			{
				Report += "CP not built in " + std::to_string(4u) + " movements\n";
				return Report;
			}
            Report += GetTextCPLines(sp, n) + " // CP (" + Algorithm::GetMetricValue(GetMetricCPLines(sp, n)) + ")\n";
		}
		else // CPLines
		{
			C.ApplyAlgorithm(CPLines[si][n]);
			if (!IsYruRUCPBuilt(C))
			{
				Report += "CP-Line not built in " + std::to_string(MaxDepthCPLines) + " movements\n";
				return Report;
			}
            Report += GetTextCPLines(sp, n) + " // CP-Line (" + Algorithm::GetMetricValue(GetMetricCPLines(sp, n)) + ")\n";
		}
		C.ApplyAlgorithm(pEO[si][n]);
		if (!IsRouxFBBuilt(C))
		{
			Report += "pEO-extension not built in " + std::to_string(MaxDepthpEO) + " movements\n";;
			return Report;
		}
        if (!pEO[si][n].Empty()) Report += GetTextpEO(sp, n) + " // pEO-extension (" + Algorithm::GetMetricValue(GetMetricpEO(sp, n)) + ")\n";

		if (!EO[si].empty())
		{
			C.ApplyAlgorithm(EO[si][n]);
			if (!C.CheckOrientation(Pgr::ALL_EDGES))
			{
				Report += "EO not built in " + std::to_string(MaxDepthEO) + " movements\n";
				return Report;
			}
            if (!EO[si][n].Empty()) Report += GetTextEO(sp, n) + " // EO (" + Algorithm::GetMetricValue(GetMetricEO(sp, n)) + ")\n";
		}

		C.ApplyAlgorithm(EOBF[si][n]);
		if (!C.CheckOrientation(Pgr::ALL_EDGES))
		{
			if (EO[si].empty()) Report += "EO-BF not built in " + std::to_string(MaxDepthEOBF) + " movements\n";
			else Report += "BF not built!\n";
			return Report;
		}
		if (!EOBF[si][n].Empty())
		{
            if (EO[si].empty()) Report += GetTextEOBF(sp, n) + " // EO-BF (" + Algorithm::GetMetricValue(GetMetricEOBF(sp, n)) + ")\n";
            else Report += GetTextEOBF(sp, n) + " // BF (" + Algorithm::GetMetricValue(GetMetricEOBF(sp, n)) + ")\n";
		}

		C.ApplyAlgorithm(F2L[si][n]);
		if (!IsF2LBuilt(C, Cube::GetDownSliceLayer(C.GetSpin())))
		{
			Report += "F2L not built in " + std::to_string(MaxDepthF2L) + " movements\n";
			return Report;
		}
        if (!F2L[si][n].Empty()) Report += GetTextF2L(sp, n) + " // F2L (" + Algorithm::GetMetricValue(GetMetricF2L(sp, n)) + ")\n";

		C.ApplyAlgorithm(Alg2GLL[si][n]);
        if (!Alg2GLL[si][n].Empty()) Report += GetText2GLL(sp, n) + " // 2GLL (" + Algorithm::GetMetricValue(GetMetric2GLL(sp, n)) + ")\n";

        // Show summary
        Report += "\nMetric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";

		Report += "2GLL case: " + GetText2GLLCase(sp, n) + "\n";

		return Report;
	}

	// Check if the solves for the given cross layer are consistent (all needed algorithms are present)
	bool CEOR::CheckSolveConsistency(const Spn Spin) const
	{
		const int si = static_cast<int>(Spin); // Spin index

		if (Inspections[si].empty()) return false;

		auto n = Inspections[si].size();

		return (Lines[si].empty() || Lines[si].size() == n) && (CPLines[si].size() == n) && (pEO[si].size() == n) && 
			   (EO[si].empty() || EO[si].size() == n) && (EOBF[si].size() == n) && (F2L[si].size() == n) && (Alg2GLL[si].size() == n);
	}

	// Get the full solve
    Algorithm CEOR::GetFullSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || n >= Inspections[si].size()) return 0u;

		Algorithm A = Inspections[si][n];
		if (!Lines[si].empty()) A += Lines[si][n];
		A += CPLines[si][n];
		A += pEO[si][n];
		if (!EO[si].empty()) A += EO[si][n];
		A += EOBF[si][n];
		A += F2L[si][n];
		A += Alg2GLL[si][n];
		
		return A;
	}
}
