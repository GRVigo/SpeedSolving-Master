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

#pragma once

#include <cstdint>

namespace grcube3
{
/*
 Spn = Spin
 Pce = Piece
 Pcp = Piece position
 App = Absolut piece position
 Pcd = Piece direction
 Pco = Piece orientation
 Ctr = Center
 Ctp = Center position
 Ctd = Center direction
 Cto = Center orientation
 Edg = Edge
 Edp = Edge position
 Aep = Absolut edge position
 Edd = Edge direction
 Edo = Edge orientation
 Cnr = Corner
 Cnp = Corner position
 Acp = Absolut corner position
 Cnd = Corner direction
 Cno = Corner orientation
 Pct = Piece type
 Stc = Sticker
 Sps = Sticker position
 Asp = Absolut sticker position
 Fce = Face
 Lyr = Layer
 Slc = Slice
 Stp = Step
 Rng = Range
 Pgr = Positions group
 Sst = Search steps
 Plc = Policy
*/

	// Type definitions
	
	using S64 = std::uint64_t; // 64 bits status
	using M16 = std::uint16_t; // 16 bits masks
	using uint = unsigned int;
	
	// Enumerators
	
	enum class Spn // All cube possible spins as faces oriented to up and front
	{ 
		UF, UR, UB, UL, DF, DR, DB, DL,
		FU, FR, FD, FL, BU, BR, BD, BL,
		RU, RF, RD, RB, LU, LF, LD, LB,
		Default = UF
	};
	
	// Centers from 0 to 5
	// Edges from 6 to 17 [First_Edge + (4 bits)]: FR 0000 - FL 0001 - BL 0010 - BR 0011 - UF 0100 - UL 0101 - UB 0110 - UR 0111 - DF 1000 - DL 1001 - DB 1010 - DR 1011
	// Corners from 18 to 23 [First_Corner + (3 bits)]: UFR 000 - UFL 001 - UBL 010 - UBR 011 - DFR 100 - DFL 101 - DBL 110 - DBR 111
	enum class Pce // Rubik's cube all pieces
	{ 
		U, D, F, B, R, L, 
		FR, FL, BL, BR, UF, UL, UB, UR, DF, DL, DB, DR,
		UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR,
		First = U, Last = DBR,
		First_Center = U, First_Edge = FR, First_Corner = UFR,
		Last_Center = L, Last_Edge = DR, Last_Corner = DBR
	};

	enum class Pcp // Rubik's cube all piece positions (don't change with the spin, relative to centers)
	{
		U, D, F, B, R, L,
		FR, FL, BL, BR, UF, UL, UB, UR, DF, DL, DB, DR,
		UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR,
		First = U, Last = DBR,
		First_Center = U, First_Edge = FR, First_Corner = UFR,
		Last_Center = L, Last_Edge = DR, Last_Corner = DBR
	};
	
	enum class App // Rubik's cube all piece positions absolutes to the cube
	{
		U, D, F, B, R, L,
		FR, FL, BL, BR, UF, UL, UB, UR, DF, DL, DB, DR,
		UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR,
		First = U, Last = DBR,
		First_Center = U, First_Edge = FR, First_Corner = UFR,
		Last_Center = L, Last_Edge = DR, Last_Corner = DBR
	};

	// Directions for edges:
	// 'E' layer edge in layer 'E': -EDGE_POS- if F/B sticker is in FB direction / -EDGE_NEG- if F/B sticker is in RL direction
	// 'E' layer edge in layer 'U/D': -EDGE_POS- if F/B sticker is in UD direction / -EDGE_NEG- if F/B sticker is in FB/RL directions
	// 'U/D' layer edge in layer 'E': -EDGE_POS- if U/D sticker is in FB direction / -EDGE_NEG- if U/D sticker is in RL direction
	// 'U/D' layer edge in layer 'U/D': -EDGE_POS- if U/D sticker is in UD direction / -EDGE_NEG- if U/D sticker is in FB/RL directions
	// Directions for corners:
	// U/D sticker direction: UD -> CORNER_UD / FB -> CORNER_FB / RL -> CORNER_RL
	enum class Pcd // All possible directions for pieces
	{ 
		NONE, // Centers don't need direction
		POS, NEG, // Edges direction (positive / negative)
		UD, FB, RL, // Corners direction (UD, FB or RL direction for U/D sticker)
		First_Edge = POS, First_Corner = UD,
		Last_Edge = NEG, Last_Corner = RL
	};
	
