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

#include "corners.h"

namespace grcube3
{	
	// Array to get the face (color) in the UFR_U sticker position
	const Fce c_Corners::fce_UFR_U[8][3] =
	{ //    UD      FB      RL
	    { Fce::U, Fce::R, Fce::F }, // UFR
	    { Fce::U, Fce::F, Fce::L }, // UFL
	    { Fce::U, Fce::L, Fce::B }, // UBL
	    { Fce::U, Fce::B, Fce::R }, // UBR
	    { Fce::D, Fce::F, Fce::R }, // DFR
	    { Fce::D, Fce::L, Fce::F }, // DFL
	    { Fce::D, Fce::B, Fce::L }, // DBL
	    { Fce::D, Fce::R, Fce::B }  // DBR
	};
	
	// Array to get the face (color) in the UFR_F sticker position
	const Fce c_Corners::fce_UFR_F[8][3] =
	{ //    UD      FB      RL
		{ Fce::F, Fce::U, Fce::R }, // UFR
		{ Fce::L, Fce::U, Fce::F }, // UFL
		{ Fce::B, Fce::U, Fce::L }, // UBL
		{ Fce::R, Fce::U, Fce::B }, // UBR
		{ Fce::R, Fce::D, Fce::F }, // DFR
		{ Fce::F, Fce::D, Fce::L }, // DFL
		{ Fce::L, Fce::D, Fce::B }, // DBL
		{ Fce::B, Fce::D, Fce::R }  // DBR
	};
	
	// Array to get the face (color) in the UFR_R sticker position
	const Fce c_Corners::fce_UFR_R[8][3] =
	{ //    UD      FB      RL
		{ Fce::R, Fce::F, Fce::U }, // UFR
		{ Fce::F, Fce::L, Fce::U }, // UFL
		{ Fce::L, Fce::B, Fce::U }, // UBL
		{ Fce::B, Fce::R, Fce::U }, // UBR
		{ Fce::F, Fce::R, Fce::D }, // DFR
		{ Fce::L, Fce::F, Fce::D }, // DFL
		{ Fce::B, Fce::L, Fce::D }, // DBL
		{ Fce::R, Fce::B, Fce::D }  // DBR
	};
	
	// Array to get the face (color) in the UFL_U sticker position
	const Fce c_Corners::fce_UFL_U[8][3] =
	{ //    UD      FB      RL
		{ Fce::U, Fce::F, Fce::R }, // UFR
		{ Fce::U, Fce::L, Fce::F }, // UFL
		{ Fce::U, Fce::B, Fce::L }, // UBL
		{ Fce::U, Fce::R, Fce::B }, // UBR
		{ Fce::D, Fce::R, Fce::F }, // DFR
		{ Fce::D, Fce::F, Fce::L }, // DFL
		{ Fce::D, Fce::L, Fce::B }, // DBL
		{ Fce::D, Fce::B, Fce::R }  // DBR
	};
	
	// Array to get the face (color) in the UFL_F sticker position
	const Fce c_Corners::fce_UFL_F[8][3] =
	{ //    UD      FB      RL
		{ Fce::R, Fce::U, Fce::F }, // UFR
		{ Fce::F, Fce::U, Fce::L }, // UFL
		{ Fce::L, Fce::U, Fce::B }, // UBL
		{ Fce::B, Fce::U, Fce::R }, // UBR
		{ Fce::F, Fce::D, Fce::R }, // DFR
		{ Fce::L, Fce::D, Fce::F }, // DFL
		{ Fce::B, Fce::D, Fce::L }, // DBL
		{ Fce::R, Fce::D, Fce::B }  // DBR
	};
	
	// Array to get the face (color) in the UFL_L sticker position
	const Fce c_Corners::fce_UFL_L[8][3] =
	{ //    UD      FB      RL
		{ Fce::F, Fce::R, Fce::U }, // UFR
		{ Fce::L, Fce::F, Fce::U }, // UFL
		{ Fce::B, Fce::L, Fce::U }, // UBL
		{ Fce::R, Fce::B, Fce::U }, // UBR
		{ Fce::R, Fce::F, Fce::D }, // DFR
		{ Fce::F, Fce::L, Fce::D }, // DFL
		{ Fce::L, Fce::B, Fce::D }, // DBL
		{ Fce::B, Fce::R, Fce::D }  // DBR
	};
	
	// Array to get the sticker in the UFR_U sticker position
	const Stc c_Corners::stc_UFR_U[8][3] =
	{ //      UD          FB           RL
	    { Stc::UFR_U, Stc::UFR_R, Stc::UFR_F }, // UFR
	    { Stc::UFL_U, Stc::UFL_F, Stc::UFL_L }, // UFL
	    { Stc::UBL_U, Stc::UBL_L, Stc::UBL_B }, // UBL
	    { Stc::UBR_U, Stc::UBR_B, Stc::UBR_R }, // UBR
	    { Stc::DFR_D, Stc::DFR_F, Stc::DFR_R }, // DFR
	    { Stc::DFL_D, Stc::DFL_L, Stc::DFL_F }, // DFL
	    { Stc::DBL_D, Stc::DBL_B, Stc::DBL_L }, // DBL
	    { Stc::DBR_D, Stc::DBR_R, Stc::DBR_B }  // DBR
	};
	
