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

#include "lbl.h"
#include "method.h"

#include <algorithm>

namespace grcube3
{
	// Default inspection algorithms (first layer down)
	const Algorithm LBL::Inspections[6] =
	{
		Algorithm("z2"), // U
		Algorithm(""),   // D
		Algorithm("x'"), // F
		Algorithm("x"),  // B
		Algorithm("z"),  // R
		Algorithm("z'")  // L
	};
	
	const Algorithm LBL::Cross1Algorithm = Algorithm("(F R U R' U' F')");
	const Algorithm LBL::Cross2Algorithm = Algorithm("(R' U' R U' R' U2 R U')");
	const Algorithm LBL::Corners1Algorithm = Algorithm("(U R U' L' U R' U' L)");
	const Algorithm LBL::Corners2Algorithm = Algorithm("(R' D' R D)");
	const Algorithm LBL::Corners2Algorithm2 = Algorithm("(R' D' R D) (R' D' R D)"); // This algorithm is always used an even number of times

	// Reset the search results
	void LBL::Reset()
	{
        CubeBase = Cube(Scramble);
		
		FLCross.clear();
		FLCorners.clear();
		SLEdges.clear();
		LLCross1.Clear();
		LLCross2.Clear();
		LLCorners1.Clear();
		LLCorners2.Clear();
		
		FirstLayer = Lyr::U;
		FLFace = Cube::LayerToFace(FirstLayer);
		FLi = static_cast<int>(FLFace); // First layer/face index
		
		TimeFLCross = TimeFLCorners = TimeSL = TimeLLCross1 = TimeLLCross2 = TimeLLCorners1 = TimeLLCorners2 = 0.0f;
		
		Metric = Metrics::Movements; // Default metric
	}
	
	// Search the best first layer cross solve algorithm
	// Return false if no cross found
    bool LBL::SearchFLCross(const Lyr FstLayer)
	{
		if (!Cube::IsExternalLayer(FstLayer)) return false;
			
		const auto time_cross_start = std::chrono::system_clock::now();
		
        FirstLayer = FstLayer;
        FLFace = Cube::LayerToFace(FirstLayer);
		FLi = static_cast<int>(FLFace); // First layer/face index
		
		bool Cross_Found = false; // Cross found flag

        Algorithm Cross_Aux, // Auxiliary algorithm for cross search
                  A1_Cross, A2_Cross, A3_Cross, A4_Cross; // Algorithms for cross
				  
        const SearchUnit U1(SequenceTypes::SINGLE, Sst::SINGLE_ALL);

        SearchLevel L(SearchCheck::CHECK);
        L.Add(U1);

        FLCross.clear();

		Pcp Cross_1, Cross_2, Cross_3, Cross_4; // Edges to complete the cross
		
		switch (FirstLayer)
        {
        case Lyr::U:
            Cross_1 = Pcp::UF;
            Cross_2 = Pcp::UB;
            Cross_3 = Pcp::UR;
            Cross_4 = Pcp::UL;
            break;
        case Lyr::D:
            Cross_1 = Pcp::DF;
            Cross_2 = Pcp::DB;
            Cross_3 = Pcp::DR;
            Cross_4 = Pcp::DL;
            break;
        case Lyr::F:
            Cross_1 = Pcp::UF;
            Cross_2 = Pcp::DF;
            Cross_3 = Pcp::FR;
            Cross_4 = Pcp::FL;
            break;
        case Lyr::B:
            Cross_1 = Pcp::UB;
            Cross_2 = Pcp::DB;
            Cross_3 = Pcp::BR;
            Cross_4 = Pcp::BL;
            break;
        case Lyr::R:
            Cross_1 = Pcp::UR;
            Cross_2 = Pcp::DR;
            Cross_3 = Pcp::FR;
            Cross_4 = Pcp::BR;
            break;
        case Lyr::L:
            Cross_1 = Pcp::UL;
            Cross_2 = Pcp::DL;
            Cross_3 = Pcp::FL;
            Cross_4 = Pcp::BL;
            break;
        default: return false;
        }

		do
		{
			Cross_Aux = Scramble + Inspections[FLi];
			for (const auto& Cross_Alg : FLCross) Cross_Aux += Cross_Alg;

			Cube C(Cross_Aux);

			const bool Cross_1_IsSolved = C.IsSolved(Cross_1),
					   Cross_2_IsSolved = C.IsSolved(Cross_2),
					   Cross_3_IsSolved = C.IsSolved(Cross_3),
					   Cross_4_IsSolved = C.IsSolved(Cross_4);

			if (Cross_1_IsSolved && Cross_2_IsSolved && Cross_3_IsSolved && Cross_4_IsSolved) break;

			DeepSearch DS_Cross(Cross_Aux);

            for (int n = 0; n < 5; n++) DS_Cross.AddSearchLevel(L); // Search depth is 5
			DS_Cross.UpdateRootData();

			if (Cross_1_IsSolved) DS_Cross.AddToMandatoryPieces(Cross_1);
			else DS_Cross.AddToOptionalPieces(Cross_1);

			if (Cross_2_IsSolved) DS_Cross.AddToMandatoryPieces(Cross_2);
			else DS_Cross.AddToOptionalPieces(Cross_2);

			if (Cross_3_IsSolved) DS_Cross.AddToMandatoryPieces(Cross_3);
			else DS_Cross.AddToOptionalPieces(Cross_3);

			if (Cross_4_IsSolved) DS_Cross.AddToMandatoryPieces(Cross_4);
			else DS_Cross.AddToOptionalPieces(Cross_4);

			DS_Cross.Run(Cores);

			DS_Cross.EvaluateShortestResult(Cross_Aux, true);

			C.ApplyAlgorithm(Cross_Aux);

			Cross_Found = false;

			if (!Cross_1_IsSolved && C.IsSolved(Cross_1))
			{
				A1_Cross = Cross_Aux;
				FLCross.push_back(A1_Cross);
				Cross_Found = true;
			}
            else if (!Cross_2_IsSolved && C.IsSolved(Cross_2))
			{
				A2_Cross = Cross_Aux;
				FLCross.push_back(A2_Cross);
				Cross_Found = true;
			}
            else if (!Cross_3_IsSolved && C.IsSolved(Cross_3))
			{
				A3_Cross = Cross_Aux;
				FLCross.push_back(A3_Cross);
				Cross_Found = true;
			}
            else if (!Cross_4_IsSolved && C.IsSolved(Cross_4))
			{
				A4_Cross = Cross_Aux;
				FLCross.push_back(A4_Cross);
				Cross_Found = true;
			}
			
            Cores = DS_Cross.GetUsedCores();

        } while (Cross_Found);

		const std::chrono::duration<double> cross_elapsed_seconds = std::chrono::system_clock::now() - time_cross_start;
        TimeFLCross = cross_elapsed_seconds.count();

        return IsFLCrossSolved();
	}

