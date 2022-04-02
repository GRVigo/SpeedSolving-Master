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

#include "cube_definitions.h"
#include "edges.h"
#include "corners.h"
#include "algorithm.h"

namespace grcube3
{
	// 3x3x3 Rubik's cube class
	class Cube: public c_Edges, public c_Corners
	{
	public:
        // Default constructor
        Cube() { spin = Spn::Default; } // Default = UF

        // Algorithm constructor (develop algorithm parentheses)
        Cube(const Algorithm& A)
		{ 
			spin = Spn::Default; // Default = UF
            if (!ApplyAlgorithm(A))
            { // Algorithm have parentheses with more than one repetition, must be developed
				Reset();
                Algorithm Aux = A.GetDeveloped(false);
                ApplyAlgorithm(Aux);
            }
		}
		
		// Reset cube to default status (solved)
        void Reset() { e_Reset(); c_Reset(); spin = Spn::Default; }
		
		// Set cube spin
        void SetSpin(const Spn);
		
		// Apply movement (step) to the cube
		// Returns false if the step is a close parentheses with more than one repetition
		bool ApplyStep(const Stp);

		// Apply algorithm to cube (algorithm should not have parentheses with more than one repetition)
        // If there are parentheses with more than one repetition returns false and cube status remains unchanged
		bool ApplyAlgorithm(const Algorithm&);

		// Equal operator -> corners & edges must match, spin is not important
		bool operator==(Cube& rhcube) const { return edges == rhcube.GetEdgesStatus() && corners == rhcube.GetCornersStatus(); }

        // Not equal operator - corners & edges must not match, spin is not important
        bool operator!=(Cube& rhcube) const { return edges != rhcube.GetEdgesStatus() || corners != rhcube.GetCornersStatus(); }

        // Get corners status
        S64 GetCornersStatus() const { return corners; }
		
		// Get corners position status
        S64 GetCornersPosStatus() const { return corners & CMASK_POS_DATA; }
		
		// Get corners direction status
        S64 GetCornersDirStatus() const { return corners & CMASK_DIR_DATA; }

        // Get edges status
        S64 GetEdgesStatus() const { return edges; }
		
		// Get edges position status
        S64 GetEdgesPosStatus() const { return edges & EMASK_POS_DATA; }
		
		// Get edges direction status
        S64 GetEdgesDirStatus() const { return edges & EMASK_DIR_DATA; }

        // Get cube spin
        Spn GetSpin() const { return spin; }

        // Get cube spin as text
        std::string GetSpinText() const { return string_Spins[static_cast<int>(spin)]; }

        // Check if cube is solved for given pieces masks
		bool IsSolved(const S64 EMask = EMASK_DATA, const S64 CMask = CMASK_DATA) const
		{
            return IsSolvedEdges(EMask) && IsSolvedCorners(CMask);
		}

        // Check if a single piece is in solve condition
		bool IsSolved(const Pce) const;

        // Check if a single piece position is in solve condition
        bool IsSolved(const Pcp P) const { return IsSolved(static_cast<Pce>(P)); }

        // Check if a group of pieces are in solve condition
		bool IsSolved(const Pgr) const;

        // Check if some groups of pieces are in solve condition
		bool IsSolved(const std::vector<Pgr>&) const;

        // Check if some piece positions are in solve condition
		bool IsSolved(const std::vector<Pcp>&) const;

        // Check if all pieces in the given layer are in solve condition
		bool IsSolved(const Lyr) const;

		// Checks is the main face of given layer has the correct orientation
		bool IsFaceOriented(const Lyr) const;

        // Checks is the main face of given layer has his face or the opposite
        bool IsFaceOriented2(const Lyr) const;
	
        // Get the current sticker in the given sticker position
        Stc GetSticker(const Sps) const;

        // Get the current position for the given sticker
        // Sps GetStickerPosition(const Stc) const;

        // Get the current piece in the given piece position
        Pce GetPiece(const Pcp) const;

        // Get the piece direction in the given piece position
        Pcd GetPieceDirection(const Pcp) const;
		
		// Get the piece orientation in the given piece position
        Pco GetPieceOrientation(const Pcp) const;

        // Get the current position for the given piece
        Pcp GetPiecePosition(const Pce) const;
			
		// Check a piece is in his given piece position (direction not checked)
		bool CheckPosition(const Pcp) const;
		
		// Check a group of pieces are in his given piece positions (direction not checked)
        bool CheckPosition(const Pgr) const;

        // Check a piece orientation in a given piece position
        bool CheckOrientation(const Pcp) const;
		
		 // Check a piece orientation in a given piece position (using faces, slower)
        bool CheckOrientation2(const Pcp) const;

        // Check a group of pieces are orientated
        bool CheckOrientation(const Pgr) const;