	enum class Pco // All possible orientations for pieces
	{ 
		NONE, // Centers don't need direction
		POS, NEG, // Edges direction (positive / negative)
		UD, FB, RL, // Corners direction (UD, FB or RL direction for U/D sticker)
		First_Edge = POS, First_Corner = UD,
		Last_Edge = NEG, Last_Corner = RL
	};

	// Rubik's cube center pieces
	enum class Ctr { U, D, F, B, R, L };

	// Rubik's cube center piece positions
	enum class Ctp { U, D, F, B, R, L };
	
	enum class Ctd // Rubik's cube center piece directions
	{
		NONE // Centers don't need direction
	};
	
	enum class Cto // Rubik's cube center piece orientations
	{
		NONE // Centers don't need orientation
	};

	enum class Edg // Rubik's cube edges pieces
	{
		FR, FL, BL, BR, UF, UL, UB, UR, DF, DL, DB, DR
	};

	enum class Edp // Rubik's cube edge positions
	{
		FR, FL, BL, BR, UF, UL, UB, UR, DF, DL, DB, DR
	};
	
	enum class Aep // Rubik's cube absolute edge positions
	{
		FR, FL, BL, BR, UF, UL, UB, UR, DF, DL, DB, DR
	};

	// 'E' layer edge in layer 'E': -EDGE_POS- if F/B sticker is in FB direction / -EDGE_NEG- if F/B sticker is in RL direction
	// 'E' layer edge in layer 'U/D': -EDGE_POS- if F/B sticker is in UD direction / -EDGE_NEG- if F/B sticker is in FB/RL directions
	// 'U/D' layer edge in layer 'E': -EDGE_POS- if U/D sticker is in FB direction / -EDGE_NEG- if U/D sticker is in RL direction
	// 'U/D' layer edge in layer 'U/D': -EDGE_POS- if U/D sticker is in UD direction / -EDGE_NEG- if U/D sticker is in FB/RL directions
	enum class Edd // Rubik's cube edge piece directions
	{
		POS, NEG // Edges direction (positive / negative)
	};
	
	enum class Edo // Rubik's cube edge piece orientations
	{
		POS, NEG // Edges orientation (positive / negative)
	};

	enum class Cnr // Rubik's cube corner pieces
	{
		UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR
	};

	enum class Cnp // Rubik's cube corner positions
	{
		UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR
	};
	
	enum class Acp // Rubik's cube absolute corner positions
	{
		UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR
	};
	
	// U/D sticker direction: UD -> CORNER_UD / FB -> CORNER_FB / RL -> CORNER_RL
	enum class Cnd // Rubik's cube corner piece directions
	{
		UD, FB, RL // Corners direction (UD, FB or RL direction for U/D layer sticker)
	};
	
	enum class Cno // Rubik's cube corner piece orientations
	{
		UD, FB, RL // Corners orientation (UD, FB or RL orientation for U/D slice sticker)
	};

	enum class Pct { CENTER, EDGE, CORNER }; // Rubik's cube pieces types
	