	// Search the first layer (four corners)
	bool LBL::SearchFLCorners()
	{
		const auto time_fl_start = std::chrono::system_clock::now();
		
		bool FL_Found = false; // First layer found flag

        Algorithm FL_Aux, // Auxiliary algorithm for FL search
                  A1_FL, A2_FL, A3_FL, A4_FL; // Algorithms for FL
				  
        const SearchUnit U_Up(SequenceTypes::SINGLE, Sst::SINGLE_U);
        const SearchUnit U_Lat(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::LATERAL_UD);

        SearchLevel L_FL_Check(SearchCheck::CHECK);
        L_FL_Check.Add(U_Up);
        L_FL_Check.Add(U_Lat);
		
        SearchLevel L_FL_End(SearchCheck::CHECK);
        L_FL_End.Add(U_Lat);

        FLCorners.clear();
		
		Pgr CROSS;
		Pcp FL_1, FL_2, FL_3, FL_4; // Corners to complete the first layer
		
		switch (FirstLayer)
        {
        case Lyr::U:
            CROSS = Pgr::CROSS_U;
            FL_1 = Pcp::UFR;
            FL_2 = Pcp::UFL;
            FL_3 = Pcp::UBR;
            FL_4 = Pcp::UBL;
            break;
        case Lyr::D:
            CROSS = Pgr::CROSS_D;
            FL_1 = Pcp::DFR;
            FL_2 = Pcp::DFL;
            FL_3 = Pcp::DBR;
            FL_4 = Pcp::DBL;
            break;
        case Lyr::F:
            CROSS = Pgr::CROSS_F;
            FL_1 = Pcp::UFR;
            FL_2 = Pcp::UFL;
            FL_3 = Pcp::DFR;
            FL_4 = Pcp::DFL;
            break;
        case Lyr::B:
            CROSS = Pgr::CROSS_B;
            FL_1 = Pcp::UBR;
            FL_2 = Pcp::UBL;
            FL_3 = Pcp::DBR;
            FL_4 = Pcp::DBL;
            break;
        case Lyr::R:
            CROSS = Pgr::CROSS_R;
            FL_1 = Pcp::UFR;
            FL_2 = Pcp::UBR;
            FL_3 = Pcp::DFR;
            FL_4 = Pcp::DBR;
            break;
        case Lyr::L:
            CROSS = Pgr::CROSS_L;
            FL_1 = Pcp::UFL;
            FL_2 = Pcp::UBL;
            FL_3 = Pcp::DFL;
            FL_4 = Pcp::DBL;
            break;
        default: return false;
        }

		do
		{
			FL_Aux = Scramble + Inspections[FLi];
			for (const auto& Cross_Alg : FLCross) FL_Aux += Cross_Alg;
			for (const auto& FL_Alg : FLCorners) FL_Aux += FL_Alg;

			Cube CubeFL(FL_Aux);

			const bool FL_1_IsSolved = CubeFL.IsSolved(FL_1),
					   FL_2_IsSolved = CubeFL.IsSolved(FL_2),
					   FL_3_IsSolved = CubeFL.IsSolved(FL_3),
					   FL_4_IsSolved = CubeFL.IsSolved(FL_4);

			if (FL_1_IsSolved && FL_2_IsSolved && FL_3_IsSolved && FL_4_IsSolved) break;

			DeepSearch DS_FL(FL_Aux);

			DS_FL.AddSearchLevel(L_FL_Check);
			DS_FL.AddSearchLevel(L_FL_Check);
			DS_FL.AddSearchLevel(L_FL_Check);
			DS_FL.AddSearchLevel(L_FL_Check);
			DS_FL.AddSearchLevel(L_FL_End);
			DS_FL.UpdateRootData();

			DS_FL.AddToMandatoryPieces(CROSS);

			if (FL_1_IsSolved) DS_FL.AddToMandatoryPieces(FL_1);
			else DS_FL.AddToOptionalPieces(FL_1);

			if (FL_2_IsSolved) DS_FL.AddToMandatoryPieces(FL_2);
			else DS_FL.AddToOptionalPieces(FL_2);

			if (FL_3_IsSolved) DS_FL.AddToMandatoryPieces(FL_3);
			else DS_FL.AddToOptionalPieces(FL_3);

			if (FL_4_IsSolved) DS_FL.AddToMandatoryPieces(FL_4);
			else DS_FL.AddToOptionalPieces(FL_4);

			DS_FL.Run(Cores);

			DS_FL.EvaluateShortestResult(FL_Aux, true);

			CubeFL.ApplyAlgorithm(FL_Aux);

			FL_Found = false;

			if (!FL_1_IsSolved && CubeFL.IsSolved(FL_1))
			{
				A1_FL = FL_Aux;
				FLCorners.push_back(A1_FL);
				FL_Found = true;
			}
            else if (!FL_2_IsSolved && CubeFL.IsSolved(FL_2))
			{
				A2_FL = FL_Aux;
				FLCorners.push_back(A2_FL);
				FL_Found = true;
			}
            else if (!FL_3_IsSolved && CubeFL.IsSolved(FL_3))
			{
				A3_FL = FL_Aux;
				FLCorners.push_back(A3_FL);
				FL_Found = true;
			}
            else if (!FL_4_IsSolved && CubeFL.IsSolved(FL_4))
			{
				A4_FL = FL_Aux;
				FLCorners.push_back(A4_FL);
				FL_Found = true;
			}

		} while (FL_Found);

		const std::chrono::duration<double> fl_elapsed_seconds = std::chrono::system_clock::now() - time_fl_start;
        TimeFLCorners = fl_elapsed_seconds.count();

        return IsFLSolved();
	}

