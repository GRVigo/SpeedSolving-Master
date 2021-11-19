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
	class c_Edges
	{
	public:
		c_Edges() { e_Reset(); } // Constructor
		~c_Edges() {} // Destructor
	
		// Get edge at specified edge position
		Edg GetEdge_FR() const { return static_cast<Edg>(GetStatusPos_FR()); }
		Edg GetEdge_FL() const { return static_cast<Edg>(GetStatusPos_FL()); }
		Edg GetEdge_BL() const { return static_cast<Edg>(GetStatusPos_BL()); }
		Edg GetEdge_BR() const { return static_cast<Edg>(GetStatusPos_BR()); }
		Edg GetEdge_UF() const { return static_cast<Edg>(GetStatusPos_UF()); }
		Edg GetEdge_UL() const { return static_cast<Edg>(GetStatusPos_UL()); }
		Edg GetEdge_UB() const { return static_cast<Edg>(GetStatusPos_UB()); }
		Edg GetEdge_UR() const { return static_cast<Edg>(GetStatusPos_UR()); }
		Edg GetEdge_DF() const { return static_cast<Edg>(GetStatusPos_DF()); }
		Edg GetEdge_DL() const { return static_cast<Edg>(GetStatusPos_DL()); }
		Edg GetEdge_DB() const { return static_cast<Edg>(GetStatusPos_DB()); }
		Edg GetEdge_DR() const { return static_cast<Edg>(GetStatusPos_DR()); }
		Edg GetEdge(const Edp) const;
	
		// Get edge direction at specified edge position
		Edd GetEdgeDir_FR() const { return static_cast<Edd>(GetStatusDir_FR()); }
		Edd GetEdgeDir_FL() const { return static_cast<Edd>(GetStatusDir_FL()); }
		Edd GetEdgeDir_BL() const { return static_cast<Edd>(GetStatusDir_BL()); }
		Edd GetEdgeDir_BR() const { return static_cast<Edd>(GetStatusDir_BR()); }
		Edd GetEdgeDir_UF() const { return static_cast<Edd>(GetStatusDir_UF()); }
		Edd GetEdgeDir_UL() const { return static_cast<Edd>(GetStatusDir_UL()); }
		Edd GetEdgeDir_UB() const { return static_cast<Edd>(GetStatusDir_UB()); }
		Edd GetEdgeDir_UR() const { return static_cast<Edd>(GetStatusDir_UR()); }
		Edd GetEdgeDir_DF() const { return static_cast<Edd>(GetStatusDir_DF()); }
		Edd GetEdgeDir_DL() const { return static_cast<Edd>(GetStatusDir_DL()); }
		Edd GetEdgeDir_DB() const { return static_cast<Edd>(GetStatusDir_DB()); }
		Edd GetEdgeDir_DR() const { return static_cast<Edd>(GetStatusDir_DR()); }
		Edd GetEdgeDirection(const Edp) const;
		
		// Get the current position for the given edge
        Edp GetEdgePosition(const Edg) const;

        // Check if edges coded in given mask are solved (default = all edges)
		bool IsSolvedEdges(const S64 EMask = EMASK_DATA) const { return (edges & EMask) == (EMASK_SOLVED & EMask); }

		// Check if an edge is solved
		bool IsSolved_FR() const { return GetEdge_FR() == Edg::FR && GetEdgeDir_FR() == Edd::POS; }
		bool IsSolved_FL() const { return GetEdge_FL() == Edg::FL && GetEdgeDir_FL() == Edd::POS; }
		bool IsSolved_BL() const { return GetEdge_BL() == Edg::BL && GetEdgeDir_BL() == Edd::POS; }
		bool IsSolved_BR() const { return GetEdge_BR() == Edg::BR && GetEdgeDir_BR() == Edd::POS; }
		bool IsSolved_UF() const { return GetEdge_UF() == Edg::UF && GetEdgeDir_UF() == Edd::POS; }
		bool IsSolved_UL() const { return GetEdge_UL() == Edg::UL && GetEdgeDir_UL() == Edd::POS; }
		bool IsSolved_UB() const { return GetEdge_UB() == Edg::UB && GetEdgeDir_UB() == Edd::POS; }
		bool IsSolved_UR() const { return GetEdge_UR() == Edg::UR && GetEdgeDir_UR() == Edd::POS; }
		bool IsSolved_DF() const { return GetEdge_DF() == Edg::DF && GetEdgeDir_DF() == Edd::POS; }
		bool IsSolved_DL() const { return GetEdge_DL() == Edg::DL && GetEdgeDir_DL() == Edd::POS; }
		bool IsSolved_DB() const { return GetEdge_DB() == Edg::DB && GetEdgeDir_DB() == Edd::POS; }
		bool IsSolved_DR() const { return GetEdge_DR() == Edg::DR && GetEdgeDir_DR() == Edd::POS; }

		// Check if an edge is in his position (no direction check)
		bool CheckPosition_FR() const { return GetEdge_FR() == Edg::FR; }
		bool CheckPosition_FL() const { return GetEdge_FL() == Edg::FL; }
		bool CheckPosition_BL() const { return GetEdge_BL() == Edg::BL; }
		bool CheckPosition_BR() const { return GetEdge_BR() == Edg::BR; }
		bool CheckPosition_UF() const { return GetEdge_UF() == Edg::UF; }
		bool CheckPosition_UL() const { return GetEdge_UL() == Edg::UL; }
		bool CheckPosition_UB() const { return GetEdge_UB() == Edg::UB; }
		bool CheckPosition_UR() const { return GetEdge_UR() == Edg::UR; }
		bool CheckPosition_DF() const { return GetEdge_DF() == Edg::DF; }
		bool CheckPosition_DL() const { return GetEdge_DL() == Edg::DL; }
		bool CheckPosition_DB() const { return GetEdge_DB() == Edg::DB; }
		bool CheckPosition_DR() const { return GetEdge_DR() == Edg::DR; }
		bool CheckEdgePosition(const Edp) const;

		// Get mask for check if edges in given list of pieces positions are in solve conditions
		static S64 GetMaskSolvedEdges(const std::vector<Edp>&);

		// Get mask for check if given edge position is in solve condition
		static S64 GetMaskSolvedEdge(const Edp);

		// Get mask for check edges position (no direction needed)
		static S64 GetMaskEdgesPosition(const std::vector<Edp>&);

		// Get mask for check edge position (no direction needed)
		static S64 GetMaskEdgePosition(const Edp);

		// Get mask for check edges direction in given edges positions
		static S64 GetMaskEdgesDirection(const std::vector<Edp>&);

		// Get mask for check edge direction in given edge position
		static S64 GetMaskEdgeDirection(const Edp);

		// Get the edge position for the given absolut edge position
		static Edp GetEdgePosition(const Aep ep, const Spn sp) { return edp_Absolut[static_cast<int>(sp)][static_cast<int>(ep)]; }

		// Get the absolut edge position for the given edge position
		static Aep GetAbsEdgePosition(const Edp ep, const Spn sp) { return aep_Positions[static_cast<int>(sp)][static_cast<int>(ep)]; }
		
	protected:
		// General masks for edges
		static constexpr auto EMASK_DATA   	 = 0x0FFFFFFFFFFFFFFFULL;	// 0000_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111
		static constexpr auto EMASK_POS_DATA = 0x0000FFFFFFFFFFFFULL;	// 0000_0000_0000_0000_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111
		static constexpr auto EMASK_DIR_DATA = 0x0FFF000000000000ULL;	// 0000_1111_1111_1111_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000
		static constexpr auto EMASK_SOLVED   = 0x0000BA9876543210ULL;	// 0000_0000_0000_0000_1011_1010_1001_1000_0111_0110_0101_0100_0011_0010_0001_0000
		
		// Masks to get edges position
		static constexpr auto EMASK_POS_FR = 0x000000000000000FULL;
		static constexpr auto EMASK_POS_FL = 0x00000000000000F0ULL;
		static constexpr auto EMASK_POS_BL = 0x0000000000000F00ULL;
		static constexpr auto EMASK_POS_BR = 0x000000000000F000ULL;
		static constexpr auto EMASK_POS_UF = 0x00000000000F0000ULL;
		static constexpr auto EMASK_POS_UL = 0x0000000000F00000ULL;
		static constexpr auto EMASK_POS_UB = 0x000000000F000000ULL;
		static constexpr auto EMASK_POS_UR = 0x00000000F0000000ULL;
		static constexpr auto EMASK_POS_DF = 0x0000000F00000000ULL;
		static constexpr auto EMASK_POS_DL = 0x000000F000000000ULL;
		static constexpr auto EMASK_POS_DB = 0x00000F0000000000ULL;
		static constexpr auto EMASK_POS_DR = 0x0000F00000000000ULL;
		
		// Masks to get edges direction
		static constexpr auto EMASK_DIR_FR = 0x0001000000000000ULL;
		static constexpr auto EMASK_DIR_FL = 0x0002000000000000ULL;
		static constexpr auto EMASK_DIR_BL = 0x0004000000000000ULL;
		static constexpr auto EMASK_DIR_BR = 0x0008000000000000ULL;
		static constexpr auto EMASK_DIR_UF = 0x0010000000000000ULL;
		static constexpr auto EMASK_DIR_UL = 0x0020000000000000ULL;
		static constexpr auto EMASK_DIR_UB = 0x0040000000000000ULL;
		static constexpr auto EMASK_DIR_UR = 0x0080000000000000ULL;
		static constexpr auto EMASK_DIR_DF = 0x0100000000000000ULL;
		static constexpr auto EMASK_DIR_DL = 0x0200000000000000ULL;
		static constexpr auto EMASK_DIR_DB = 0x0400000000000000ULL;
		static constexpr auto EMASK_DIR_DR = 0x0800000000000000ULL;
		
		// Masks for edges (position + direction)
		static constexpr auto EMASK_FR = 0x000100000000000FULL;
		static constexpr auto EMASK_FL = 0x00020000000000F0ULL;
		static constexpr auto EMASK_BL = 0x0004000000000F00ULL;
		static constexpr auto EMASK_BR = 0x000800000000F000ULL;
		static constexpr auto EMASK_UF = 0x00100000000F0000ULL;
		static constexpr auto EMASK_UL = 0x0020000000F00000ULL;
		static constexpr auto EMASK_UB = 0x004000000F000000ULL;
		static constexpr auto EMASK_UR = 0x00800000F0000000ULL;
		static constexpr auto EMASK_DF = 0x0100000F00000000ULL;
		static constexpr auto EMASK_DL = 0x020000F000000000ULL;
		static constexpr auto EMASK_DB = 0x04000F0000000000ULL;
		static constexpr auto EMASK_DR = 0x0800F00000000000ULL;

		// Masks for layers (edges in layer)
		static constexpr auto EMASK_U = EMASK_UF | EMASK_UL | EMASK_UB | EMASK_UR;
		static constexpr auto EMASK_D = EMASK_DF | EMASK_DL | EMASK_DB | EMASK_DR;
		static constexpr auto EMASK_F = EMASK_FR | EMASK_FL | EMASK_UF | EMASK_DF;
		static constexpr auto EMASK_B = EMASK_BL | EMASK_BR | EMASK_UB | EMASK_DB;
		static constexpr auto EMASK_R = EMASK_FR | EMASK_BR | EMASK_UR | EMASK_DR;
		static constexpr auto EMASK_L = EMASK_FL | EMASK_BL | EMASK_UL | EMASK_DL;
		static constexpr auto EMASK_E = EMASK_FR | EMASK_FL | EMASK_BL | EMASK_BR;
		static constexpr auto EMASK_M = EMASK_UF | EMASK_DF | EMASK_DB | EMASK_UB;
		static constexpr auto EMASK_S = EMASK_UR | EMASK_DR | EMASK_DL | EMASK_UL;
		
		// Two possible directions for edges: positive and negative
		static constexpr auto EDGE_DIR_POS = 0x0000000000000000ULL;
		static constexpr auto EDGE_DIR_NEG = 0x0000000000000001ULL;

		S64 edges; // Edg status

		const static Fce fce_FR_F[12][2]; // Array to get the face (color) in the FR_F sticker position
		const static Fce fce_FR_R[12][2]; // Array to get the face (color) in the FR_R sticker position
				
		static constexpr auto fce_FL_F = fce_FR_F; // Array to get the face (color) in the FL_F sticker position
		static constexpr auto fce_FL_L = fce_FR_R; // Array to get the face (color) in the FL_L sticker position
		static constexpr auto fce_BL_B = fce_FR_F; // Array to get the face (color) in the BL_B sticker position
		static constexpr auto fce_BL_L = fce_FR_R; // Array to get the face (color) in the BL_L sticker position
		static constexpr auto fce_BR_B = fce_FR_F; // Array to get the face (color) in the BR_B sticker position
		static constexpr auto fce_BR_R = fce_FR_R; // Array to get the face (color) in the BR_R sticker position
		
		static constexpr auto fce_UF_U = fce_FR_F; // Array to get the face (color) in the UF_U sticker position
		static constexpr auto fce_UF_F = fce_FR_R; // Array to get the face (color) in the UF_F sticker position
		static constexpr auto fce_UL_U = fce_FR_F; // Array to get the face (color) in the UL_U sticker position
		static constexpr auto fce_UL_L = fce_FR_R; // Array to get the face (color) in the UL_L sticker position
		static constexpr auto fce_UB_U = fce_FR_F; // Array to get the face (color) in the UB_U sticker position
		static constexpr auto fce_UB_B = fce_FR_R; // Array to get the face (color) in the UB_B sticker position
		static constexpr auto fce_UR_U = fce_FR_F; // Array to get the face (color) in the UR_U sticker position
		static constexpr auto fce_UR_R = fce_FR_R; // Array to get the face (color) in the UR_R sticker position
		
		static constexpr auto fce_DF_D = fce_FR_F; // Array to get the face (color) in the DF_D sticker position
		static constexpr auto fce_DF_F = fce_FR_R; // Array to get the face (color) in the DF_F sticker position
		static constexpr auto fce_DL_D = fce_FR_F; // Array to get the face (color) in the DL_D sticker position
		static constexpr auto fce_DL_L = fce_FR_R; // Array to get the face (color) in the DL_L sticker position
		static constexpr auto fce_DB_D = fce_FR_F; // Array to get the face (color) in the DB_D sticker position
		static constexpr auto fce_DB_B = fce_FR_R; // Array to get the face (color) in the DB_B sticker position
		static constexpr auto fce_DR_D = fce_FR_F; // Array to get the face (color) in the DR_D sticker position
		static constexpr auto fce_DR_R = fce_FR_R; // Array to get the face (color) in the DR_R sticker position
		
		const static Stc stc_FR_F[12][2]; // Array to get the sticker in the FR_F sticker position
		const static Stc stc_FR_R[12][2]; // Array to get the sticker in the FR_R sticker position

		static constexpr auto stc_FL_F = stc_FR_F; // Array to get the sticker in the FL_F sticker position
		static constexpr auto stc_FL_L = stc_FR_R; // Array to get the sticker in the FL_L sticker position
		static constexpr auto stc_BL_B = stc_FR_F; // Array to get the sticker in the BL_B sticker position
		static constexpr auto stc_BL_L = stc_FR_R; // Array to get the sticker in the BL_L sticker position
		static constexpr auto stc_BR_B = stc_FR_F; // Array to get the sticker in the BR_B sticker position
		static constexpr auto stc_BR_R = stc_FR_R; // Array to get the sticker in the BR_R sticker position
		
		static constexpr auto stc_UF_U = stc_FR_F; // Array to get the sticker in the UF_U sticker position
		static constexpr auto stc_UF_F = stc_FR_R; // Array to get the sticker in the UF_F sticker position
		static constexpr auto stc_UL_U = stc_FR_F; // Array to get the sticker in the UL_U sticker position
		static constexpr auto stc_UL_L = stc_FR_R; // Array to get the sticker in the UL_L sticker position
		static constexpr auto stc_UB_U = stc_FR_F; // Array to get the sticker in the UB_U sticker position
		static constexpr auto stc_UB_B = stc_FR_R; // Array to get the sticker in the UB_B sticker position
		static constexpr auto stc_UR_U = stc_FR_F; // Array to get the sticker in the UR_U sticker position
		static constexpr auto stc_UR_R = stc_FR_R; // Array to get the sticker in the UR_R sticker position
		
		static constexpr auto stc_DF_D = stc_FR_F; // Array to get the sticker in the DF_D sticker position
		static constexpr auto stc_DF_F = stc_FR_R; // Array to get the sticker in the DF_F sticker position
		static constexpr auto stc_DL_D = stc_FR_F; // Array to get the sticker in the DL_D sticker position
		static constexpr auto stc_DL_L = stc_FR_R; // Array to get the sticker in the DL_L sticker position
		static constexpr auto stc_DB_D = stc_FR_F; // Array to get the sticker in the DB_D sticker position
		static constexpr auto stc_DB_B = stc_FR_R; // Array to get the sticker in the DB_B sticker position
		static constexpr auto stc_DR_D = stc_FR_F; // Array to get the sticker in the DR_D sticker position
		static constexpr auto stc_DR_R = stc_FR_R; // Array to get the sticker in the DR_R sticker position	

		// Absolute edge positions from spin and edge positions
        const static Aep aep_Positions[24][12];
		
		// Edge positions from spin and absolut edge positions
        const static Edp edp_Absolut[24][12];

		// Edges directions for positive orientation
		const static Edd edd_OrientationPOS[24][12][12];
		
		// Movements
		void e_U();	void e_U2(); void e_Up();
		void e_D();	void e_D2(); void e_Dp();
		void e_F();	void e_F2(); void e_Fp();
		void e_B();	void e_B2(); void e_Bp();
		void e_R();	void e_R2(); void e_Rp();
		void e_L();	void e_L2(); void e_Lp();
				
		void e_Reset() { edges = EMASK_SOLVED; } // Reset edges status

		// Change edges status bits (position only)
		void PutStatusPos_FR(const S64 P) { edges = (edges & ~EMASK_POS_FR) | P; }
		void PutStatusPos_FL(const S64 P) { edges = (edges & ~EMASK_POS_FL) | (P << 4); }
		void PutStatusPos_BL(const S64 P) { edges = (edges & ~EMASK_POS_BL) | (P << 8); }
		void PutStatusPos_BR(const S64 P) { edges = (edges & ~EMASK_POS_BR) | (P << 12); }
		void PutStatusPos_UF(const S64 P) { edges = (edges & ~EMASK_POS_UF) | (P << 16); }
		void PutStatusPos_UL(const S64 P) { edges = (edges & ~EMASK_POS_UL) | (P << 20); }
		void PutStatusPos_UB(const S64 P) { edges = (edges & ~EMASK_POS_UB) | (P << 24); }
		void PutStatusPos_UR(const S64 P) { edges = (edges & ~EMASK_POS_UR) | (P << 28); }
		void PutStatusPos_DF(const S64 P) { edges = (edges & ~EMASK_POS_DF) | (P << 32); }
		void PutStatusPos_DL(const S64 P) { edges = (edges & ~EMASK_POS_DL) | (P << 36); }
		void PutStatusPos_DB(const S64 P) { edges = (edges & ~EMASK_POS_DB) | (P << 40); }
		void PutStatusPos_DR(const S64 P) { edges = (edges & ~EMASK_POS_DR) | (P << 44); }
	
		// Change edges status bits (direction only)
		void PutStatusDir_FR(const S64 D) { edges = (edges & ~EMASK_DIR_FR) | (D << 48); }
		void PutStatusDir_FL(const S64 D) { edges = (edges & ~EMASK_DIR_FL) | (D << 49); }
		void PutStatusDir_BL(const S64 D) { edges = (edges & ~EMASK_DIR_BL) | (D << 50); }
		void PutStatusDir_BR(const S64 D) { edges = (edges & ~EMASK_DIR_BR) | (D << 51); }
		void PutStatusDir_UF(const S64 D) { edges = (edges & ~EMASK_DIR_UF) | (D << 52); }
		void PutStatusDir_UL(const S64 D) { edges = (edges & ~EMASK_DIR_UL) | (D << 53); }
		void PutStatusDir_UB(const S64 D) { edges = (edges & ~EMASK_DIR_UB) | (D << 54); }
		void PutStatusDir_UR(const S64 D) { edges = (edges & ~EMASK_DIR_UR) | (D << 55); }
		void PutStatusDir_DF(const S64 D) { edges = (edges & ~EMASK_DIR_DF) | (D << 56); }
		void PutStatusDir_DL(const S64 D) { edges = (edges & ~EMASK_DIR_DL) | (D << 57); }
		void PutStatusDir_DB(const S64 D) { edges = (edges & ~EMASK_DIR_DB) | (D << 58); }
		void PutStatusDir_DR(const S64 D) { edges = (edges & ~EMASK_DIR_DR) | (D << 59); }
		
		// Get edges status bits (positions only)
		S64 GetStatusPos_FR() const { return (edges & EMASK_POS_FR); }
		S64 GetStatusPos_FL() const { return (edges & EMASK_POS_FL) >> 4; }
		S64 GetStatusPos_BL() const { return (edges & EMASK_POS_BL) >> 8; }
		S64 GetStatusPos_BR() const { return (edges & EMASK_POS_BR) >> 12; }
		S64 GetStatusPos_UF() const { return (edges & EMASK_POS_UF) >> 16; }
		S64 GetStatusPos_UL() const { return (edges & EMASK_POS_UL) >> 20; }
		S64 GetStatusPos_UB() const { return (edges & EMASK_POS_UB) >> 24; }
		S64 GetStatusPos_UR() const { return (edges & EMASK_POS_UR) >> 28; }
		S64 GetStatusPos_DF() const { return (edges & EMASK_POS_DF) >> 32; }
		S64 GetStatusPos_DL() const { return (edges & EMASK_POS_DL) >> 36; }
		S64 GetStatusPos_DB() const { return (edges & EMASK_POS_DB) >> 40; }
		S64 GetStatusPos_DR() const { return (edges & EMASK_POS_DR) >> 44; }
		
		// Get edges status bits (directions only)
		S64 GetStatusDir_FR() const { return (edges & EMASK_DIR_FR) >> 48; }
		S64 GetStatusDir_FL() const { return (edges & EMASK_DIR_FL) >> 49; }
		S64 GetStatusDir_BL() const { return (edges & EMASK_DIR_BL) >> 50; }
		S64 GetStatusDir_BR() const { return (edges & EMASK_DIR_BR) >> 51; }
		S64 GetStatusDir_UF() const { return (edges & EMASK_DIR_UF) >> 52; }
		S64 GetStatusDir_UL() const { return (edges & EMASK_DIR_UL) >> 53; }
		S64 GetStatusDir_UB() const { return (edges & EMASK_DIR_UB) >> 54; }
		S64 GetStatusDir_UR() const { return (edges & EMASK_DIR_UR) >> 55; }
		S64 GetStatusDir_DF() const { return (edges & EMASK_DIR_DF) >> 56; }
		S64 GetStatusDir_DL() const { return (edges & EMASK_DIR_DL) >> 57; }
		S64 GetStatusDir_DB() const { return (edges & EMASK_DIR_DB) >> 58; }
		S64 GetStatusDir_DR() const { return (edges & EMASK_DIR_DR) >> 59; }
	
		// Invert edges direction status bit
		void InvertStatusDir_FR() { edges = edges & EMASK_DIR_FR ? edges & ~EMASK_DIR_FR : edges | EMASK_DIR_FR; }
		void InvertStatusDir_FL() { edges = edges & EMASK_DIR_FL ? edges & ~EMASK_DIR_FL : edges | EMASK_DIR_FL; }
		void InvertStatusDir_BL() { edges = edges & EMASK_DIR_BL ? edges & ~EMASK_DIR_BL : edges | EMASK_DIR_BL; }
		void InvertStatusDir_BR() { edges = edges & EMASK_DIR_BR ? edges & ~EMASK_DIR_BR : edges | EMASK_DIR_BR; }
		void InvertStatusDir_UF() { edges = edges & EMASK_DIR_UF ? edges & ~EMASK_DIR_UF : edges | EMASK_DIR_UF; }
		void InvertStatusDir_UL() { edges = edges & EMASK_DIR_UL ? edges & ~EMASK_DIR_UL : edges | EMASK_DIR_UL; }
		void InvertStatusDir_UB() { edges = edges & EMASK_DIR_UB ? edges & ~EMASK_DIR_UB : edges | EMASK_DIR_UB; }
		void InvertStatusDir_UR() { edges = edges & EMASK_DIR_UR ? edges & ~EMASK_DIR_UR : edges | EMASK_DIR_UR; }
		void InvertStatusDir_DF() { edges = edges & EMASK_DIR_DF ? edges & ~EMASK_DIR_DF : edges | EMASK_DIR_DF; }
		void InvertStatusDir_DL() { edges = edges & EMASK_DIR_DL ? edges & ~EMASK_DIR_DL : edges | EMASK_DIR_DL; }
		void InvertStatusDir_DB() { edges = edges & EMASK_DIR_DB ? edges & ~EMASK_DIR_DB : edges | EMASK_DIR_DB; }
		void InvertStatusDir_DR() { edges = edges & EMASK_DIR_DR ? edges & ~EMASK_DIR_DR : edges | EMASK_DIR_DR; }
		void InvertStatusDir(const Edp);

		// Check if the given edge is in the given layer
		bool IsEdgeInLayerU(const Edg P) const { return (GetEdge_UF() == P || GetEdge_UB() == P || GetEdge_UR() == P || GetEdge_UL() == P); }
		bool IsEdgeInLayerD(const Edg P) const { return (GetEdge_DF() == P || GetEdge_DB() == P || GetEdge_DR() == P || GetEdge_DL() == P); }
		bool IsEdgeInLayerF(const Edg P) const { return (GetEdge_UF() == P || GetEdge_DF() == P || GetEdge_FR() == P || GetEdge_FL() == P); }
		bool IsEdgeInLayerB(const Edg P) const { return (GetEdge_UB() == P || GetEdge_DB() == P || GetEdge_BR() == P || GetEdge_BL() == P); }
		bool IsEdgeInLayerR(const Edg P) const { return (GetEdge_UR() == P || GetEdge_DR() == P || GetEdge_FR() == P || GetEdge_BR() == P); }
		bool IsEdgeInLayerL(const Edg P) const { return (GetEdge_UL() == P || GetEdge_DL() == P || GetEdge_FL() == P || GetEdge_BL() == P); }
		bool IsEdgeInLayerE(const Edg P) const { return (GetEdge_FR() == P || GetEdge_FL() == P || GetEdge_BR() == P || GetEdge_BL() == P); }
		bool IsEdgeInLayerM(const Edg P) const { return (GetEdge_UF() == P || GetEdge_UB() == P || GetEdge_DF() == P || GetEdge_DB() == P); }
		bool IsEdgeInLayerS(const Edg P) const { return (GetEdge_UR() == P || GetEdge_UL() == P || GetEdge_DR() == P || GetEdge_DL() == P); }
		bool IsEdgeInLayer(const Edg, const Lyr) const;
	};
}