        // Check if a list of pieces are orientated
        bool CheckOrientation(const std::vector<Pcp>&) const;

		// Get the solved face of the sticker in the given sticker position
        Fce GetSolvedFace(const Sps P) const { return fce_Stickers[static_cast<int>(P)]; }

        // Get the current sticker face in the given sticker position
        Fce GetFace(const Sps) const;

		// Check if given piece is in given layer
		bool IsPieceInLayer(const Pce, const Lyr) const;

		// Check if given pieces are in given layer
		bool ArePiecesInLayer(const std::vector<Pce>&, const Lyr) const;

		// Functions to check orientation using the faces
        bool EO_POS(const Edp) const; // Edge in given edge position orientated
        bool EO() const; // Check if all edges are oriented
        bool CO_UD(const Cnp) const; // Corner in given corner position orientated UD
        bool CO_FB(const Cnp) const; // Corner in given corner position orientated FB
        bool CO_RL(const Cnp) const; // Corner in given corner position orientated RL
        bool CO() const; // Check if all corners are oriented

        // Get given pieces report for current cube
        std::string GetReport(const Pgr) const;

        const static std::array<Lyr, 6u> ExtLayers; // Array of external layers
        
        // Get a given cube spin as text
        static std::string GetSpinText(const Spn s) { return string_Spins[static_cast<int>(s)]; }

        // Get a given cube piece as text
        static std::string GetPieceText(const Pce p) { return string_Pieces[static_cast<int>(p)]; }

        // Get a given cube piece position as text
        static std::string GetPiecePositionText(const Pcp p) { return string_Pieces[static_cast<int>(p)]; }

        // Get a given cube piece direction as text
        static std::string GetPieceDirectionText(const Pcd pd) { return string_Directions[static_cast<int>(pd)]; }

        // Get a given cube piece orientation as text
        static std::string GetPieceOrientationText(const Pco po) { return string_Directions[static_cast<int>(po)]; }

        // Get a given cube sticker as text
        static std::string GetStickerText(const Stc s) { return string_Stickers[static_cast<int>(s)]; }

        // Get a given cube sticker position as text
        static std::string GetStickerPositionText(const Sps sp) { return string_Stickers[static_cast<int>(sp)]; }

        // Get steps to pass from a start spin to a final spin
        static void GetSpinsSteps(const Spn sS, const Spn sF, Stp& s1, Stp& s2)
        {
            s1 = stp_Turns[static_cast<int>(sS)][static_cast<int>(sF)][0];
            s2 = stp_Turns[static_cast<int>(sS)][static_cast<int>(sF)][1];
        }

		// Return the opposite layer
		static Lyr OppositeLayer(const Lyr Ly) { return lyr_OppositeLayers[static_cast<int>(Ly)]; }
		
		// Return the inner adjacent layer to a external layer
		static Lyr AdjacentLayer(const Lyr Ly) { return lyr_AdjacentLayers[static_cast<int>(Ly)]; }

		// Return the opposite face
		static Fce OppositeFace(const Fce Fc) { return fce_OppositeFaces[static_cast<int>(Fc)]; }

		// Add pieces positions from a group of pieces positions to a list
		static bool AddToPositionsList(const Pgr, std::vector<Pcp>&);

        // Add pieces positions from groups of pieces positions to a list
		static bool AddToPositionsList(const std::vector<Pgr>&, std::vector<Pcp>&);

        // Get the char for representing a layer
		static char GetLayerChar(const Lyr Ly) { return char_Layers[static_cast<int>(Ly)]; }

        // Check if a piece is a center
        static bool IsCenter(const Pce P) { return pct_Pieces[static_cast<int>(P)] == Pct::CENTER; }

        // Check if a piece is an edge
        static bool IsEdge(const Pce P) { return pct_Pieces[static_cast<int>(P)] == Pct::EDGE; }

        // Check if a piece is a corner
        static bool IsCorner(const Pce P) { return pct_Pieces[static_cast<int>(P)] == Pct::CORNER; }

        // Check if a piece position is a center
        static bool IsCenter(const Pcp P) { return pct_Pieces[static_cast<int>(P)] == Pct::CENTER; }

        // Check if a piece position is an edge
        static bool IsEdge(const Pcp P) { return pct_Pieces[static_cast<int>(P)] == Pct::EDGE; }

        // Check if a piece position is a corner
        static bool IsCorner(const Pcp P) { return pct_Pieces[static_cast<int>(P)] == Pct::CORNER; }

        // Check if a layer is external
        static bool IsExternalLayer(const Lyr ly) { return ly == Lyr::U || ly == Lyr::D || ly == Lyr::F || ly == Lyr::B || ly == Lyr::R || ly == Lyr::L; }

