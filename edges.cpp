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

#include "edges.h"

namespace grcube3
{
	// Criterion for edges direction:
	// 'E' layer edge into layer 'E': Positive direction if 'F/B' sticker points towards 'F/B' direction
	// 'E' layer edge into layer 'U/D': Positive direction if 'F/B' sticker points towards 'U/D' direction
	// 'U/D' layer edge into layer 'E': Positive direction if 'U/D' sticker points towards 'F/B' direction
	// 'U/D' layer edge into layer 'U/D': Positive direction if 'U/D' sticker points towards 'U/D' direction
	
	// Array to get the face (color) in the FR_F sticker position
	const Fce c_Edges::fce_FR_F[12][2] = 
	{ //    Pos     Neg
		{ Fce::F, Fce::R }, // FR
		{ Fce::F, Fce::L }, // FL
		{ Fce::B, Fce::L }, // BL
		{ Fce::B, Fce::R }, // BR
		{ Fce::U, Fce::F }, // UF
		{ Fce::U, Fce::L }, // UL
		{ Fce::U, Fce::B }, // UB
		{ Fce::U, Fce::R }, // UR
		{ Fce::D, Fce::F }, // DF
		{ Fce::D, Fce::L }, // DL
		{ Fce::D, Fce::B }, // DB
		{ Fce::D, Fce::R }, // DR
	};
	
	// Array to get the face (color) in the FR_R sticker position
	const Fce c_Edges::fce_FR_R[12][2] = 
	{ //    Pos     Neg
		{ Fce::R, Fce::F }, // FR
		{ Fce::L, Fce::F }, // FL
		{ Fce::L, Fce::B }, // BL
		{ Fce::R, Fce::B }, // BR
		{ Fce::F, Fce::U }, // UF
		{ Fce::L, Fce::U }, // UL
		{ Fce::B, Fce::U }, // UB
		{ Fce::R, Fce::U }, // UR
		{ Fce::F, Fce::D }, // DF
		{ Fce::L, Fce::D }, // DL
		{ Fce::B, Fce::D }, // DB
		{ Fce::R, Fce::D }, // DR
	};
	
	// Array to get the sticker in the FR_F sticker position
	const Stc c_Edges::stc_FR_F[12][2] = 
	{ //     Pos        Neg
		{ Stc::FR_F, Stc::FR_R }, // FR
		{ Stc::FL_F, Stc::FL_L }, // FL
		{ Stc::BL_B, Stc::BL_L }, // BL
		{ Stc::BR_B, Stc::BR_R }, // BR
		{ Stc::UF_U, Stc::UF_F }, // UF
		{ Stc::UL_U, Stc::UL_L }, // UL
		{ Stc::UB_U, Stc::UB_B }, // UB
		{ Stc::UR_U, Stc::UR_R }, // UR
		{ Stc::DF_D, Stc::DF_F }, // DF
		{ Stc::DL_D, Stc::DL_L }, // DL
		{ Stc::DB_D, Stc::DB_B }, // DB
		{ Stc::DR_D, Stc::DR_R }, // DR
	};
	
	// Array to get the sticker in the FR_R sticker position
	const Stc c_Edges::stc_FR_R[12][2] = 
	{ //     Pos        Neg
		{ Stc::FR_R, Stc::FR_F }, // FR
		{ Stc::FL_L, Stc::FL_F }, // FL
		{ Stc::BL_L, Stc::BL_B }, // BL
		{ Stc::BR_R, Stc::BR_B }, // BR
		{ Stc::UF_F, Stc::UF_U }, // UF
		{ Stc::UL_L, Stc::UL_U }, // UL
		{ Stc::UB_B, Stc::UB_U }, // UB
		{ Stc::UR_R, Stc::UR_U }, // UR
		{ Stc::DF_F, Stc::DF_D }, // DF
		{ Stc::DL_L, Stc::DL_D }, // DL
		{ Stc::DB_B, Stc::DB_D }, // DB
		{ Stc::DR_R, Stc::DR_D }, // DR
	};

