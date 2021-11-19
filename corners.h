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

#pragma once

#include <vector>

#include "cube_definitions.h"

namespace grcube3
{
	class c_Corners
	{
	public:
		c_Corners() { c_Reset(); }
		~c_Corners() {}
	
		// Get corner in corner position
		Cnr GetCorner_UFR() const { return static_cast<Cnr>(GetStatusPos_UFR()); }
		Cnr GetCorner_UFL() const { return static_cast<Cnr>(GetStatusPos_UFL()); }
		Cnr GetCorner_UBL() const { return static_cast<Cnr>(GetStatusPos_UBL()); }
		Cnr GetCorner_UBR() const { return static_cast<Cnr>(GetStatusPos_UBR()); }
		Cnr GetCorner_DFR() const { return static_cast<Cnr>(GetStatusPos_DFR()); }
		Cnr GetCorner_DFL() const { return static_cast<Cnr>(GetStatusPos_DFL()); }
		Cnr GetCorner_DBL() const { return static_cast<Cnr>(GetStatusPos_DBL()); }
		Cnr GetCorner_DBR() const { return static_cast<Cnr>(GetStatusPos_DBR()); }
		Cnr GetCorner(const Cnp) const;
	
		// Get corner direction in corner position
		Cnd GetCornerDir_UFR() const { return static_cast<Cnd>(GetStatusDir_UFR()); }
		Cnd GetCornerDir_UFL() const { return static_cast<Cnd>(GetStatusDir_UFL()); }
		Cnd GetCornerDir_UBL() const { return static_cast<Cnd>(GetStatusDir_UBL()); }
		Cnd GetCornerDir_UBR() const { return static_cast<Cnd>(GetStatusDir_UBR()); }
		Cnd GetCornerDir_DFR() const { return static_cast<Cnd>(GetStatusDir_DFR()); }
		Cnd GetCornerDir_DFL() const { return static_cast<Cnd>(GetStatusDir_DFL()); }
		Cnd GetCornerDir_DBL() const { return static_cast<Cnd>(GetStatusDir_DBL()); }
		Cnd GetCornerDir_DBR() const { return static_cast<Cnd>(GetStatusDir_DBR()); }
		Cnd GetCornerDirection(const Cnp) const;

		// Get the current position for the given corner
        Cnp GetCornerPosition(const Cnr) const;

        // Check if corners coded in given mask are solved (default = all corners)
		bool IsSolvedCorners(const S64 CMask = CMASK_DATA) const { return (corners & CMask) == (CMASK_SOLVED & CMask); }

		// Check if a corner is solved
		bool IsSolved_UFR() const { return GetCorner_UFR() == Cnr::UFR && GetCornerDir_UFR() == Cnd::UD; }
		bool IsSolved_UFL() const { return GetCorner_UFL() == Cnr::UFL && GetCornerDir_UFL() == Cnd::UD; }
		bool IsSolved_UBL() const { return GetCorner_UBL() == Cnr::UBL && GetCornerDir_UBL() == Cnd::UD; }
		bool IsSolved_UBR() const { return GetCorner_UBR() == Cnr::UBR && GetCornerDir_UBR() == Cnd::UD; }
		bool IsSolved_DFR() const { return GetCorner_DFR() == Cnr::DFR && GetCornerDir_DFR() == Cnd::UD; }
		bool IsSolved_DFL() const { return GetCorner_DFL() == Cnr::DFL && GetCornerDir_DFL() == Cnd::UD; }
		bool IsSolved_DBL() const { return GetCorner_DBL() == Cnr::DBL && GetCornerDir_DBL() == Cnd::UD; }
		bool IsSolved_DBR() const { return GetCorner_DBR() == Cnr::DBR && GetCornerDir_DBR() == Cnd::UD; }
	
