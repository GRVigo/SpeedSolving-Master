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
	<mailto:grvigo@hotmail.com>
*/

#include <thread>
#include <mutex>

#include "deep_search.h"

namespace grcube3
{
    uint SearchUnit::UnitsAmount = 0u; // Initialize static variable
	
	// Algorithm search class constructor
    DeepSearch::DeepSearch(const Algorithm& scr, const Plc Pol) // Scramble to start the search
	{	
        MaxDepth = MinDepth = 0u;
        SearchPolicy = Pol;
		UsedCores = 0; // Not used cores yet
		RootBranches = 0u;
        SetScramble(scr); // Apply the scramble
	}

	// Add a single piece to mandatory pieces mask
	void DeepSearch::AddToMandatoryPieces(const Pcp P)
	{
		// Separate pieces in edges and corners
        if (Cube::IsEdge(P)) MandatorySolvedMasks.MaskE |= Cube::GetMaskSolvedEdge(Cube::PcpToEdp(P));
        else if (Cube::IsCorner(P)) MandatorySolvedMasks.MaskC |= Cube::GetMaskSolvedCorner(Cube::PcpToCnp(P));
	}

	// Add a group of pieces to mandatory pieces mask
	void DeepSearch::AddToMandatoryPieces(const Pgr MandatoryPG)
	{
		std::vector<Pcp> AuxPositionsList; // Pieces positions list to solve

		// Get the solving mask for mandatory pieces positions
		Cube::AddToPositionsList(MandatoryPG, AuxPositionsList); // Get a list for the pieces positions in this group
        for (const auto P : AuxPositionsList) AddToMandatoryPieces(P);
	}
	
	// Add a piece position as optional solved piece position
	void DeepSearch::AddToOptionalPieces(const Pcp P)
	{
		std::vector<Edp> AuxEdgesList; // Edges list to solve
		std::vector<Cnp> AuxCornersList; // Corners list to solve
		
		if (Cube::IsEdge(P)) AuxEdgesList.push_back(Cube::PcpToEdp(P));
		else if (Cube::IsCorner(P)) AuxCornersList.push_back(Cube::PcpToCnp(P));
		else return;
		
        OptionalSolvedMasks.push_back(MasksPair(Cube::GetMaskSolvedEdges(AuxEdgesList), Cube::GetMaskSolvedCorners(AuxCornersList)));
	}

	// Add a pieces positions group as optional solved pieces positions
	void DeepSearch::AddToOptionalPieces(const Pgr OptionalPG)
	{
		std::vector<Pcp> AuxPiecesList; // Pieces positions list to solve
		std::vector<Edp> AuxEdgesList; // Edges positions list to solve
		std::vector<Cnp> AuxCornersList; // Corners positions list to solve

		// Get the solving masks for optional pieces positions groups
		Cube::AddToPositionsList(OptionalPG, AuxPiecesList); // Each pieces group

        for (const auto P : AuxPiecesList) // Separate pieces in edges and corners
		{
			if (Cube::IsEdge(P)) AuxEdgesList.push_back(Cube::PcpToEdp(P));
			else if (Cube::IsCorner(P)) AuxCornersList.push_back(Cube::PcpToCnp(P));
		}
        OptionalSolvedMasks.push_back(MasksPair(Cube::GetMaskSolvedEdges(AuxEdgesList), Cube::GetMaskSolvedCorners(AuxCornersList)));
	}

	// Add a piece as mandatory solved piece positions for orientation
    void DeepSearch::AddToMandatoryOrientations(const Pcp P) { MandatoryOrientedPos.push_back(P); }

	// Add a pieces group as mandatory orientated pieces positions
	void DeepSearch::AddToMandatoryOrientations(const Pgr PG)
	{
		std::vector<Pcp> AuxPositionsList; // Pieces positions list to solve

		// Get the solving mask for mandatory pieces positions
		Cube::AddToPositionsList(PG, AuxPositionsList); // Get a list for the pieces positions in this group
        for (const auto P : AuxPositionsList) AddToMandatoryOrientations(P);
	}

	// Add a piece as mandatory piece positioned (no orientation needed)
	void DeepSearch::AddToMandatoryPositions(const Pcp P)
	{
		// Separate pieces in edges and corners
        if (Cube::IsEdge(P)) MandatorySolvedMasks.MaskE |= Cube::GetMaskEdgePosition(Cube::PcpToEdp(P));
        else if (Cube::IsCorner(P)) MandatorySolvedMasks.MaskC |= Cube::GetMaskCornerPosition(Cube::PcpToCnp(P));
	}