	// Array to get the sticker in the UFR_F sticker position
	const Stc c_Corners::stc_UFR_F[8][3] =
	{ //      UD          FB          RL
		{ Stc::UFR_F, Stc::UFR_U, Stc::UFR_R }, // UFR
		{ Stc::UFL_L, Stc::UFL_U, Stc::UFL_F }, // UFL
		{ Stc::UBL_B, Stc::UBL_U, Stc::UBL_L }, // UBL
		{ Stc::UBR_R, Stc::UBR_U, Stc::UBR_B }, // UBR
		{ Stc::DFR_R, Stc::DFR_D, Stc::DFR_F }, // DFR
		{ Stc::DFL_F, Stc::DFL_D, Stc::DFL_L }, // DFL
		{ Stc::DBL_L, Stc::DBL_D, Stc::DBL_B }, // DBL
		{ Stc::DBR_B, Stc::DBR_D, Stc::DBR_R }  // DBR
	};
	
	// Array to get the sticker in the UFR_R sticker position
	const Stc c_Corners::stc_UFR_R[8][3] =
	{ //      UD          FB          RL
		{ Stc::UFR_R, Stc::UFR_F, Stc::UFR_U }, // UFR
		{ Stc::UFL_F, Stc::UFL_L, Stc::UFL_U }, // UFL
		{ Stc::UBL_L, Stc::UBL_B, Stc::UBL_U }, // UBL
		{ Stc::UBR_B, Stc::UBR_R, Stc::UBR_U }, // UBR
		{ Stc::DFR_F, Stc::DFR_R, Stc::DFR_D }, // DFR
		{ Stc::DFL_L, Stc::DFL_F, Stc::DFL_D }, // DFL
		{ Stc::DBL_B, Stc::DBL_L, Stc::DBL_D }, // DBL
		{ Stc::DBR_R, Stc::DBR_B, Stc::DBR_D }  // DBR
	};
	
	// Array to get the sticker in the UFL_U sticker position
	const Stc c_Corners::stc_UFL_U[8][3] =
	{ //      UD          FB          RL
		{ Stc::UFR_U, Stc::UFR_F, Stc::UFR_R }, // UFR
		{ Stc::UFL_U, Stc::UFL_L, Stc::UFL_F }, // UFL
		{ Stc::UBL_U, Stc::UBL_B, Stc::UBL_L }, // UBL
		{ Stc::UBR_U, Stc::UBR_R, Stc::UBR_B }, // UBR
		{ Stc::DFR_D, Stc::DFR_R, Stc::DFR_F }, // DFR
		{ Stc::DFL_D, Stc::DFL_F, Stc::DFL_L }, // DFL
		{ Stc::DBL_D, Stc::DBL_L, Stc::DBL_B }, // DBL
		{ Stc::DBR_D, Stc::DBR_B, Stc::DBR_R }  // DBR
	};
	
	// Array to get the sticker in the UFL_F sticker position
	const Stc c_Corners::stc_UFL_F[8][3] =
	{ //      UD          FB          RL
		{ Stc::UFR_R, Stc::UFR_U, Stc::UFR_F }, // UFR
		{ Stc::UFL_F, Stc::UFL_U, Stc::UFL_L }, // UFL
		{ Stc::UBL_L, Stc::UBL_U, Stc::UBL_B }, // UBL
		{ Stc::UBR_B, Stc::UBR_U, Stc::UBR_R }, // UBR
		{ Stc::DFR_F, Stc::DFR_D, Stc::DFR_R }, // DFR
		{ Stc::DFL_L, Stc::DFL_D, Stc::DFL_F }, // DFL
		{ Stc::DBL_B, Stc::DBL_D, Stc::DBL_L }, // DBL
		{ Stc::DBR_R, Stc::DBR_D, Stc::DBR_B }  // DBR
	};
	
	// Array to get the sticker in the UFL_L sticker position
	const Stc c_Corners::stc_UFL_L[8][3] =
	{ //      UD          FB          RL
		{ Stc::UFR_F, Stc::UFR_R, Stc::UFR_U }, // UFR
		{ Stc::UFL_L, Stc::UFL_F, Stc::UFL_U }, // UFL
		{ Stc::UBL_B, Stc::UBL_L, Stc::UBL_U }, // UBL
		{ Stc::UBR_R, Stc::UBR_B, Stc::UBR_U }, // UBR
		{ Stc::DFR_R, Stc::DFR_F, Stc::DFR_D }, // DFR
		{ Stc::DFL_F, Stc::DFL_L, Stc::DFL_D }, // DFL
		{ Stc::DBL_L, Stc::DBL_B, Stc::DBL_D }, // DBL
		{ Stc::DBR_B, Stc::DBR_R, Stc::DBR_D }  // DBR
	};