		// Check if a corner is in his position (no direction/orientation check)
		bool CheckPosition_UFR() const { return GetCorner_UFR() == Cnr::UFR; }
		bool CheckPosition_UFL() const { return GetCorner_UFL() == Cnr::UFL; }
		bool CheckPosition_UBL() const { return GetCorner_UBL() == Cnr::UBL; }
		bool CheckPosition_UBR() const { return GetCorner_UBR() == Cnr::UBR; }
		bool CheckPosition_DFR() const { return GetCorner_DFR() == Cnr::DFR; }
		bool CheckPosition_DFL() const { return GetCorner_DFL() == Cnr::DFL; }
		bool CheckPosition_DBL() const { return GetCorner_DBL() == Cnr::DBL; }
		bool CheckPosition_DBR() const { return GetCorner_DBR() == Cnr::DBR; }
		bool CheckCornerPosition(const Cnp) const;

		// Get mask for check if corners positions in given list of pieces are in solve conditions
		static S64 GetMaskSolvedCorners(const std::vector<Cnp>&);

		// Get mask for check if given corner position is in solve condition
		static S64 GetMaskSolvedCorner(const Cnp);

		// Get mask for check corners direction in given corners positions
		static S64 GetMaskCornersDirection(const std::vector<Cnp>&);

		// Get mask for check corner direction in given corner position
		static S64 GetMaskCornerDirection(const Cnp);

		// Get mask for check corners position (no orientation needed)
		static S64 GetMaskCornersPosition(const std::vector<Cnp>&);

		// Get mask for check corner position (no orientation needed)
		static S64 GetMaskCornerPosition(const Cnp);

		// Get the edge position for the given absolut edge position
		static Cnp GetCornerPosition(const Acp cp, const Spn sp) { return cnp_Absolut[static_cast<int>(sp)][static_cast<int>(cp)]; }

		// Get the absolut edge position for the given edge position
		static Acp GetAbsCornerPosition(const Cnp cp, const Spn sp) { return acp_Positions[static_cast<int>(sp)][static_cast<int>(cp)]; }

	protected:
		// General masks for corners
        static constexpr auto CMASK_DATA 	 = 0x3737373737373737ULL;	// 0011_0111_0011_0111_0011_0111_0011_0111_0011_0111_0011_0111_0011_0111_0011_0111
		static constexpr auto CMASK_POS_DATA = 0x0707070707070707ULL;	// 0000_0111_0000_0111_0000_0111_0000_0111_0000_0111_0000_0111_0000_0111_0000_0111
		static constexpr auto CMASK_DIR_DATA = 0x3030303030303030ULL;	// 0011_0000_0011_0000_0011_0000_0011_0000_0011_0000_0011_0000_0011_0000_0011_0000
		static constexpr auto CMASK_SOLVED   = 0x0706050403020100ULL;	// 0000_0111_0000_0110_0000_0101_0000_0100_0000_0011_0000_0010_0000_0001_0000_0000
		
		// Masks for corners position
		static constexpr auto CMASK_POS_UFR = 0x0000000000000007ULL;
		static constexpr auto CMASK_POS_UFL = 0x0000000000000700ULL;
		static constexpr auto CMASK_POS_UBL = 0x0000000000070000ULL;
		static constexpr auto CMASK_POS_UBR = 0x0000000007000000ULL;
		static constexpr auto CMASK_POS_DFR = 0x0000000700000000ULL;
		static constexpr auto CMASK_POS_DFL = 0x0000070000000000ULL;
		static constexpr auto CMASK_POS_DBL = 0x0007000000000000ULL;
		static constexpr auto CMASK_POS_DBR = 0x0700000000000000ULL;
		
		// Masks for corners direction
		static constexpr auto CMASK_DIR_UFR = 0x0000000000000030ULL;
		static constexpr auto CMASK_DIR_UFL = 0x0000000000003000ULL;
		static constexpr auto CMASK_DIR_UBL = 0x0000000000300000ULL;
		static constexpr auto CMASK_DIR_UBR = 0x0000000030000000ULL;
		static constexpr auto CMASK_DIR_DFR = 0x0000003000000000ULL;
		static constexpr auto CMASK_DIR_DFL = 0x0000300000000000ULL;
		static constexpr auto CMASK_DIR_DBL = 0x0030000000000000ULL;
		static constexpr auto CMASK_DIR_DBR = 0x3000000000000000ULL;
		