	enum class Stc // Rubik's cube all stickers
	{ 
		U, D, F, B, R, L,  // Centers
		// Edges
		FR_F, FR_R, FL_F, FL_L, BL_B, BL_L, BR_B, BR_R, // Middle layer
		UF_U, UF_F, UL_U, UL_L, UB_U, UB_B, UR_U, UR_R, // Upper layer
		DF_D, DF_F, DL_D, DL_L, DB_D, DB_B, DR_D, DR_R, // Lower layer
		// Corners
		UFR_U, UFR_F, UFR_R, UFL_U, UFL_F, UFL_L, // Upper layer
		UBL_U, UBL_B, UBL_L, UBR_U, UBR_B, UBR_R, // Upper layer
		DFR_D, DFR_F, DFR_R, DFL_D, DFL_F, DFL_L, // Lower layer
		DBL_D, DBL_B, DBL_L, DBR_D, DBR_B, DBR_R, // Lower layer
		First = U, Last = DBR_R,
		First_Center = U, First_Edge = FR_F, First_Corner = UFR_U,
		Last_Center = L, Last_Edge = DR_R, Last_Corner = DBR_R
	};

	enum class Sps // Rubik's cube all stickers positions (don't change with the spin, relative to centers)
	{
		U, D, F, B, R, L,  // Centers
		// Edges
		FR_F, FR_R, FL_F, FL_L, BL_B, BL_L, BR_B, BR_R, // Middle layer
		UF_U, UF_F, UL_U, UL_L, UB_U, UB_B, UR_U, UR_R, // Upper layer
		DF_D, DF_F, DL_D, DL_L, DB_D, DB_B, DR_D, DR_R, // Lower layer
		// Corners
		UFR_U, UFR_F, UFR_R, UFL_U, UFL_F, UFL_L, // Upper layer
		UBL_U, UBL_B, UBL_L, UBR_U, UBR_B, UBR_R, // Upper layer
		DFR_D, DFR_F, DFR_R, DFL_D, DFL_F, DFL_L, // Lower layer
		DBL_D, DBL_B, DBL_L, DBR_D, DBR_B, DBR_R, // Lower layer
		First = U, Last = DBR_R,
		First_Center = U, First_Edge = FR_F, First_Corner = UFR_U,
		Last_Center = L, Last_Edge = DR_R, Last_Corner = DBR_R
	};
	
	enum class Asp // Rubik's cube all sticker positions absolutes to the cube
	{
		U, D, F, B, R, L,  // Centers
		// Edges
		FR_F, FR_R, FL_F, FL_L, BL_B, BL_L, BR_B, BR_R, // Middle layer
		UF_U, UF_F, UL_U, UL_L, UB_U, UB_B, UR_U, UR_R, // Upper layer
		DF_D, DF_F, DL_D, DL_L, DB_D, DB_B, DR_D, DR_R, // Lower layer
		// Corners
		UFR_U, UFR_F, UFR_R, UFL_U, UFL_F, UFL_L, // Upper layer
		UBL_U, UBL_B, UBL_L, UBR_U, UBR_B, UBR_R, // Upper layer
		DFR_D, DFR_F, DFR_R, DFL_D, DFL_F, DFL_L, // Lower layer
		DBL_D, DBL_B, DBL_L, DBR_D, DBR_B, DBR_R, // Lower layer
		First = U, Last = DBR_R,
		First_Center = U, First_Edge = FR_F, First_Corner = UFR_U,
		Last_Center = L, Last_Edge = DR_R, Last_Corner = DBR_R
	};
	
	enum class Fce // Rubik's cube faces
	{ 
		U, D, F, B, R, L
	};
	
	enum class Lyr // Rubik's cube layers
	{ 
		NONE, U, D, F, B, R, L, E, S, M
	};
	
	enum class Slc // Rubik's cube slices (layers absolutes to the cube)
	{ 
		NONE, U, D, F, B, R, L, E, S, M
	};
	
	enum class Stp // All possible movements
	{ 
		NONE, // No movement
	
		// Single layer movements
		U, Up, U2,
		D, Dp, D2,
		F, Fp, F2,
		B, Bp, B2,
		R, Rp, R2,
		L, Lp, L2,
	