        // Check if a layer is internal
        static bool IsInternalLayer(const Lyr ly) { return ly == Lyr::E || ly == Lyr::S || ly == Lyr::M; }

        // Convert piece to edge (check if it's a edge before calling)
        static Edg PceToEdg(const Pce P) { return static_cast<Edg>(static_cast<int>(P) - static_cast<int>(Pce::First_Edge)); }

        // Convert piece to corner (check if it's a corner before calling)
        static Cnr PceToCnr(const Pce P) { return static_cast<Cnr>(static_cast<int>(P) - static_cast<int>(Pce::First_Corner)); }

        // Convert piece position to edge position (check if it's a edge position before calling)
        static Edp PcpToEdp(const Pcp P) { return static_cast<Edp>(static_cast<int>(P) - static_cast<int>(Pcp::First_Edge)); }

        // Convert piece position to corner position (check if it's a corner position before calling)
        static Cnp PcpToCnp(const Pcp P) { return static_cast<Cnp>(static_cast<int>(P) - static_cast<int>(Pcp::First_Corner)); }

        // Convert piece direction to edge direction (check if it's a edge before calling)
        static Edd PcdToEdd(const Pcd P) { return static_cast<Edd>(static_cast<int>(P) - static_cast<int>(Pcd::First_Edge)); }

        // Convert piece direction to corner direction (check if it's a corner before calling)
        static Cnd PcdToCnd(const Pcd P) { return static_cast<Cnd>(static_cast<int>(P) - static_cast<int>(Pcd::First_Corner)); }

        // Convert piece orientation to edge orientation (check if it's a edge before calling)
        static Edo PcoToEdo(const Pco P) { return static_cast<Edo>(static_cast<int>(P) - static_cast<int>(Pco::First_Edge)); }

        // Convert piece orientation to corner orientation (check if it's a corner before calling)
        static Cno PcoToCno(const Pco P) { return static_cast<Cno>(static_cast<int>(P) - static_cast<int>(Pco::First_Corner)); }

        // Convert edge to piece
        static Pce EdgToPce(const Edg P) { return static_cast<Pce>(static_cast<int>(P) + static_cast<int>(Pce::First_Edge)); }

        // Convert corner to piece
        static Pce CnrToPce(const Cnr P) { return static_cast<Pce>(static_cast<int>(P) + static_cast<int>(Pce::First_Corner)); }

        // Convert edge position to piece position
        static Pcp EdpToPcp(const Edp P) { return static_cast<Pcp>(static_cast<int>(P) + static_cast<int>(Pcp::First_Edge)); }

        // Convert edge direction to piece direction
        static Pcd EddToPcd(const Edd P) { return static_cast<Pcd>(static_cast<int>(P) + static_cast<int>(Pcd::First_Edge)); }

        // Convert edge orientation to piece orientation
        static Pco EdoToPco(const Edo P) { return static_cast<Pco>(static_cast<int>(P) + static_cast<int>(Pco::First_Edge)); }

        // Convert edge position to piece position
        static App AepToApp(const Aep P) { return static_cast<App>(static_cast<int>(P) + static_cast<int>(App::First_Edge)); }

        // Convert corner position to piece position
        static Pcp CnpToPcp(const Cnp P) { return static_cast<Pcp>(static_cast<int>(P) + static_cast<int>(Pcp::First_Corner)); }

        // Convert corner direction to piece direction
        static Pcd CndToPcd(const Cnd P) { return static_cast<Pcd>(static_cast<int>(P) + static_cast<int>(Pcd::First_Corner)); }

        // Convert corner orientation to piece orientation
        static Pco CnoToPco(const Cno P) { return static_cast<Pco>(static_cast<int>(P) + static_cast<int>(Pco::First_Corner)); }

        // Convert face to layer
        static Lyr FaceToLayer(const Fce F) { return static_cast<Lyr>(static_cast<int>(Lyr::U) + static_cast<int>(F)); }

        // Convert layer to face (no controls!)
        static Fce LayerToFace(const Lyr L) { return static_cast<Fce>(static_cast<int>(L) - static_cast<int>(Lyr::U)); }

		// Get the layer in up slice for the given spin
        static Lyr GetUpSliceLayer(const Spn sp) { return GetSliceLayer(Slc::U, sp); }
		
		// Get the layer in front slice for the given spin
        static Lyr GetFrontSliceLayer(const Spn sp) { return GetSliceLayer(Slc::F, sp); }
		
		// Get the layer in right slice for the given spin
        static Lyr GetRightSliceLayer(const Spn sp) { return GetSliceLayer(Slc::R, sp); }
		
        // Get the layer in down slice for the given spin
        static Lyr GetDownSliceLayer(const Spn sp) { return GetSliceLayer(Slc::D, sp); }
		