		// Masks for corners (position + direction)
		static constexpr auto CMASK_UFR = 0x0000000000000037ULL;
		static constexpr auto CMASK_UFL = 0x0000000000003700ULL;
		static constexpr auto CMASK_UBL = 0x0000000000370000ULL;
		static constexpr auto CMASK_UBR = 0x0000000037000000ULL;
		static constexpr auto CMASK_DFR = 0x0000003700000000ULL;
		static constexpr auto CMASK_DFL = 0x0000370000000000ULL;
		static constexpr auto CMASK_DBL = 0x0037000000000000ULL;
		static constexpr auto CMASK_DBR = 0x3700000000000000ULL;

		// Masks for layers (corners in layer)
		static constexpr auto CMASK_U = CMASK_UFR | CMASK_UFL | CMASK_UBL | CMASK_UBR;
		static constexpr auto CMASK_D = CMASK_DFR | CMASK_DFL | CMASK_DBL | CMASK_DBR;
		static constexpr auto CMASK_F = CMASK_UFR | CMASK_UFL | CMASK_DFR | CMASK_DFL;
		static constexpr auto CMASK_B = CMASK_UBL | CMASK_UBR | CMASK_DBL | CMASK_DBR;
		static constexpr auto CMASK_R = CMASK_UFR | CMASK_UBR | CMASK_DFR | CMASK_DBR;
		static constexpr auto CMASK_L = CMASK_UFL | CMASK_UBL | CMASK_DFL | CMASK_DBL;
		static constexpr auto CMASK_E = 0x0ULL;
		static constexpr auto CMASK_M = 0x0ULL;
		static constexpr auto CMASK_S = 0x0ULL;
		
		// Three possible directions for corners: UD, FB and RL
		static constexpr auto CORNER_DIR_UD = 0x0000000000000000ULL;
		static constexpr auto CORNER_DIR_FB = 0x0000000000000001ULL;
		static constexpr auto CORNER_DIR_RL = 0x0000000000000002ULL;
			
		S64 corners; // Corners status

		static const Fce fce_UFR_U[8][3]; // Array to get the face (color) in the UFR_U sticker position
		static const Fce fce_UFR_F[8][3]; // Array to get the face (color) in the UFR_F sticker position
		static const Fce fce_UFR_R[8][3]; // Array to get the face (color) in the UFR_R sticker position
		static const Fce fce_UFL_U[8][3]; // Array to get the face (color) in the UFL_U sticker position
		static const Fce fce_UFL_F[8][3]; // Array to get the face (color) in the UFL_F sticker position
		static const Fce fce_UFL_L[8][3]; // Array to get the face (color) in the UFL_L sticker position

		static constexpr auto fce_UBR_U = fce_UFL_U; // Array to get the face (color) in the UBR_U sticker position
		static constexpr auto fce_UBR_B = fce_UFL_F; // Array to get the face (color) in the UBR_B sticker position
		static constexpr auto fce_UBR_R = fce_UFL_L; // Array to get the face (color) in the UBR_R sticker position
		static constexpr auto fce_UBL_U = fce_UFR_U; // Array to get the face (color) in the UBL_U sticker position
		static constexpr auto fce_UBL_B = fce_UFR_F; // Array to get the face (color) in the UBL_B sticker position
		static constexpr auto fce_UBL_L = fce_UFR_R; // Array to get the face (color) in the UBL_L sticker position
		