    // Edge positions from spin and absolut edge positions
    const Edp c_Edges::edp_Absolut[24][12] =
    {
        { Edp::FR, Edp::FL, Edp::BL, Edp::BR, Edp::UF, Edp::UL, Edp::UB, Edp::UR, Edp::DF, Edp::DL, Edp::DB, Edp::DR }, // UF
        { Edp::BR, Edp::FR, Edp::FL, Edp::BL, Edp::UR, Edp::UF, Edp::UL, Edp::UB, Edp::DR, Edp::DF, Edp::DL, Edp::DB }, // UR
        { Edp::BL, Edp::BR, Edp::FR, Edp::FL, Edp::UB, Edp::UR, Edp::UF, Edp::UL, Edp::DB, Edp::DR, Edp::DF, Edp::DL }, // UB
        { Edp::FL, Edp::BL, Edp::BR, Edp::FR, Edp::UL, Edp::UB, Edp::UR, Edp::UF, Edp::DL, Edp::DB, Edp::DR, Edp::DF }, // UL
        { Edp::FL, Edp::FR, Edp::BR, Edp::BL, Edp::DF, Edp::DR, Edp::DB, Edp::DL, Edp::UF, Edp::UR, Edp::UB, Edp::UL }, // DF
        { Edp::FR, Edp::BR, Edp::BL, Edp::FL, Edp::DR, Edp::DB, Edp::DL, Edp::DF, Edp::UR, Edp::UB, Edp::UL, Edp::UF }, // DR
        { Edp::BR, Edp::BL, Edp::FL, Edp::FR, Edp::DB, Edp::DL, Edp::DF, Edp::DR, Edp::UB, Edp::UL, Edp::UF, Edp::UR }, // DB
        { Edp::BL, Edp::FL, Edp::FR, Edp::BR, Edp::DL, Edp::DF, Edp::DR, Edp::DB, Edp::UL, Edp::UF, Edp::UR, Edp::UB }, // DL
        { Edp::UL, Edp::UR, Edp::DR, Edp::DL, Edp::UF, Edp::FR, Edp::DF, Edp::FL, Edp::UB, Edp::BR, Edp::DB, Edp::BL }, // FU
        { Edp::UR, Edp::DR, Edp::DL, Edp::UL, Edp::FR, Edp::DF, Edp::FL, Edp::UF, Edp::BR, Edp::DB, Edp::BL, Edp::UB }, // FR
        { Edp::DR, Edp::DL, Edp::UL, Edp::UR, Edp::DF, Edp::FL, Edp::UF, Edp::FR, Edp::DB, Edp::BL, Edp::UB, Edp::BR }, // FD
        { Edp::DL, Edp::UL, Edp::UR, Edp::DR, Edp::FL, Edp::UF, Edp::FR, Edp::DF, Edp::BL, Edp::UB, Edp::BR, Edp::DB }, // FL
        { Edp::UR, Edp::UL, Edp::DL, Edp::DR, Edp::UB, Edp::BL, Edp::DB, Edp::BR, Edp::UF, Edp::FL, Edp::DF, Edp::FR }, // BU
        { Edp::DR, Edp::UR, Edp::UL, Edp::DL, Edp::BR, Edp::UB, Edp::BL, Edp::DB, Edp::FR, Edp::UF, Edp::FL, Edp::DF }, // BR
        { Edp::DL, Edp::DR, Edp::UR, Edp::UL, Edp::DB, Edp::BR, Edp::UB, Edp::BL, Edp::DF, Edp::FR, Edp::UF, Edp::FL }, // BD
        { Edp::UL, Edp::DL, Edp::DR, Edp::UR, Edp::BL, Edp::DB, Edp::BR, Edp::UB, Edp::FL, Edp::DF, Edp::FR, Edp::UF }, // BL
        { Edp::UF, Edp::UB, Edp::DB, Edp::DF, Edp::UR, Edp::BR, Edp::DR, Edp::FR, Edp::UL, Edp::BL, Edp::DL, Edp::FL }, // RU
        { Edp::DF, Edp::UF, Edp::UB, Edp::DB, Edp::FR, Edp::UR, Edp::BR, Edp::DR, Edp::FL, Edp::UL, Edp::BL, Edp::DL }, // RF
        { Edp::DB, Edp::DF, Edp::UF, Edp::UB, Edp::DR, Edp::FR, Edp::UR, Edp::BR, Edp::DL, Edp::FL, Edp::UL, Edp::BL }, // RD
        { Edp::UB, Edp::DB, Edp::DF, Edp::UF, Edp::BR, Edp::DR, Edp::FR, Edp::UR, Edp::BL, Edp::DL, Edp::FL, Edp::UL }, // RB
        { Edp::UB, Edp::UF, Edp::DF, Edp::DB, Edp::UL, Edp::FL, Edp::DL, Edp::BL, Edp::UR, Edp::FR, Edp::DR, Edp::BR }, // LU
        { Edp::UF, Edp::DF, Edp::DB, Edp::UB, Edp::FL, Edp::DL, Edp::BL, Edp::UL, Edp::FR, Edp::DR, Edp::BR, Edp::UR }, // LF
        { Edp::DF, Edp::DB, Edp::UB, Edp::UF, Edp::DL, Edp::BL, Edp::UL, Edp::FL, Edp::DR, Edp::BR, Edp::UR, Edp::FR }, // LD
        { Edp::DB, Edp::UB, Edp::UF, Edp::DF, Edp::BL, Edp::UL, Edp::FL, Edp::DL, Edp::BR, Edp::UR, Edp::FR, Edp::DR }  // LB
    };
	