    // Piece positions from spin and absolut corner positions
    const Cnp c_Corners::cnp_Absolut[24][8] =
    { //     UFR       UFL       UBL       UBR       DFR       DFL       DBL       DBR
        { Cnp::UFR, Cnp::UFL, Cnp::UBL, Cnp::UBR, Cnp::DFR, Cnp::DFL, Cnp::DBL, Cnp::DBR }, // UF
        { Cnp::UBR, Cnp::UFR, Cnp::UFL, Cnp::UBL, Cnp::DBR, Cnp::DFR, Cnp::DFL, Cnp::DBL }, // UR
        { Cnp::UBL, Cnp::UBR, Cnp::UFR, Cnp::UFL, Cnp::DBL, Cnp::DBR, Cnp::DFR, Cnp::DFL }, // UB
        { Cnp::UFL, Cnp::UBL, Cnp::UBR, Cnp::UFR, Cnp::DFL, Cnp::DBL, Cnp::DBR, Cnp::DFR }, // UL
        { Cnp::DFL, Cnp::DFR, Cnp::DBR, Cnp::DBL, Cnp::UFL, Cnp::UFR, Cnp::UBR, Cnp::UBL }, // DF
        { Cnp::DFR, Cnp::DBR, Cnp::DBL, Cnp::DFL, Cnp::UFR, Cnp::UBR, Cnp::UBL, Cnp::UFL }, // DR
        { Cnp::DBR, Cnp::DBL, Cnp::DFL, Cnp::DFR, Cnp::UBR, Cnp::UBL, Cnp::UFL, Cnp::UFR }, // DB
        { Cnp::DBL, Cnp::DFL, Cnp::DFR, Cnp::DBR, Cnp::UBL, Cnp::UFL, Cnp::UFR, Cnp::UBR }, // DL
        { Cnp::UFL, Cnp::UFR, Cnp::DFR, Cnp::DFL, Cnp::UBL, Cnp::UBR, Cnp::DBR, Cnp::DBL }, // FU
        { Cnp::UFR, Cnp::DFR, Cnp::DFL, Cnp::UFL, Cnp::UBR, Cnp::DBR, Cnp::DBL, Cnp::UBL }, // FR
        { Cnp::DFR, Cnp::DFL, Cnp::UFL, Cnp::UFR, Cnp::DBR, Cnp::DBL, Cnp::UBL, Cnp::UBR }, // FD
        { Cnp::DFL, Cnp::UFL, Cnp::UFR, Cnp::DFR, Cnp::DBL, Cnp::UBL, Cnp::UBR, Cnp::DBR }, // FL
        { Cnp::UBR, Cnp::UBL, Cnp::DBL, Cnp::DBR, Cnp::UFR, Cnp::UFL, Cnp::DFL, Cnp::DFR }, // BU
        { Cnp::DBR, Cnp::UBR, Cnp::UBL, Cnp::DBL, Cnp::DFR, Cnp::UFR, Cnp::UFL, Cnp::DFL }, // BR
        { Cnp::DBL, Cnp::DBR, Cnp::UBR, Cnp::UBL, Cnp::DFL, Cnp::DFR, Cnp::UFR, Cnp::UFL }, // BD
        { Cnp::UBL, Cnp::DBL, Cnp::DBR, Cnp::UBR, Cnp::UFL, Cnp::DFL, Cnp::DFR, Cnp::UFR }, // BL
        { Cnp::UFR, Cnp::UBR, Cnp::DBR, Cnp::DFR, Cnp::UFL, Cnp::UBL, Cnp::DBL, Cnp::DFL }, // RU
        { Cnp::DFR, Cnp::UFR, Cnp::UBR, Cnp::DBR, Cnp::DFL, Cnp::UFL, Cnp::UBL, Cnp::DBL }, // RF
        { Cnp::DBR, Cnp::DFR, Cnp::UFR, Cnp::UBR, Cnp::DBL, Cnp::DFL, Cnp::UFL, Cnp::UBL }, // RD
        { Cnp::UBR, Cnp::DBR, Cnp::DFR, Cnp::UFR, Cnp::UBL, Cnp::DBL, Cnp::DFL, Cnp::UFL }, // RB
        { Cnp::UBL, Cnp::UFL, Cnp::DFL, Cnp::DBL, Cnp::UBR, Cnp::UFR, Cnp::DFR, Cnp::DBR }, // LU
        { Cnp::UFL, Cnp::DFL, Cnp::DBL, Cnp::UBL, Cnp::UFR, Cnp::DFR, Cnp::DBR, Cnp::UBR }, // LF
        { Cnp::DFL, Cnp::DBL, Cnp::UBL, Cnp::UFL, Cnp::DFR, Cnp::DBR, Cnp::UBR, Cnp::UFR }, // LD
        { Cnp::DBL, Cnp::UBL, Cnp::UFL, Cnp::DFL, Cnp::DBR, Cnp::UBR, Cnp::UFR, Cnp::DFR }  // LB
    };