    // Search the best second layer algorithms
    bool LBL::SearchSLEdges()
    {
        const auto time_sl_start = std::chrono::system_clock::now();

        bool SL_Found = false; // SL found flag

        Algorithm SL_Aux, // Auxiliary algorithm for second layer search
                  A1_SL, A2_SL, A3_SL, A4_SL; // Algorithms for second layer

        const SearchUnit U_Up(SequenceTypes::SINGLE, Sst::SINGLE_U);
        const SearchUnit U_Lat(SequenceTypes::CONJUGATE_SINGLE, Sst::SINGLE_U, Sst::LATERAL_UD);

        SearchLevel L_SL_Check(SearchCheck::CHECK);
        L_SL_Check.Add(U_Up);
        L_SL_Check.Add(U_Lat);
		
        SearchLevel L_SL_End(SearchCheck::CHECK);
        L_SL_End.Add(U_Lat);

        SLEdges.clear();

        Pgr SL_1, SL_2, SL_3, SL_4, CROSS;
		
        switch (FirstLayer)
        {
        case Lyr::U:
            CROSS = Pgr::CROSS_U;
            SL_1 = Pgr::F2L_U_UFR;
            SL_2 = Pgr::F2L_U_UFL;
            SL_3 = Pgr::F2L_U_UBR;
            SL_4 = Pgr::F2L_U_UBL;
            break;
        case Lyr::D:
            CROSS = Pgr::CROSS_D;
            SL_1 = Pgr::F2L_D_DFR;
            SL_2 = Pgr::F2L_D_DFL;
            SL_3 = Pgr::F2L_D_DBR;
            SL_4 = Pgr::F2L_D_DBL;
            break;
        case Lyr::F:
            CROSS = Pgr::CROSS_F;
            SL_1 = Pgr::F2L_F_UFR;
            SL_2 = Pgr::F2L_F_UFL;
            SL_3 = Pgr::F2L_F_DFR;
            SL_4 = Pgr::F2L_F_DFL;
            break;
        case Lyr::B:
            CROSS = Pgr::CROSS_B;
            SL_1 = Pgr::F2L_B_UBR;
            SL_2 = Pgr::F2L_B_UBL;
            SL_3 = Pgr::F2L_B_DBR;
            SL_4 = Pgr::F2L_B_DBL;
            break;
        case Lyr::R:
            CROSS = Pgr::CROSS_R;
            SL_1 = Pgr::F2L_R_UFR;
            SL_2 = Pgr::F2L_R_UBR;
            SL_3 = Pgr::F2L_R_DFR;
            SL_4 = Pgr::F2L_R_DBR;
            break;
        case Lyr::L:
            CROSS = Pgr::CROSS_L;
            SL_1 = Pgr::F2L_L_UFL;
            SL_2 = Pgr::F2L_L_UBL;
            SL_3 = Pgr::F2L_L_DFL;
            SL_4 = Pgr::F2L_L_DBL;
            break;
        default: return false;
        }

		do
		{
			SL_Aux = Scramble + Inspections[FLi];
			for (const auto& Cross_Alg : FLCross) SL_Aux += Cross_Alg;
			for (const auto& FL_Alg : FLCorners) SL_Aux += FL_Alg;
			for (const auto& SL_Alg : SLEdges) SL_Aux += SL_Alg;

			Cube CubeSL(SL_Aux);

			const bool SL_1_IsSolved = CubeSL.IsSolved(SL_1),
					   SL_2_IsSolved = CubeSL.IsSolved(SL_2),
					   SL_3_IsSolved = CubeSL.IsSolved(SL_3),
					   SL_4_IsSolved = CubeSL.IsSolved(SL_4);

			if (SL_1_IsSolved && SL_2_IsSolved && SL_3_IsSolved && SL_4_IsSolved) break;

			DeepSearch DS_SL(SL_Aux);

			DS_SL.AddSearchLevel(L_SL_Check);
			DS_SL.AddSearchLevel(L_SL_Check);
			DS_SL.AddSearchLevel(L_SL_Check);
			DS_SL.AddSearchLevel(L_SL_Check);
			DS_SL.AddSearchLevel(L_SL_End);
			DS_SL.UpdateRootData();

			DS_SL.AddToMandatoryPieces(CROSS);

			if (SL_1_IsSolved) DS_SL.AddToMandatoryPieces(SL_1);
			else DS_SL.AddToOptionalPieces(SL_1);

			if (SL_2_IsSolved) DS_SL.AddToMandatoryPieces(SL_2);
			else DS_SL.AddToOptionalPieces(SL_2);

			if (SL_3_IsSolved) DS_SL.AddToMandatoryPieces(SL_3);
			else DS_SL.AddToOptionalPieces(SL_3);

			if (SL_4_IsSolved) DS_SL.AddToMandatoryPieces(SL_4);
			else DS_SL.AddToOptionalPieces(SL_4);

			DS_SL.Run(Cores);

			std::vector<Algorithm> SLSolves;
            Method::EvaluateCFOPF2LResult(SLSolves, 1u, DS_SL.GetSolves(), CubeSL, FirstLayer, Plc::SHORT);

			if (SLSolves.empty()) return false;

			CubeSL.ApplyAlgorithm(SLSolves[0]);

			SL_Found = false;

			if (!SL_1_IsSolved && CubeSL.IsSolved(SL_1))
			{
				A1_SL = SLSolves[0];
				SLEdges.push_back(A1_SL);
				SL_Found = true;
			}
            else if (!SL_2_IsSolved && CubeSL.IsSolved(SL_2))
			{
				A2_SL = SLSolves[0];
				SLEdges.push_back(A2_SL);
				SL_Found = true;
			}
            else if (!SL_3_IsSolved && CubeSL.IsSolved(SL_3))
			{
				A3_SL = SLSolves[0];
				SLEdges.push_back(A3_SL);
				SL_Found = true;
			}
            else if (!SL_4_IsSolved && CubeSL.IsSolved(SL_4))
			{
				A4_SL = SLSolves[0];
				SLEdges.push_back(A4_SL);
				SL_Found = true;
			}
		} while (SL_Found);

        const std::chrono::duration<double> sl_elapsed_seconds = std::chrono::system_clock::now() - time_sl_start;
        TimeSL = sl_elapsed_seconds.count();

        return IsSLSolved();
    }