		static constexpr auto fce_DFR_D = fce_UFL_U; // Array to get the face (color) in the DFR_D sticker position
		static constexpr auto fce_DFR_F = fce_UFL_F; // Array to get the face (color) in the DFR_F sticker position
		static constexpr auto fce_DFR_R = fce_UFL_L; // Array to get the face (color) in the DFR_R sticker position
		static constexpr auto fce_DFL_D = fce_UFR_U; // Array to get the face (color) in the DFL_D sticker position
		static constexpr auto fce_DFL_F = fce_UFR_F; // Array to get the face (color) in the DFL_F sticker position
		static constexpr auto fce_DFL_L = fce_UFR_R; // Array to get the face (color) in the DFL_L sticker position

		static constexpr auto fce_DBR_D = fce_UFR_U; // Array to get the face (color) in the DBR_D sticker position
		static constexpr auto fce_DBR_B = fce_UFR_F; // Array to get the face (color) in the DBR_B sticker position
		static constexpr auto fce_DBR_R = fce_UFR_R; // Array to get the face (color) in the DBR_R sticker position
		static constexpr auto fce_DBL_D = fce_UFL_U; // Array to get the face (color) in the DBL_D sticker position
		static constexpr auto fce_DBL_B = fce_UFL_F; // Array to get the face (color) in the DBL_B sticker position
		static constexpr auto fce_DBL_L = fce_UFL_L; // Array to get the face (color) in the DBL_L sticker position

		static const Stc stc_UFR_U[8][3]; // Array to get the sticker in the UFR_U sticker position
		static const Stc stc_UFR_F[8][3]; // Array to get the sticker in the UFR_F sticker position
		static const Stc stc_UFR_R[8][3]; // Array to get the sticker in the UFR_R sticker position
		static const Stc stc_UFL_U[8][3]; // Array to get the sticker in the UFL_U sticker position
		static const Stc stc_UFL_F[8][3]; // Array to get the sticker in the UFL_F sticker position
		static const Stc stc_UFL_L[8][3]; // Array to get the sticker in the UFL_L sticker position

		static constexpr auto stc_UBR_U = stc_UFL_U; // Array to get the sticker in the UBR_U sticker position
		static constexpr auto stc_UBR_B = stc_UFL_F; // Array to get the sticker in the UBR_B sticker position
		static constexpr auto stc_UBR_R = stc_UFL_L; // Array to get the sticker in the UBR_R sticker position
		static constexpr auto stc_UBL_U = stc_UFR_U; // Array to get the sticker in the UBL_U sticker position
		static constexpr auto stc_UBL_B = stc_UFR_F; // Array to get the sticker in the UBL_B sticker position
		static constexpr auto stc_UBL_L = stc_UFR_R; // Array to get the sticker in the UBL_L sticker position
		
		static constexpr auto stc_DFR_D = stc_UFL_U; // Array to get the sticker in the DFR_D sticker position
		static constexpr auto stc_DFR_F = stc_UFL_F; // Array to get the sticker in the DFR_F sticker position
		static constexpr auto stc_DFR_R = stc_UFL_L; // Array to get the sticker in the DFR_R sticker position
		static constexpr auto stc_DFL_D = stc_UFR_U; // Array to get the sticker in the DFL_D sticker position
		static constexpr auto stc_DFL_F = stc_UFR_F; // Array to get the sticker in the DFL_F sticker position
		static constexpr auto stc_DFL_L = stc_UFR_R; // Array to get the sticker in the DFL_L sticker position

		static constexpr auto stc_DBR_D = stc_UFR_U; // Array to get the sticker in the DBR_D sticker position
		static constexpr auto stc_DBR_B = stc_UFR_F; // Array to get the sticker in the DBR_B sticker position
		static constexpr auto stc_DBR_R = stc_UFR_R; // Array to get the sticker in the DBR_R sticker position
		static constexpr auto stc_DBL_D = stc_UFL_U; // Array to get the sticker in the DBL_D sticker position
		static constexpr auto stc_DBL_B = stc_UFL_F; // Array to get the sticker in the DBL_B sticker position
		static constexpr auto stc_DBL_L = stc_UFL_L; // Array to get the sticker in the DBL_L sticker position

		// Absolute corner positions from spin and corner positions
		const static Acp acp_Positions[24][8];