    // Absolute corner positions from spin and corner positions
    const Acp c_Corners::acp_Positions[24][8] =
    { //     UFR       UFL       UBL       UBR       DFR       DFL       DBL       DBR
        { Acp::UFR, Acp::UFL, Acp::UBL, Acp::UBR, Acp::DFR, Acp::DFL, Acp::DBL, Acp::DBR }, // UF
        { Acp::UFL, Acp::UBL, Acp::UBR, Acp::UFR, Acp::DFL, Acp::DBL, Acp::DBR, Acp::DFR }, // UR
        { Acp::UBL, Acp::UBR, Acp::UFR, Acp::UFL, Acp::DBL, Acp::DBR, Acp::DFR, Acp::DFL }, // UB
        { Acp::UBR, Acp::UFR, Acp::UFL, Acp::UBL, Acp::DBR, Acp::DFR, Acp::DFL, Acp::DBL }, // UL
        { Acp::DFL, Acp::DFR, Acp::DBR, Acp::DBL, Acp::UFL, Acp::UFR, Acp::UBR, Acp::UBL }, // DF
        { Acp::DFR, Acp::DBR, Acp::DBL, Acp::DFL, Acp::UFR, Acp::UBR, Acp::UBL, Acp::UFL }, // DR
        { Acp::DBR, Acp::DBL, Acp::DFL, Acp::DFR, Acp::UBR, Acp::UBL, Acp::UFL, Acp::UFR }, // DB
        { Acp::DBL, Acp::DFL, Acp::DFR, Acp::DBR, Acp::UBL, Acp::UFL, Acp::UFR, Acp::UBR }, // DL
        { Acp::UFL, Acp::UFR, Acp::DFR, Acp::DFL, Acp::UBL, Acp::UBR, Acp::DBR, Acp::DBL }, // FU
        { Acp::UFR, Acp::UBR, Acp::DBR, Acp::DFR, Acp::UFL, Acp::UBL, Acp::DBL, Acp::DFL }, // FR
        { Acp::UBR, Acp::UBL, Acp::DBL, Acp::DBR, Acp::UFR, Acp::UFL, Acp::DFL, Acp::DFR }, // FD
        { Acp::UBL, Acp::UFL, Acp::DFL, Acp::DBL, Acp::UBR, Acp::UFR, Acp::DFR, Acp::DBR }, // FL
        { Acp::DFR, Acp::DFL, Acp::UFL, Acp::UFR, Acp::DBR, Acp::DBL, Acp::UBL, Acp::UBR }, // BU
        { Acp::DFL, Acp::DBL, Acp::UBL, Acp::UFL, Acp::DFR, Acp::DBR, Acp::UBR, Acp::UFR }, // BR
        { Acp::DBL, Acp::DBR, Acp::UBR, Acp::UBL, Acp::DFL, Acp::DFR, Acp::UFR, Acp::UFL }, // BD
        { Acp::DBR, Acp::DFR, Acp::UFR, Acp::UBR, Acp::DBL, Acp::DFL, Acp::UFL, Acp::UBL }, // BL
        { Acp::UFR, Acp::DFR, Acp::DFL, Acp::UFL, Acp::UBR, Acp::DBR, Acp::DBL, Acp::UBL }, // RU
        { Acp::UFL, Acp::DFL, Acp::DBL, Acp::UBL, Acp::UFR, Acp::DFR, Acp::DBR, Acp::UBR }, // RF
        { Acp::UBL, Acp::DBL, Acp::DBR, Acp::UBR, Acp::UFL, Acp::DFL, Acp::DFR, Acp::UFR }, // RD
        { Acp::UBR, Acp::DBR, Acp::DFR, Acp::UFR, Acp::UBL, Acp::DBL, Acp::DFL, Acp::UFL }, // RB
        { Acp::DFL, Acp::UFL, Acp::UFR, Acp::DFR, Acp::DBL, Acp::UBL, Acp::UBR, Acp::DBR }, // LU
        { Acp::DFR, Acp::UFR, Acp::UBR, Acp::DBR, Acp::DFL, Acp::UFL, Acp::UBL, Acp::DBL }, // LF
        { Acp::DBR, Acp::UBR, Acp::UBL, Acp::DBL, Acp::DFR, Acp::UFR, Acp::UFL, Acp::DFL }, // LD
        { Acp::DBL, Acp::UBL, Acp::UFL, Acp::DFL, Acp::DBR, Acp::UBR, Acp::UFR, Acp::DFR }  // LB
    };