		// Get the layer in back slice for the given spin
        static Lyr GetBackSliceLayer(const Spn sp) { return GetSliceLayer(Slc::B, sp); }
		
		// Get the layer in left slice for the given spin
        static Lyr GetLeftSliceLayer(const Spn sp) { return GetSliceLayer(Slc::L, sp); }
		
		// Get the layer in given slice for the given spin
        static Lyr GetSliceLayer(const Slc sl, const Spn sp) { return lyr_Slice[static_cast<int>(sp)][static_cast<int>(sl)]; }
		
		// Get the piece position from an absolut piece position for the given spin
		static Pcp FromAbsPosition(const App ap, const Spn sp) { return pcp_Absolut[static_cast<int>(sp)][static_cast<int>(ap)]; }
		
		// Get the sticker position from an absolut sticker position for the given spin
		static Sps FromAbsPosition(const Asp as, const Spn sp) { return sps_Absolut[static_cast<int>(sp)][static_cast<int>(as)]; }

		// Get the absolut piece position from an piece position for the given spin
		static App AbsFromPosition(const Pcp ap, const Spn sp) { return app_Positions[static_cast<int>(sp)][static_cast<int>(ap)]; }
		
		// Get the absolut sticker position from an sticker position for the given spin
		static Asp AbsFromPosition(const Sps as, const Spn sp) { return asp_Positions[static_cast<int>(sp)][static_cast<int>(as)]; }
		
	private:
		
		Spn spin; // Cube spin
		
		// Cube x turn
        void x()  { spin = spn_Turns_x[static_cast<int>(spin)]; }

        // Cube x' turn
        void xp() { spin = spn_Turns_xp[static_cast<int>(spin)]; }

        // Cube x2 turn
        void x2() { spin = spn_Turns_x2[static_cast<int>(spin)]; }

        // Cube y turn
        void y()  { spin = spn_Turns_y[static_cast<int>(spin)]; }

        // Cube y' turn
        void yp() { spin = spn_Turns_yp[static_cast<int>(spin)]; }

        // Cube y2 turn
        void y2() { spin = spn_Turns_y2[static_cast<int>(spin)]; }

        // Cube z turn
        void z()  { spin = spn_Turns_z[static_cast<int>(spin)]; }

        // Cube z' turn
        void zp() { spin = spn_Turns_zp[static_cast<int>(spin)]; }

        // Cube z2 turn
        void z2() { spin = spn_Turns_z2[static_cast<int>(spin)]; }

        void U(); void U2(); void Up(); // Cube U slice movements
        void D(); void D2(); void Dp(); // Cube D slice movements
        void F(); void F2(); void Fp(); // Cube F slice movements
        void B(); void B2(); void Bp(); // Cube B slice movements
        void R(); void R2(); void Rp(); // Cube R slice movements
        void L(); void L2(); void Lp(); // Cube L slice movements

		// Arrays to follow spins when a cube has a turn
        const static Spn spn_Turns_x[24];
        const static Spn spn_Turns_x2[24];
        const static Spn spn_Turns_xp[24];
        const static Spn spn_Turns_y[24];
        const static Spn spn_Turns_y2[24];
        const static Spn spn_Turns_yp[24];
        const static Spn spn_Turns_z[24];
        const static Spn spn_Turns_z2[24];
        const static Spn spn_Turns_zp[24];

        // Two turns array for get an spin from an inital spin
        const static Stp stp_Turns[24][24][2];
		
		// Array to know the solved face of the stickers
		const static Fce fce_Stickers[54];
		
		// Array to know the piece of the stickers
		const static Pce pce_Stickers[54];

		// Array to know the pieces type
		const static Pct pct_Pieces[26];

		// Array to know the opposite layer
		const static Lyr lyr_OppositeLayers[10];
		
		// Array to know the inner adjacent layer to an external layer
		const static Lyr lyr_AdjacentLayers[10];

		// Array to know the opposite face
		const static Fce fce_OppositeFaces[6];

		// Array to know the layer char
		const static char char_Layers[10];

        // Array with spins string representation
        const static std::string string_Spins[24];

        // Array with pieces string representation
        const static std::string string_Pieces[26];

        // Array with stickers string representation
        const static std::string string_Stickers[54];

        // Strings with all possible directions for pieces
        const static std::string string_Directions[6];
		
		// Lyr from spin and slice
		const static Lyr lyr_Slice[24][10];
		
		// Piece positions from spin and absolut piece positions
		const static Pcp pcp_Absolut[24][26];
		
		// Absolute piece positions from spin and piece positions
        const static App app_Positions[24][26];
		
		// Sticker positions from spin and absolut sticker positions
		const static Sps sps_Absolut[24][54];
		
		// Absolut sticker positions from spin and sticker positions
		const static Asp asp_Positions[24][54];
	};
}