		// Corner positions from spin and absolut corner positions
		const static Cnp cnp_Absolut[24][8];

		// Corners directions for UD orientation
		const static Cnd cnd_OrientationUD[24][8][8];

		// Corners directions for FB orientation
		const static Cnd cnd_OrientationFB[24][8][8];

		// Corners directions for RL orientation
		const static Cnd cnd_OrientationRL[24][8][8];
		
		// Movements
		void c_U();	void c_U2(); void c_Up();
		void c_D();	void c_D2(); void c_Dp();
		void c_F();	void c_F2(); void c_Fp();
		void c_B();	void c_B2(); void c_Bp();
		void c_R();	void c_R2(); void c_Rp();
		void c_L();	void c_L2(); void c_Lp();

		void c_Reset() { corners = CMASK_SOLVED; } // Reset corners status
		
		// Get corners status bits (position + direction)
		S64 GetStatus_UFR() const { return (corners & CMASK_UFR); }
		S64 GetStatus_UFL() const { return (corners & CMASK_UFL) >> 8; }
		S64 GetStatus_UBL() const { return (corners & CMASK_UBL) >> 16; }
		S64 GetStatus_UBR() const { return (corners & CMASK_UBR) >> 24; }
		S64 GetStatus_DFR() const { return (corners & CMASK_DFR) >> 32; }
		S64 GetStatus_DFL() const { return (corners & CMASK_DFL) >> 40; }
		S64 GetStatus_DBL() const { return (corners & CMASK_DBL) >> 48; }
		S64 GetStatus_DBR() const { return (corners & CMASK_DBR) >> 56; }
	
		// Change corners status bits (position + direction)
		void PutStatus_UFR(const S64 S) { corners = (corners & ~CMASK_UFR) | S; }
		void PutStatus_UFL(const S64 S) { corners = (corners & ~CMASK_UFL) | (S << 8); }
		void PutStatus_UBL(const S64 S) { corners = (corners & ~CMASK_UBL) | (S << 16); }
		void PutStatus_UBR(const S64 S) { corners = (corners & ~CMASK_UBR) | (S << 24); }
		void PutStatus_DFR(const S64 S) { corners = (corners & ~CMASK_DFR) | (S << 32); }
		void PutStatus_DFL(const S64 S) { corners = (corners & ~CMASK_DFL) | (S << 40); }
		void PutStatus_DBL(const S64 S) { corners = (corners & ~CMASK_DBL) | (S << 48); }
		void PutStatus_DBR(const S64 S) { corners = (corners & ~CMASK_DBR) | (S << 56); }
	
		// Get corners status bits (position only)
		S64 GetStatusPos_UFR() const { return (corners & CMASK_POS_UFR); }
		S64 GetStatusPos_UFL() const { return (corners & CMASK_POS_UFL) >> 8; }
		S64 GetStatusPos_UBL() const { return (corners & CMASK_POS_UBL) >> 16; }
		S64 GetStatusPos_UBR() const { return (corners & CMASK_POS_UBR) >> 24; }
		S64 GetStatusPos_DFR() const { return (corners & CMASK_POS_DFR) >> 32; }
		S64 GetStatusPos_DFL() const { return (corners & CMASK_POS_DFL) >> 40; }
		S64 GetStatusPos_DBL() const { return (corners & CMASK_POS_DBL) >> 48; }
		S64 GetStatusPos_DBR() const { return (corners & CMASK_POS_DBR) >> 56; }
		