	// Search the last layer cross (only orientation)
	void LBL::SearchLLCross1()
	{
		const auto time_llcross1_start = std::chrono::system_clock::now();
		
		Algorithm AlgStart = Scramble + Inspections[FLi];
		for (const auto& Cross_Alg : FLCross) AlgStart += Cross_Alg;
		for (const auto& FL_Alg : FLCorners) AlgStart += FL_Alg;
		for (const auto& SL_Alg : SLEdges) AlgStart += SL_Alg;
		
		const Cube C(AlgStart);

        DeepSearch DSLLCross1(AlgStart, Plc::SHORT);
		
		switch (FirstLayer)
		{
		case Lyr::U:
			if (C.CheckOrientation(Pgr::CROSS_D)) return;
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_U);
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_E);
			DSLLCross1.AddToMandatoryOrientations(Pgr::CROSS_D);
			break;
		case Lyr::D:
			if (C.CheckOrientation(Pgr::CROSS_U)) return;
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_D);
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_E);
			DSLLCross1.AddToMandatoryOrientations(Pgr::CROSS_U);
			break;
		case Lyr::F:
			if (C.CheckOrientation(Pgr::CROSS_B)) return;
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_F);
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_S);
			DSLLCross1.AddToMandatoryOrientations(Pgr::CROSS_B);
			break;
		case Lyr::B:
			if (C.CheckOrientation(Pgr::CROSS_F)) return;
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_B);
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_S);
			DSLLCross1.AddToMandatoryOrientations(Pgr::CROSS_F);
			break;
		case Lyr::R:
			if (C.CheckOrientation(Pgr::CROSS_L)) return;
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_R);
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_M);
			DSLLCross1.AddToMandatoryOrientations(Pgr::CROSS_L);
			break;
		case Lyr::L:
			if (C.CheckOrientation(Pgr::CROSS_R)) return;
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_L);
			DSLLCross1.AddToMandatoryPieces(Pgr::LAYER_M);
			DSLLCross1.AddToMandatoryOrientations(Pgr::CROSS_R);
			break;
		default: return;
		}
		
        const SearchUnit U1(SequenceTypes::SINGLE, Sst::SINGLE_U);
        SearchUnit U2(SequenceTypes::SEQUENCE, Sst::NONE);
        U2.AddAlgorithm(Cross1Algorithm);
		
        SearchLevel L(SearchCheck::CHECK);
        L.Add(U1);
		L.Add(U2);
        
        for (int l = 0; l < 5; l++) DSLLCross1.AddSearchLevel(L);

        DSLLCross1.UpdateRootData();

        DSLLCross1.Run(Cores);
		
		DSLLCross1.EvaluateShortestResult(LLCross1, true);
		
		const std::chrono::duration<double> llcross1_elapsed_seconds = std::chrono::system_clock::now() - time_llcross1_start;
        TimeLLCross1 = llcross1_elapsed_seconds.count();
	}
		
	// Search the last layer (permutation)
	void LBL::SearchLLCross2()
	{
		const auto time_llcross2_start = std::chrono::system_clock::now();
		
		Algorithm AlgStart = Scramble + Inspections[FLi];
		for (const auto& Cross_Alg : FLCross) AlgStart += Cross_Alg;
		for (const auto& FL_Alg : FLCorners) AlgStart += FL_Alg;
		for (const auto& SL_Alg : SLEdges) AlgStart += SL_Alg;
		AlgStart += LLCross1;
		
		const Cube C(AlgStart);

        DeepSearch DSLLCross2(AlgStart, Plc::SHORT);
		
		switch (FirstLayer)
		{
		case Lyr::U:
			if (C.IsSolved(Pgr::CROSS_D)) return;
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_U);
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_E);
			DSLLCross2.AddToMandatoryPieces(Pgr::CROSS_D);
			break;
		case Lyr::D:
			if (C.IsSolved(Pgr::CROSS_U)) return;
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_D);
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_E);
			DSLLCross2.AddToMandatoryPieces(Pgr::CROSS_U);
			break;
		case Lyr::F:
			if (C.IsSolved(Pgr::CROSS_B)) return;
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_F);
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_S);
			DSLLCross2.AddToMandatoryPieces(Pgr::CROSS_B);
			break;
		case Lyr::B:
			if (C.IsSolved(Pgr::CROSS_F)) return;
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_B);
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_S);
			DSLLCross2.AddToMandatoryPieces(Pgr::CROSS_F);
			break;
		case Lyr::R:
			if (C.IsSolved(Pgr::CROSS_L)) return;
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_R);
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_M);
			DSLLCross2.AddToMandatoryPieces(Pgr::CROSS_L);
			break;
		case Lyr::L:
			if (C.IsSolved(Pgr::CROSS_R)) return;
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_L);
			DSLLCross2.AddToMandatoryPieces(Pgr::LAYER_M);
			DSLLCross2.AddToMandatoryPieces(Pgr::CROSS_R);
			break;
		default: return;
		}
		
        const SearchUnit U1(SequenceTypes::SINGLE, Sst::SINGLE_U);
        SearchUnit U2(SequenceTypes::SEQUENCE, Sst::NONE);
        U2.AddAlgorithm(Cross2Algorithm);
		
        SearchLevel L(SearchCheck::CHECK);
        L.Add(U1);
		L.Add(U2);
        
        for (int l = 0; l < 5; l++) DSLLCross2.AddSearchLevel(L);

        DSLLCross2.UpdateRootData();

        DSLLCross2.Run(Cores);
		
		DSLLCross2.EvaluateShortestResult(LLCross2, true);
		
		const std::chrono::duration<double> llcross2_elapsed_seconds = std::chrono::system_clock::now() - time_llcross2_start;
        TimeLLCross2 = llcross2_elapsed_seconds.count();
	}
	
	// Search the last layer corners (only permutation)
	void LBL::SearchLLCorners1()
	{
		const auto time_llcorners1_start = std::chrono::system_clock::now();
		
		Algorithm AlgStart = Scramble + Inspections[FLi];
		for (const auto& Cross_Alg : FLCross) AlgStart += Cross_Alg;
		for (const auto& FL_Alg : FLCorners) AlgStart += FL_Alg;
		for (const auto& SL_Alg : SLEdges) AlgStart += SL_Alg;
		AlgStart += LLCross1;
		AlgStart += LLCross2;
		
		const Cube C(AlgStart);

        DeepSearch DSLLCorners1(AlgStart, Plc::SHORT);
		
		switch (FirstLayer)
		{
		case Lyr::U:
			if (C.CheckPosition(Pgr::CORNERS_D)) return;
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_U);
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_E);
			DSLLCorners1.AddToMandatoryPieces(Pgr::CROSS_D);
			DSLLCorners1.AddToMandatoryPositions(Pgr::CORNERS_D);
			break;
		case Lyr::D:
		if (C.CheckPosition(Pgr::CORNERS_U)) return;
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_D);
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_E);
			DSLLCorners1.AddToMandatoryPieces(Pgr::CROSS_U);
			DSLLCorners1.AddToMandatoryPositions(Pgr::CORNERS_U);
			break;
		case Lyr::F:
			if (C.CheckPosition(Pgr::CORNERS_B)) return;
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_F);
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_S);
			DSLLCorners1.AddToMandatoryPieces(Pgr::CROSS_B);
			DSLLCorners1.AddToMandatoryPositions(Pgr::CORNERS_B);
			break;
		case Lyr::B:
			if (C.CheckPosition(Pgr::CORNERS_F)) return;
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_B);
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_S);
			DSLLCorners1.AddToMandatoryPieces(Pgr::CROSS_F);
			DSLLCorners1.AddToMandatoryPositions(Pgr::CORNERS_F);
			break;
		case Lyr::R:
			if (C.CheckPosition(Pgr::CORNERS_L)) return;
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_R);
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_M);
			DSLLCorners1.AddToMandatoryPieces(Pgr::CROSS_L);
			DSLLCorners1.AddToMandatoryPositions(Pgr::CORNERS_L);
			break;
		case Lyr::L:
			if (C.CheckPosition(Pgr::CORNERS_R)) return;
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_L);
			DSLLCorners1.AddToMandatoryPieces(Pgr::LAYER_M);
			DSLLCorners1.AddToMandatoryPieces(Pgr::CROSS_R);
			DSLLCorners1.AddToMandatoryPositions(Pgr::CORNERS_R);
			break;
		default: return;
		}
		
        const SearchUnit U1(SequenceTypes::SINGLE, Sst::SINGLE_U);
        SearchUnit U2(SequenceTypes::SEQUENCE, Sst::NONE);
        U2.AddAlgorithm(Corners1Algorithm);
		
        SearchLevel L(SearchCheck::CHECK);
        L.Add(U1);
		L.Add(U2);
        
        for (int l = 0; l < 5; l++) DSLLCorners1.AddSearchLevel(L);

        DSLLCorners1.UpdateRootData();

        DSLLCorners1.Run(Cores);
		
		DSLLCorners1.EvaluateShortestResult(LLCorners1, true);
		
		const std::chrono::duration<double> llcorners1_elapsed_seconds = std::chrono::system_clock::now() - time_llcorners1_start;
        TimeLLCorners1 = llcorners1_elapsed_seconds.count();
	}
	
	// Search the last layer corners (orientation)
	void LBL::SearchLLCorners2()
	{
		const auto time_llcorners2_start = std::chrono::system_clock::now();
		
		Algorithm AlgStart = Scramble + Inspections[FLi];
		for (const auto& Cross_Alg : FLCross) AlgStart += Cross_Alg;
		for (const auto& FL_Alg : FLCorners) AlgStart += FL_Alg;
		for (const auto& SL_Alg : SLEdges) AlgStart += SL_Alg;
		AlgStart += LLCross1;
		AlgStart += LLCross2;
		AlgStart += LLCorners1;

		if (Cube(AlgStart).IsSolved()) return;

        DeepSearch DSLLCorners2(AlgStart, Plc::SHORT);
		
		DSLLCorners2.AddToMandatoryPieces(Pgr::ALL);
		
        const SearchUnit U1(SequenceTypes::SINGLE, Sst::SINGLE_U);
        SearchUnit U2(SequenceTypes::SEQUENCE, Sst::NONE);
        U2.AddAlgorithm(Corners2Algorithm2);
		
        SearchLevel L(SearchCheck::CHECK);
        L.Add(U1);
		L.Add(U2);
		
		SearchLevel L_End(SearchCheck::CHECK);
		L_End.Add(U1);
        
        for (int l = 0; l < 14; l++) DSLLCorners2.AddSearchLevel(L);
		DSLLCorners2.AddSearchLevel(L_End);

        DSLLCorners2.UpdateRootData();

        DSLLCorners2.Run(Cores);
		
        EvaluateLLCOResult(LLCorners2, DSLLCorners2.GetSolves());
		
		const std::chrono::duration<double> llcorners2_elapsed_seconds = std::chrono::system_clock::now() - time_llcorners2_start;
        TimeLLCorners2 = llcorners2_elapsed_seconds.count();
	}
		
    // Check if the first layer cross is solved
    bool LBL::IsFLCrossSolved() const
    {
        Cube CubeCross = CubeBase;
        CubeCross.ApplyAlgorithm(Inspections[FLi]);
		for (const auto& Cross_Alg : FLCross) CubeCross.ApplyAlgorithm(Cross_Alg);

        switch (FirstLayer)
        {
        case Lyr::U: return CubeCross.IsSolved(Pgr::CROSS_U);
        case Lyr::D: return CubeCross.IsSolved(Pgr::CROSS_D);
        case Lyr::F: return CubeCross.IsSolved(Pgr::CROSS_F);
        case Lyr::B: return CubeCross.IsSolved(Pgr::CROSS_B);
        case Lyr::R: return CubeCross.IsSolved(Pgr::CROSS_R);
        case Lyr::L: return CubeCross.IsSolved(Pgr::CROSS_L);
        default: return false;
        }
    }

	// Check if the first layer is solved
    bool LBL::IsFLSolved() const
    {
        Cube CubeFL = CubeBase;
        CubeFL.ApplyAlgorithm(Inspections[FLi]);
        for (const auto& Cross_Alg : FLCross) CubeFL.ApplyAlgorithm(Cross_Alg);
        for (const auto& A : FLCorners) CubeFL.ApplyAlgorithm(A);
		
		return CubeFL.IsSolved(FirstLayer);
    }
	
    // Check if the second layer is solved
    bool LBL::IsSLSolved() const
    {
        Cube CubeSL = CubeBase;
        CubeSL.ApplyAlgorithm(Inspections[FLi]);
        for (const auto& Cross_Alg : FLCross) CubeSL.ApplyAlgorithm(Cross_Alg);
		for (const auto& A : FLCorners) CubeSL.ApplyAlgorithm(A);
        for (const auto& A : SLEdges) CubeSL.ApplyAlgorithm(A);
		
		return CubeSL.IsSolved(FirstLayer) && CubeSL.IsSolved(Cube::AdjacentLayer(FirstLayer));
    }
	
	// Check full solve
    bool LBL::IsSolved() const
    {
        Cube CubeLBL = CubeBase;
        CubeLBL.ApplyAlgorithm(Inspections[FLi]);
        for (const auto& Cross_Alg : FLCross) CubeLBL.ApplyAlgorithm(Cross_Alg);
		for (const auto& A : FLCorners) CubeLBL.ApplyAlgorithm(A);
        for (const auto& A : SLEdges) CubeLBL.ApplyAlgorithm(A);
		CubeLBL.ApplyAlgorithm(LLCross1);
		CubeLBL.ApplyAlgorithm(LLCross2);
		CubeLBL.ApplyAlgorithm(LLCorners1);
		CubeLBL.ApplyAlgorithm(LLCorners2);
		
		return CubeLBL.IsSolved();
    }

	// Returns the shorter LBL last layer corners orientation algorithm from the solves
	uint LBL::EvaluateLLCOResult(Algorithm& ShortSolve, const std::vector<Algorithm>& Solves)
	{
		if (Solves.size() <= 0) { ShortSolve.Clear(); return 0u; } // No solves
		uint Score, MaxScore = 0u, MaxScoreIndex = 0u, Index = 0u;
		for (const auto& s : Solves) // Check each solve to get the best one
		{
			// Last movement must be an U-Slice
			if (Algorithm::GetLayer(s.Last()) != Lyr::U) { Index++; continue; }
			Score = (1000u - s.GetSize()) * 100u;
			Score -= s.GetSubjectiveScore();

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

	std::string LBL::GetTextLLCross_1(bool Compress) const 
	{ 
		if (!Compress) return LLCross1.ToString();
		
		Algorithm ACompress = Algorithm::Compress(LLCross1, Cross1Algorithm);

		return ACompress.ToString();
	}
	
	std::string LBL::GetTextLLCross_2(bool Compress) const 
	{ 
		if (!Compress) return LLCross2.ToString();

		Algorithm ACompress = Algorithm::Compress(LLCross2, Cross2Algorithm);

		return ACompress.ToString();
	}
	
	std::string LBL::GetTextLLCorners_1(bool Compress) const 
	{ 
		if (!Compress) return LLCorners1.ToString();

		Algorithm ACompress = Algorithm::Compress(LLCorners1, Corners1Algorithm);

		return ACompress.ToString();
	}
	
	std::string LBL::GetTextLLCorners_2(bool Compress) const 
	{
		if (!Compress) return LLCorners2.ToString();

		Algorithm ACompress = Algorithm::Compress(LLCorners2, Corners2Algorithm);

		return ACompress.ToString();
	}

    // Get a solve report
    std::string LBL::GetReport() const
    {
        std::string Report;
        // Report = "LBL search with first layer ";
        // Report += Cube::GetLayerChar(FirstLayer);
        // Report += ":\n---------------------------------\n";
        // Report += "Scramble (" + std::to_string(Scramble.GetNumSteps()) + "): " + GetTextScramble() + "\n\n";

        if (!IsFLCrossSolved())
        {
            Report += "Cross not solved!\n";
            Report += "Search time: " + std::to_string(GetTime()) + " s\n";
            return Report;
        }

        if (GetLengthInspection() > 0u) Report += GetTextInspection() + " // Inspection\n";

		// Show first layer cross solves
		std::string CrossString = GetTextFLEdge_1();
        if (!CrossString.empty()) Report += "(" + CrossString + ") ";
        CrossString = GetTextFLEdge_2();
        if (!CrossString.empty()) Report += "(" + CrossString + ") ";
        CrossString = GetTextFLEdge_3();
        if (!CrossString.empty()) Report += "(" + CrossString + ") ";
        CrossString = GetTextFLEdge_4();
        if (!CrossString.empty()) Report += "(" + CrossString + ")";
        Report += " // First layer cross (" + Algorithm::GetMetricValue(GetMetricFLCross()) + ")\n";

        // Show first layer (corners) solves
        if (!IsFLSolved())
        {
            Report += "First layer (corners) not solved!\n";
            Report += "Search time: " + std::to_string(GetTime()) + " s\n";
            return Report;
        }

        std::string FLString = GetTextFLCorner_1();
        if (!FLString.empty()) Report += "(" + FLString + ") ";
        FLString = GetTextFLCorner_2();
        if (!FLString.empty()) Report += "(" + FLString + ") ";
        FLString = GetTextFLCorner_3();
        if (!FLString.empty()) Report += "(" + FLString + ") ";
        FLString = GetTextFLCorner_4();
        if (!FLString.empty()) Report += "(" + FLString + ")";
        Report += " // First layer corners (" + Algorithm::GetMetricValue(GetMetricFLCorners()) + ")\n";
		
        // Show second layer solves
        if (!IsSLSolved())
        {
            Report += "Second layer not solved!\n";
            Report += "Search time: " + std::to_string(GetTime()) + " s\n";
            return Report;
        }

        std::string SLString = GetTextSLEdge_1();
        if (!SLString.empty()) Report += "(" + SLString + ") ";
        SLString = GetTextSLEdge_2();
        if (!SLString.empty()) Report += "(" + SLString + ") ";
        SLString = GetTextSLEdge_3();
        if (!SLString.empty()) Report += "(" + SLString + ") ";
        SLString = GetTextSLEdge_4();
        if (!SLString.empty()) Report += "(" + SLString + ")";
        Report += " // Second layer edges (" + Algorithm::GetMetricValue(GetMetricSL()) + ")\n";

		// Show last layer solves
        if (GetLengthLLCross1() > 0u)
            Report += GetTextLLCross_1(true) + " // Last layer cross orientation (" + Algorithm::GetMetricValue(GetMetricLLCross1()) + ")\n";
        if (GetLengthLLCross2() > 0u)
            Report += GetTextLLCross_2(true) + " // Last layer cross permutation (" + Algorithm::GetMetricValue(GetMetricLLCross2()) + ")\n";
        if (GetLengthLLCorners1() > 0u)
            Report += GetTextLLCorners_1(true) + " // Last layer corners permutation (" + Algorithm::GetMetricValue(GetMetricLLCorners1()) + ")\n";
        if (GetLengthLLCorners2() > 0u)
            Report += GetTextLLCorners_2(true) + " // Last layer corners orientation (" + Algorithm::GetMetricValue(GetMetricLLCorners2()) + ")\n";
		
        // Show metric
        Report += "\nMetric: " + Algorithm::GetMetricValue(GetMetricSolve()) + " " + Algorithm::GetMetricString(Metric) + "\n";

        return Report;
    };

    // Get a solve time report
    std::string LBL::GetTimeReport() const
    {
        std::string Report;

        Report += "Total time: " + std::to_string(GetTime()) + " s\n";
        Report += "First layer cross time: " + std::to_string(GetTimeFLCross()) + " s\n";
        Report += "First layer corners time: " + std::to_string(GetTimeFLCorners()) + " s\n";
        Report += "Second layer time: " + std::to_string(GetTimeSL()) + " s\n";
        Report += "Last layer time: " + std::to_string(GetTimeLL()) + " s\n";

        return Report;
    }

	// Get the solve metric
    float LBL::GetMetricSolve() const
    {
        Algorithm A;
		
        for (const auto& Alg : FLCross) A += Alg;
		for (const auto& Alg : FLCorners) A += Alg;
        for (const auto& Alg : SLEdges) A += Alg;
		A += LLCross1;
		A += LLCross2;
		A += LLCorners1;
		A += LLCorners2;
        
        return A.GetMetric(Metric);
    }
}