	// Add a pieces group as mandatory pieces in his position (orientation not needed)
	void DeepSearch::AddToMandatoryPositions(const Pgr PG)
	{
		std::vector<Pcp> AuxPiecesList; // Pieces positions list to solve

		// Get the solving mask for mandatory pieces positions
		Cube::AddToPositionsList(PG, AuxPiecesList); // Get a list for the pieces positions in this group
        for (const auto P : AuxPiecesList) AddToMandatoryPositions(P);
	}

	// Updates root algorithms and sequence data
	// All units in level 0 are developed
	bool DeepSearch::UpdateRootData()
	{
		Roots.clear();
		if (Levels.empty()) return false;

        for (const auto& U : Levels[0].Units) // First level only
		{
			switch (U.Type)
			{
            case SequenceTypes::SINGLE:
                for (const auto S : U.MainSteps)
				{
					Algorithm R;
					R.Append(S);
					Roots.push_back(R);
				}
				break;

            case SequenceTypes::DOUBLE:
                for (const auto S1 : U.MainSteps)
				{
                    for (const auto S2 : U.MainSteps)
					{
						Algorithm R;
						R.Append(S1);
						R.AppendShrink(S2);
						if (R.GetSize() == 2u)
						{
							if (Algorithm::OppositeSteps(R[0], R[1]) && R[0] > R[1]) continue; // "D U" is the same than "U D"
							Roots.push_back(R);
						}
					}
				}
				break;

            case SequenceTypes::TRIPLE:
                for (const auto S1 : U.MainSteps)
				{
                    for (const auto S2 : U.MainSteps)
					{
                        for (const auto S3 : U.MainSteps)
						{
							Algorithm R;
							R.Append(S1);
							R.AppendShrink(S2);
							R.AppendShrink(S3);
							if (R.GetSize() == 3u)
							{
								if ((Algorithm::OppositeSteps(R[0], R[1]) && R[0] > R[1]) ||
									(Algorithm::OppositeSteps(R[1], R[2]) && R[1] > R[2])) continue; // "D U F" is the same than "U D F" 
								Roots.push_back(R);
							}
						}
					}
				}
				break;

            case SequenceTypes::CONJUGATE_SINGLE:
                for (const auto aS : U.AuxSteps)
				{
                    for (const auto S : U.MainSteps)
					{
						Algorithm R;
						R.Append(aS);
						R.Append(S);
						R.Append(Algorithm::InvertedStep(aS));
						Roots.push_back(R);
					}
				}
				break;

            case SequenceTypes::SEQUENCE:
                for (const auto& A : U.MainAlgs) Roots.push_back(A);
				break;
				
            case SequenceTypes::CONJUGATE:
                for (const auto aS : U.AuxSteps)
				{
                    for (const auto& A : U.MainAlgs)
					{
						Algorithm R;
						R.Append(aS);
						R.Append(A);
						R.Append(Algorithm::InvertedStep(aS));
						Roots.push_back(R);
					}
				}
				break;
			
			default: return false;
			}
		}
		RootBranches = static_cast<uint>(Roots.size());
		return RootBranches > 0u;
	}
	
	// Run the search -> -1: use no threads, 0: use all threads avaliable, other: use specified number of threads
	void DeepSearch::Run(const int UseThreads)
	{
		if (RootBranches == 0u) return;
		
		if (UseThreads >= 0) // Multithreading
		{
            UsedCores = (UseThreads == 0) || (UseThreads >= GetSystemCores()) ? GetSystemCores() : UseThreads;

			std::vector<std::thread> Pool; // Threads pool

			for (uint nc = 0; nc < UsedCores; nc++) Pool.push_back(std::thread(&DeepSearch::RunThread, this, nc)); // Add a thread per core
            for (auto& t : Pool) t.join(); // Wait for all threads
		}
        else // Without multithreading (for debugging, slower)
		{
            for (const auto& R : Roots) RunSearch(R, 0u, 0u, 0u);
			UsedCores = -1;
		}
	}
	
    // Run search a thread
    void DeepSearch::RunThread(const uint NCore) { for (uint n = NCore; n < RootBranches; n += UsedCores) RunSearch(Roots[n], 0u, 0u, 0u); }
    