		// Double layer movements
		u, up, u2,
		d, dp, d2,
		f, fp, f2,
		b, bp, b2,
		r, rp, r2,
		l, lp, l2,
	
		// Middle layer movements
		E, Ep, E2,
		S, Sp, S2,
		M, Mp, M2,
	
		// Full cube turns
		x, xp, x2,
		y, yp, y2,
		z, zp, z2,
	
		// Parentheses
		PARENTHESIS_OPEN, 			// Open parenthesis
		PARENTHESIS_CLOSE_1_REP, 	// Close parenthesis
		PARENTHESIS_CLOSE_2_REP, 	// Close parenthesis with two repetitions
		PARENTHESIS_CLOSE_3_REP, 	// Close parenthesis with three repetitions
		PARENTHESIS_CLOSE_4_REP, 	// Close parenthesis with four repetitions
		PARENTHESIS_CLOSE_5_REP, 	// Close parenthesis with five repetitions
		PARENTHESIS_CLOSE_6_REP, 	// Close parenthesis with six repetitions
		PARENTHESIS_CLOSE_7_REP, 	// Close parenthesis with seven repetitions
		PARENTHESIS_CLOSE_8_REP, 	// Close parenthesis with eight repetitions
		PARENTHESIS_CLOSE_9_REP 	// Close parenthesis with nine repetitions
	};
	
	enum class Rng // All possible ranges (types) for movements (steps)
	{ 
		NONE, 		// No movement
		SINGLE, 	// Single external layer (U, D, F, B, R, L)
		DOUBLE, 	// Double layer (u, d, f, b, r, l)
		INTERNAL, 	// Single internal layer (E, S, M)
		TURN, 		// Turn (x, y, z)
		PARENTHESES	// Parentheses
	};
	
	//  Groups of pieces positions for search
	enum class Pgr 
	{
		ALL, // All cube pieces
        ALL_EDGES, ALL_CORNERS, // All edges and corners
		LAYER_U, LAYER_D, LAYER_F, LAYER_B, LAYER_R, LAYER_L, // External layers pieces
		LAYER_E, LAYER_S, LAYER_M, // Internal layers pieces

		// CFOP method
		CROSS_U, CROSS_D, CROSS_F, CROSS_B, CROSS_R, CROSS_L, // Crosses - four edges in the layer
		F2L_U, F2L_U_UFR, F2L_U_UFL, F2L_U_UBR, F2L_U_UBL, // First two layers pieces starting with U cross
		F2L_D, F2L_D_DFR, F2L_D_DFL, F2L_D_DBR, F2L_D_DBL, // First two layers pieces starting with D cross
		F2L_F, F2L_F_UFR, F2L_F_UFL, F2L_F_DFR, F2L_F_DFL, // First two layers pieces starting with F cross
		F2L_B, F2L_B_UBR, F2L_B_UBL, F2L_B_DBR, F2L_B_DBL, // First two layers pieces starting with B cross
		F2L_R, F2L_R_UFR, F2L_R_UBR, F2L_R_DFR, F2L_R_DBR, // First two layers pieces starting with R cross
		F2L_L, F2L_L_UFL, F2L_L_UBL, F2L_L_DFL, F2L_L_DBL, // First two layers pieces starting with L cross
		LL_U, LL_D, LL_F, LL_B, LL_R, LL_L, // Last layer (for OLL & PLL)

