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

#include "mehta.h"
#include "collection.h"

namespace grcube3
{	
	// Reset the search results
	void Mehta::Reset()
	{
        for (int i = 0; i < 24; i++)
        {
			Inspections[i].clear();
			AlgFB[i].clear();
			Alg3QB[i].clear();
			AlgEOLE[i].clear();
			Alg6CO[i].clear();
			Alg6CP[i].clear();
			AlgL5EP[i].clear();
			AlgAPDR[i].clear();
			AlgPLL[i].clear();
			AlgDCAL[i].clear();
			AlgCDRLL[i].clear();
			AlgJTLE[i].clear();
			AlgTDR[i].clear();
			AlgZBLL[i].clear();

			CasesEOLE[i].clear();
			Cases6CO[i].clear();
			Cases6CP[i].clear();
			CasesL5EP[i].clear();
			CasesAPDR[i].clear();
			CasesPLL[i].clear();
			CasesDCAL[i].clear();
			CasesCDRLL[i].clear();
			CasesJTLE[i].clear();
			CasesTDR[i].clear();
			CasesZBLL[i].clear();
        }
		 
        MaxDepthFB = MaxDepth3QB = 0u;

		TimeFB = Time3QB = TimeEOLE = Time6CO = Time6CP = TimeL5EP = TimeAPDR = TimePLL = TimeDCAL = TimeCDRLL = TimeJTLE = TimeTDR = TimeZBLL = 0.0;

		Metric = Metrics::Movements; // Default metric
	}