		// Change corners status bits (position only)
		void PutStatusPos_UFR(const S64 P) { corners = (corners & ~CMASK_POS_UFR) | P; }
		void PutStatusPos_UFL(const S64 P) { corners = (corners & ~CMASK_POS_UFL) | (P << 8); }
		void PutStatusPos_UBL(const S64 P) { corners = (corners & ~CMASK_POS_UBL) | (P << 16); }
		void PutStatusPos_UBR(const S64 P) { corners = (corners & ~CMASK_POS_UBR) | (P << 24); }
		void PutStatusPos_DFR(const S64 P) { corners = (corners & ~CMASK_POS_DFR) | (P << 32); }
		void PutStatusPos_DFL(const S64 P) { corners = (corners & ~CMASK_POS_DFL) | (P << 40); }
		void PutStatusPos_DBL(const S64 P) { corners = (corners & ~CMASK_POS_DBL) | (P << 48); }
		void PutStatusPos_DBR(const S64 P) { corners = (corners & ~CMASK_POS_DBR) | (P << 56); }
	
		// Get corners status bits (direction only)
		S64 GetStatusDir_UFR() const { return (corners & CMASK_DIR_UFR) >> 4; }
		S64 GetStatusDir_UFL() const { return (corners & CMASK_DIR_UFL) >> 12; }
		S64 GetStatusDir_UBL() const { return (corners & CMASK_DIR_UBL) >> 20; }
		S64 GetStatusDir_UBR() const { return (corners & CMASK_DIR_UBR) >> 28; }
		S64 GetStatusDir_DFR() const { return (corners & CMASK_DIR_DFR) >> 36; }
		S64 GetStatusDir_DFL() const { return (corners & CMASK_DIR_DFL) >> 44; }
		S64 GetStatusDir_DBL() const { return (corners & CMASK_DIR_DBL) >> 52; }
		S64 GetStatusDir_DBR() const { return (corners & CMASK_DIR_DBR) >> 60; }
	
		// Change corners status bits (direction only)
		void PutStatusDir_UFR(const S64 D) { corners = (corners & ~CMASK_DIR_UFR) | (D << 4); }
		void PutStatusDir_UFL(const S64 D) { corners = (corners & ~CMASK_DIR_UFL) | (D << 12); }
		void PutStatusDir_UBL(const S64 D) { corners = (corners & ~CMASK_DIR_UBL) | (D << 20); }
		void PutStatusDir_UBR(const S64 D) { corners = (corners & ~CMASK_DIR_UBR) | (D << 28); }
		void PutStatusDir_DFR(const S64 D) { corners = (corners & ~CMASK_DIR_DFR) | (D << 36); }
		void PutStatusDir_DFL(const S64 D) { corners = (corners & ~CMASK_DIR_DFL) | (D << 44); }
		void PutStatusDir_DBL(const S64 D) { corners = (corners & ~CMASK_DIR_DBL) | (D << 52); }
		void PutStatusDir_DBR(const S64 D) { corners = (corners & ~CMASK_DIR_DBR) | (D << 60); }

		// Check if the given corner is in the given layer
		bool IsCornerInLayerU(const Cnr P) const { return (GetCorner_UFR() == P || GetCorner_UFL() == P || 
			                                               GetCorner_UBR() == P || GetCorner_UBL() == P); }
		bool IsCornerInLayerD(const Cnr P) const { return (GetCorner_DFR() == P || GetCorner_DFL() == P ||
			                                               GetCorner_DBR() == P || GetCorner_DBL() == P); }
		bool IsCornerInLayerF(const Cnr P) const { return (GetCorner_UFR() == P || GetCorner_UFL() == P ||
			                                               GetCorner_DFR() == P || GetCorner_DFL() == P); }
		bool IsCornerInLayerB(const Cnr P) const { return (GetCorner_UBR() == P || GetCorner_UBL() == P ||
			                                               GetCorner_DBR() == P || GetCorner_DBL() == P); }
		bool IsCornerInLayerR(const Cnr P) const { return (GetCorner_UFR() == P || GetCorner_UBR() == P ||
			                                               GetCorner_DFR() == P || GetCorner_DBR() == P); }
		bool IsCornerInLayerL(const Cnr P) const { return (GetCorner_UFL() == P || GetCorner_UBL() == P ||
			                                               GetCorner_DFL() == P || GetCorner_DBL() == P); }
		bool IsCornerInLayer(const Cnr, const Lyr) const;
	};
}