	// Recursive search code
    void DeepSearch::RunSearch(const Algorithm& Alg, // Current search algorithm
							   uint Depth, // Current search depth
                               const uint SeqId, // Current sequence identifier (0 means root sequence or not in a sequence)
                               uint SeqSize) // Current sequence size
	{
		if (SeqId == 0u) // Root or not in a sequence
		{
			// Check if current algorithm solves the pieces
			if (Levels[Depth].Check == SearchCheck::CHECK) CheckSolve(Alg);
			
			if (IncCheckDepth(Depth)) return; // It's no necessary to continue the search

			NextLevel(Alg, Depth); // Prepare the branches for the next level of the search
		}
			
        else for (const auto& U : Levels[Depth].Units)
		{
            if (U.Id == SeqId)
			{
				switch (U.Type)
				{
                case SequenceTypes::CONJUGATE_SINGLE:

					if (SeqSize >= 3u) // Check if sequence is completed
					{
						if (Levels[Depth].Check == SearchCheck::CHECK) CheckSolve(Alg); // Check if current algorithm solves the pieces
						if (IncCheckDepth(Depth)) return; // It's no necessary to continue the search
						NextLevel(Alg, Depth); // Go to next level
					}
					else // Sequence is not completed
					{
						if (++SeqSize == 2u)
						{
                            for (const auto S : U.MainSteps)
							{
								Algorithm Alg2 = Alg;
                                if (!Alg2.AppendShrink(S)) RunSearch(Alg2, Depth, SeqId, SeqSize); // Recursive
							}
						}
						else // Sequence size == 3u -> Add the last step
						{
							Algorithm Alg2 = Alg;
							Alg2.Append(Alg.PenultimateInverted());
                            RunSearch(Alg2, Depth, SeqId, SeqSize); // Recursive
						}
					}
					break;
					
                case SequenceTypes::CONJUGATE:

					if (SeqSize >= 3u) // Check if sequence is completed
					{
						if (Levels[Depth].Check == SearchCheck::CHECK) CheckSolve(Alg); // Check if current algorithm solves the pieces
						if (IncCheckDepth(Depth)) return; // It's no necessary to continue the search
						NextLevel(Alg, Depth); // Go to next level
					}
					else // Sequence is not completed
					{
						if (++SeqSize == 2u)
						{
                            for (const auto& MA : U.MainAlgs)
							{
								Algorithm Alg2 = Alg;
                                Alg2.Append(MA);
                                RunSearch(Alg2, Depth, SeqId, SeqSize); // Recursive
							}
						}
						else // Sequence size == 3u -> Add the last step
						{
							Algorithm Alg2 = Alg;
							Alg2.Append(Alg.PenultimateInverted());
                            RunSearch(Alg2, Depth, SeqId, SeqSize); // Recursive
						}
					}
					break;
				
				default: // Not into a sequence
					
					// Check if current algorithm solves the pieces
					if (Levels[Depth].Check == SearchCheck::CHECK) CheckSolve(Alg);
				
					if (IncCheckDepth(Depth)) return; // It's no necessary to continue the search

					NextLevel(Alg, Depth); // Prepare the branches for the next level of the search
					break;
				}
			}
		}
    }
	
	// Check if an algorithm solves the pieces
    inline void DeepSearch::CheckSolve(const Algorithm& A)
	{
		Cube CubeTest = CubeBase;
		CubeTest.ApplyAlgorithm(A);

        if (CubeTest.IsSolved(MandatorySolvedMasks.MaskE, MandatorySolvedMasks.MaskC) && // True if no mandatory pieces
			CubeTest.CheckOrientation(MandatoryOrientedPos))
		{
            if (OptionalSolvedMasks.empty()) // Solve algorithm found (only mandatory)
			{
				std::lock_guard<std::mutex> guard(SearchMutex); // Thread safe code
                if (SearchPolicy == Plc::SHORT && A.GetSize() < MaxDepth) MaxDepth = A.GetSize() + 1u;
				Solves.push_back(A); 
			}
			else // Check solve algorithm found (mandatory + at least an optional)
			{
                for (const auto& O : OptionalSolvedMasks)
				{
					if (CubeTest.IsSolved(O.MaskE, O.MaskC))
					{ // Solve algorithm found
						std::lock_guard<std::mutex> guard(SearchMutex); // Thread safe code
                        if (SearchPolicy == Plc::SHORT && A.GetSize() < MaxDepth) MaxDepth = A.GetSize() + 1u;
						Solves.push_back(A);  
                        break;
					}
				}
			}
		}
	}
	