    // Search the best first blocks solve algorithm with the given search depth
	// Return false if no first blocks found
    bool Mehta::SearchFB(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_fb_start = std::chrono::system_clock::now();

        MaxDepthFB = (MaxDepth <= 4u ? 4u : MaxDepth);

        // Deep search for first block
		DS.Clear();
        DS.SetScramble(Scramble);
		
		// Mheta first blocks are Roux first blocks with other spin
        DS.AddToOptionalPieces(Pgr::RB_B1);
		DS.AddToOptionalPieces(Pgr::LF_B1);
		DS.AddToOptionalPieces(Pgr::BL_B1);
		DS.AddToOptionalPieces(Pgr::FR_B1);

		DS.AddToOptionalPieces(Pgr::LB_B1);
		DS.AddToOptionalPieces(Pgr::RF_B1);
		DS.AddToOptionalPieces(Pgr::FL_B1);
		DS.AddToOptionalPieces(Pgr::BR_B1);

		DS.AddToOptionalPieces(Pgr::LD_B1);
		DS.AddToOptionalPieces(Pgr::RU_B1);
		DS.AddToOptionalPieces(Pgr::UL_B1);
		DS.AddToOptionalPieces(Pgr::DR_B1);

		DS.AddToOptionalPieces(Pgr::RD_B1);
		DS.AddToOptionalPieces(Pgr::LU_B1);
		DS.AddToOptionalPieces(Pgr::DL_B1);
		DS.AddToOptionalPieces(Pgr::UR_B1);

		DS.AddToOptionalPieces(Pgr::FD_B1);
		DS.AddToOptionalPieces(Pgr::BU_B1);
		DS.AddToOptionalPieces(Pgr::DB_B1);
		DS.AddToOptionalPieces(Pgr::UF_B1);

		DS.AddToOptionalPieces(Pgr::BD_B1);
		DS.AddToOptionalPieces(Pgr::FU_B1);
		DS.AddToOptionalPieces(Pgr::UB_B1);
		DS.AddToOptionalPieces(Pgr::DF_B1);

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

	// Search 3 Quarters Belt - Solve 3 E-slice edges relative to the centers
	void Mehta::Search3QB(const uint MaxDepth)
	{
		const auto time_3qb_start = std::chrono::system_clock::now();
		
		bool Skip = false; // Skip the search (for multi threading)

		MaxDepth3QB = (MaxDepth <= 4u ? 4u : MaxDepth);

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
        const SearchUnit URoot(SequenceTypes::DOUBLE);
        const SearchUnit U(SequenceTypes::SINGLE);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(URoot);

		SearchLevel L_Check(SearchCheck::CHECK);
		L_Check.Add(U);

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Alg3QB[sp].clear();

			Pgr B1, BELT1, BELT2, BELT3, BELT4;

			switch (spin)
			{
			case Spn::UF: B1 = Pgr::RB_B1; BELT1 = Pgr::BELT_UD_1; BELT2 = Pgr::BELT_UD_2; BELT3 = Pgr::BELT_UD_3; BELT4 = Pgr::BELT_UD_4; break;
			case Spn::UB: B1 = Pgr::LF_B1; BELT1 = Pgr::BELT_UD_1; BELT2 = Pgr::BELT_UD_2; BELT3 = Pgr::BELT_UD_3; BELT4 = Pgr::BELT_UD_4; break;
			case Spn::UR: B1 = Pgr::BL_B1; BELT1 = Pgr::BELT_UD_1; BELT2 = Pgr::BELT_UD_2; BELT3 = Pgr::BELT_UD_3; BELT4 = Pgr::BELT_UD_4; break;
			case Spn::UL: B1 = Pgr::FR_B1; BELT1 = Pgr::BELT_UD_1; BELT2 = Pgr::BELT_UD_2; BELT3 = Pgr::BELT_UD_3; BELT4 = Pgr::BELT_UD_4; break;

			case Spn::DF: B1 = Pgr::LB_B1; BELT1 = Pgr::BELT_UD_1; BELT2 = Pgr::BELT_UD_2; BELT3 = Pgr::BELT_UD_3; BELT4 = Pgr::BELT_UD_4; break;
			case Spn::DB: B1 = Pgr::RF_B1; BELT1 = Pgr::BELT_UD_1; BELT2 = Pgr::BELT_UD_2; BELT3 = Pgr::BELT_UD_3; BELT4 = Pgr::BELT_UD_4; break;
			case Spn::DR: B1 = Pgr::FL_B1; BELT1 = Pgr::BELT_UD_1; BELT2 = Pgr::BELT_UD_2; BELT3 = Pgr::BELT_UD_3; BELT4 = Pgr::BELT_UD_4; break;
			case Spn::DL: B1 = Pgr::BR_B1; BELT1 = Pgr::BELT_UD_1; BELT2 = Pgr::BELT_UD_2; BELT3 = Pgr::BELT_UD_3; BELT4 = Pgr::BELT_UD_4; break;

			case Spn::FU: B1 = Pgr::LD_B1; BELT1 = Pgr::BELT_FB_1; BELT2 = Pgr::BELT_FB_2; BELT3 = Pgr::BELT_FB_3; BELT4 = Pgr::BELT_FB_4; break;
			case Spn::FD: B1 = Pgr::RU_B1; BELT1 = Pgr::BELT_FB_1; BELT2 = Pgr::BELT_FB_2; BELT3 = Pgr::BELT_FB_3; BELT4 = Pgr::BELT_FB_4; break;
			case Spn::FR: B1 = Pgr::UL_B1; BELT1 = Pgr::BELT_FB_1; BELT2 = Pgr::BELT_FB_2; BELT3 = Pgr::BELT_FB_3; BELT4 = Pgr::BELT_FB_4; break;
			case Spn::FL: B1 = Pgr::DR_B1; BELT1 = Pgr::BELT_FB_1; BELT2 = Pgr::BELT_FB_2; BELT3 = Pgr::BELT_FB_3; BELT4 = Pgr::BELT_FB_4; break;

			case Spn::BU: B1 = Pgr::RD_B1; BELT1 = Pgr::BELT_FB_1; BELT2 = Pgr::BELT_FB_2; BELT3 = Pgr::BELT_FB_3; BELT4 = Pgr::BELT_FB_4; break;
			case Spn::BD: B1 = Pgr::LU_B1; BELT1 = Pgr::BELT_FB_1; BELT2 = Pgr::BELT_FB_2; BELT3 = Pgr::BELT_FB_3; BELT4 = Pgr::BELT_FB_4; break;
			case Spn::BR: B1 = Pgr::DL_B1; BELT1 = Pgr::BELT_FB_1; BELT2 = Pgr::BELT_FB_2; BELT3 = Pgr::BELT_FB_3; BELT4 = Pgr::BELT_FB_4; break;
			case Spn::BL: B1 = Pgr::UR_B1; BELT1 = Pgr::BELT_FB_1; BELT2 = Pgr::BELT_FB_2; BELT3 = Pgr::BELT_FB_3; BELT4 = Pgr::BELT_FB_4; break;

			case Spn::RU: B1 = Pgr::FD_B1; BELT1 = Pgr::BELT_RL_1; BELT2 = Pgr::BELT_RL_2; BELT3 = Pgr::BELT_RL_3; BELT4 = Pgr::BELT_RL_4; break;
			case Spn::RD: B1 = Pgr::BU_B1; BELT1 = Pgr::BELT_RL_1; BELT2 = Pgr::BELT_RL_2; BELT3 = Pgr::BELT_RL_3; BELT4 = Pgr::BELT_RL_4; break;
			case Spn::RF: B1 = Pgr::DB_B1; BELT1 = Pgr::BELT_RL_1; BELT2 = Pgr::BELT_RL_2; BELT3 = Pgr::BELT_RL_3; BELT4 = Pgr::BELT_RL_4; break;
			case Spn::RB: B1 = Pgr::UF_B1; BELT1 = Pgr::BELT_RL_1; BELT2 = Pgr::BELT_RL_2; BELT3 = Pgr::BELT_RL_3; BELT4 = Pgr::BELT_RL_4; break;

			case Spn::LU: B1 = Pgr::BD_B1; BELT1 = Pgr::BELT_RL_1; BELT2 = Pgr::BELT_RL_2; BELT3 = Pgr::BELT_RL_3; BELT4 = Pgr::BELT_RL_4; break;
			case Spn::LD: B1 = Pgr::FU_B1; BELT1 = Pgr::BELT_RL_1; BELT2 = Pgr::BELT_RL_2; BELT3 = Pgr::BELT_RL_3; BELT4 = Pgr::BELT_RL_4; break;
			case Spn::LF: B1 = Pgr::UB_B1; BELT1 = Pgr::BELT_RL_1; BELT2 = Pgr::BELT_RL_2; BELT3 = Pgr::BELT_RL_3; BELT4 = Pgr::BELT_RL_4; break;
			case Spn::LB: B1 = Pgr::DF_B1; BELT1 = Pgr::BELT_RL_1; BELT2 = Pgr::BELT_RL_2; BELT3 = Pgr::BELT_RL_3; BELT4 = Pgr::BELT_RL_4; break;

			default: return; // Should not happend
			}

			uint n = 0u;
			for (const auto& FB : AlgFB[sp])
			{
				Alg3QB[sp].push_back(Algorithm(""));
				
				if (Skip) continue;
				
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FB);
				
				Cube CubeMehta(AlgStart);
				
				if (!IsMehtaFBBuilt(CubeMehta, spin)) continue;

				// Deep search for 3QB
				DS.Clear();
                DS.SetScramble(AlgStart);

				DS.AddToMandatoryPieces(B1);
				DS.AddToOptionalPieces(BELT1);
				DS.AddToOptionalPieces(BELT2);
				DS.AddToOptionalPieces(BELT3);
				DS.AddToOptionalPieces(BELT4);

				DS.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
				for (uint l = 2u; l < MaxDepth3QB; l++) DS.AddSearchLevel(L_Check); // Add needed search levels

				DS.UpdateRootData();

				DS.Run(Cores);
				Skip = DS.CheckSkipSearch();

				std::vector<Algorithm> Solves;
				
                EvaluateMehta3QBResult(Solves, 1u, DS.GetSolves(), CubeMehta, spin, Plc::BEST);

				if (!Solves.empty())
				{
					CubeMehta.ApplyAlgorithm(Solves[0]);
					if (IsMehta3QBBuilt(CubeMehta)) Alg3QB[sp][n] = Solves[0];
				}
				n++;
			}
		}

