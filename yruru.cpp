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

#include "yruru.h"
#include "roux.h"
#include "cfop.h"
#include "collection.h"

#include <chrono>
#include <algorithm>

namespace grcube3
{	
	// Reset the search results
	void YruRU::Reset()
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

		SearchSpins.clear();
		for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s));
		
		Metric = Metrics::Movements; // Default metric
	}
	
	// Search the best Lines solve algorithms with the given search depth - CP solves discarded
	// Return false if no Lines found
    bool YruRU::SearchLines(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_lines_start = std::chrono::system_clock::now();

        MaxDepthCPLines = (MaxDepth < 4u ? 4u : MaxDepth);

        DeepSearch DSLines(Scramble); // Deep search for Lines
		
        DSLines.AddToOptionalPieces(Pgr::CPLINE_UF); // CPLINE_RB
		DSLines.AddToOptionalPieces(Pgr::CPLINE_UB); // CPLINE_LF
		DSLines.AddToOptionalPieces(Pgr::CPLINE_UR); // CPLINE_BL
		DSLines.AddToOptionalPieces(Pgr::CPLINE_UL); // CPLINE_FR

		DSLines.AddToOptionalPieces(Pgr::CPLINE_DF); // CPLINE_LB
		DSLines.AddToOptionalPieces(Pgr::CPLINE_DB); // CPLINE_RF
		DSLines.AddToOptionalPieces(Pgr::CPLINE_DR); // CPLINE_FL
		DSLines.AddToOptionalPieces(Pgr::CPLINE_DL); // CPLINE_BR

		DSLines.AddToOptionalPieces(Pgr::CPLINE_FU); // CPLINE_LD
		DSLines.AddToOptionalPieces(Pgr::CPLINE_FD); // CPLINE_RU
		DSLines.AddToOptionalPieces(Pgr::CPLINE_BU); // CPLINE_RD
		DSLines.AddToOptionalPieces(Pgr::CPLINE_BD); // CPLINE_LU

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
        const SearchUnit URoot(SequenceType::DOUBLE);
        const SearchUnit U(SequenceType::SINGLE);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(URoot);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U);

        SearchLevel L_NoCheck(SearchCheck::NO_CHECK);
        L_NoCheck.Add(U);

        DSLines.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        DSLines.AddSearchLevel(L_Check); // Level 2
        DSLines.AddSearchLevel(L_Check); // Level 3
        for (uint l = 4u; l < MaxDepthCPLines; l++) DSLines.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

        DSLines.UpdateRootData();
		// DSLines.SetMinDeep(DSLine.GetMaxDepth() - 2u);

        DSLines.Run(Cores);

        Cores = DSLines.GetCoresUsed(); // Update to the real number of cores used

        EvaluateLines(DSLines.Solves, MaxSolves);

        const std::chrono::duration<double> lines_elapsed_seconds = std::chrono::system_clock::now() - time_lines_start;
        TimeLines = lines_elapsed_seconds.count();

        return !DSLines.Solves.empty();
	}
	
	// Search the best CP solve algorithms
	void YruRU::SearchCP()
    {
        const auto time_cp_start = std::chrono::system_clock::now();

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
        const SearchUnit URoot(SequenceType::DOUBLE);
        const SearchUnit U(SequenceType::SINGLE);

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
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(Lines[sp][n]);
				Cube CubeYruRU(AlgStart);

				if (!IsLineBuilt(CubeYruRU, spin))
				{
					CPLines[sp].push_back(Algorithm(""));
					continue;
				}

				DeepSearch DSCP(AlgStart); // Deep search for CP

				DSCP.AddToMandatoryPieces(LINE);

				DSCP.AddSearchLevel(L_Root);
				for (uint l = 2u; l < 4u; l++) DSCP.AddSearchLevel(L_Check); // Add needed search levels

				DSCP.UpdateRootData();

				DSCP.Run(Cores);

				Cores = DSCP.GetCoresUsed(); // Update to the real number of cores used

				std::vector<Algorithm> Solves;
			
				EvaluateCPLinesResult(Solves, 1u, DSCP.Solves, CubeYruRU, spin);

				if (Solves.empty()) CPLines[sp].push_back("");
				else CPLines[sp].push_back(Solves[0]);
			}
        }

        const std::chrono::duration<double> cp_elapsed_seconds = std::chrono::system_clock::now() - time_cp_start;
        TimeCPLines = cp_elapsed_seconds.count();
    }

    // Search the best CP-Lines solve algorithms with the given search depth
	// Return false if no CP-Lines found
    bool YruRU::SearchCPLines(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_cplines_start = std::chrono::system_clock::now();

        MaxDepthCPLines = (MaxDepth < 4u ? 4u : MaxDepth);

        DeepSearch DSCPLines(Scramble); // Deep search for CP-Line
		
        DSCPLines.AddToOptionalPieces(Pgr::CPLINE_UF); // CPLINE_RB
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_UB); // CPLINE_LF
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_UR); // CPLINE_BL
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_UL); // CPLINE_FR

		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_DF); // CPLINE_LB
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_DB); // CPLINE_RF
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_DR); // CPLINE_FL
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_DL); // CPLINE_BR

		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_FU); // CPLINE_LD
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_FD); // CPLINE_RU
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_BU); // CPLINE_RD
		DSCPLines.AddToOptionalPieces(Pgr::CPLINE_BD); // CPLINE_LU

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
        const SearchUnit URoot(SequenceType::DOUBLE);
        const SearchUnit U(SequenceType::SINGLE);

        SearchLevel L_Root(SearchCheck::NO_CHECK);
        L_Root.Add(URoot);

        SearchLevel L_Check(SearchCheck::CHECK);
        L_Check.Add(U);

        SearchLevel L_NoCheck(SearchCheck::NO_CHECK);
        L_NoCheck.Add(U);

        DSCPLines.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        DSCPLines.AddSearchLevel(L_Check); // Level 2
        DSCPLines.AddSearchLevel(L_Check); // Level 3
        for (uint l = 4u; l < MaxDepthCPLines; l++) DSCPLines.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

        DSCPLines.UpdateRootData();
		// DSCPLines.SetMinDeep(DSCPLine.GetMaxDepth() - 2u);

        DSCPLines.Run(Cores);

        Cores = DSCPLines.GetCoresUsed(); // Update to the real number of cores used

        EvaluateCPLines(DSCPLines.Solves, MaxSolves);

        const std::chrono::duration<double> cplines_elapsed_seconds = std::chrono::system_clock::now() - time_cplines_start;
        TimeCPLines = cplines_elapsed_seconds.count();

        return !DSCPLines.Solves.empty();
	}
	
	// Search the best lines solve algorithms from an algorithms vector
    void YruRU::EvaluateLines(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best line solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			EvaluateLinesResult(Lines[sp], MaxSolves, Solves, CubeBase, spin);
        }

		for (int sp = 0; sp < 24; sp++)
		{
			Spn spin = static_cast<Spn>(sp);

			for (auto& line : Lines[sp])
			{
				Stp T1, T2;
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				Algorithm Insp;
				if (T1 != Stp::NONE) { Insp.Append(T1); line.TransformTurn(T1); }
				if (T2 != Stp::NONE) { Insp.Append(T2); line.TransformTurn(T2); }
 
				Inspections[sp].push_back(Insp);
			}
		}
	}
	
	// Search the best CP-Lines solve algorithms from an algorithms vector
    void YruRU::EvaluateCPLines(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best CP-Line solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			EvaluateCPLinesResult(CPLines[sp], MaxSolves, Solves, CubeBase, spin);
        }

		for (int sp = 0; sp < 24; sp++)
		{
			Spn spin = static_cast<Spn>(sp);

			for (auto& cp : CPLines[sp])
			{
				Stp T1, T2;
				Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
				Algorithm Insp;
				if (T1 != Stp::NONE) { Insp.Append(T1); cp.TransformTurn(T1); }
				if (T2 != Stp::NONE) { Insp.Append(T2); cp.TransformTurn(T2); }
 
				Inspections[sp].push_back(Insp);
			}
		}
	}

    // Search the best pEO extension solve algorithm
	void YruRU::SearchpEO(const uint MaxDepth)
    {
        const auto time_peo_start = std::chrono::system_clock::now();

		MaxDepthpEO = (MaxDepth <= 4u ? 4u : MaxDepth);

		const SearchUnit URoot(SequenceType::DOUBLE, Sst::YRURU_urUR);
		const SearchUnit U_pEO(SequenceType::SINGLE, Sst::YRURU_urUR);

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
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!Lines[sp].empty()) AlgStart.Append(Lines[sp][n]);
				AlgStart.Append(CPLines[sp][n]);
				Cube CubeYruRU(AlgStart);

				if (!IsCPBuilt(CubeYruRU))
				{
					pEO[sp].push_back(Algorithm(""));
					continue;
				}

				DeepSearch DSpEO(AlgStart, Plc::SHORT); // Deep search for pEO extension

				DSpEO.AddToMandatoryPieces(B1);

				DSpEO.AddSearchLevel(L_Root);
				for (uint l = 2u; l < MaxDepthpEO; l++) DSpEO.AddSearchLevel(L_Check); // Add needed search levels

				DSpEO.UpdateRootData();

				DSpEO.Run(Cores);

				Cores = DSpEO.GetCoresUsed(); // Update to the real number of cores used

				Algorithm Solve;
				
				EvaluatepEOResult(Solve, DSpEO.Solves, CubeYruRU, spin);

				pEO[sp].push_back(Solve);
			}
        }

        const std::chrono::duration<double> peo_elapsed_seconds = std::chrono::system_clock::now() - time_peo_start;
        TimepEO = peo_elapsed_seconds.count();
    }
	
	// Complete the EO-BF in two steps - EO first
	void YruRU::SearchEO(const uint MaxDepth)
	{
		auto time_eo_start = std::chrono::system_clock::now();

		MaxDepthEO = (MaxDepth <= 4u ? 4u : MaxDepth);

		const SearchUnit URoot(SequenceType::DOUBLE, Sst::YRURU_rUR);
		const SearchUnit U_U(SequenceType::SINGLE, Sst::YRURU_rUR);

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
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!Lines[sp].empty()) AlgStart.Append(Lines[sp][n]);
				AlgStart.Append(CPLines[sp][n]);
				AlgStart.Append(pEO[sp][n]);
				Cube CubeYruRU(AlgStart);
				
				if (!Roux::IsFBBuilt(CubeYruRU))
				{
					EO[sp].push_back(Algorithm(""));
					continue;
				}
				
				DeepSearch DSEO(AlgStart, Plc::SHORT); // Deep search for EO

				DSEO.AddToMandatoryPieces(B1);
				DSEO.AddToMandatoryOrientations(Pgr::ALL_EDGES);

				DSEO.AddSearchLevel(L_Root);
				for (uint l = 2u; l < MaxDepthEO; l++) DSEO.AddSearchLevel(L_EO_Check); // Add needed search levels

				DSEO.UpdateRootData();

				DSEO.Run(Cores);

				Cores = DSEO.GetCoresUsed(); // Update to the real number of cores used

				Algorithm Solve;
				DSEO.EvaluateShortestResult(Solve, true);

				CubeYruRU.ApplyAlgorithm(Solve);
				if (CubeYruRU.CheckOrientation(Pgr::ALL_EDGES)) EO[sp].push_back(Solve);
				else EO[sp].push_back(Algorithm(""));
			}
		}

		const std::chrono::duration<double> eo_elapsed_seconds = std::chrono::system_clock::now() - time_eo_start;
		TimeEO = eo_elapsed_seconds.count();
	}

	// Complete the EO-BF in a single step or two steps as second step
    void YruRU::SearchEOBF(const uint MaxDepth)
    {
        auto time_eobf_start = std::chrono::system_clock::now();

		MaxDepthEOBF = (MaxDepth <= 4u ? 4u : MaxDepth);

		const SearchUnit URoot(SequenceType::DOUBLE, Sst::YRURU_rUR);
		const SearchUnit U_U(SequenceType::SINGLE, Sst::YRURU_rUR);
		
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
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!Lines[sp].empty()) AlgStart.Append(Lines[sp][n]);
				AlgStart.Append(CPLines[sp][n]);
				AlgStart.Append(pEO[sp][n]);
				if (!EO[sp].empty()) AlgStart.Append(EO[sp][n]);
				Cube CubeYruRU(AlgStart);

				if (!Roux::IsFBBuilt(CubeYruRU) || (!EO[sp].empty() && !CubeYruRU.CheckOrientation(Pgr::ALL_EDGES)))
				{
					EOBF[sp].push_back(Algorithm(""));
					continue;
				}

				DeepSearch DSEOBF(AlgStart, Plc::SHORT); // Deep search for EO-BF

				DSEOBF.AddToMandatoryPieces(B1);
				DSEOBF.AddToMandatoryPieces(BF);
				DSEOBF.AddToMandatoryOrientations(Pgr::ALL_EDGES);

				DSEOBF.AddSearchLevel(L_Root);
				for (uint l = 2u; l < MaxDepthEOBF; l++) DSEOBF.AddSearchLevel(L_EOBF_Check); // Add needed search levels

				DSEOBF.UpdateRootData();

				DSEOBF.Run(Cores);

				Cores = DSEOBF.GetCoresUsed(); // Update to the real number of cores used

				std::vector<Algorithm> Solves;
				CFOP::EvaluateF2LResult(Solves, 1u, DSEOBF.Solves, CubeYruRU, Cube::GetDownSliceLayer(spin), Plc::BEST_SOLVES);

				if (Solves.empty()) EOBF[sp].push_back(Algorithm(""));
				else
				{
					CubeYruRU.ApplyAlgorithm(Solves[0]);
					if (CubeYruRU.CheckOrientation(Pgr::ALL_EDGES)) EOBF[sp].push_back(Solves[0]);
					else EOBF[sp].push_back(Algorithm(""));
				}
			}
		}
		
        const std::chrono::duration<double> eobf_elapsed_seconds = std::chrono::system_clock::now() - time_eobf_start;
        TimeEOBF = eobf_elapsed_seconds.count();
    }
	
	// F2L search
    void YruRU::SearchF2L(const uint MaxDepth)
	{
		const auto time_F2L_start = std::chrono::system_clock::now();

		MaxDepthF2L = (MaxDepth <= 4u ? 4u : MaxDepth);

		const SearchUnit U_Root(SequenceType::DOUBLE, Sst::YRURU_UR);
		const SearchUnit U_UR(SequenceType::SINGLE, Sst::YRURU_UR);

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
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				if (!Lines[sp].empty()) AlgStart.Append(Lines[sp][n]);
				AlgStart.Append(CPLines[sp][n]);
				AlgStart.Append(pEO[sp][n]);
				if (!EO[sp].empty()) AlgStart.Append(EO[sp][n]);
				AlgStart.Append(EOBF[sp][n]);
				const Cube CubeYruRU(AlgStart);

				if (!Roux::IsFBBuilt(CubeYruRU) || !CubeYruRU.CheckOrientation(Pgr::ALL_EDGES))
				{
					F2L[sp].push_back(Algorithm(""));
					continue;
				}

				if (CubeYruRU.IsSolved(DownLayer) && CubeYruRU.IsSolved(MiddleLayer) && CubeYruRU.CheckOrientation(Pgr::ALL_EDGES))
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
    
    // 2GLL search
    void YruRU::Search2GLL()
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

				Collection::SolveLL(Alg2GLL[sp][n], Cases2GLL[sp][n], AUFStep, AlgSets::_2GLL, CubeYruRU);

				Alg2GLL[sp][n].Append(AUFStep);
			}
		}

        const std::chrono::duration<double> _2GLL_elapsed_seconds = std::chrono::system_clock::now() - time_2GLL_start;
        Time2GLL = _2GLL_elapsed_seconds.count();
	}

	// Set regrips
	void YruRU::SetRegrips()
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

	// Check if the CP-Line is built (only line)
	bool YruRU::IsLineBuilt(const Cube& C)
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
	bool YruRU::IsLineBuilt(const Cube& C, const Spn sp)
	{
		Cube CAux = C;
		CAux.SetSpin(sp);
		return IsLineBuilt(CAux);
	}

	// Corners permutation (begginers version)
	bool YruRU::IsCPBuilt(const Cube& C)
	{
		if (!IsLineBuilt(C)) return false; // Line for CP (1x1x3 block) must be solved

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
	/*
	// Corners permutation (advanced version) - TODO
	bool YruRU::IsCPBuilt2(const Cube& C) // TODO - not tested, needs some work
	{
		if (!IsLineBuilt(C)) return false; // Line for CP (1x1x3 block) must be solved

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
	*/

	// Corners permutation
	bool YruRU::IsCPBuilt(const Cube& C, const Spn spin)
	{
		Cube CAux = C;
		CAux.SetSpin(spin);
		return IsCPBuilt(CAux);
	}

	// Returns best solve algorithm from the Solves vector class member and his score for the given spin (CP not solved)
	bool YruRU::EvaluateLinesResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp)
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

			if (!IsLineBuilt(CheckCube, Sp) || IsCPBuilt(CheckCube, Sp)) continue; // Discard CP skip solves

			Score += 50u * (s.GetSize() > 50u ? 0u : 50u - s.GetSize());
			Score += s.GetSubjectiveScore();

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
	bool YruRU::EvaluateCPLinesResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp)
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

			if (!IsCPBuilt(CheckCube, Sp)) continue;

			Score += 50u * (s.GetSize() > 50u ? 0u : 50u - s.GetSize());
			Score += s.GetSubjectiveScore();

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
	bool YruRU::EvaluatepEOResult(Algorithm& BestSolve, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp)
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
			if (!Roux::IsFBBuilt(CheckCube, Sp)) continue;

			Score += 50u * (s.GetSize() > 50u ? 0u : 50u - s.GetSize());
			Score += s.GetSubjectiveScore();

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

    // Get a solve report
    std::string YruRU::GetReport(const bool cancellations, bool debug) const
    {
        std::string Report  = "YruRU search for Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble(), ReportLine;
        Report += "\n--------------------------------------------------------------------------------\n";

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
					
					if (IsLineBuilt(C, spin))
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

					if (IsCPBuilt(C)) ReportLine += "(" + CPLines[sp][n].ToString() + ")";
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

					if (IsCPBuilt(C))
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

				if (Roux::IsFBBuilt(C)) ReportLine += " (" + pEO[sp][n].ToString() + ")";
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

				if (CFOP::IsF2LBuilt(C, Cube::GetDownSliceLayer(spin))) ReportLine += " (" + F2L[sp][n].ToString() + ")";
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
    std::string YruRU::GetTimeReport() const
    {
        std::string Report;

        Report += "Total search time: " + std::to_string(GetFullTime()) + " s\n";
		if (TimeLines == 0.0) Report += "CP-Lines search time: " + std::to_string(GetTimeCPLines()) + " s\n";
		else
		{
			Report += "Lines search time: " + std::to_string(GetTimeLines()) + " s\n";
			Report += "CP search time: " + std::to_string(GetTimeCPLines()) + " s\n";
		}
        Report += "pEO-extension search time: " + std::to_string(GetTimepEO()) + " s\n";
		if (TimeEO == 0.0) Report += "EO-BF search time: " + std::to_string(GetTimeEOBF()) + " s\n";
		else
		{
			Report += "EO search time: " + std::to_string(GetTimeEO()) + " s\n";
			Report += "BF search time: " + std::to_string(GetTimeEOBF()) + " s\n";
		}
        Report += "F2L search time: " + std::to_string(GetTimeF2L()) + " s\n";
        Report += "2GLL search time: " + std::to_string(GetTime2GLL()) + " s\n";
        Report += "Threads used: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : 0) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string YruRU::GetReport(const Spn sp, const uint n) const
	{
		const int si = static_cast<int>(sp);

		if (!CheckSolveConsistency(sp) || n >= Inspections[si].size())
		{
			std::string FReport = "No solve for spin ";
			FReport += Cube::GetSpinText(sp);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

        std::string Report  = "YruRU search with orientation " + Cube::GetSpinText(sp);
        Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += "Inspection [" + C.GetSpinText() + "]: " + Inspections[si][n].ToString() + "\n\n";

		if (!Lines[si].empty()) // Lines + CP
		{
			C.ApplyAlgorithm(Lines[si][n]);
			if (!IsLineBuilt(C, sp))
			{
				Report += "Line not built in " + std::to_string(MaxDepthCPLines) + " movements\n";
				return Report;
			}
			Report += "Line (" + Algorithm::GetMetricValue(GetMetricLines(sp, n)) + "): " + Lines[si][n].ToString() + "\n";
			
			C.ApplyAlgorithm(CPLines[si][n]);
			if (!IsCPBuilt(C))
			{
				Report += "CP not built in " + std::to_string(4u) + " movements\n";
				return Report;
			}
			Report += "CP (" + Algorithm::GetMetricValue(GetMetricCPLines(sp, n)) + "): " + CPLines[si][n].ToString() + "\n";
		}
		else // CPLines
		{
			C.ApplyAlgorithm(CPLines[si][n]);
			if (!IsCPBuilt(C))
			{
				Report += "CP-Line not built in " + std::to_string(MaxDepthCPLines) + " movements\n";
				return Report;
			}
			Report += "CP-Line (" + Algorithm::GetMetricValue(GetMetricCPLines(sp, n)) + "): " + CPLines[si][n].ToString() + "\n";
		}
		C.ApplyAlgorithm(pEO[si][n]);
		if (!Roux::IsFBBuilt(C))
		{
			Report += "pEO-extension not built in " + std::to_string(MaxDepthpEO) + " movements\n";;
			return Report;
		}
		if (!pEO[si][n].Empty()) Report += "pEO-extension (" + Algorithm::GetMetricValue(GetMetricpEO(sp, n)) + "): " +
			pEO[si][n].ToString() + "\n";

		if (!EO[si].empty())
		{
			C.ApplyAlgorithm(EO[si][n]);
			if (!C.CheckOrientation(Pgr::ALL_EDGES))
			{
				Report += "EO not built in " + std::to_string(MaxDepthEO) + " movements\n";
				return Report;
			}
			if (!EO[si][n].Empty()) Report += "EO (" + Algorithm::GetMetricValue(GetMetricEO(sp, n)) + "): " + EO[si][n].ToString() + "\n";
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
			if (EO[si].empty()) Report += "EO-BF (" + Algorithm::GetMetricValue(GetMetricEOBF(sp, n)) + "): " + EOBF[si][n].ToString() + "\n";
			else Report += "BF (" + Algorithm::GetMetricValue(GetMetricEOBF(sp, n)) + "): " + EOBF[si][n].ToString() + "\n";
		}

		C.ApplyAlgorithm(F2L[si][n]);
		if (!CFOP::IsF2LBuilt(C, Cube::GetDownSliceLayer(C.GetSpin())))
		{
			Report += "F2L not built in " + std::to_string(MaxDepthF2L) + " movements\n";
			return Report;
		}
		if (!F2L[si][n].Empty()) Report += "F2L (" + Algorithm::GetMetricValue(GetMetricF2L(sp, n)) + "): " + F2L[si][n].ToString() + "\n";

		C.ApplyAlgorithm(Alg2GLL[si][n]);
		if (!Alg2GLL[si][n].Empty()) Report += "2GLL (" + Algorithm::GetMetricValue(GetMetric2GLL(sp, n)) + "): " + Alg2GLL[si][n].ToString() + "\n";

        // Show summary
        Report += "\nSolve metric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";

		Report += "2GLL case: " + GetText2GLLCase(sp, n) + "\n";

		return Report;
	}

	// Check if the solves for the given cross layer are consistent (all needed algorithms are present)
	bool YruRU::CheckSolveConsistency(const Spn Spin) const
	{
		const int si = static_cast<int>(Spin); // Spin index

		if (Inspections[si].empty()) return false;

		auto n = Inspections[si].size();

		return (Lines[si].empty() || Lines[si].size() == n) && (CPLines[si].size() == n) && (pEO[si].size() == n) && 
			   (EO[si].empty() || EO[si].size() == n) && (EOBF[si].size() == n) && (F2L[si].size() == n) && (Alg2GLL[si].size() == n);
	}

    // Check if in the given spin the solve is OK
    bool YruRU::IsSolved(const Spn CurrentSpin, const uint n) const
    {
        const int Si = static_cast<int>(CurrentSpin); // Spin index

		if (!CheckSolveConsistency(CurrentSpin) || CPLines[Si].size() <= n) return 0u;

        Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[Si][n]);
		if (!Lines[Si].empty()) C.ApplyAlgorithm(Lines[Si][n]);
        C.ApplyAlgorithm(CPLines[Si][n]);
        C.ApplyAlgorithm(pEO[Si][n]);
		if (!EO[Si].empty()) C.ApplyAlgorithm(EO[Si][n]);
        C.ApplyAlgorithm(EOBF[Si][n]);
		C.ApplyAlgorithm(F2L[Si][n]);
		C.ApplyAlgorithm(Alg2GLL[Si][n]);
		
        return C.IsSolved();
    }

	// Get the solve metric
    float YruRU::GetMetricSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || CPLines[si].size() <= n) return 0u;

		Algorithm A;
		if (!Lines[si].empty()) A += Lines[si][n];
		A += CPLines[si][n];
		A += pEO[si][n];
		if (!EO[si].empty()) A += EO[si][n];
		A += EOBF[si][n];
		A += F2L[si][n];
		A += Alg2GLL[si][n];
		
		return A.GetMetric(Metric);
	}

	// Get the full solve with cancellations
	Algorithm YruRU::GetCancellations(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || CPLines[si].size() <= n)
		{
			std::string FReport = "No solve with cancellations for spin ";
			FReport += Cube::GetSpinText(spin);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

		Algorithm A = Inspections[si][n];
		if (!Lines[si].empty()) A += Lines[si][n];
		A += CPLines[si][n];
		A += pEO[si][n];
		if (!EO[si].empty()) A += EO[si][n];
		A += EOBF[si][n];
		A += F2L[si][n];
		A += Alg2GLL[si][n];

		return A.GetCancellations();
	}

	// Get the best solve report
	std::string YruRU::GetBestReport(const bool Cancellations) const
	{
		float M, min_M = 0.0f;
		uint Bestn = 0u;
		Spn BestSpin = Spn::Default;

		for (int sp = 0; sp < 24; sp++)
		{
			const Spn Spin = static_cast<Spn>(sp);

			if (!CheckSolveConsistency(Spin)) continue;

			for (uint n = 0u; n < Inspections[sp].size(); n++)
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
		if (min_M == 0.0f) return "No YruRU solves!\n";

		if (Cancellations)
		{
			Algorithm C = GetCancellations(BestSpin, Bestn);
			return GetReport(BestSpin, Bestn) + "\nCancellations (" + Algorithm::GetMetricValue(C.GetMetric(Metric)) + " " + 
			       Algorithm::GetMetricString(Metric) + "): " + C.ToString() + "\n";
		}

		return GetReport(BestSpin, Bestn);
	}
}
