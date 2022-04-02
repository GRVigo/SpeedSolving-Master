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

#include <chrono>
#include <algorithm>

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

		SearchSpins.clear();
		for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s));
		
		Metric = Metrics::Movements; // Default metric
	}

    // Search the best first blocks solve algorithm with the given search depth
	// Return false if no first blocks found
    bool Mehta::SearchFB(const uint MaxDepth, const uint MaxSolves)
	{
        const auto time_fb_start = std::chrono::system_clock::now();

        MaxDepthFB = (MaxDepth <= 4u ? 4u : MaxDepth);

        DeepSearch DSFB(Scramble); // Deep search for first block
		
		// Mheta first blocks are Roux first blocks with other spin
        DSFB.AddToOptionalPieces(Pgr::RB_B1);
		DSFB.AddToOptionalPieces(Pgr::LF_B1);
		DSFB.AddToOptionalPieces(Pgr::BL_B1);
		DSFB.AddToOptionalPieces(Pgr::FR_B1);

		DSFB.AddToOptionalPieces(Pgr::LB_B1);
		DSFB.AddToOptionalPieces(Pgr::RF_B1);
		DSFB.AddToOptionalPieces(Pgr::FL_B1);
		DSFB.AddToOptionalPieces(Pgr::BR_B1);

		DSFB.AddToOptionalPieces(Pgr::LD_B1);
		DSFB.AddToOptionalPieces(Pgr::RU_B1);
		DSFB.AddToOptionalPieces(Pgr::UL_B1);
		DSFB.AddToOptionalPieces(Pgr::DR_B1);

		DSFB.AddToOptionalPieces(Pgr::RD_B1);
		DSFB.AddToOptionalPieces(Pgr::LU_B1);
		DSFB.AddToOptionalPieces(Pgr::DL_B1);
		DSFB.AddToOptionalPieces(Pgr::UR_B1);

		DSFB.AddToOptionalPieces(Pgr::FD_B1);
		DSFB.AddToOptionalPieces(Pgr::BU_B1);
		DSFB.AddToOptionalPieces(Pgr::DB_B1);
		DSFB.AddToOptionalPieces(Pgr::UF_B1);

		DSFB.AddToOptionalPieces(Pgr::BD_B1);
		DSFB.AddToOptionalPieces(Pgr::FU_B1);
		DSFB.AddToOptionalPieces(Pgr::UB_B1);
		DSFB.AddToOptionalPieces(Pgr::DF_B1);

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

        DSFB.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
        DSFB.AddSearchLevel(L_NoCheck); // Level 2
        DSFB.AddSearchLevel(L_Check); // Level 3
        for (uint l = 4u; l < MaxDepthFB; l++) DSFB.AddSearchLevel(L_Check); // Levels 4 to MaxDepth

        DSFB.UpdateRootData();
		// DSFB.SetMinDeep(DSFB.GetMaxDepth() - 2u);

        DSFB.Run(Cores);

        Cores = DSFB.GetCoresUsed(); // Update to the real number of cores used

        EvaluateFB(DSFB.Solves, MaxSolves);

        const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
        TimeFB = fb_elapsed_seconds.count();

        return !DSFB.Solves.empty();
	}

	// Search 3 Quarters Belt - Solve 3 E-slice edges relative to the centers
	void Mehta::Search3QB(const uint MaxDepth)
	{
		const auto time_3qb_start = std::chrono::system_clock::now();

		MaxDepth3QB = (MaxDepth <= 4u ? 4u : MaxDepth);

		// First level is extended in the search to improve the multithreading - first level will not be checked
		// (it's supose that the first block not will be solved in a single movement)
		const SearchUnit URoot(SequenceType::DOUBLE);
		const SearchUnit U(SequenceType::SINGLE);

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
				Algorithm AlgStart = Scramble;
				AlgStart.Append(Inspections[sp][n]);
				AlgStart.Append(FB);
				
				Cube CubeMehta(AlgStart);
				
				if (!IsFBBuilt(CubeMehta, spin))
				{
					Alg3QB[sp].push_back(Algorithm(""));
					continue;
				}

				DeepSearch DS3QB(AlgStart); // Deep search for 3QB

				DS3QB.AddToMandatoryPieces(B1);
				DS3QB.AddToOptionalPieces(BELT1);
				DS3QB.AddToOptionalPieces(BELT2);
				DS3QB.AddToOptionalPieces(BELT3);
				DS3QB.AddToOptionalPieces(BELT4);

				DS3QB.AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
				for (uint l = 2u; l < MaxDepth3QB; l++) DS3QB.AddSearchLevel(L_Check); // Add needed search levels

				DS3QB.UpdateRootData();

				DS3QB.Run(Cores);

				Cores = DS3QB.GetCoresUsed(); // Update to the real number of cores used

				std::vector<Algorithm> Solves;
				
				Evaluate3QBResult(Solves, 1u, DS3QB.Solves, CubeMehta, spin, Plc::BEST_SOLVES);

				if (Solves.empty()) Alg3QB[sp].push_back(Algorithm(""));
				else
				{
					CubeMehta.ApplyAlgorithm(Solves[0]);
					if (Is3QBBuilt(CubeMehta)) Alg3QB[sp].push_back(Solves[0]);
					else Alg3QB[sp].push_back(Algorithm(""));
				}
				n++;
			}
		}

		const std::chrono::duration<double> _3qb_elapsed_seconds = std::chrono::system_clock::now() - time_3qb_start;
		Time3QB = _3qb_elapsed_seconds.count();
	}
	
	// Edge Orientation + Last Edge - Insert the remaining E-slice edge while orienting all the edges
	void Mehta::SearchEOLE()
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
				
				if (!Is3QBBuilt(CubeEOLE, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::GetEOLECases(); Index++)
				{
					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto EMov : Algorithm::EMovs) // E movement before algorithm
						{
							Cube CubeMetha = CubeEOLE;

							if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
							if (EMov != Stp::NONE) CubeMetha.ApplyStep(EMov);
							CubeMetha.ApplyAlgorithm(Collection::GetEOLE(Index));
							if (EMov != Stp::NONE) CubeMetha.ApplyStep(Algorithm::InvertedStep(EMov));

							if (IsEOLEBuilt(CubeMetha, spin))
							{
								Found = true;

								CasesEOLE[sp][n] = Collection::GetEOLECase(Index);

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
								AlgEOLE[sp][n].Append(Collection::GetEOLE(Index));
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
	void Mehta::Search6CO()
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
				
				if (!IsEOLEBuilt(Cube6CO, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::Get6COCases(); Index++)
				{
					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeMetha = Cube6CO;

						if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
						CubeMetha.ApplyAlgorithm(Collection::Get6CO(Index));

						if (Is6COBuilt(CubeMetha, spin))
						{
							Found = true;

							Cases6CO[sp][n] = Collection::Get6COCase(Index);

							if (UMov != Stp::NONE)
							{
								Alg6CO[sp][n].Append(Stp::PARENTHESIS_OPEN);
								Alg6CO[sp][n].Append(UMov);
								Alg6CO[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							Alg6CO[sp][n].Append(Collection::Get6CO(Index));

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
	void Mehta::Search6CP()
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
				
				if (!Is6COBuilt(Cube6CP, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::Get6CPCases(); Index++)
				{
					for (const auto U1Mov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto U2Mov : Algorithm::UMovs) // U movement after algorithm
						{
							Cube CubeMetha = Cube6CP;

							if (U1Mov != Stp::NONE) CubeMetha.ApplyStep(U1Mov);
							CubeMetha.ApplyAlgorithm(Collection::Get6CP(Index));
							if (U2Mov != Stp::NONE) CubeMetha.ApplyStep(U2Mov);

							if (Is6CPBuilt(CubeMetha, spin))
							{
								Found = true;

								Cases6CP[sp][n] = Collection::Get6CPCase(Index);

								if (U1Mov != Stp::NONE)
								{
									Alg6CP[sp][n].Append(Stp::PARENTHESIS_OPEN);
									Alg6CP[sp][n].Append(U1Mov);
									Alg6CP[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								Alg6CP[sp][n].Append(Collection::Get6CP(Index));

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
    void Mehta::SearchL5EP()
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
				
				if ((!Alg6CP[sp].empty() && !Is6CPBuilt(CubeL5EP, spin)) || 
				    (!AlgCDRLL[sp].empty() && !IsCDRLLBuilt(CubeL5EP, spin))) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::GetL5EPCases(); Index++)
				{
					for (const auto U1Mov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto U2Mov : Algorithm::UMovs) // U movement after algorithm
						{
							Cube CubeMetha = CubeL5EP;

							if (U1Mov != Stp::NONE) CubeMetha.ApplyStep(U1Mov);
							CubeMetha.ApplyAlgorithm(Collection::GetL5EP(Index));
							if (U2Mov != Stp::NONE) CubeMetha.ApplyStep(U2Mov);

							if (CubeMetha.IsSolved())
							{
								Found = true;

								CasesL5EP[sp][n] = Collection::GetL5EPCase(Index);

								if (U1Mov != Stp::NONE)
								{
									AlgL5EP[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgL5EP[sp][n].Append(U1Mov);
									AlgL5EP[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								AlgL5EP[sp][n].Append(Collection::GetL5EP(Index));

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
	void Mehta::SearchAPDR()
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
				
				if (!Is6COBuilt(CubeAPDR, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::GetAPDRCases(); Index++)
				{
					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeMetha = CubeAPDR;

						if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
						CubeMetha.ApplyAlgorithm(Collection::GetAPDR(Index));

						if (IsAPDRBuilt(CubeMetha, spin))
						{
							Found = true;

							CasesAPDR[sp][n] = Collection::GetAPDRCase(Index);

							if (UMov != Stp::NONE)
							{
								AlgAPDR[sp][n].Append(Stp::PARENTHESIS_OPEN);
								AlgAPDR[sp][n].Append(UMov);
								AlgAPDR[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							AlgAPDR[sp][n].Append(Collection::GetAPDR(Index));

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
	void Mehta::SearchPLL()
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
				
				if (!IsAPDRBuilt(CubePLL, spin)) continue; // IsJTLEBuilt == IsAPDRBuit

				Stp AUFStep;

				Collection::SolveLL(AlgPLL[sp][n], CasesPLL[sp][n], AUFStep, AlgSets::PLL, CubePLL);

				if (AUFStep != Stp::NONE) AlgPLL[sp][n].Append(AUFStep);
			}
		}

		const std::chrono::duration<double> PLL_elapsed_seconds = std::chrono::system_clock::now() - time_PLL_start;
		TimePLL = PLL_elapsed_seconds.count();
	}
	
	// Solve the 2 corners of the D layer
	void Mehta::SearchDCAL()
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
				
				if (!IsEOLEBuilt(CubeDCAL, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::GetDCALCases(); Index++)
				{
					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeMetha = CubeDCAL;

						if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
						CubeMetha.ApplyAlgorithm(Collection::GetDCAL(Index));

						if (IsDCALBuilt(CubeMetha, spin))
						{
							Found = true;

							CasesDCAL[sp][n] = Collection::GetDCALCase(Index);

							if (UMov != Stp::NONE)
							{
								AlgDCAL[sp][n].Append(Stp::PARENTHESIS_OPEN);
								AlgDCAL[sp][n].Append(UMov);
								AlgDCAL[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							AlgDCAL[sp][n].Append(Collection::GetDCAL(Index));

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
	void Mehta::SearchCDRLL()
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
				
				if (!IsDCALBuilt(CubeCDRLL, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::GetCDRLLCases(); Index++)
				{
					for (const auto U1Mov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto U2Mov : Algorithm::UMovs) // U movement after algorithm
						{
							Cube CubeMetha = CubeCDRLL;

							if (U1Mov != Stp::NONE) CubeMetha.ApplyStep(U1Mov);
							CubeMetha.ApplyAlgorithm(Collection::GetCDRLL(Index));
							if (U2Mov != Stp::NONE) CubeMetha.ApplyStep(U2Mov);

							if (IsCDRLLBuilt(CubeMetha, spin))
							{
								Found = true;

								CasesCDRLL[sp][n] = Collection::GetCDRLLCase(Index);

								if (U1Mov != Stp::NONE)
								{
									AlgCDRLL[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgCDRLL[sp][n].Append(U1Mov);
									AlgCDRLL[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}

								AlgCDRLL[sp][n].Append(Collection::GetCDRLL(Index));

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
	void Mehta::SearchJTLE()
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
				
				if (!IsDCALBuilt(CubeJTLE, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::GetJTLECases(); Index++)
				{
					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						Cube CubeMetha = CubeJTLE;

						if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
						CubeMetha.ApplyAlgorithm(Collection::GetJTLE(Index));

						if (IsJTLEBuilt(CubeMetha, spin))
						{
							Found = true;

							CasesJTLE[sp][n] = Collection::GetJTLECase(Index);

							if (UMov != Stp::NONE)
							{
								AlgJTLE[sp][n].Append(Stp::PARENTHESIS_OPEN);
								AlgJTLE[sp][n].Append(UMov);
								AlgJTLE[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}

							AlgJTLE[sp][n].Append(Collection::GetJTLE(Index));

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
	void Mehta::SearchTDR()
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
				
				if (!IsEOLEBuilt(CubeTDR, spin)) continue;

				bool Found = false;

				for (uint Index = 0u; Index < Collection::GetTDRCases(); Index++)
				{
					for (const auto UMov : Algorithm::UMovs) // U movement before algorithm
					{
						for (const auto DMov : Algorithm::DMovs) // D movement after algorithm
						{
							Cube CubeMetha = CubeTDR;
							if (UMov != Stp::NONE) CubeMetha.ApplyStep(UMov);
							CubeMetha.ApplyAlgorithm(Collection::GetTDR(Index));
							if (DMov != Stp::NONE) CubeMetha.ApplyStep(DMov);

							if (IsTDRBuilt(CubeMetha, spin))
							{
								Found = true;

								CasesTDR[sp][n] = Collection::GetTDRCase(Index);

								if (UMov != Stp::NONE)
								{
									AlgTDR[sp][n].Append(Stp::PARENTHESIS_OPEN);
									AlgTDR[sp][n].Append(UMov);
									AlgTDR[sp][n].Append(Stp::PARENTHESIS_CLOSE_1_REP);
								}
								AlgTDR[sp][n].Append(Collection::GetTDR(Index));
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
	void Mehta::SearchZBLL()
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
				
				if (!IsTDRBuilt(CubeZBLL, spin)) continue;

				Stp AUFStep;

				Collection::SolveLL(AlgZBLL[sp][n], CasesZBLL[sp][n], AUFStep, AlgSets::ZBLL, CubeZBLL);

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
			EvaluateFBResult(AlgFB[sp], MaxSolves, Solves, CubeBase, spin, Plc::SHORT);
        }

		for (int sp = 0; sp < 24; sp++)
		{
			Spn spin = static_cast<Spn>(sp);
			Stp T1, T2;
			Cube::GetSpinsSteps(CubeBase.GetSpin(), spin, T1, T2);
			Algorithm Insp;
			if (T1 != Stp::NONE) Insp.Append(T1);
			if (T2 != Stp::NONE) Insp.Append(T2);

			for (auto& block : AlgFB[sp])
			{
				if (T1 != Stp::NONE) block.TransformTurn(T1);
				if (T2 != Stp::NONE) block.TransformTurn(T2);
				Inspections[sp].push_back(Insp);
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

	// Check if the Mehta first block is built
	bool Mehta::IsFBBuilt(const Cube& C)
	{
		switch (C.GetSpin())
		{
		case Spn::UF: return C.IsSolved(Pgr::RB_B1);
		case Spn::UB: return C.IsSolved(Pgr::LF_B1);
		case Spn::UR: return C.IsSolved(Pgr::BL_B1);
		case Spn::UL: return C.IsSolved(Pgr::FR_B1);

		case Spn::DF: return C.IsSolved(Pgr::LB_B1);
		case Spn::DB: return C.IsSolved(Pgr::RF_B1);
		case Spn::DR: return C.IsSolved(Pgr::FL_B1);
		case Spn::DL: return C.IsSolved(Pgr::BR_B1);

		case Spn::FU: return C.IsSolved(Pgr::LD_B1);
		case Spn::FD: return C.IsSolved(Pgr::RU_B1);
		case Spn::FR: return C.IsSolved(Pgr::UL_B1);
		case Spn::FL: return C.IsSolved(Pgr::DR_B1);

		case Spn::BU: return C.IsSolved(Pgr::RD_B1);
		case Spn::BD: return C.IsSolved(Pgr::LU_B1);
		case Spn::BR: return C.IsSolved(Pgr::DL_B1);
		case Spn::BL: return C.IsSolved(Pgr::UR_B1);

		case Spn::RU: return C.IsSolved(Pgr::FD_B1);
		case Spn::RD: return C.IsSolved(Pgr::BU_B1);
		case Spn::RF: return C.IsSolved(Pgr::DB_B1);
		case Spn::RB: return C.IsSolved(Pgr::UF_B1);

		case Spn::LU: return C.IsSolved(Pgr::BD_B1);
		case Spn::LD: return C.IsSolved(Pgr::FU_B1);
		case Spn::LF: return C.IsSolved(Pgr::UB_B1);
		case Spn::LB: return C.IsSolved(Pgr::DF_B1);

		default: return false; // Should not happend
		}
	}

	// Check if the first Mehta block is built with the given spin
	bool Mehta::IsFBBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return IsFBBuilt(CubeMehta);
	}

	// Check if the Mehta 3QB is built (also first block)
	bool Mehta::Is3QBBuilt(const Cube& C)
	{
		if (!IsFBBuilt(C)) return false;

		// Pieces to search
		Pce E_1, E_2, E_3, E_4;

		// Get the required pieces based on the spin
		switch (C.GetSpin())
		{
		case Spn::UF:
		case Spn::UB:
		case Spn::UR:
		case Spn::UL:
		case Spn::DF:
		case Spn::DB:
		case Spn::DR:
		case Spn::DL: E_1 = Pce::FR; E_2 = Pce::FL; E_3 = Pce::BR; E_4 = Pce::BL; break;

		case Spn::FU:
		case Spn::FD:
		case Spn::FR:
		case Spn::FL:
		case Spn::BU:
		case Spn::BD:
		case Spn::BR:
		case Spn::BL: E_1 = Pce::UR; E_2 = Pce::UL; E_3 = Pce::DR; E_4 = Pce::DL; break;

		case Spn::RU:
		case Spn::RD:
		case Spn::RF:
		case Spn::RB:
		case Spn::LU:
		case Spn::LD:
		case Spn::LF:
		case Spn::LB: E_1 = Pce::UF; E_2 = Pce::UB; E_3 = Pce::DF; E_4 = Pce::DB; break;

		default: return false; // Invalid spin (should not happend)
		}

		uint score = 0u;

		if (C.IsSolved(E_1)) score++;
		if (C.IsSolved(E_2)) score++;
		if (C.IsSolved(E_3)) score++;
		if (C.IsSolved(E_4)) score++;

		return score >= 3u;
	}

	// Check if Mehta 3QB is built with the given spin
	bool Mehta::Is3QBBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return Is3QBBuilt(CubeMehta);
	}

	// Check if the Mehta EOLE is built (also first block)
	bool Mehta::IsEOLEBuilt(const Cube& C)
	{
		if (!IsFBBuilt(C) || !C.CheckOrientation(Pgr::ALL_EDGES)) return false;

		// Get the required pieces based on the spin
		switch (C.GetSpin())
		{
		case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL:
		case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return C.IsSolved(Lyr::E);

		case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL:
		case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return C.IsSolved(Lyr::S);

		case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB:
		case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return C.IsSolved(Lyr::M);

		default: return false; // Invalid spin (should not happend)
		}
	}

	// Check if Mehta EOLE is built with the given spin
	bool Mehta::IsEOLEBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return IsEOLEBuilt(CubeMehta);
	}
	
	// Check if the Mehta 6CO is built
	bool Mehta::Is6COBuilt(const Cube& C)
	{
		return IsEOLEBuilt(C) && C.CheckOrientation(Pgr::ALL_CORNERS);
	}

	// Check if Mehta 6CO is built with the given spin
	bool Mehta::Is6COBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return Is6COBuilt(CubeMehta);
	}

	// Check if the Mehta 6CP is built
	bool Mehta::Is6CPBuilt(const Cube& C)
	{
		return IsEOLEBuilt(C) && C.IsSolved(Pgr::ALL_CORNERS);
	}

	// Check if Mehta 6CP is built with the given spin
	bool Mehta::Is6CPBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return Is6CPBuilt(CubeMehta);
	}

	// Check if the Mehta APDR is built
	bool Mehta::IsAPDRBuilt(const Cube& C)
	{
		// Get the required pieces based on the spin
		switch (C.GetSpin())
		{
		case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return C.IsSolved(Lyr::D) && C.IsSolved(Lyr::E) && C.CheckOrientation(Pgr::LAYER_U);
		case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return C.IsSolved(Lyr::U) && C.IsSolved(Lyr::E) && C.CheckOrientation(Pgr::LAYER_D);

		case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return C.IsSolved(Lyr::B) && C.IsSolved(Lyr::S) && C.CheckOrientation(Pgr::LAYER_F);
		case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return C.IsSolved(Lyr::F) && C.IsSolved(Lyr::S) && C.CheckOrientation(Pgr::LAYER_B);

		case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return C.IsSolved(Lyr::L) && C.IsSolved(Lyr::M) && C.CheckOrientation(Pgr::LAYER_R);
		case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return C.IsSolved(Lyr::R) && C.IsSolved(Lyr::M) && C.CheckOrientation(Pgr::LAYER_L);

		default: return false; // Invalid spin (should not happend)
		}
	}

	// Check if Mehta APDR is built with the given spin
	bool Mehta::IsAPDRBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return IsAPDRBuilt(CubeMehta);
	}

	// Check if the Mehta DCAL is built
	bool Mehta::IsDCALBuilt(const Cube& C)
	{
		if (!IsEOLEBuilt(C)) return false;
		return C.IsSolved(Cube::FromAbsPosition(App::DFR, C.GetSpin())) && C.IsSolved(Cube::FromAbsPosition(App::DBR, C.GetSpin()));
	}

	// Check if Mehta DCAL is built with the given spin
	bool Mehta::IsDCALBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return IsDCALBuilt(CubeMehta);
	}

	// Check if the Mehta CDRLL is built
	bool Mehta::IsCDRLLBuilt(const Cube& C)
	{
		if (!IsDCALBuilt(C)) return false;
		
		switch (C.GetSpin())
		{
		case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL: return C.IsSolved(Pgr::CORNERS_U);
		case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: return C.IsSolved(Pgr::CORNERS_D);
		
		case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL: return C.IsSolved(Pgr::CORNERS_F);
		case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: return C.IsSolved(Pgr::CORNERS_B);
		
		case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB: return C.IsSolved(Pgr::CORNERS_R);
		case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: return C.IsSolved(Pgr::CORNERS_L);
		
		default: return false;
		}
	}

	// Check if Mehta CDRLL is built with the given spin
	bool Mehta::IsCDRLLBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return IsCDRLLBuilt(CubeMehta);
	}

	// Check if the Mehta JTLE is built (same conditions than APDR)
	bool Mehta::IsJTLEBuilt(const Cube& C)
	{
		return IsAPDRBuilt(C);
	}

	// Check if Mehta JTLE is built with the given spin
	bool Mehta::IsJTLEBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return IsJTLEBuilt(CubeMehta);
	}

	// Check if the Mehta TDR is built
	bool Mehta::IsTDRBuilt(const Cube& C)
	{
		if (!C.CheckOrientation(Pgr::ALL_EDGES)) return false;
		switch (Cube::GetUpSliceLayer(C.GetSpin()))
		{
		case Lyr::U: return C.IsSolved(Pgr::LAYER_D) && C.IsSolved(Pgr::LAYER_E);
		case Lyr::D: return C.IsSolved(Pgr::LAYER_U) && C.IsSolved(Pgr::LAYER_E);
		case Lyr::F: return C.IsSolved(Pgr::LAYER_B) && C.IsSolved(Pgr::LAYER_S);
		case Lyr::B: return C.IsSolved(Pgr::LAYER_F) && C.IsSolved(Pgr::LAYER_S);
		case Lyr::R: return C.IsSolved(Pgr::LAYER_L) && C.IsSolved(Pgr::LAYER_M);
		case Lyr::L: return C.IsSolved(Pgr::LAYER_R) && C.IsSolved(Pgr::LAYER_M);
		default: return false;
		}
	}

	// Check if Mehta TDR is built with the given spin
	bool Mehta::IsTDRBuilt(const Cube& C, const Spn S)
	{
		Cube CubeMehta = C;
		CubeMehta.SetSpin(S); // Mehta cube with desired spin
		return IsTDRBuilt(CubeMehta);
	}

	// Returns best first block solve algorithm from the Solves vector class member and his score for the given spin
	bool Mehta::EvaluateFBResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
	{
		BestSolves.clear();

		if (Solves.empty()) return false; // No solves

		// Pieces to search
		Pce E_1, E_2, E_3, E_4;

		// Get the required pieces based on the spin
		switch (Sp)
		{
		case Spn::UF: case Spn::UB: case Spn::UR: case Spn::UL:
		case Spn::DF: case Spn::DB: case Spn::DR: case Spn::DL: 
			E_1 = Pce::FR; E_2 = Pce::FL; E_3 = Pce::BR; E_4 = Pce::BL; break;

		case Spn::FU: case Spn::FD: case Spn::FR: case Spn::FL:
		case Spn::BU: case Spn::BD: case Spn::BR: case Spn::BL: 
			E_1 = Pce::UR; E_2 = Pce::UL; E_3 = Pce::DR; E_4 = Pce::DL; break;

		case Spn::RU: case Spn::RD: case Spn::RF: case Spn::RB:
		case Spn::LU: case Spn::LD: case Spn::LF: case Spn::LB: 
			E_1 = Pce::UF; E_2 = Pce::UB; E_3 = Pce::DF; E_4 = Pce::DB; break;

		default: return 0u; // Invalid spin (should not happend)
		}

		// Based on the search policy, different conditions have different weights in the final score
		uint SolveSizeMagnitude, SolveBeltMagnitude;
		switch (Policy)
		{
		case Plc::SHORT:
			SolveSizeMagnitude = 1000u;
			SolveBeltMagnitude = 100u;
			break;
		case Plc::BEST_SOLVES:
		default:
			SolveSizeMagnitude = 100u;
			SolveBeltMagnitude = 1000u;
			break;
		}

		uint Score;

		std::vector<std::pair<uint, Algorithm>> ScoredSolves;

		for (const auto& s : Solves) // Check each solve to get the best one
		{
			Score = 0u;

			Cube CheckCube = CBase;
			CheckCube.ApplyAlgorithm(s);

			if (!IsFBBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

			// Evaluate solve size
			Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
			Score += s.GetSubjectiveScore();

			// Check belt edges solves
			if (CheckCube.IsSolved(E_1)) Score += SolveBeltMagnitude;
			if (CheckCube.IsSolved(E_2)) Score += SolveBeltMagnitude;
			if (CheckCube.IsSolved(E_3)) Score += SolveBeltMagnitude;
			if (CheckCube.IsSolved(E_4)) Score += SolveBeltMagnitude;

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

	// Returns best 3QB solve algorithm from the Solves vector class member and his score for the given spin
	bool Mehta::Evaluate3QBResult(std::vector<Algorithm>& BestSolves, const uint MaxSolves, const std::vector<Algorithm>& Solves, const Cube& CBase, const Spn Sp, const Plc Policy)
	{
		BestSolves.clear();

		if (Solves.empty()) return false; // No solves

		Pce E_1, E_2, E_3, E_4; // Pieces to search
		Pcp EP_1, EP_2, EP_3, EP_4; // Edge positions to check orientation

		// Get the required pieces based on the spin
		switch (Sp)
		{
		case Spn::UF:
		case Spn::UB:
		case Spn::UR:
		case Spn::UL: E_1 = Pce::FR; E_2 = Pce::FL; E_3 = Pce::BR; E_4 = Pce::BL; EP_1 = Pcp::UF; EP_2 = Pcp::UB; EP_3 = Pcp::UR; EP_4 = Pcp::UL; break;

		case Spn::DF:
		case Spn::DB:
		case Spn::DR:
		case Spn::DL: E_1 = Pce::FR; E_2 = Pce::FL; E_3 = Pce::BR; E_4 = Pce::BL; EP_1 = Pcp::DF; EP_2 = Pcp::DB; EP_3 = Pcp::DR; EP_4 = Pcp::DL; break;

		case Spn::FU:
		case Spn::FD:
		case Spn::FR:
		case Spn::FL: E_1 = Pce::UR; E_2 = Pce::UL; E_3 = Pce::DR; E_4 = Pce::DL; EP_1 = Pcp::UF; EP_2 = Pcp::DF; EP_3 = Pcp::FR; EP_4 = Pcp::FL; break;

		case Spn::BU:
		case Spn::BD:
		case Spn::BR:
		case Spn::BL: E_1 = Pce::UR; E_2 = Pce::UL; E_3 = Pce::DR; E_4 = Pce::DL; EP_1 = Pcp::UB; EP_2 = Pcp::DB; EP_3 = Pcp::BR; EP_4 = Pcp::BL; break;

		case Spn::RU:
		case Spn::RD:
		case Spn::RF:
		case Spn::RB: E_1 = Pce::UF; E_2 = Pce::UB; E_3 = Pce::DF; E_4 = Pce::DB; EP_1 = Pcp::UR; EP_2 = Pcp::DR; EP_3 = Pcp::FR; EP_4 = Pcp::FL; break;

		case Spn::LU:
		case Spn::LD:
		case Spn::LF:
		case Spn::LB: E_1 = Pce::UF; E_2 = Pce::UB; E_3 = Pce::DF; E_4 = Pce::DB; EP_1 = Pcp::UL; EP_2 = Pcp::DL; EP_3 = Pcp::FL; EP_4 = Pcp::BL; break;

		default: return 0u; // Invalid spin (should not happend)
		}

		// Based on the search policy, different conditions have different weights in the final score
		uint SolveSizeMagnitude, SolveBeltMagnitude, SolveEOMagnitude;
		switch (Policy)
		{
		case Plc::SHORT:
			SolveSizeMagnitude = 1000u;
			SolveBeltMagnitude = 100u;
			SolveEOMagnitude = 10u;
			break;
		case Plc::BEST_SOLVES:
		default:
			SolveSizeMagnitude = 10u;
			SolveBeltMagnitude = 1000u;
			SolveEOMagnitude = 100u;
			break;
		}

		uint Score;

		std::vector<std::pair<uint, Algorithm>> ScoredSolves;

		for (const auto& s : Solves) // Check each solve to get the best one
		{
			Score = 0u;

			Cube CheckCube = CBase;
			CheckCube.ApplyAlgorithm(s);

			if (!Is3QBBuilt(CheckCube, Sp)) continue; // Invalid solve for this spin

			// Evaluate solve size
			Score = (s.GetSize() > 50u ? 0u : 50u - s.GetSize()) * SolveSizeMagnitude;
			Score += s.GetSubjectiveScore();

			// Check belt edges solves
			if (CheckCube.IsSolved(E_1)) Score += SolveBeltMagnitude;
			if (CheckCube.IsSolved(E_2)) Score += SolveBeltMagnitude;
			if (CheckCube.IsSolved(E_3)) Score += SolveBeltMagnitude;
			if (CheckCube.IsSolved(E_4)) Score += SolveBeltMagnitude;
			
			// Give preference to belt with unsolved edge in FR absolut position
			if (!CheckCube.IsSolved(Cube::FromAbsPosition(App::FR, Sp))) Score += SolveBeltMagnitude / 2u;
			
			// Check up layer edges orientation
			if (CheckCube.CheckOrientation(EP_1)) Score += SolveEOMagnitude;
			if (CheckCube.CheckOrientation(EP_2)) Score += SolveEOMagnitude;
			if (CheckCube.CheckOrientation(EP_3)) Score += SolveEOMagnitude;
			if (CheckCube.CheckOrientation(EP_4)) Score += SolveEOMagnitude;

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
    std::string Mehta::GetReport(const bool cancellations, bool debug) const
    {
        std::string Report  = "Mehta search for Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble(), ReportLine;
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
				C.ApplyAlgorithm(AlgFB[sp][n]);

				if (IsFBBuilt(C))
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

				if (Is3QBBuilt(C)) ReportLine += " (" + Alg3QB[sp][n].ToString() + ")";
				else
				{
					ReportLine += "3QB not built in " + std::to_string(MaxDepth3QB) + " steps";
					if (!Alg3QB[sp][n].Empty()) ReportLine += ": (" + Alg3QB[sp][n].ToString() + ")\n";
					else ReportLine.push_back('\n');
					if (debug) Report += ReportLine;
					continue;
				}

				C.ApplyAlgorithm(AlgEOLE[sp][n]);

				if (IsEOLEBuilt(C)) ReportLine += " (" + AlgEOLE[sp][n].ToString() + ")";
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

					if (Is6COBuilt(C)) ReportLine += " (" + Alg6CO[sp][n].ToString() + ")";
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

						if (Is6CPBuilt(C)) ReportLine += " (" + Alg6CP[sp][n].ToString() + ")";
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

						if (IsAPDRBuilt(C)) ReportLine += " (" + AlgAPDR[sp][n].ToString() + ")";
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

					if (IsDCALBuilt(C)) ReportLine += " (" + AlgDCAL[sp][n].ToString() + ")";
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

						if (IsCDRLLBuilt(C)) ReportLine += " (" + AlgCDRLL[sp][n].ToString() + ")";
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

						if (IsJTLEBuilt(C)) ReportLine += " (" + AlgJTLE[sp][n].ToString() + ")";
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

					if (IsTDRBuilt(C)) ReportLine += " (" + AlgTDR[sp][n].ToString() + ")";
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

        Report += "Total search time: " + std::to_string(GetFullTime()) + " s\n";
        Report += "First blocks search time: " + std::to_string(GetTimeFB()) + " s\n";
        Report += "3QB search time: " + std::to_string(GetTime3QB()) + " s\n";
		Report += "EOLE search time: " + std::to_string(GetTimeEOLE()) + " s\n";

        if (GetTime6CO() > 0.0) Report += "6CO search time: " + std::to_string(GetTime6CO()) + " s\n";
		if (GetTimeDCAL() > 0.0) Report += "DCAL search time: " + std::to_string(GetTimeDCAL()) + " s\n";
		if (GetTimeTDR() > 0.0) Report += "TDR search time: " + std::to_string(GetTimeTDR()) + " s\n";
        if (GetTime6CP() > 0.0) Report += "6CP search time: " + std::to_string(GetTime6CP()) + " s\n";
		if (GetTimeAPDR() > 0.0) Report += "APDR search time: " + std::to_string(GetTimeAPDR()) + " s\n";
		if (GetTimeCDRLL() > 0.0) Report += "CDRLL search time: " + std::to_string(GetTimeCDRLL()) + " s\n";
		if (GetTimeJTLE() > 0.0) Report += "JTLE search time: " + std::to_string(GetTimeJTLE()) + " s\n";
		if (GetTimeL5EP() > 0.0) Report += "L5EP search time: " + std::to_string(GetTimeL5EP()) + " s\n";
		if (GetTimePLL() > 0.0) Report += "PLL search time: " + std::to_string(GetTimePLL()) + " s\n";
		if (GetTimeZBLL() > 0.0) Report += "ZBLL search time: " + std::to_string(GetTimeZBLL()) + " s\n";

        Report += "Threads used: " + std::to_string(GetUsedCores() > 0 ? GetUsedCores() : 0) +
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

        std::string Report  = "Mehta search with orientation " + Cube::GetSpinText(sp);
        Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n";

		Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[si][n]);
        if (!Inspections[si][n].Empty()) Report += "Inspection [" + C.GetSpinText() + "]: " + Inspections[si][n].ToString() + "\n\n";

        C.ApplyAlgorithm(AlgFB[si][n]);
        if (!IsFBBuilt(C))
        {
			Report += "First block not built in " + std::to_string(MaxDepthFB) + " movements\n";
            return Report;
        }
        Report += "First block (" + Algorithm::GetMetricValue(GetMetricFB(sp, n)) + "): " + AlgFB[si][n].ToString() + "\n";

		C.ApplyAlgorithm(Alg3QB[si][n]);
		if (!Is3QBBuilt(C))
		{
			Report += "3QB not built in " + std::to_string(MaxDepth3QB) + " movements\n";
			return Report;
		}
		Report += "3QB (" + Algorithm::GetMetricValue(GetMetric3QB(sp, n)) + "): " + Alg3QB[si][n].ToString() + "\n";

        C.ApplyAlgorithm(AlgEOLE[si][n]);
		if (!IsEOLEBuilt(C))
		{
			Report += "EOLE not built\n";
			return Report;
		}
		Report += "EOLE (" + Algorithm::GetMetricValue(GetMetricEOLE(sp, n)) + "): " + AlgEOLE[si][n].ToString() + "\n";

		if (!Alg6CO[si].empty()) // Mehta-6CP or Mehta-APDR
        {
            C.ApplyAlgorithm(Alg6CO[si][n]);
            Report += "6CO (" + Algorithm::GetMetricValue(GetMetric6CO(sp, n)) + "): " + Alg6CO[si][n].ToString() + "\n";

			if (!Alg6CP[si].empty()) // Mehta-6CP
			{
				C.ApplyAlgorithm(Alg6CP[si][n]);
				Report += "6CP (" + Algorithm::GetMetricValue(GetMetric6CP(sp, n)) + "): " + Alg6CP[si][n].ToString() + "\n";

				C.ApplyAlgorithm(AlgL5EP[si][n]);
				Report += "L5EP (" + Algorithm::GetMetricValue(GetMetricL5EP(sp, n)) + "): " + AlgL5EP[si][n].ToString() + "\n";
			}
			else if (!AlgAPDR[si].empty()) // Mehta-APDR
			{
				C.ApplyAlgorithm(AlgAPDR[si][n]);
				Report += "APDR (" + Algorithm::GetMetricValue(GetMetricAPDR(sp, n)) + "): " + AlgAPDR[si][n].ToString() + "\n";

				C.ApplyAlgorithm(AlgPLL[si][n]);
				Report += "PLL (" + Algorithm::GetMetricValue(GetMetricPLL(sp, n)) + "): " + AlgPLL[si][n].ToString() + "\n";
			}
        }
        else if (!AlgDCAL[si].empty()) // Mehta-CDRLL or Mehta-JTLE
        {
			C.ApplyAlgorithm(AlgDCAL[si][n]);
			Report += "DCAL (" + Algorithm::GetMetricValue(GetMetricDCAL(sp, n)) + "): " + AlgDCAL[si][n].ToString() + "\n";

			if (!AlgCDRLL[si].empty()) // Mehta-CDRLL
			{
				C.ApplyAlgorithm(AlgCDRLL[si][n]);
				Report += "CDRLL (" + Algorithm::GetMetricValue(GetMetricCDRLL(sp, n)) + "): " + AlgCDRLL[si][n].ToString() + "\n";

				C.ApplyAlgorithm(AlgL5EP[si][n]);
				Report += "L5EP (" + Algorithm::GetMetricValue(GetMetricL5EP(sp, n)) + "): " + AlgL5EP[si][n].ToString() + "\n";
			}
			else if (!AlgJTLE[si].empty()) // Mehta-JTLE
			{
				C.ApplyAlgorithm(AlgJTLE[si][n]);
				Report += "JTLE (" + Algorithm::GetMetricValue(GetMetricJTLE(sp, n)) + "): " + AlgJTLE[si][n].ToString() + "\n";

				C.ApplyAlgorithm(AlgPLL[si][n]);
				Report += "PLL (" + Algorithm::GetMetricValue(GetMetricPLL(sp, n)) + "): " + AlgPLL[si][n].ToString() + "\n";
			}
        }
        else if (!AlgTDR[si].empty()) // Mehta-TDR
		{
			C.ApplyAlgorithm(AlgTDR[si][n]);
			Report += "TDR (" + Algorithm::GetMetricValue(GetMetricTDR(sp, n)) + "): " + AlgTDR[si][n].ToString() + "\n";

			C.ApplyAlgorithm(AlgZBLL[si][n]);
			Report += "ZBLL (" + Algorithm::GetMetricValue(GetMetricZBLL(sp, n)) + "): " + AlgZBLL[si][n].ToString() + "\n";
        }

        // Show summary
        Report += "\nSolve metric: " + Algorithm::GetMetricValue(GetMetricSolve(sp, n)) + " " + Algorithm::GetMetricString(Metric) + "\n";
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

    // Check if in the given spin the solve is OK
    bool Mehta::IsSolved(const Spn CurrentSpin, const uint n) const
    {
        const int Si = static_cast<int>(CurrentSpin); // Spin index

		if (!CheckSolveConsistency(CurrentSpin) || n >= Inspections[Si].size()) return 0u;

        Cube C = CubeBase;
        C.ApplyAlgorithm(Inspections[Si][n]);
        C.ApplyAlgorithm(AlgFB[Si][n]);
        C.ApplyAlgorithm(Alg3QB[Si][n]);
        C.ApplyAlgorithm(AlgEOLE[Si][n]);
		if (!Alg6CO[Si].empty())
		{
			C.ApplyAlgorithm(Alg6CO[Si][n]);
			if (!Alg6CP[Si].empty())
			{
				C.ApplyAlgorithm(Alg6CP[Si][n]);
				C.ApplyAlgorithm(AlgL5EP[Si][n]);
			}
			else if (!AlgAPDR[Si].empty())
			{
				C.ApplyAlgorithm(AlgAPDR[Si][n]);
				C.ApplyAlgorithm(AlgPLL[Si][n]);
			}
		}
		else if (!AlgDCAL[Si].empty())
		{
			C.ApplyAlgorithm(AlgDCAL[Si][n]);
			if (!AlgCDRLL[Si].empty())
			{
				C.ApplyAlgorithm(AlgCDRLL[Si][n]);
				C.ApplyAlgorithm(AlgL5EP[Si][n]);
			}
			else if (!AlgJTLE[Si].empty())
			{
				C.ApplyAlgorithm(AlgJTLE[Si][n]);
				C.ApplyAlgorithm(AlgPLL[Si][n]);
			}
		}
		else if (!AlgTDR[Si].empty())
		{
			C.ApplyAlgorithm(AlgTDR[Si][n]);
			C.ApplyAlgorithm(AlgZBLL[Si][n]);
		}

        return C.IsSolved();
    }

	// Get the solve metric
    float Mehta::GetMetricSolve(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || Inspections[si].size() <= n) return 0u;

		Algorithm A = AlgFB[si][n];
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
		
		return A.GetMetric(Metric);
	}

	// Get the full solve with cancellations
	Algorithm Mehta::GetCancellations(const Spn spin, const uint n) const
	{
		const int si = static_cast<int>(spin); // Spin index

		if (!CheckSolveConsistency(spin) || Inspections[si].size() <= n)
		{
			std::string FReport = "No solve with cancellations for spin ";
			FReport += Cube::GetSpinText(spin);
			FReport += ", position " + std::to_string(n) + "\n";
			return FReport;
		}

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

		return A.GetCancellations();
	}

	// Get the best solve report
	std::string Mehta::GetBestReport(const bool Cancellations) const
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
		if (min_M == 0.0f) return "No Mehta solves!\n";

		if (Cancellations)
		{
			Algorithm C = GetCancellations(BestSpin, Bestn);
			return GetReport(BestSpin, Bestn) + "\nCancellations (" + Algorithm::GetMetricValue(C.GetMetric(Metric)) + " " + 
			       Algorithm::GetMetricString(Metric) + "): " + C.ToString() + "\n";
		}

		return GetReport(BestSpin, Bestn);
	}
}