	// Prepare the branches for the next level of the search
    inline void DeepSearch::NextLevel(const Algorithm& A, const uint D)
	{
		if (A.GetSize() == 0u || D >= MaxDepth) return;
		
		Stp LastStep = A.Last();
		for (const auto& U : Levels[D].Units)
		{
			switch (U.Type)
			{
            case SequenceTypes::SINGLE:
				for (const auto S : U.MainSteps)
				{
					if (Algorithm::OppositeSteps(LastStep, S) && LastStep > S) continue; // As in an algorithm "... U D ..." branch is the same than "... D U ..." branch, compute only one.
					Algorithm Alg2 = A; // Continue the search only on a longer algorithm (shorter or equal lenght means that will be calculated upper in the tree)
					if (!Alg2.AppendShrink(S)) RunSearch(Alg2, D, U.Id); // Recursive
				}
				break;
            case SequenceTypes::SEQUENCE:
                for (const auto& MA : U.MainAlgs)
				{
					Algorithm Alg2 = A;
					Alg2.Append(MA);
					RunSearch(Alg2, D); // Recursive
				}
				break;
            case SequenceTypes::CONJUGATE_SINGLE:
            case SequenceTypes::CONJUGATE:
				for (const auto S : U.AuxSteps)
				{
					if (Algorithm::OppositeSteps(LastStep, S) && LastStep > S) continue; // As in an algorithm "... U D ..." branch is the same than "... D U ..." branch, compute only one.
					Algorithm Alg2 = A; // Continue the search only on a longer algorithm (shorter or equal lenght means that will be calculated upper in the tree)
					if (!Alg2.AppendShrink(S)) RunSearch(Alg2, D, U.Id, 1u); // Recursive
				}
				break;

			// TODO: Add more sequence types
			default:
				break;
			}
		}
	}

    // Returns the shorter algorithm from the solves
    uint DeepSearch::EvaluateShortestResult(Algorithm& ShortSolve, const bool EvaluateMovs)
    {
        if (Solves.size() <= 0) { ShortSolve.Clear(); return 0u; } // No solves
        uint Score, MaxScore = 0u, MaxScoreIndex = 0u, Index = 0u;
        for (const auto& s : Solves) // Check each solve to get the best one
        {
            Score = (1000u - s.GetSize()) * 100u;
			if (EvaluateMovs) Score -= s.GetSubjectiveScore();

            if (Score > MaxScore)
            {
                MaxScore = Score;
                MaxScoreIndex = Index;
            }
            Index++;
        }
        if (MaxScore > 0u) ShortSolve = Solves[MaxScoreIndex];
        else ShortSolve.Clear();

        return MaxScore;
    }