	// Absolute edge positions from spin and edge positions
	const Aep c_Edges::aep_Positions[24][12] =
	{
		{ Aep::FR, Aep::FL, Aep::BL, Aep::BR, Aep::UF, Aep::UL, Aep::UB, Aep::UR, Aep::DF, Aep::DL, Aep::DB, Aep::DR }, // UF
		{ Aep::FL, Aep::BL, Aep::BR, Aep::FR, Aep::UL, Aep::UB, Aep::UR, Aep::UF, Aep::DL, Aep::DB, Aep::DR, Aep::DF }, // UR
		{ Aep::BL, Aep::BR, Aep::FR, Aep::FL, Aep::UB, Aep::UR, Aep::UF, Aep::UL, Aep::DB, Aep::DR, Aep::DF, Aep::DL }, // UB
		{ Aep::BR, Aep::FR, Aep::FL, Aep::BL, Aep::UR, Aep::UF, Aep::UL, Aep::UB, Aep::DR, Aep::DF, Aep::DL, Aep::DB }, // UL
		{ Aep::FL, Aep::FR, Aep::BR, Aep::BL, Aep::DF, Aep::DR, Aep::DB, Aep::DL, Aep::UF, Aep::UR, Aep::UB, Aep::UL }, // DF
		{ Aep::FR, Aep::BR, Aep::BL, Aep::FL, Aep::DR, Aep::DB, Aep::DL, Aep::DF, Aep::UR, Aep::UB, Aep::UL, Aep::UF }, // DR
		{ Aep::BR, Aep::BL, Aep::FL, Aep::FR, Aep::DB, Aep::DL, Aep::DF, Aep::DR, Aep::UB, Aep::UL, Aep::UF, Aep::UR }, // DB
		{ Aep::BL, Aep::FL, Aep::FR, Aep::BR, Aep::DL, Aep::DF, Aep::DR, Aep::DB, Aep::UL, Aep::UF, Aep::UR, Aep::UB }, // DL
		{ Aep::UL, Aep::UR, Aep::DR, Aep::DL, Aep::UF, Aep::FR, Aep::DF, Aep::FL, Aep::UB, Aep::BR, Aep::DB, Aep::BL }, // FU
		{ Aep::UF, Aep::UB, Aep::DB, Aep::DF, Aep::UR, Aep::BR, Aep::DR, Aep::FR, Aep::UL, Aep::BL, Aep::DL, Aep::FL }, // FR
		{ Aep::UR, Aep::UL, Aep::DL, Aep::DR, Aep::UB, Aep::BL, Aep::DB, Aep::BR, Aep::UF, Aep::FL, Aep::DF, Aep::FR }, // FD
		{ Aep::UB, Aep::UF, Aep::DF, Aep::DB, Aep::UL, Aep::FL, Aep::DL, Aep::BL, Aep::UR, Aep::FR, Aep::DR, Aep::BR }, // FL
		{ Aep::DR, Aep::DL, Aep::UL, Aep::UR, Aep::DF, Aep::FL, Aep::UF, Aep::FR, Aep::DB, Aep::BL, Aep::UB, Aep::BR }, // BU
		{ Aep::DF, Aep::DB, Aep::UB, Aep::UF, Aep::DL, Aep::BL, Aep::UL, Aep::FL, Aep::DR, Aep::BR, Aep::UR, Aep::FR }, // BR
		{ Aep::DL, Aep::DR, Aep::UR, Aep::UL, Aep::DB, Aep::BR, Aep::UB, Aep::BL, Aep::DF, Aep::FR, Aep::UF, Aep::FL }, // BD
		{ Aep::DB, Aep::DF, Aep::UF, Aep::UB, Aep::DR, Aep::FR, Aep::UR, Aep::BR, Aep::DL, Aep::FL, Aep::UL, Aep::BL }, // BL
		{ Aep::UR, Aep::DR, Aep::DL, Aep::UL, Aep::FR, Aep::DF, Aep::FL, Aep::UF, Aep::BR, Aep::DB, Aep::BL, Aep::UB }, // RU
		{ Aep::UF, Aep::DF, Aep::DB, Aep::UB, Aep::FL, Aep::DL, Aep::BL, Aep::UL, Aep::FR, Aep::DR, Aep::BR, Aep::UR }, // RF
		{ Aep::UL, Aep::DL, Aep::DR, Aep::UR, Aep::BL, Aep::DB, Aep::BR, Aep::UB, Aep::FL, Aep::DF, Aep::FR, Aep::UF }, // RD
		{ Aep::UB, Aep::DB, Aep::DF, Aep::UF, Aep::BR, Aep::DR, Aep::FR, Aep::UR, Aep::BL, Aep::DL, Aep::FL, Aep::UL }, // RB
		{ Aep::DL, Aep::UL, Aep::UR, Aep::DR, Aep::FL, Aep::UF, Aep::FR, Aep::DF, Aep::BL, Aep::UB, Aep::BR, Aep::DB }, // LU
		{ Aep::DF, Aep::UF, Aep::UB, Aep::DB, Aep::FR, Aep::UR, Aep::BR, Aep::DR, Aep::FL, Aep::UL, Aep::BL, Aep::DL }, // LF
		{ Aep::DR, Aep::UR, Aep::UL, Aep::DL, Aep::BR, Aep::UB, Aep::BL, Aep::DB, Aep::FR, Aep::UF, Aep::FL, Aep::DF }, // LD
		{ Aep::DB, Aep::UB, Aep::UF, Aep::DF, Aep::BL, Aep::UL, Aep::FL, Aep::DL, Aep::BR, Aep::UR, Aep::FR, Aep::DR }  // LB
	};	