		// Roux method
		UF_B1, UR_B1, UB_B1, UL_B1, DF_B1, DR_B1, DB_B1, DL_B1, // Spin + Block 1 (Left)
		FU_B1, FR_B1, FD_B1, FL_B1, BU_B1, BR_B1, BD_B1, BL_B1,
		RU_B1, RF_B1, RD_B1, RB_B1, LU_B1, LF_B1, LD_B1, LB_B1,
		UF_B2, UR_B2, UB_B2, UL_B2, DF_B2, DR_B2, DB_B2, DL_B2, // Spin + Block 2 (Right)
		FU_B2, FR_B2, FD_B2, FL_B2, BU_B2, BR_B2, BD_B2, BL_B2,
		RU_B2, RF_B2, RD_B2, RB_B2, LU_B2, LF_B2, LD_B2, LB_B2,
		UF_B1S1, UF_B1S2, UB_B1S1, UB_B1S2, UR_B1S1, UR_B1S2, UL_B1S1, UL_B1S2, // Spin + Square in block
		UF_B2S1, UF_B2S2, UB_B2S1, UB_B2S2, UR_B2S1, UR_B2S2, UL_B2S1, UL_B2S2,
		DF_B1S1, DF_B1S2, DB_B1S1, DB_B1S2, DR_B1S1, DR_B1S2, DL_B1S1, DL_B1S2,
		DF_B2S1, DF_B2S2, DB_B2S1, DB_B2S2, DR_B2S1, DR_B2S2, DL_B2S1, DL_B2S2,
		FU_B1S1, FU_B1S2, FD_B1S1, FD_B1S2, FR_B1S1, FR_B1S2, FL_B1S1, FL_B1S2,
		FU_B2S1, FU_B2S2, FD_B2S1, FD_B2S2, FR_B2S1, FR_B2S2, FL_B2S1, FL_B2S2,
		BU_B1S1, BU_B1S2, BD_B1S1, BD_B1S2, BR_B1S1, BR_B1S2, BL_B1S1, BL_B1S2,
		BU_B2S1, BU_B2S2, BD_B2S1, BD_B2S2, BR_B2S1, BR_B2S2, BL_B2S1, BL_B2S2,
		RU_B1S1, RU_B1S2, RD_B1S1, RD_B1S2, RF_B1S1, RF_B1S2, RB_B1S1, RB_B1S2,
		RU_B2S1, RU_B2S2, RD_B2S1, RD_B2S2, RF_B2S1, RF_B2S2, RB_B2S1, RB_B2S2,
		LU_B1S1, LU_B1S2, LD_B1S1, LD_B1S2, LF_B1S1, LF_B1S2, LB_B1S1, LB_B1S2,
		LU_B2S1, LU_B2S2, LD_B2S1, LD_B2S2, LF_B2S1, LF_B2S2, LB_B2S1, LB_B2S2,

		CORNERS_U, CORNERS_D, CORNERS_F, CORNERS_B, CORNERS_R, CORNERS_L, // Four corners in each layer

		// Roux last six edges
		ROUX_L6E_UF, ROUX_L6E_UB, ROUX_L6E_UR, ROUX_L6E_UL,
		ROUX_L6E_DF, ROUX_L6E_DB, ROUX_L6E_DR, ROUX_L6E_DL,
		ROUX_L6E_FU, ROUX_L6E_FD, ROUX_L6E_FR, ROUX_L6E_FL,
		ROUX_L6E_BU, ROUX_L6E_BD, ROUX_L6E_BR, ROUX_L6E_BL,
		ROUX_L6E_RU, ROUX_L6E_RD, ROUX_L6E_RF, ROUX_L6E_RB,
		ROUX_L6E_LU, ROUX_L6E_LD, ROUX_L6E_LF, ROUX_L6E_LB,
		
        // Petrus first blocks
        PETRUS_UFR, PETRUS_UFL, PETRUS_UBR, PETRUS_UBL, PETRUS_DFR, PETRUS_DFL, PETRUS_DBR, PETRUS_DBL,