		const std::chrono::duration<double> _3qb_elapsed_seconds = std::chrono::system_clock::now() - time_3qb_start;
		Time3QB = _3qb_elapsed_seconds.count();
	}
	
	// Edge Orientation + Last Edge - Insert the remaining E-slice edge while orienting all the edges
	void Mehta::SearchEOLE(const Plc Pol)
	{
		const auto time_eole_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgEOLE[sp].clear();
			CasesEOLE[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgEOLE[sp].push_back(Algorithm(""));
				CasesEOLE[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];

				Cube CubeEOLE = CubeBase;
				CubeEOLE.ApplyAlgorithm(Alg);
				
				if (!IsMehta3QBBuilt(CubeEOLE, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_EOLE.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_EOLE.GetAlgorithm(Index, Pol, Metric);

					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto EMov : Algorithm::EMovs) // E movement before algorithm
						{
							Cube CubeMetha = CubeEOLE;

							if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
							if (EMov != Stp::NONE) CubeMetha.ApplyStep(EMov);
							CubeMetha.ApplyAlgorithm(A);
							if (EMov != Stp::NONE) CubeMetha.ApplyStep(Algorithm::InvertedStep(EMov));

							if (IsMehtaEOLEBuilt(CubeMetha, spin))
							{
								Found = true;

								CasesEOLE[sp][n] = Algset_EOLE.GetCaseName(Index);

								if (UMov != Stp::NONE)
								{
									AlgEOLE[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgEOLE[sp][n].Append(UMov);
									AlgEOLE[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								if (EMov != Stp::NONE)
								{
									AlgEOLE[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgEOLE[sp][n].Append(EMov);
									AlgEOLE[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}
								AlgEOLE[sp][n].Append(A);
								if (EMov != Stp::NONE)
								{
									AlgEOLE[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgEOLE[sp][n].Append(Algorithm::InvertedStep(EMov));
									AlgEOLE[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
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

		const std::chrono::duration<double> eole_elapsed_seconds = std::chrono::system_clock::now() - time_eole_start;
		TimeEOLE = eole_elapsed_seconds.count();
	}
	
	// Orient the 6 remaining corners
	void Mehta::Search6CO(const Plc Pol)
	{
		const auto time_6co_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Alg6CO[sp].clear();
			Cases6CO[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				Alg6CO[sp].push_back(Algorithm(""));
				Cases6CO[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];

				Cube Cube6CO = CubeBase;
				Cube6CO.ApplyAlgorithm(Alg);
				
				if (!IsMehtaEOLEBuilt(Cube6CO, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_6CO.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_6CO.GetAlgorithm(Index, Pol, Metric);

					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeMetha = Cube6CO;

						if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
						CubeMetha.ApplyAlgorithm(A);

						if (IsMehta6COBuilt(CubeMetha, spin))
						{
							Found = true;

							Cases6CO[sp][n] = Algset_6CO.GetCaseName(Index);

							if (UMov != Stp::NONE)
							{
								Alg6CO[sp][n].Append(Stp::PARENTHESIS_OPEN);
								Alg6CO[sp][n].Append(UMov);
								Alg6CO[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							Alg6CO[sp][n].Append(A);

							break;
						}
					}
					if (Found) break; // Release for loop
				}
			}
		}

		const std::chrono::duration<double> _6co_elapsed_seconds = std::chrono::system_clock::now() - time_6co_start;
		Time6CO = _6co_elapsed_seconds.count();
	}
	
	// Permute the 6 remaining corners
	void Mehta::Search6CP(const Plc Pol)
	{
		const auto time_6cp_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			Alg6CP[sp].clear();
			Cases6CP[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				Alg6CP[sp].push_back(Algorithm(""));
				Cases6CP[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];
				Alg += Alg6CO[sp][n];

				Cube Cube6CP = CubeBase;
				Cube6CP.ApplyAlgorithm(Alg);
				
				if (!IsMehta6COBuilt(Cube6CP, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_6CP.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_6CP.GetAlgorithm(Index, Pol, Metric);

					for (const auto U1Mov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto U2Mov : Algorithm::UMovs) // U movement after algorithm
						{
							Cube CubeMetha = Cube6CP;

							if (U1Mov != Stp::NONE) CubeMetha.ApplyStep(U1Mov);
							CubeMetha.ApplyAlgorithm(A);
							if (U2Mov != Stp::NONE) CubeMetha.ApplyStep(U2Mov);

							if (IsMehta6CPBuilt(CubeMetha, spin))
							{
								Found = true;

								Cases6CP[sp][n] = Algset_6CP.GetCaseName(Index);

								if (U1Mov != Stp::NONE)
								{
									Alg6CP[sp][n].Append(Stp::PARENTHESIS_OPEN);
									Alg6CP[sp][n].Append(U1Mov);
									Alg6CP[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								Alg6CP[sp][n].Append(A);

								if (U2Mov != Stp::NONE)
								{
									Alg6CP[sp][n].Append(Stp::PARENTHESIS_OPEN);
									Alg6CP[sp][n].Append(U2Mov);
									Alg6CP[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
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

		const std::chrono::duration<double> _6cp_elapsed_seconds = std::chrono::system_clock::now() - time_6cp_start;
		Time6CP = _6cp_elapsed_seconds.count();
	}
	
	// Solve the cube by permuting the last 5 edges
    void Mehta::SearchL5EP(const Plc Pol)
	{
		const auto time_l5ep_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgL5EP[sp].clear();
			CasesL5EP[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgL5EP[sp].push_back(Algorithm(""));
				CasesL5EP[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];
				
				if (!Alg6CP[sp].empty())
				{
					Alg += Alg6CO[sp][n];
					Alg += Alg6CP[sp][n];
				}
				else if (!AlgCDRLL[sp].empty())
				{
					Alg += AlgDCAL[sp][n];
					Alg += AlgCDRLL[sp][n];
				}

				Cube CubeL5EP = CubeBase;
				CubeL5EP.ApplyAlgorithm(Alg);
				
				if ((!Alg6CP[sp].empty() && !IsMehta6CPBuilt(CubeL5EP, spin)) ||
				    (!AlgCDRLL[sp].empty() && !IsMehtaCDRLLBuilt(CubeL5EP, spin))) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_L5EP.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_L5EP.GetAlgorithm(Index, Pol, Metric);

					for (const auto U1Mov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto U2Mov : Algorithm::UMovs) // U movement after algorithm
						{
							Cube CubeMetha = CubeL5EP;

							if (U1Mov != Stp::NONE) CubeMetha.ApplyStep(U1Mov);
							CubeMetha.ApplyAlgorithm(A);
							if (U2Mov != Stp::NONE) CubeMetha.ApplyStep(U2Mov);

							if (CubeMetha.IsSolved())
							{
								Found = true;

								CasesL5EP[sp][n] = Algset_L5EP.GetCaseName(Index);

								if (U1Mov != Stp::NONE)
								{
									AlgL5EP[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgL5EP[sp][n].Append(U1Mov);
									AlgL5EP[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								AlgL5EP[sp][n].Append(A);

								if (U2Mov != Stp::NONE)
								{
									AlgL5EP[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgL5EP[sp][n].Append(U2Mov);
									AlgL5EP[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
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

		const std::chrono::duration<double> l5ep_elapsed_seconds = std::chrono::system_clock::now() - time_l5ep_start;
		TimeL5EP = l5ep_elapsed_seconds.count();
	}

	// Solve the DR block
	void Mehta::SearchAPDR(const Plc Pol)
	{
		const auto time_apdr_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgAPDR[sp].clear();
			CasesAPDR[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgAPDR[sp].push_back(Algorithm(""));
				CasesAPDR[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];
				Alg += Alg6CO[sp][n];

				Cube CubeAPDR = CubeBase;
				CubeAPDR.ApplyAlgorithm(Alg);
				
				if (!IsMehta6COBuilt(CubeAPDR, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_APDR.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_APDR.GetAlgorithm(Index, Pol, Metric);

					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeMetha = CubeAPDR;

						if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
						CubeMetha.ApplyAlgorithm(A);

						if (IsMehtaAPDRBuilt(CubeMetha, spin))
						{
							Found = true;

							CasesAPDR[sp][n] = Algset_APDR.GetCaseName(Index);

							if (UMov != Stp::NONE)
							{
								AlgAPDR[sp][n].Append(Stp::PARENTHESIS_OPEN);
								AlgAPDR[sp][n].Append(UMov);
								AlgAPDR[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							AlgAPDR[sp][n].Append(A);

							break;
						}
					}
					if (Found) break; // Release for loop
				}
			}
		}

		const std::chrono::duration<double> apdr_elapsed_seconds = std::chrono::system_clock::now() - time_apdr_start;
		TimeAPDR = apdr_elapsed_seconds.count();
	}
	
	// Solve the last layer
	void Mehta::SearchPLL(const Plc Pol)
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
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];
				
				if (!AlgAPDR[sp].empty())
				{
					Alg += Alg6CO[sp][n];
					Alg += AlgAPDR[sp][n];
				}
				else if (!AlgJTLE[sp].empty())
				{
					Alg += AlgDCAL[sp][n];
					Alg += AlgJTLE[sp][n];
				}

				Cube CubePLL = CubeBase;
				CubePLL.ApplyAlgorithm(Alg);
				
				if (!IsMehtaAPDRBuilt(CubePLL, spin)) continue; // IsJTLEBuilt == IsAPDRBuit

				Stp AUFStep;

				SolveLL(AlgPLL[sp][n], CasesPLL[sp][n], AUFStep, Algset_PLL, Pol, Metric, CubePLL);

				if (AUFStep != Stp::NONE) AlgPLL[sp][n].Append(AUFStep);
			}
		}

		const std::chrono::duration<double> PLL_elapsed_seconds = std::chrono::system_clock::now() - time_PLL_start;
		TimePLL = PLL_elapsed_seconds.count();
	}
	
	// Solve the 2 corners of the D layer
	void Mehta::SearchDCAL(const Plc Pol)
	{
		const auto time_dcal_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgDCAL[sp].clear();
			CasesDCAL[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgDCAL[sp].push_back(Algorithm(""));
				CasesDCAL[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];

				Cube CubeDCAL = CubeBase;
				CubeDCAL.ApplyAlgorithm(Alg);
				
				if (!IsMehtaEOLEBuilt(CubeDCAL, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_DCAL.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_DCAL.GetAlgorithm(Index, Pol, Metric);

					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeMetha = CubeDCAL;

						if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
						CubeMetha.ApplyAlgorithm(A);

						if (IsMehtaDCALBuilt(CubeMetha, spin))
						{
							Found = true;

							CasesDCAL[sp][n] = Algset_DCAL.GetCaseName(Index);

							if (UMov != Stp::NONE)
							{
								AlgDCAL[sp][n].Append(Stp::PARENTHESIS_OPEN);
								AlgDCAL[sp][n].Append(UMov);
								AlgDCAL[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							AlgDCAL[sp][n].Append(A);

							break;
						}
					}
					if (Found) break; // Release for loop
				}
			}
		}

		const std::chrono::duration<double> dcal_elapsed_seconds = std::chrono::system_clock::now() - time_dcal_start;
		TimeDCAL = dcal_elapsed_seconds.count();
	}		
	
	// Orient and permute the U layer corners (like COLL)
	void Mehta::SearchCDRLL(const Plc Pol)
	{
		const auto time_cdrll_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgCDRLL[sp].clear();
			CasesCDRLL[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgCDRLL[sp].push_back(Algorithm(""));
				CasesCDRLL[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];
				Alg += AlgDCAL[sp][n];

				Cube CubeCDRLL = CubeBase;
				CubeCDRLL.ApplyAlgorithm(Alg);
				
				if (!IsMehtaDCALBuilt(CubeCDRLL, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_CDRLL.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_CDRLL.GetAlgorithm(Index, Pol, Metric);

					for (const auto U1Mov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto U2Mov : Algorithm::UMovs) // U movement after algorithm
						{
							Cube CubeMetha = CubeCDRLL;

							if (U1Mov != Stp::NONE) CubeMetha.ApplyStep(U1Mov);
							CubeMetha.ApplyAlgorithm(A);
							if (U2Mov != Stp::NONE) CubeMetha.ApplyStep(U2Mov);

							if (IsMehtaCDRLLBuilt(CubeMetha, spin))
							{
								Found = true;

								CasesCDRLL[sp][n] = Algset_CDRLL.GetCaseName(Index);

								if (U1Mov != Stp::NONE)
								{
									AlgCDRLL[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgCDRLL[sp][n].Append(U1Mov);
									AlgCDRLL[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								AlgCDRLL[sp][n].Append(A);

								if (U2Mov != Stp::NONE)
								{
									AlgCDRLL[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgCDRLL[sp][n].Append(U2Mov);
									AlgCDRLL[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
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

		const std::chrono::duration<double> cdrll_elapsed_seconds = std::chrono::system_clock::now() - time_cdrll_start;
		TimeCDRLL = cdrll_elapsed_seconds.count();
	}		
	
	// Orient the U layer corners while inserting the DR edge
	void Mehta::SearchJTLE(const Plc Pol)
	{
		const auto time_jtle_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgJTLE[sp].clear();
			CasesJTLE[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgJTLE[sp].push_back(Algorithm(""));
				CasesJTLE[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];
				Alg += AlgDCAL[sp][n];

				Cube CubeJTLE = CubeBase;
				CubeJTLE.ApplyAlgorithm(Alg);
				
				if (!IsMehtaDCALBuilt(CubeJTLE, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_JTLE.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_JTLE.GetAlgorithm(Index, Pol, Metric);

					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeMetha = CubeJTLE;

						if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
						CubeMetha.ApplyAlgorithm(A);

						if (IsMehtaJTLEBuilt(CubeMetha, spin))
						{
							Found = true;

							CasesJTLE[sp][n] = Algset_JTLE.GetCaseName(Index);

							if (UMov != Stp::NONE)
							{
								AlgJTLE[sp][n].Append(Stp::PARENTHESIS_OPEN);
								AlgJTLE[sp][n].Append(UMov);
								AlgJTLE[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							AlgJTLE[sp][n].Append(A);

							break;
						}
					}
					if (Found) break; // Release for loop
				}
			}
		}

		const std::chrono::duration<double> jtle_elapsed_seconds = std::chrono::system_clock::now() - time_jtle_start;
		TimeCDRLL = jtle_elapsed_seconds.count();
	}
	
	// Solve the DR 1x1x3 block
	void Mehta::SearchTDR(const Plc Pol)
	{
		const auto time_tdr_start = std::chrono::system_clock::now();

		for (const auto spin : SearchSpins)
		{
			int sp = static_cast<int>(spin);

			AlgTDR[sp].clear();
			CasesTDR[sp].clear();

			for (uint n = 0u; n < Inspections[sp].size(); n++)
			{
				AlgTDR[sp].push_back(Algorithm(""));
				CasesTDR[sp].push_back("");

				Algorithm Alg = Inspections[sp][n];
				Alg += AlgFB[sp][n];
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];

				Cube CubeTDR = CubeBase;
				CubeTDR.ApplyAlgorithm(Alg);
				
				if (!IsMehtaEOLEBuilt(CubeTDR, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Algset_TDR.GetCasesNumber(); Index++)
				{
					const Algorithm A = Algset_TDR.GetAlgorithm(Index, Pol, Metric);

					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto DMov : Algorithm::DMovs) // D movement after algorithm
						{
							Cube CubeMetha = CubeTDR;
							if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
							CubeMetha.ApplyAlgorithm(A);
							if (DMov != Stp::NONE) CubeMetha.ApplyStep(DMov);

							if (IsMehtaTDRBuilt(CubeMetha, spin))
							{
								Found = true;

								CasesTDR[sp][n] = Algset_TDR.GetCaseName(Index);

								if (UMov != Stp::NONE)
								{
									AlgTDR[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgTDR[sp][n].Append(UMov);
									AlgTDR[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}
								AlgTDR[sp][n].Append(A);
								if (DMov != Stp::NONE)
								{
									AlgTDR[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgTDR[sp][n].Append(DMov);
									AlgTDR[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}
								break;
							}
						}
						if (Found) break;
					}
					if (Found) break;
				}
			}
		}

		const std::chrono::duration<double> tdr_elapsed_seconds = std::chrono::system_clock::now() - time_tdr_start;
		TimeTDR = tdr_elapsed_seconds.count();
	}

	// Solve last layer
	void Mehta::SearchZBLL(const Plc Pol)
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
				Alg += Alg3QB[sp][n];
				Alg += AlgEOLE[sp][n];
				Alg += AlgTDR[sp][n];

				Cube CubeZBLL = CubeBase;
				CubeZBLL.ApplyAlgorithm(Alg);
				
				if (!IsMehtaTDRBuilt(CubeZBLL, spin)) continue;

				Stp AUFStep;

				SolveLL(AlgZBLL[sp][n], CasesZBLL[sp][n], AUFStep, Algset_ZBLL, Pol, Metric, CubeZBLL);

				if (AUFStep != Stp::NONE) AlgZBLL[sp][n].Append(AUFStep);
			}
		}

		const std::chrono::duration<double> ZBLL_elapsed_seconds = std::chrono::system_clock::now() - time_ZBLL_start;
		TimeZBLL = ZBLL_elapsed_seconds.count();
	}

	// Search the best first block solve algorithms from an algorithms vector
    void Mehta::EvaluateFB(const std::vector<Algorithm>& Solves, const uint MaxSolves)
	{
		// Best first block solve algorithms adapted to spin
        for (const auto spin : SearchSpins)
        {
			int sp = static_cast<int>(spin);
			Stp T1, T2;
			Algorithm Insp;

			EvaluateMehtaFBResult(AlgFB[sp], MaxSolves, Solves, CubeBase, spin, Plc::SHORT);

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

	// Set regrips
	void Mehta::SetRegrips()
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
    std::string Mehta::GetReport(const bool cancellations, bool debug) const
    {
        std::string Report, ReportLine;
        // Report = "Mehta: Scramble [" + std::to_string(Scramble.GetNumSteps()) + "] " + GetTextScramble();
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

				if (IsMehtaFBBuilt(C))
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
						ReportLine += "First block not built in " + std::to_string(MaxDepthFB) + " steps";
						if (!AlgFB[sp][n].Empty()) ReportLine += ": (" + AlgFB[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
					}
					if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(Alg3QB[sp][n]);

				if (IsMehta3QBBuilt(C)) ReportLine += " (" + Alg3QB[sp][n].ToString() + ")";
				else
				{
					ReportLine += "3QB not built in " + std::to_string(MaxDepth3QB) + " steps";
					if (!Alg3QB[sp][n].Empty()) ReportLine += ": (" + Alg3QB[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(AlgEOLE[sp][n]);

				if (IsMehtaEOLEBuilt(C)) ReportLine += " (" + AlgEOLE[sp][n].ToString() + ")";
				else
				{
					ReportLine += "EOLE not built";
					if (!AlgEOLE[sp][n].Empty()) ReportLine += ": (" + AlgEOLE[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				if (!Alg6CO[sp].empty()) // Mehta-6CP or Mehta-APDR
				{
					C.ApplyAlgorithm(Alg6CO[sp][n]);

					if (IsMehta6COBuilt(C)) ReportLine += " (" + Alg6CO[sp][n].ToString() + ")";
					else
					{
						ReportLine += "6CO not built";
						if (!Alg6CO[sp][n].Empty()) ReportLine += ": (" + Alg6CO[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}

					if (!Alg6CP[sp].empty()) // Mehta-6CP
					{
						C.ApplyAlgorithm(Alg6CP[sp][n]);

						if (IsMehta6CPBuilt(C)) ReportLine += " (" + Alg6CP[sp][n].ToString() + ")";
						else
						{
							ReportLine += "6CP not built";
							if (!Alg6CP[sp][n].Empty()) ReportLine += ": (" + Alg6CP[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
							if (debug) Report += ReportLine;
							continue;
						}

						C.ApplyAlgorithm(AlgL5EP[sp][n]);

						if (C.IsSolved()) ReportLine += " (" + AlgL5EP[sp][n].ToString() + ")";
						else
						{
							ReportLine += "L5EP not built";
							if (!AlgL5EP[sp][n].Empty()) ReportLine += ": (" + AlgL5EP[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
							if (debug) Report += ReportLine;
							continue;
						}
					}
					else if (!AlgAPDR[sp].empty()) // Mehta-APDR
					{
						C.ApplyAlgorithm(AlgAPDR[sp][n]);

						if (IsMehtaAPDRBuilt(C)) ReportLine += " (" + AlgAPDR[sp][n].ToString() + ")";
						else
						{
							ReportLine += "APDR not built";
							if (!AlgAPDR[sp][n].Empty()) ReportLine += ": (" + AlgAPDR[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
							if (debug) Report += ReportLine;
							continue;
						}

						C.ApplyAlgorithm(AlgPLL[sp][n]);

						if (C.IsSolved()) ReportLine += " (" + AlgPLL[sp][n].ToString() + ")";
						else
						{
							ReportLine += "PLL not built";
							if (!AlgPLL[sp][n].Empty()) ReportLine += ": (" + AlgPLL[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
							if (debug) Report += ReportLine;
							continue;
						}
					}
				}
				else if (!AlgDCAL[sp].empty()) // Mehta-CDRLL or Mehta-JTLE
				{
					C.ApplyAlgorithm(AlgDCAL[sp][n]);

					if (IsMehtaDCALBuilt(C)) ReportLine += " (" + AlgDCAL[sp][n].ToString() + ")";
					else
					{
						ReportLine += "DCAL not built";
						if (!AlgDCAL[sp][n].Empty()) ReportLine += ": (" + AlgDCAL[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}

					if (!AlgCDRLL[sp].empty()) // Mehta-CDRLL
					{
						C.ApplyAlgorithm(AlgCDRLL[sp][n]);

						if (IsMehtaCDRLLBuilt(C)) ReportLine += " (" + AlgCDRLL[sp][n].ToString() + ")";
						else
						{
							ReportLine += "CDRLL not built";
							if (!AlgCDRLL[sp][n].Empty()) ReportLine += ": (" + AlgCDRLL[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
							if (debug) Report += ReportLine;
							continue;
						}

						C.ApplyAlgorithm(AlgL5EP[sp][n]);

						if (C.IsSolved()) ReportLine += " (" + AlgL5EP[sp][n].ToString() + ")";
						else
						{
							ReportLine += "L5EP not built";
							if (!AlgL5EP[sp][n].Empty()) ReportLine += ": (" + AlgL5EP[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
							if (debug) Report += ReportLine;
							continue;
						}
					}
					else if (!AlgJTLE[sp].empty()) // Mehta-JTLL
					{
						C.ApplyAlgorithm(AlgJTLE[sp][n]);

						if (IsMehtaJTLEBuilt(C)) ReportLine += " (" + AlgJTLE[sp][n].ToString() + ")";
						else
						{
							ReportLine += "JTLE not built";
							if (!AlgJTLE[sp][n].Empty()) ReportLine += ": (" + AlgJTLE[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
							if (debug) Report += ReportLine;
							continue;
						}

						C.ApplyAlgorithm(AlgPLL[sp][n]);

						if (C.IsSolved()) ReportLine += " (" + AlgPLL[sp][n].ToString() + ")";
						else
						{
							ReportLine += "PLL not built";
							if (!AlgPLL[sp][n].Empty()) ReportLine += ": (" + AlgPLL[sp][n].ToString() + ")\n";
							else ReportLine.push_back('\n');
							if (debug) Report += ReportLine;
							continue;
						}
					}
				}
				else if (!AlgTDR[sp].empty()) // Mehta-TDR
				{
					C.ApplyAlgorithm(AlgTDR[sp][n]);

					if (IsMehtaTDRBuilt(C)) ReportLine += " (" + AlgTDR[sp][n].ToString() + ")";
					else
					{
						ReportLine += "TDR not built";
						if (!AlgTDR[sp][n].Empty()) ReportLine += ": (" + AlgTDR[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
					}

					C.ApplyAlgorithm(AlgZBLL[sp][n]);

					if (C.IsSolved()) ReportLine += " (" + AlgZBLL[sp][n].ToString() + ")";
					else
					{
						ReportLine += "ZBLL not built";
						if (!AlgZBLL[sp][n].Empty()) ReportLine += ": (" + AlgZBLL[sp][n].ToString() + ")\n";
						else ReportLine.push_back('\n');
						if (debug) Report += ReportLine;
						continue;
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
    std::string Mehta::GetTimeReport() const
    {
        std::string Report;

        Report += "Total time: " + std::to_string(GetFullTime()) + " s\n";
        Report += "First blocks time: " + std::to_string(GetTimeFB()) + " s\n";
        Report += "3QB time: " + std::to_string(GetTime3QB()) + " s\n";
        Report += "EOLE time: " + std::to_string(GetTimeEOLE()) + " s\n";

        if (GetTime6CO() > 0.0) Report += "6CO time: " + std::to_string(GetTime6CO()) + " s\n";
        if (GetTimeDCAL() > 0.0) Report += "DCAL time: " + std::to_string(GetTimeDCAL()) + " s\n";
        if (GetTimeTDR() > 0.0) Report += "TDR time: " + std::to_string(GetTimeTDR()) + " s\n";
        if (GetTime6CP() > 0.0) Report += "6CP time: " + std::to_string(GetTime6CP()) + " s\n";
        if (GetTimeAPDR() > 0.0) Report += "APDR time: " + std::to_string(GetTimeAPDR()) + " s\n";
        if (GetTimeCDRLL() > 0.0) Report += "CDRLL time: " + std::to_string(GetTimeCDRLL()) + " s\n";
        if (GetTimeJTLE() > 0.0) Report += "JTLE time: " + std::to_string(GetTimeJTLE()) + " s\n";
        if (GetTimeL5EP() > 0.0) Report += "L5EP time: " + std::to_string(GetTimeL5EP()) + " s\n";
        if (GetTimePLL() > 0.0) Report += "PLL time: " + std::to_string(GetTimePLL()) + " s\n";
        if (GetTimeZBLL() > 0.0) Report += "ZBLL time: " + std::to_string(GetTimeZBLL()) + " s\n";

        Report += "Threads used: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : DeepSearch::GetSystemCores()) +
                  " of " + std::to_string(DeepSearch::GetSystemCores()) + "\n";

        return Report;
    }

	// Get a solve report for given spin
    std::string Mehta::GetReport(const Spn sp, const uint n) const
	{
		const int si = static_cast<int>(sp);

		if (!CheckSolveConsistency(sp) || AlgFB[si].size() <= n)
		{
			std::string FReport = "No solve for spin ";
			FReport += Cube::GetSpinText(sp);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

        std::string Report;
        // Report += "Mehta search with orientation " + Cube::GetSpinText(sp);
        // Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += GetTextInspection(sp,n) + " // Inspection [" + C.GetSpinText() + "]\n";

        C.ApplyAlgorithm(AlgFB[si][n]);
        if (!IsMehtaFBBuilt(C))
        {
			Report += "First block not built in " + std::to_string(MaxDepthFB) + " movements\n";
            return Report;
        }
        Report += GetTextFB(sp, n) + " // First block (" + Algorithm::GetMetricValue(GetMetricFB(sp, n)) + ")\n";

		C.ApplyAlgorithm(Alg3QB[si][n]);
		if (!IsMehta3QBBuilt(C))
		{
			Report += "3QB not built in " + std::to_string(MaxDepth3QB) + " movements\n";
			return Report;
		}
        if (!Alg3QB[si][n].Empty()) Report += GetText3QB(sp, n) + " // 3QB (" + Algorithm::GetMetricValue(GetMetric3QB(sp, n)) + ")\n";

        C.ApplyAlgorithm(AlgEOLE[si][n]);
		if (!IsMehtaEOLEBuilt(C))
		{
			Report += "EOLE not built\n";
			return Report;
		}
        if (!AlgEOLE[si][n].Empty()) Report += GetTextEOLE(sp, n) + " // EOLE (" + Algorithm::GetMetricValue(GetMetricEOLE(sp, n)) + ")\n";

		if (!Alg6CO[si].empty()) // Mehta-6CP or Mehta-APDR
        {
            // C.ApplyAlgorithm(Alg6CO[si][n]);
            if (!Alg6CO[si][n].Empty()) Report += GetText6CO(sp, n) + " // 6CO (" + Algorithm::GetMetricValue(GetMetric6CO(sp, n)) + ")\n";

			if (!Alg6CP[si].empty()) // Mehta-6CP
			{
                // C.ApplyAlgorithm(Alg6CP[si][n]);
                if (!Alg6CP[si][n].Empty()) Report += GetText6CP(sp, n) + " // 6CP (" + Algorithm::GetMetricValue(GetMetric6CP(sp, n)) + ")\n";

                // C.ApplyAlgorithm(AlgL5EP[si][n]);
                if (!AlgL5EP[si][n].Empty()) Report += GetTextL5EP(sp, n) + " // L5EP (" + Algorithm::GetMetricValue(GetMetricL5EP(sp, n)) + ")\n";
			}
			else if (!AlgAPDR[si].empty()) // Mehta-APDR
			{
                // C.ApplyAlgorithm(AlgAPDR[si][n]);
                if (!AlgAPDR[si][n].Empty()) Report += GetTextAPDR(sp, n) + " // APDR (" + Algorithm::GetMetricValue(GetMetricAPDR(sp, n)) + ")\n";

                // C.ApplyAlgorithm(AlgPLL[si][n]);
                if (!AlgPLL[si][n].Empty()) Report += GetTextPLL(sp, n) + " // PLL (" + Algorithm::GetMetricValue(GetMetricPLL(sp, n)) + ")\n";
			}
        }
        else if (!AlgDCAL[si].empty()) // Mehta-CDRLL or Mehta-JTLE
        {
            // C.ApplyAlgorithm(AlgDCAL[si][n]);
            if (!AlgDCAL[si][n].Empty()) Report += GetTextDCAL(sp, n) + " // DCAL (" + Algorithm::GetMetricValue(GetMetricDCAL(sp, n)) + ")\n";

			if (!AlgCDRLL[si].empty()) // Mehta-CDRLL
			{
                // C.ApplyAlgorithm(AlgCDRLL[si][n]);
                if (!AlgCDRLL[si][n].Empty()) Report += GetTextCDRLL(sp, n) + " // CDRLL (" + Algorithm::GetMetricValue(GetMetricCDRLL(sp, n)) + ")\n";

                // C.ApplyAlgorithm(AlgL5EP[si][n]);
                if (!AlgL5EP[si][n].Empty()) Report += GetTextL5EP(sp, n) + " // L5EP (" + Algorithm::GetMetricValue(GetMetricL5EP(sp, n)) + ")\n";
			}
			else if (!AlgJTLE[si].empty()) // Mehta-JTLE
			{
                // C.ApplyAlgorithm(AlgJTLE[si][n]);
                if (!AlgJTLE[si][n].Empty()) Report += GetTextJTLE(sp, n) + " // JTLE (" + Algorithm::GetMetricValue(GetMetricJTLE(sp, n)) + ")\n";

                // C.ApplyAlgorithm(AlgPLL[si][n]);
                if (!AlgPLL[si][n].Empty()) Report += GetTextPLL(sp, n) + " // PLL (" + Algorithm::GetMetricValue(GetMetricPLL(sp, n)) + ")\n";
			}
        }
        else if (!AlgTDR[si].empty()) // Mehta-TDR
		{
            // C.ApplyAlgorithm(AlgTDR[si][n]);
            if (!AlgTDR[si][n].Empty()) Report += GetTextTDR(sp, n) + " // TDR (" + Algorithm::GetMetricValue(GetMetricTDR(sp, n)) + ")\n";

            // C.ApplyAlgorithm(AlgZBLL[si][n]);
            if (!AlgZBLL[si][n].Empty()) Report += GetTextZBLL(sp, n) + " // ZBLL (" + Algorithm::GetMetricValue(GetMetricZBLL(sp, n)) + ")\n";
        }

        // Show summary
        Report += "\nMetric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
		Report += "EOLE case: " + GetTextEOLECase(sp, n) + "\n";
		if (!Alg6CO[si].empty()) // Metha-6CP or Mehta-APDR
		{
			Report += "6CO case: " + GetText6COCase(sp, n) + "\n";
			if (!Alg6CP[si].empty()) // // Metha-6CP
			{
				Report += "6CP case: " + GetText6CPCase(sp, n) + "\n";
				Report += "L5EP case: " + GetTextL5EPCase(sp, n) + "\n";
			}
			else if (!AlgAPDR[si].empty()) // // Metha-APDR
			{
				Report += "APDR case: " + GetTextAPDRCase(sp, n) + "\n";
				Report += "PLL case: " + GetTextPLLCase(sp, n) + "\n";
			}
		}
		else if (!AlgDCAL[si].empty()) // Metha-CDRLL or Mehta-JTLE
		{
			Report += "DCAL case: " + GetTextDCALCase(sp, n) + "\n";
			if (!AlgCDRLL[si].empty()) // Metha-CDRLL
			{
				Report += "CDRLL case: " + GetTextCDRLLCase(sp, n) + "\n";
				Report += "L5EP case: " + GetTextL5EPCase(sp, n) + "\n";
			}
			else if (!AlgJTLE[si].empty()) // Metha-JTLE
			{
				Report += "JTLE case: " + GetTextJTLECase(sp, n) + "\n";
				Report += "PLL case: " + GetTextPLLCase(sp, n) + "\n";
			}
		}
		else if (!AlgTDR[si].empty()) // Mehta-TDR
		{
			Report += "TDR case: " + GetTextTDRCase(sp, n) + "\n";
			Report += "ZBLL case: " + GetTextZBLLCase(sp, n) + "\n";
		}
		return Report;
	}

	// Check if the solves for the given spin are consistent (all needed algorithms are present)
	bool Mehta::CheckSolveConsistency(const Spn Spin) const
	{
		const int si = static_cast<int>(Spin); // Spin index

		if (Inspections[si].empty()) return false;

		auto n = Inspections[si].size();

		return (AlgFB[si].size() == n) && (Alg3QB[si].size() == n) && (AlgEOLE[si].size() == n) &&
			   (((Alg6CO[si].size() == n) && (Alg6CP[si].size() == n) && (AlgL5EP[si].size() == n)) || // Mehta-6CP
			   ((Alg6CO[si].size() == n) && (AlgAPDR[si].size() == n) && (AlgPLL[si].size() == n)) || // Mehta-APDR
			   ((AlgDCAL[si].size() == n) && (AlgCDRLL[si].size() == n) && (AlgL5EP[si].size() == n)) || // Mehta-CDRLL
			   ((AlgDCAL[si].size() == n) && (AlgJTLE[si].size() == n) && (AlgPLL[si].size() == n)) || // Mehta-JTLE
			   ((AlgTDR[si].size() == n) && (AlgZBLL[si].size() == n))); // Mehta-TDR
	}

	// Get the full solve
    Algorithm Mehta::GetFullSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || n >= Inspections[si].size()) return 0u;

		Algorithm A = Inspections[si][n];
		A += AlgFB[si][n];
		A += Alg3QB[si][n];
		A += AlgEOLE[si][n];

		if (!Alg6CO[si].empty())
		{
			A += Alg6CO[si][n];
			if (!Alg6CP[si].empty())
			{
				A += Alg6CP[si][n];
				A += AlgL5EP[si][n];
			}
			else if (!AlgAPDR[si].empty())
			{
				A += AlgAPDR[si][n];
				A += AlgPLL[si][n];
			}
		}
		else if (!AlgDCAL[si].empty())
		{
			A += AlgDCAL[si][n];
			if (!AlgCDRLL[si].empty())
			{
				A += AlgCDRLL[si][n];
				A += AlgL5EP[si][n];
			}
			else if (!AlgJTLE[si].empty())
			{
				A += AlgJTLE[si][n];
				A += AlgPLL[si][n];
			}
		}
		else if (!AlgTDR[si].empty())
		{
			A += AlgTDR[si][n];
			A += AlgZBLL[si][n];
		}
		
		return A;
	}
}