    const Cnd c_Corners::cnd_OrientationUD[24][8][8] = // Corners directions for UD orientation
    {
        // Spin UF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UF
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UB
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UR
        },
        // Spin UR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UF
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UB
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UR
        },
        // Spin UB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UF
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UB
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UR
        },
        // Spin UL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UF
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UB
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UR
        },
        // Spin DF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UF
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UB
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UR
        },
        // Spin DR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UF
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UB
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UR
        },
        // Spin DB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UF
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UB
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UR
        },
        // Spin DL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // FL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UF
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UL
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UB
             { Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, Cnd::UD, }, // UR
        },
        // Spin FU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UF
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UB
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UR
        },
        // Spin FR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UF
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UB
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UR
        },
        // Spin FD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UF
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UB
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UR
        },
        // Spin FL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UF
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UB
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UR
        },
        // Spin BU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UF
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UB
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UR
        },
        // Spin BR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UF
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UB
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UR
        },
        // Spin BD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UF
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UB
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UR
        },
        // Spin BL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UF
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UB
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UR
        },
        // Spin RU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UF
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UB
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UR
        },
        // Spin RF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UF
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UB
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UR
        },
        // Spin RD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UF
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UB
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UR
        },
        // Spin RB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UF
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UB
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UR
        },
        // Spin LU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UF
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UB
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UR
        },
        // Spin LF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UF
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UB
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UR
        },
        // Spin LD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UF
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UB
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UR
        },
        // Spin LB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UF
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UB
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UR
        },
    };

    const Cnd c_Corners::cnd_OrientationFB[24][8][8] = // Corners directions for FB orientation
    {
        // Spin UF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UF
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UB
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UR
        },
        // Spin UR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UF
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UB
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UR
        },
        // Spin UB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UF
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UB
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UR
        },
        // Spin UL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UF
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UB
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UR
        },
        // Spin DF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UF
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UB
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UR
        },
        // Spin DR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UF
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UB
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UR
        },
        // Spin DB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UF
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UB
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UR
        },
        // Spin DL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UF
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UB
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UR
        },
        // Spin FU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UF
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UB
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UR
        },
        // Spin FR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UF
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UB
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UR
        },
        // Spin FD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UF
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UB
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UR
        },
        // Spin FL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UF
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UB
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UR
        },
        // Spin BU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UF
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UB
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UR
        },
        // Spin BR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UF
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UB
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UR
        },
        // Spin BD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UF
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UB
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UR
        },
        // Spin BL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UF
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UB
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UR
        },
        // Spin RU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UF
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UB
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UR
        },
        // Spin RF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UF
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UB
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UR
        },
        // Spin RD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UF
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UB
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UR
        },
        // Spin RB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UF
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UB
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UR
        },
        // Spin LU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UF
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UB
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UR
        },
        // Spin LF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UF
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UB
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UR
        },
        // Spin LD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UF
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UB
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UR
        },
        // Spin LB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UF
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UB
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UR
        },
    };

    const Cnd c_Corners::cnd_OrientationRL[24][8][8] = // Corners directions for RL orientation
    {
        // Spin UF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UF
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UB
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UR
        },
        // Spin UR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UF
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UB
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UR
        },
        // Spin UB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UF
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UB
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UR
        },
        // Spin UL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UF
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UB
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UR
        },
        // Spin DF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UF
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UB
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UR
        },
        // Spin DR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UF
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UB
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UR
        },
        // Spin DB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UF
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UB
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UR
        },
        // Spin DL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // FR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // FL
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // BL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UF
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UL
             { Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, }, // UB
             { Cnd::RL, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::FB, Cnd::RL, Cnd::FB, Cnd::RL, }, // UR
        },
        // Spin FU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UF
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UB
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UR
        },
        // Spin FR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UF
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UB
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UR
        },
        // Spin FD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UF
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UB
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UR
        },
        // Spin FL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UF
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UB
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UR
        },
        // Spin BU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UF
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UB
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UR
        },
        // Spin BR
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UF
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UB
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UR
        },
        // Spin BD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // FR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // FL
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // BL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UF
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UL
             { Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, }, // UB
             { Cnd::RL, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::UD, Cnd::RL, Cnd::UD, Cnd::RL, }, // UR
        },
        // Spin BL
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // FL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // BR
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UF
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UL
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UB
             { Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, Cnd::FB, }, // UR
        },
        // Spin RU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UF
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UB
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UR
        },
        // Spin RF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UF
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UB
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UR
        },
        // Spin RD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UF
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UB
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UR
        },
        // Spin RB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UF
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UB
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UR
        },
        // Spin LU
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UF
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UB
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UR
        },
        // Spin LF
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UF
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UB
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UR
        },
        // Spin LD
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // FR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // FL
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // BL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // BR
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UF
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UL
             { Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, }, // UB
             { Cnd::FB, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::UD, Cnd::FB, Cnd::UD, Cnd::FB, }, // UR
        },
        // Spin LB
        { //     UFR      UFL      UBL      UBR      DFR      DFL      DBL      DBR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // FL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // BR
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UF
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UL
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UB
             { Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, Cnd::RL, }, // UR
        },
    };

	void c_Corners::c_U() // U
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_UBR());
		PutStatus_UBR(GetStatus_UBL());
		PutStatus_UBL(GetStatus_UFL());
		PutStatus_UFL(buffer);

		buffer = GetStatusDir_UFR();
		if (buffer == CORNER_DIR_FB) PutStatusDir_UFR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UFR(CORNER_DIR_FB);
		
		buffer = GetStatusDir_UFL();
		if (buffer == CORNER_DIR_FB) PutStatusDir_UFL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UFL(CORNER_DIR_FB);
	
		buffer = GetStatusDir_UBL();
		if (buffer == CORNER_DIR_FB) PutStatusDir_UBL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UBL(CORNER_DIR_FB);
		
		buffer = GetStatusDir_UBR();
		if (buffer == CORNER_DIR_FB) PutStatusDir_UBR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UBR(CORNER_DIR_FB);
	}
	
	void c_Corners::c_U2() // U2
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_UBL());
		PutStatus_UBL(buffer);
	
		buffer = GetStatus_UFL();
		PutStatus_UFL(GetStatus_UBR());
		PutStatus_UBR(buffer);
	}
	
	void c_Corners::c_Up() // U'
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_UFL());
		PutStatus_UFL(GetStatus_UBL());
		PutStatus_UBL(GetStatus_UBR());
		PutStatus_UBR(buffer);

		buffer = GetStatusDir_UFR();
		if (buffer == CORNER_DIR_FB) PutStatusDir_UFR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UFR(CORNER_DIR_FB);
		
		buffer = GetStatusDir_UFL();
		if (buffer == CORNER_DIR_FB) PutStatusDir_UFL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UFL(CORNER_DIR_FB);
	
		buffer = GetStatusDir_UBL();
		if (buffer == CORNER_DIR_FB) PutStatusDir_UBL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UBL(CORNER_DIR_FB);
		
		buffer = GetStatusDir_UBR();
		if (buffer == CORNER_DIR_FB) PutStatusDir_UBR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UBR(CORNER_DIR_FB);
	}
	
	void c_Corners::c_D() // D
	{
		S64 buffer = GetStatus_DFR();
		PutStatus_DFR(GetStatus_DFL());
		PutStatus_DFL(GetStatus_DBL());
		PutStatus_DBL(GetStatus_DBR());
		PutStatus_DBR(buffer);

		buffer = GetStatusDir_DFR();
		if (buffer == CORNER_DIR_FB) PutStatusDir_DFR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DFR(CORNER_DIR_FB);
		
		buffer = GetStatusDir_DFL();
		if (buffer == CORNER_DIR_FB) PutStatusDir_DFL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DFL(CORNER_DIR_FB);
	
		buffer = GetStatusDir_DBL();
		if (buffer == CORNER_DIR_FB) PutStatusDir_DBL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DBL(CORNER_DIR_FB);
		
		buffer = GetStatusDir_DBR();
		if (buffer == CORNER_DIR_FB) PutStatusDir_DBR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DBR(CORNER_DIR_FB);
	}
	
	void c_Corners::c_D2() // D2
	{
		S64 buffer = GetStatus_DFR();
		PutStatus_DFR(GetStatus_DBL());
		PutStatus_DBL(buffer);
	
		buffer = GetStatus_DFL();
		PutStatus_DFL(GetStatus_DBR());
		PutStatus_DBR(buffer);
	}
	
	void c_Corners::c_Dp() // D'
	{
		S64 buffer = GetStatus_DFR();
		PutStatus_DFR(GetStatus_DBR());
		PutStatus_DBR(GetStatus_DBL());
		PutStatus_DBL(GetStatus_DFL());
		PutStatus_DFL(buffer);

		buffer = GetStatusDir_DFR();
		if (buffer == CORNER_DIR_FB) PutStatusDir_DFR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DFR(CORNER_DIR_FB);
		
		buffer = GetStatusDir_DFL();
		if (buffer == CORNER_DIR_FB) PutStatusDir_DFL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DFL(CORNER_DIR_FB);
	
		buffer = GetStatusDir_DBL();
		if (buffer == CORNER_DIR_FB) PutStatusDir_DBL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DBL(CORNER_DIR_FB);
		
		buffer = GetStatusDir_DBR();
		if (buffer == CORNER_DIR_FB) PutStatusDir_DBR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DBR(CORNER_DIR_FB);
	}
	
	void c_Corners::c_F() // F
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_UFL());
		PutStatus_UFL(GetStatus_DFL());
		PutStatus_DFL(GetStatus_DFR());
		PutStatus_DFR(buffer);
	
		buffer = GetStatusDir_UFR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UFR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UFR(CORNER_DIR_UD);
		
		buffer = GetStatusDir_UFL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UFL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UFL(CORNER_DIR_UD);
	
		buffer = GetStatusDir_DFL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DFL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DFL(CORNER_DIR_UD);
		
		buffer = GetStatusDir_DFR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DFR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DFR(CORNER_DIR_UD);	
	}
	
	void c_Corners::c_F2() // F2
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_DFL());
		PutStatus_DFL(buffer);
	
		buffer = GetStatus_UFL();
		PutStatus_UFL(GetStatus_DFR());
		PutStatus_DFR(buffer);
	}
	
	void c_Corners::c_Fp() // F'
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_DFR());
		PutStatus_DFR(GetStatus_DFL());
		PutStatus_DFL(GetStatus_UFL());
		PutStatus_UFL(buffer);
	
		buffer = GetStatusDir_UFR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UFR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UFR(CORNER_DIR_UD);
		
		buffer = GetStatusDir_UFL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UFL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UFL(CORNER_DIR_UD);
	
		buffer = GetStatusDir_DFL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DFL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DFL(CORNER_DIR_UD);
		
		buffer = GetStatusDir_DFR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DFR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DFR(CORNER_DIR_UD);
	}
	
	void c_Corners::c_B() // B
	{
		S64 buffer = GetStatus_UBR();
		PutStatus_UBR(GetStatus_DBR());
		PutStatus_DBR(GetStatus_DBL());
		PutStatus_DBL(GetStatus_UBL());
		PutStatus_UBL(buffer);
		
		buffer = GetStatusDir_UBR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UBR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UBR(CORNER_DIR_UD);
		
		buffer = GetStatusDir_UBL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UBL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UBL(CORNER_DIR_UD);
	
		buffer = GetStatusDir_DBL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DBL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DBL(CORNER_DIR_UD);
		
		buffer = GetStatusDir_DBR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DBR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DBR(CORNER_DIR_UD);
	}
	
	void c_Corners::c_B2() // B2
	{
		S64 buffer = GetStatus_UBL();
		PutStatus_UBL(GetStatus_DBR());
		PutStatus_DBR(buffer);
	
		buffer = GetStatus_UBR();
		PutStatus_UBR(GetStatus_DBL());
		PutStatus_DBL(buffer);
	}
	
	void c_Corners::c_Bp() // B'
	{
		S64 buffer = GetStatus_UBR();
		PutStatus_UBR(GetStatus_UBL());
		PutStatus_UBL(GetStatus_DBL());
		PutStatus_DBL(GetStatus_DBR());
		PutStatus_DBR(buffer);
	
		buffer = GetStatusDir_UBR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UBR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UBR(CORNER_DIR_UD);
		
		buffer = GetStatusDir_UBL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UBL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_UBL(CORNER_DIR_UD);
	
		buffer = GetStatusDir_DBL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DBL(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DBL(CORNER_DIR_UD);
		
		buffer = GetStatusDir_DBR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DBR(CORNER_DIR_RL);
		else if (buffer == CORNER_DIR_RL) PutStatusDir_DBR(CORNER_DIR_UD);
	}
	
	void c_Corners::c_R() // R
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_DFR());
		PutStatus_DFR(GetStatus_DBR());
		PutStatus_DBR(GetStatus_UBR());
		PutStatus_UBR(buffer);
	
		buffer = GetStatusDir_UFR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UFR(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_UFR(CORNER_DIR_UD);
		
		buffer = GetStatusDir_DFR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DFR(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_DFR(CORNER_DIR_UD);
	
		buffer = GetStatusDir_DBR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DBR(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_DBR(CORNER_DIR_UD);
		
		buffer = GetStatusDir_UBR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UBR(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_UBR(CORNER_DIR_UD);
	}
	
	void c_Corners::c_R2() // R2
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_DBR());
		PutStatus_DBR(buffer);
	
		buffer = GetStatus_UBR();
		PutStatus_UBR(GetStatus_DFR());
		PutStatus_DFR(buffer);
	}
	
	void c_Corners::c_Rp() // R'
	{
		S64 buffer = GetStatus_UFR();
		PutStatus_UFR(GetStatus_UBR());
		PutStatus_UBR(GetStatus_DBR());
		PutStatus_DBR(GetStatus_DFR());
		PutStatus_DFR(buffer);
	
		buffer = GetStatusDir_UFR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UFR(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_UFR(CORNER_DIR_UD);
		
		buffer = GetStatusDir_DFR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DFR(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_DFR(CORNER_DIR_UD);
	
		buffer = GetStatusDir_DBR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DBR(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_DBR(CORNER_DIR_UD);
		
		buffer = GetStatusDir_UBR();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UBR(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_UBR(CORNER_DIR_UD);
	}
	
	void c_Corners::c_L() // L
	{
		S64 buffer = GetStatus_UFL();
		PutStatus_UFL(GetStatus_UBL());
		PutStatus_UBL(GetStatus_DBL());
		PutStatus_DBL(GetStatus_DFL());
		PutStatus_DFL(buffer);
	
		buffer = GetStatusDir_UFL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UFL(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_UFL(CORNER_DIR_UD);
		
		buffer = GetStatusDir_UBL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UBL(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_UBL(CORNER_DIR_UD);
	
		buffer = GetStatusDir_DBL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DBL(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_DBL(CORNER_DIR_UD);
		
		buffer = GetStatusDir_DFL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DFL(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_DFL(CORNER_DIR_UD);
	}
	
	void c_Corners::c_L2() // L2
	{
		S64 buffer = GetStatus_UFL();
		PutStatus_UFL(GetStatus_DBL());
		PutStatus_DBL(buffer);
	
		buffer = GetStatus_UBL();
		PutStatus_UBL(GetStatus_DFL());
		PutStatus_DFL(buffer);
	}
	
	void c_Corners::c_Lp() // L'
	{
		S64 buffer = GetStatus_UFL();
		PutStatus_UFL(GetStatus_DFL());
		PutStatus_DFL(GetStatus_DBL());
		PutStatus_DBL(GetStatus_UBL());
		PutStatus_UBL(buffer);
	
		buffer = GetStatusDir_UFL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UFL(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_UFL(CORNER_DIR_UD);
		
		buffer = GetStatusDir_UBL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_UBL(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_UBL(CORNER_DIR_UD);
	
		buffer = GetStatusDir_DBL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DBL(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_DBL(CORNER_DIR_UD);
		
		buffer = GetStatusDir_DFL();
		if (buffer == CORNER_DIR_UD) PutStatusDir_DFL(CORNER_DIR_FB);
		else if (buffer == CORNER_DIR_FB) PutStatusDir_DFL(CORNER_DIR_UD);
	}

	// Get mask for check if given corner is in solve condition
	S64 c_Corners::GetMaskSolvedCorner(const Cnp c)
	{
		switch (c)
		{
			case Cnp::UFR: return CMASK_UFR;
			case Cnp::UFL: return CMASK_UFL;
			case Cnp::UBL: return CMASK_UBL;
			case Cnp::UBR: return CMASK_UBR;
			case Cnp::DFR: return CMASK_DFR;
			case Cnp::DFL: return CMASK_DFL;
			case Cnp::DBL: return CMASK_DBL;
			case Cnp::DBR: return CMASK_DBR;
			default: return 0ULL;
		}
	}
	
    // Get mask for check if corners in given list of pieces are in solve conditions
	S64 c_Corners::GetMaskSolvedCorners(const std::vector<Cnp>& p_list)
	{
		S64 c_mask = 0ULL;
		for (auto P : p_list) c_mask |= GetMaskSolvedCorner(P);
		return c_mask;
	}
	
	// Get mask for check corner direction in given corner position
    S64 c_Corners::GetMaskCornerDirection(const Cnp cp)
	{
		switch (cp)
		{
			case Cnp::UFR: return CMASK_DIR_UFR;
			case Cnp::UFL: return CMASK_DIR_UFL;
			case Cnp::UBL: return CMASK_DIR_UBL;
			case Cnp::UBR: return CMASK_DIR_UBR;
			case Cnp::DFR: return CMASK_DIR_DFR;
			case Cnp::DFL: return CMASK_DIR_DFL;
			case Cnp::DBL: return CMASK_DIR_DBL;
			case Cnp::DBR: return CMASK_DIR_DBR;
			default: return 0ull;
		}
	}

	// Get mask for check corners direction in given corners positions
    S64 c_Corners::GetMaskCornersDirection(const std::vector<Cnp>& p_list)
	{
		S64 c_mask = 0ull;	
		for (auto P : p_list) c_mask |= GetMaskCornerDirection(P);
		return c_mask;
	}

	// Get mask for check corners position (no direction needed)
	S64 c_Corners::GetMaskCornerPosition(const Cnp cp)
	{
		switch (cp)
		{
			case Cnp::UFR: return CMASK_POS_UFR;
			case Cnp::UFL: return CMASK_POS_UFL;
			case Cnp::UBL: return CMASK_POS_UBL;
			case Cnp::UBR: return CMASK_POS_UBR;
			case Cnp::DFR: return CMASK_POS_DFR;
			case Cnp::DFL: return CMASK_POS_DFL;
			case Cnp::DBL: return CMASK_POS_DBL;
			case Cnp::DBR: return CMASK_POS_DBR;
			default: return 0ull;
		}
	}

	// Get mask for check corner position (no direction needed)
	S64 c_Corners::GetMaskCornersPosition(const std::vector<Cnp>& p_list)
	{
		S64 c_mask = 0ull;	
		for (auto P : p_list) c_mask |= GetMaskCornerPosition(P);
		return c_mask;
	}

	// Checks the position of the corner in the given position (no direction)
	bool c_Corners::CheckCornerPosition(const Cnp pc) const
	{
		switch (pc)
		{
		case Cnp::UFR: return CheckPosition_UFR();
		case Cnp::UFL: return CheckPosition_UFL();
		case Cnp::UBR: return CheckPosition_UBR();
		case Cnp::UBL: return CheckPosition_UBL();

		case Cnp::DFR: return CheckPosition_DFR();
		case Cnp::DFL: return CheckPosition_DFL();
		case Cnp::DBR: return CheckPosition_DBR();
		case Cnp::DBL: return CheckPosition_DBL();

		default: return false;
		}
	}

	// Check if the given corner is in the given layer
	bool c_Corners::IsCornerInLayer(const Cnr P, const Lyr Ly) const
	{
		switch (Ly)
		{
		case Lyr::U: return IsCornerInLayerU(P);
		case Lyr::D: return IsCornerInLayerD(P);
		case Lyr::F: return IsCornerInLayerF(P);
		case Lyr::B: return IsCornerInLayerB(P);
		case Lyr::R: return IsCornerInLayerR(P);
		case Lyr::L: return IsCornerInLayerL(P);
		default: return false;
		}
	}
	
	// Get the current corner position for the given corner
    Cnp c_Corners::GetCornerPosition(const Cnr C) const
    {
		if (GetCorner_UFR() == C) return Cnp::UFR;
		if (GetCorner_UFL() == C) return Cnp::UFL;
		if (GetCorner_UBR() == C) return Cnp::UBR;
		if (GetCorner_UBL() == C) return Cnp::UBL;
		if (GetCorner_DFR() == C) return Cnp::DFR;
		if (GetCorner_DFL() == C) return Cnp::DFL;
		if (GetCorner_DBR() == C) return Cnp::DBR;
		if (GetCorner_DBL() == C) return Cnp::DBL;

		return static_cast<Cnp>(C); // Should not happend, avoid a warning
    }

	// Get the corner in the given corner position
	Cnr c_Corners::GetCorner(const Cnp cp) const
	{
		switch (cp)
		{
		case Cnp::UFR: return GetCorner_UFR();
		case Cnp::UFL: return GetCorner_UFL();
		case Cnp::UBR: return GetCorner_UBR();
		case Cnp::UBL: return GetCorner_UBL();
		case Cnp::DFR: return GetCorner_DFR();
		case Cnp::DFL: return GetCorner_DFL();
		case Cnp::DBR: return GetCorner_DBR();
		case Cnp::DBL: return GetCorner_DBL();

		default: return Cnr::UFR; // Should not happend, avoids a warning
		}
	}

	// Get the corner direction in the given corner position
	Cnd c_Corners::GetCornerDirection(const Cnp cp) const
	{
		switch (cp)
		{
		case Cnp::UFR: return GetCornerDir_UFR();
		case Cnp::UFL: return GetCornerDir_UFL();
		case Cnp::UBR: return GetCornerDir_UBR();
		case Cnp::UBL: return GetCornerDir_UBL();
		case Cnp::DFR: return GetCornerDir_DFR();
		case Cnp::DFL: return GetCornerDir_DFL();
		case Cnp::DBR: return GetCornerDir_DBR();
		case Cnp::DBL: return GetCornerDir_DBL();

		default: return Cnd::UD; // Should not happend, avoids a warning
		}
	}
}