        // Petrus extended first blocks
        PETRUS_UFR_X, PETRUS_UFL_X, PETRUS_UBR_X, PETRUS_UBL_X, PETRUS_DFR_X, PETRUS_DFL_X, PETRUS_DBR_X, PETRUS_DBL_X,
        PETRUS_UFR_Y, PETRUS_UFL_Y, PETRUS_UBR_Y, PETRUS_UBL_Y, PETRUS_DFR_Y, PETRUS_DFL_Y, PETRUS_DBR_Y, PETRUS_DBL_Y,
        PETRUS_UFR_Z, PETRUS_UFL_Z, PETRUS_UBR_Z, PETRUS_UBL_Z, PETRUS_DFR_Z, PETRUS_DFL_Z, PETRUS_DBR_Z, PETRUS_DBL_Z,
        
        // ZZ EO Lines
        EOLINE_UF, EOLINE_UB, EOLINE_UR, EOLINE_UL, EOLINE_DF, EOLINE_DB, EOLINE_DR, EOLINE_DL, 
        EOLINE_FU, EOLINE_FD, EOLINE_FR, EOLINE_FL, EOLINE_BU, EOLINE_BD, EOLINE_BR, EOLINE_BL, 
        EOLINE_RU, EOLINE_RD, EOLINE_RF, EOLINE_RB, EOLINE_LU, EOLINE_LD, EOLINE_LF, EOLINE_LB,

		// yruRU CP Lines
		CPLINE_UF, CPLINE_UB, CPLINE_UR, CPLINE_UL, CPLINE_DF, CPLINE_DB, CPLINE_DR, CPLINE_DL,
		CPLINE_FU, CPLINE_FD, CPLINE_FR, CPLINE_FL, CPLINE_BU, CPLINE_BD, CPLINE_BR, CPLINE_BL,
		CPLINE_RU, CPLINE_RD, CPLINE_RF, CPLINE_RB, CPLINE_LU, CPLINE_LD, CPLINE_LF, CPLINE_LB,

		// Mehta three edges belts
		BELT_UD_1, BELT_UD_2, BELT_UD_3, BELT_UD_4,
		BELT_FB_1, BELT_FB_2, BELT_FB_3, BELT_FB_4,
		BELT_RL_1, BELT_RL_2, BELT_RL_3, BELT_RL_4,
	};

	// Groups of steps for search
	enum class Sst
	{
		NONE,
		SINGLE_ALL, // U, U', U2, ..., L', L2
		SINGLE_U, SINGLE_D, SINGLE_F, SINGLE_B, SINGLE_R, SINGLE_L, // Single movements in given external layer
		SINGLE_M, SINGLE_S, SINGLE_E, // Single movements in given internal layer
		SINGLE_UR, // Single movements in U and R layers
		SINGLE_UM, // Single movements in U and M layers
		LATERAL_UD, LATERAL_FB, LATERAL_RL, // Lateral movements, only 90 degrees both directions
		ROUX_SB, // Movements for Roux second block search
		LATERAL_SINGLE_FBp, // Movements F or B' only
		PETRUS_EB, // Petrus extended block movements
		PETRUS_OR_U, PETRUS_OR_R, // Petrus edges orientation return movements 
		PETRUS_UD_R, // Petrus lateral movements for B', R, R' and F
		ZZ_LATERAL, // ZZ Lateral movements for F2L (R, R', L, L')
		ZZ_LATERAL2, // ZZ Lateral movements for F2L (R, R', R2, L, L', L2)
		YRURU_urUR, // YruRU movements for CP-Line
		YRURU_rUR, // YruRU movements for pEO Extension
		YRURU_UR, // YruRU movements for  EOBF / F2L
		NAUTILUS_rRUM, // Moveset for Nautilus dFR
	};
	
	// Search evaluation policies
	enum class Plc
	{
		BEST, // Maximum amount of pieces solved / best score
		SHORT, // Minimum amount of steps
		FIRST, // First solve / algorithm
		RANDOM, // Random algorithm
	};

    // Supported metrics
    enum class Metrics { Movements, HTM, QTM, STM, QSTM, ETM, ATM, PTM, HTM15, OBTM };
}