	const Edd c_Edges::edd_OrientationPOS[24][12][12] = // Edges directions for positive orientation
	{
		// Spin UF
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin UR
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin UB
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin UL
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin DF
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin DR
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin DB
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin DL
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin FU
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }  // DR
		},
		// Spin FR
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin FD
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }  // DR
		},
		// Spin FL
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin BU
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }  // DR
		},
		// Spin BR
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin BD
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }  // DR
		},
		// Spin BL
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DR
		},
		// Spin RU
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }  // DR
		},
		// Spin RF
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin RD
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }  // DR
		},
		// Spin RB
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin LU
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }  // DR
		},
		// Spin LF
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		},
		// Spin LD
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // BR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // UR
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }, // DL
			 { Edd::NEG, Edd::NEG, Edd::NEG, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS, Edd::NEG, Edd::POS }  // DR
		},
		// Spin LB
		{ //      FR        FL        BL        BR        UF        UL        UB        UR        DF        DL        DB        DR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // FL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // BR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // UR
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DF
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DL
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }, // DB
			 { Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS, Edd::POS }  // DR
		}
	};

	void c_Edges::e_U() // Movement U
	{
		S64 buffer = GetStatusPos_UF();
		PutStatusPos_UF(GetStatusPos_UR());
		PutStatusPos_UR(GetStatusPos_UB());
		PutStatusPos_UB(GetStatusPos_UL());
		PutStatusPos_UL(buffer);
	
		buffer = GetStatusDir_UF();
		PutStatusDir_UF(GetStatusDir_UR());
		PutStatusDir_UR(GetStatusDir_UB());
		PutStatusDir_UB(GetStatusDir_UL());
		PutStatusDir_UL(buffer);
	}
	
	void c_Edges::e_U2() // Movement U2
	{
		S64 buffer = GetStatusPos_UF();
		PutStatusPos_UF(GetStatusPos_UB());
		PutStatusPos_UB(buffer);
	
		buffer = GetStatusDir_UF();
		PutStatusDir_UF(GetStatusDir_UB());
		PutStatusDir_UB(buffer);
	
		buffer = GetStatusPos_UL();
		PutStatusPos_UL(GetStatusPos_UR());
		PutStatusPos_UR(buffer);
	
		buffer = GetStatusDir_UL();
		PutStatusDir_UL(GetStatusDir_UR());
		PutStatusDir_UR(buffer);
	}
	
	void c_Edges::e_Up() // Movement U'
	{
		S64 buffer = GetStatusPos_UF();
		PutStatusPos_UF(GetStatusPos_UL());
		PutStatusPos_UL(GetStatusPos_UB());
		PutStatusPos_UB(GetStatusPos_UR());
		PutStatusPos_UR(buffer);
	
		buffer = GetStatusDir_UF();
		PutStatusDir_UF(GetStatusDir_UL());
		PutStatusDir_UL(GetStatusDir_UB());
		PutStatusDir_UB(GetStatusDir_UR());
		PutStatusDir_UR(buffer);
	}
	
	void c_Edges::e_D() // Movement D
	{
		S64 buffer = GetStatusPos_DF();
		PutStatusPos_DF(GetStatusPos_DL());
		PutStatusPos_DL(GetStatusPos_DB());
		PutStatusPos_DB(GetStatusPos_DR());
		PutStatusPos_DR(buffer);
	
		buffer = GetStatusDir_DF();
		PutStatusDir_DF(GetStatusDir_DL());
		PutStatusDir_DL(GetStatusDir_DB());
		PutStatusDir_DB(GetStatusDir_DR());
		PutStatusDir_DR(buffer);
	}
	
	void c_Edges::e_D2() // Movement D2
	{
		S64 buffer = GetStatusPos_DF();
		PutStatusPos_DF(GetStatusPos_DB());
		PutStatusPos_DB(buffer);
	
		buffer = GetStatusDir_DF();
		PutStatusDir_DF(GetStatusDir_DB());
		PutStatusDir_DB(buffer);
	
		buffer = GetStatusPos_DL();
		PutStatusPos_DL(GetStatusPos_DR());
		PutStatusPos_DR(buffer);
	
		buffer = GetStatusDir_DL();
		PutStatusDir_DL(GetStatusDir_DR());
		PutStatusDir_DR(buffer);
	}
	
	void c_Edges::e_Dp() // Movement D'
	{
		S64 buffer = GetStatusPos_DF();
		PutStatusPos_DF(GetStatusPos_DR());
		PutStatusPos_DR(GetStatusPos_DB());
		PutStatusPos_DB(GetStatusPos_DL());
		PutStatusPos_DL(buffer);
	
		buffer = GetStatusDir_DF();
		PutStatusDir_DF(GetStatusDir_DR());
		PutStatusDir_DR(GetStatusDir_DB());
		PutStatusDir_DB(GetStatusDir_DL());
		PutStatusDir_DL(buffer);
	}
	
	void c_Edges::e_F() // Movement F
	{
		S64 buffer = GetStatusPos_FR();
		PutStatusPos_FR(GetStatusPos_UF());
		PutStatusPos_UF(GetStatusPos_FL());
		PutStatusPos_FL(GetStatusPos_DF());
		PutStatusPos_DF(buffer);
	
		buffer = GetStatusDir_FR();
		PutStatusDir_FR(GetStatusDir_UF() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_UF(GetStatusDir_FL() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_FL(GetStatusDir_DF() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_DF(buffer == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
	
	}
	
	void c_Edges::e_F2() // Movement F2
	{
		S64 buffer = GetStatusPos_FR();
		PutStatusPos_FR(GetStatusPos_FL());
		PutStatusPos_FL(buffer);
	
		buffer = GetStatusDir_FR();
		PutStatusDir_FR(GetStatusDir_FL());
		PutStatusDir_FL(buffer);
	
		buffer = GetStatusPos_UF();
		PutStatusPos_UF(GetStatusPos_DF());
		PutStatusPos_DF(buffer);
	
		buffer = GetStatusDir_UF();
		PutStatusDir_UF(GetStatusDir_DF());
		PutStatusDir_DF(buffer);
	}
	
	void c_Edges::e_Fp() // Movement F'
	{
		S64 buffer = GetStatusPos_FR();
		PutStatusPos_FR(GetStatusPos_DF());
		PutStatusPos_DF(GetStatusPos_FL());
		PutStatusPos_FL(GetStatusPos_UF());
		PutStatusPos_UF(buffer);
	
		buffer = GetStatusDir_FR();
		PutStatusDir_FR(GetStatusDir_DF() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_DF(GetStatusDir_FL() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_FL(GetStatusDir_UF() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_UF(buffer == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
	}
	
	void c_Edges::e_B() // Movement B
	{
		S64 buffer = GetStatusPos_BL();
		PutStatusPos_BL(GetStatusPos_UB());
		PutStatusPos_UB(GetStatusPos_BR());
		PutStatusPos_BR(GetStatusPos_DB());
		PutStatusPos_DB(buffer);
	
		buffer = GetStatusDir_BL();
		PutStatusDir_BL(GetStatusDir_UB() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_UB(GetStatusDir_BR() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_BR(GetStatusDir_DB() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_DB(buffer == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
	}

	void c_Edges::e_B2() // Movement B2
	{
		S64 buffer = GetStatusPos_BL();
		PutStatusPos_BL(GetStatusPos_BR());
		PutStatusPos_BR(buffer);
	
		buffer = GetStatusDir_BL();
		PutStatusDir_BL(GetStatusDir_BR());
		PutStatusDir_BR(buffer);
	
		buffer = GetStatusPos_UB();
		PutStatusPos_UB(GetStatusPos_DB());
		PutStatusPos_DB(buffer);
	
		buffer = GetStatusDir_UB();
		PutStatusDir_UB(GetStatusDir_DB());
		PutStatusDir_DB(buffer);
	}
	
	void c_Edges::e_Bp() // Movement B'
	{
		S64 buffer = GetStatusPos_BR();
		PutStatusPos_BR(GetStatusPos_UB());
		PutStatusPos_UB(GetStatusPos_BL());
		PutStatusPos_BL(GetStatusPos_DB());
		PutStatusPos_DB(buffer);
	
		buffer = GetStatusDir_BR();
		PutStatusDir_BR(GetStatusDir_UB() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_UB(GetStatusDir_BL() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_BL(GetStatusDir_DB() == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
		PutStatusDir_DB(buffer == EDGE_DIR_POS ? EDGE_DIR_NEG : EDGE_DIR_POS);
	}
	
	void c_Edges::e_R() // Movement R
	{
		S64 buffer = GetStatusPos_FR();
		PutStatusPos_FR(GetStatusPos_DR());
		PutStatusPos_DR(GetStatusPos_BR());
		PutStatusPos_BR(GetStatusPos_UR());
		PutStatusPos_UR(buffer);
	
		buffer = GetStatusDir_FR();
		PutStatusDir_FR(GetStatusDir_DR());
		PutStatusDir_DR(GetStatusDir_BR());
		PutStatusDir_BR(GetStatusDir_UR());
		PutStatusDir_UR(buffer);
	}
	
	void c_Edges::e_R2() // Movement R2
	{
		S64 buffer = GetStatusPos_FR();
		PutStatusPos_FR(GetStatusPos_BR());
		PutStatusPos_BR(buffer);
	
		buffer = GetStatusDir_FR();
		PutStatusDir_FR(GetStatusDir_BR());
		PutStatusDir_BR(buffer);
	
		buffer = GetStatusPos_UR();
		PutStatusPos_UR(GetStatusPos_DR());
		PutStatusPos_DR(buffer);
	
		buffer = GetStatusDir_UR();
		PutStatusDir_UR(GetStatusDir_DR());
		PutStatusDir_DR(buffer);
	}
	
	void c_Edges::e_Rp() // Movement R'
	{
		S64 buffer = GetStatusPos_FR();
		PutStatusPos_FR(GetStatusPos_UR());
		PutStatusPos_UR(GetStatusPos_BR());
		PutStatusPos_BR(GetStatusPos_DR());
		PutStatusPos_DR(buffer);
	
		buffer = GetStatusDir_FR();
		PutStatusDir_FR(GetStatusDir_UR());
		PutStatusDir_UR(GetStatusDir_BR());
		PutStatusDir_BR(GetStatusDir_DR());
		PutStatusDir_DR(buffer);
	}
	
	void c_Edges::e_L() // Movement L
	{
		S64 buffer = GetStatusPos_FL();
		PutStatusPos_FL(GetStatusPos_UL());
		PutStatusPos_UL(GetStatusPos_BL());
		PutStatusPos_BL(GetStatusPos_DL());
		PutStatusPos_DL(buffer);
	
		buffer = GetStatusDir_FL();
		PutStatusDir_FL(GetStatusDir_UL());
		PutStatusDir_UL(GetStatusDir_BL());
		PutStatusDir_BL(GetStatusDir_DL());
		PutStatusDir_DL(buffer);
	}
	
	void c_Edges::e_L2() // Movement L2
	{
		S64 buffer = GetStatusPos_FL();
		PutStatusPos_FL(GetStatusPos_BL());
		PutStatusPos_BL(buffer);
	
		buffer = GetStatusDir_FL();
		PutStatusDir_FL(GetStatusDir_BL());
		PutStatusDir_BL(buffer);
	
		buffer = GetStatusPos_UL();
		PutStatusPos_UL(GetStatusPos_DL());
		PutStatusPos_DL(buffer);
	
		buffer = GetStatusDir_UL();
		PutStatusDir_UL(GetStatusDir_DL());
		PutStatusDir_DL(buffer);
	}
	
	void c_Edges::e_Lp() // Movement L'
	{
		S64 buffer = GetStatusPos_FL();
		PutStatusPos_FL(GetStatusPos_DL());
		PutStatusPos_DL(GetStatusPos_BL());
		PutStatusPos_BL(GetStatusPos_UL());
		PutStatusPos_UL(buffer);
	
		buffer = GetStatusDir_FL();
		PutStatusDir_FL(GetStatusDir_DL());
		PutStatusDir_DL(GetStatusDir_BL());
		PutStatusDir_BL(GetStatusDir_UL());
		PutStatusDir_UL(buffer);
	}
	
	// Invert edges direction status bit
	void c_Edges::InvertStatusDir(const Edp ep)
	{
		switch (ep)
		{
		case Edp::FR: InvertStatusDir_FR(); return;	
		case Edp::FL: InvertStatusDir_FL(); return;
		case Edp::BL: InvertStatusDir_BL(); return;
		case Edp::BR: InvertStatusDir_BR(); return;
		case Edp::UF: InvertStatusDir_UF(); return;
		case Edp::UL: InvertStatusDir_UL(); return;
		case Edp::UB: InvertStatusDir_UB(); return;
		case Edp::UR: InvertStatusDir_UR(); return;
		case Edp::DF: InvertStatusDir_DF(); return;
		case Edp::DL: InvertStatusDir_DL(); return;
		case Edp::DB: InvertStatusDir_DB(); return;
		case Edp::DR: InvertStatusDir_DR(); return;
		}
	}
	
	// Get mask for check if given edge position is in solve condition
	S64 c_Edges::GetMaskSolvedEdge(const Edp e)
	{
		switch (e)
		{
        case Edp::FR: return EMASK_FR;
        case Edp::FL: return EMASK_FL;
        case Edp::BL: return EMASK_BL;
        case Edp::BR: return EMASK_BR;
        case Edp::UF: return EMASK_UF;
        case Edp::UL: return EMASK_UL;
        case Edp::UB: return EMASK_UB;
        case Edp::UR: return EMASK_UR;
        case Edp::DF: return EMASK_DF;
        case Edp::DL: return EMASK_DL;
        case Edp::DB: return EMASK_DB;
        case Edp::DR: return EMASK_DR;
        default: return 0ULL;
		}
	}

    // Get mask for check if edges positions in given list of pieces are in solve conditions
	S64 c_Edges::GetMaskSolvedEdges(const std::vector<Edp>& p_list)
	{
		S64 e_mask = 0ULL;
		for (auto P : p_list) e_mask |= GetMaskSolvedEdge(P);
		return e_mask;
	}
	
	// Get mask for check edge direction in given edge position
    S64 c_Edges::GetMaskEdgeDirection(const Edp ep)
	{
		switch (ep)
		{
        case Edp::FR: return EMASK_DIR_FR;
        case Edp::FL: return EMASK_DIR_FL;
        case Edp::BL: return EMASK_DIR_BL;
        case Edp::BR: return EMASK_DIR_BR;
        case Edp::UF: return EMASK_DIR_UF;
        case Edp::UL: return EMASK_DIR_UL;
        case Edp::UB: return EMASK_DIR_UB;
        case Edp::UR: return EMASK_DIR_UR;
        case Edp::DF: return EMASK_DIR_DF;
        case Edp::DL: return EMASK_DIR_DL;
        case Edp::DB: return EMASK_DIR_DB;
        case Edp::DR: return EMASK_DIR_DR;
        default: return 0ULL;
		}
	}
	
	// Get mask for check edges direction in given edges positions
    S64 c_Edges::GetMaskEdgesDirection(const std::vector<Edp>& p_list)
	{
		S64 e_mask = 0ULL;
		for (auto P : p_list) e_mask |= GetMaskEdgeDirection(P);
		return e_mask;
	}
	
	// Get mask for check edges position (no direction needed)
	S64 c_Edges::GetMaskEdgePosition(const Edp ep)
	{
		switch (ep)
		{
			case Edp::FR: return EMASK_POS_FR;
			case Edp::FL: return EMASK_POS_FL;
			case Edp::BL: return EMASK_POS_BL;
			case Edp::BR: return EMASK_POS_BR;
			case Edp::UF: return EMASK_POS_UF;
			case Edp::UL: return EMASK_POS_UL;
			case Edp::UB: return EMASK_POS_UB;
			case Edp::UR: return EMASK_POS_UR;
			case Edp::DF: return EMASK_POS_DF;
			case Edp::DL: return EMASK_POS_DL;
			case Edp::DB: return EMASK_POS_DB;
			case Edp::DR: return EMASK_POS_DR;
			default: return 0ULL;
		}
	}
	
	// Get mask for check edge position (no direction needed)
	S64 c_Edges::GetMaskEdgesPosition(const std::vector<Edp>& p_list)
	{
		S64 e_mask = 0ull;	
		for (auto P : p_list) e_mask |= GetMaskEdgePosition(P);
		return e_mask;
	}
	
	// Checks the position of the edge in the given position (no direction)
	bool c_Edges::CheckEdgePosition(const Edp pe) const
	{
		switch (pe)
		{
		case Edp::UF: return CheckPosition_UF();
		case Edp::UB: return CheckPosition_UB();
		case Edp::UR: return CheckPosition_UR();
		case Edp::UL: return CheckPosition_UL();

		case Edp::DF: return CheckPosition_DF();
		case Edp::DB: return CheckPosition_DB();
		case Edp::DR: return CheckPosition_DR();
		case Edp::DL: return CheckPosition_DL();

		case Edp::FR: return CheckPosition_FR();
		case Edp::FL: return CheckPosition_FL();
		case Edp::BR: return CheckPosition_BR();
		case Edp::BL: return CheckPosition_BL();

		default: return false;
		}
	}

	// Check if the given edge is in the given layer
	bool c_Edges::IsEdgeInLayer(const Edg P, const Lyr Ly) const
	{
		switch (Ly)
		{
		case Lyr::U: return IsEdgeInLayerU(P);
		case Lyr::D: return IsEdgeInLayerD(P);
		case Lyr::F: return IsEdgeInLayerF(P);
		case Lyr::B: return IsEdgeInLayerB(P);
		case Lyr::R: return IsEdgeInLayerR(P);
		case Lyr::L: return IsEdgeInLayerL(P);
		case Lyr::M: return IsEdgeInLayerM(P);
		case Lyr::E: return IsEdgeInLayerE(P);
		case Lyr::S: return IsEdgeInLayerS(P);
		default: return false;
		}
	}
	
	// Get the edge in the given edge position
	Edg c_Edges::GetEdge(const Edp ep) const
	{
		switch (ep)
		{
		case Edp::UF: return GetEdge_UF();
		case Edp::UB: return GetEdge_UB();
		case Edp::UR: return GetEdge_UR();
		case Edp::UL: return GetEdge_UL();
		case Edp::DF: return GetEdge_DF();
		case Edp::DB: return GetEdge_DB();
		case Edp::DR: return GetEdge_DR();
		case Edp::DL: return GetEdge_DL();
		case Edp::FR: return GetEdge_FR();
		case Edp::FL: return GetEdge_FL();
		case Edp::BR: return GetEdge_BR();
		case Edp::BL: return GetEdge_BL();

		default: return Edg::UF; // Should not happend, avoids a warning
		}
	}

	// Get the current edge position for the given edge
	Edp c_Edges::GetEdgePosition(const Edg E) const
	{
		if (GetEdge_UF() == E) return Edp::UF;
		if (GetEdge_UB() == E) return Edp::UB;
		if (GetEdge_UR() == E) return Edp::UR;
		if (GetEdge_UL() == E) return Edp::UL;
		if (GetEdge_DF() == E) return Edp::DF;
		if (GetEdge_DB() == E) return Edp::DB;
		if (GetEdge_DR() == E) return Edp::DR;
		if (GetEdge_DL() == E) return Edp::DL;
		if (GetEdge_FR() == E) return Edp::FR;
		if (GetEdge_FL() == E) return Edp::FL;
		if (GetEdge_BR() == E) return Edp::BR;
		if (GetEdge_BL() == E) return Edp::BL;

		return static_cast<Edp>(E); // Should not happend, avoid a warning
	}

	// Get the edge direction in the given edge position
	Edd c_Edges::GetEdgeDirection(const Edp ep) const
	{
		switch (ep)
		{
		case Edp::UF: return GetEdgeDir_UF();
		case Edp::UB: return GetEdgeDir_UB();
		case Edp::UR: return GetEdgeDir_UR();
		case Edp::UL: return GetEdgeDir_UL();
		case Edp::DF: return GetEdgeDir_DF();
		case Edp::DB: return GetEdgeDir_DB();
		case Edp::DR: return GetEdgeDir_DR();
		case Edp::DL: return GetEdgeDir_DL();
		case Edp::FR: return GetEdgeDir_FR();
		case Edp::FL: return GetEdgeDir_FL();
		case Edp::BR: return GetEdgeDir_BR();
		case Edp::BL: return GetEdgeDir_BL();

		default: return Edd::POS; // Should not happend, avoids a warning
		}
	}
}