	// Search the best solve algorithms with the given search depth
	void DeepSearch::SearchBase(const uint depth, const Plc pol, const int cores)
	{
		ResetPieces();
		ResetSearchLevels();
		Solves.clear();

		// Add most common initial structures
		AddToOptionalPieces(Pgr::CROSS_U);
		AddToOptionalPieces(Pgr::CROSS_D);
		AddToOptionalPieces(Pgr::CROSS_F);
		AddToOptionalPieces(Pgr::CROSS_B);
		AddToOptionalPieces(Pgr::CROSS_R);
		AddToOptionalPieces(Pgr::CROSS_L);

		AddToOptionalPieces(Pgr::PETRUS_UFR);
		AddToOptionalPieces(Pgr::PETRUS_UFL);
		AddToOptionalPieces(Pgr::PETRUS_UBR);
		AddToOptionalPieces(Pgr::PETRUS_UBL);

		AddToOptionalPieces(Pgr::PETRUS_DFR);
		AddToOptionalPieces(Pgr::PETRUS_DFL);
		AddToOptionalPieces(Pgr::PETRUS_DBR);
		AddToOptionalPieces(Pgr::PETRUS_DBL);

		if (pol == Plc::SHORT)
		{
			AddToOptionalPieces(Pgr::UF_B1);
			AddToOptionalPieces(Pgr::UB_B1);
			AddToOptionalPieces(Pgr::UR_B1);
			AddToOptionalPieces(Pgr::UL_B1);

			AddToOptionalPieces(Pgr::DF_B1);
			AddToOptionalPieces(Pgr::DB_B1);
			AddToOptionalPieces(Pgr::DR_B1);
			AddToOptionalPieces(Pgr::DL_B1);

			AddToOptionalPieces(Pgr::FU_B1);
			AddToOptionalPieces(Pgr::FD_B1);
			AddToOptionalPieces(Pgr::FR_B1);
			AddToOptionalPieces(Pgr::FL_B1);

			AddToOptionalPieces(Pgr::BU_B1);
			AddToOptionalPieces(Pgr::BD_B1);
			AddToOptionalPieces(Pgr::BR_B1);
			AddToOptionalPieces(Pgr::BL_B1);

			AddToOptionalPieces(Pgr::RU_B1);
			AddToOptionalPieces(Pgr::RD_B1);
			AddToOptionalPieces(Pgr::RF_B1);
			AddToOptionalPieces(Pgr::RB_B1);

			AddToOptionalPieces(Pgr::LU_B1);
			AddToOptionalPieces(Pgr::LD_B1);
			AddToOptionalPieces(Pgr::LF_B1);
			AddToOptionalPieces(Pgr::LB_B1);
		}

		else // Plc::BEST => Exhaustive search
		{
			AddToOptionalPieces(Pgr::UF_B1S2);
			AddToOptionalPieces(Pgr::UB_B1S2);
			AddToOptionalPieces(Pgr::UR_B1S2);
			AddToOptionalPieces(Pgr::UL_B1S2);

			AddToOptionalPieces(Pgr::DF_B1S2);
			AddToOptionalPieces(Pgr::DB_B1S2);
			AddToOptionalPieces(Pgr::DR_B1S2);
			AddToOptionalPieces(Pgr::DL_B1S2);

			AddToOptionalPieces(Pgr::FU_B1S2);
			AddToOptionalPieces(Pgr::FD_B1S2);
			AddToOptionalPieces(Pgr::FR_B1S2);
			AddToOptionalPieces(Pgr::FL_B1S2);

			AddToOptionalPieces(Pgr::BU_B1S2);
			AddToOptionalPieces(Pgr::BD_B1S2);
			AddToOptionalPieces(Pgr::BR_B1S2);
			AddToOptionalPieces(Pgr::BL_B1S2);

			AddToOptionalPieces(Pgr::RU_B1S2);
			AddToOptionalPieces(Pgr::RD_B1S2);
			AddToOptionalPieces(Pgr::RF_B1S2);
			AddToOptionalPieces(Pgr::RB_B1S2);

			AddToOptionalPieces(Pgr::LU_B1S2);
			AddToOptionalPieces(Pgr::LD_B1S2);
			AddToOptionalPieces(Pgr::LF_B1S2);
			AddToOptionalPieces(Pgr::LB_B1S2);

			AddToOptionalPieces(Pgr::CPLINE_UF);
			AddToOptionalPieces(Pgr::CPLINE_UB);
			AddToOptionalPieces(Pgr::CPLINE_UR);
			AddToOptionalPieces(Pgr::CPLINE_UL);

			AddToOptionalPieces(Pgr::CPLINE_DF);
			AddToOptionalPieces(Pgr::CPLINE_DB);
			AddToOptionalPieces(Pgr::CPLINE_DR);
			AddToOptionalPieces(Pgr::CPLINE_DL);

			AddToOptionalPieces(Pgr::CPLINE_FU);
			AddToOptionalPieces(Pgr::CPLINE_FD);
			AddToOptionalPieces(Pgr::CPLINE_RD);
			AddToOptionalPieces(Pgr::CPLINE_LU);
		}

        const SearchUnit URoot(SequenceTypes::DOUBLE);
        const SearchUnit U(SequenceTypes::SINGLE);

		SearchLevel L_Root(SearchCheck::NO_CHECK);
		L_Root.Add(URoot);

		SearchLevel L_Check(SearchCheck::CHECK);
		L_Check.Add(U);

		AddSearchLevel(L_Root); // Level 1 (two steps -DOUBLE- root algorithms)
		for (uint l = 2u; l < depth; l++) AddSearchLevel(L_Check); // Levels 2 to given depth

		UpdateRootData();

		Run(cores);
	}
}
