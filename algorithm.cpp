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

#include <random>

#include "algorithm.h"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace grcube3
{
    const std::vector<Algorithm> Algorithm::Cancellations2 = LoadXMLCancellationsFile("algsets/cancel2.xml"); // Load two steps cancellations
    const std::vector<Algorithm> Algorithm::Cancellations3 = LoadXMLCancellationsFile("algsets/cancel3.xml"); // Load three steps cancellations

	// Array with subjective score for each movement (0 = No influence or very pleasant movement, 10 = very annoying movement)
	const uint Algorithm::m_scores[]
	{
		0u, // No movement

		// Single layer
		1u, 0u, 4u,  // U, U', U2
		5u, 4u, 6u,   // D, D', D2
		0u, 2u, 5u,  // F, F', F2
		6u, 5u, 7u,   // B, B', B2
		0u, 0u, 2u, // R, R', R2
		3u, 2u, 6u,   // L, L', L2

		// Double layer
		5u, 5u, 7u, // u, u', u2
		7u, 7u, 8u, // d, d', d2
		6u, 6u, 8u, // f, f', f2
		7u, 8u, 8u, // b, b', b2
		5u, 6u, 7u, // r, r', r2
		6u, 6u, 7u, // l, l', l2  

		// Middle layers
		9u, 9u, 9u, // E, E', E2
		7u, 8u, 9u,	// S, S', S2
		8u, 2u, 4u,	// M, M', M2

		// Full cube turns
		8u, 8u, 8u, // x, x', x2
		7u, 7u, 7u,	// y, y', y2
		8u, 8u, 8u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};

	const Stp Algorithm::m_inverted[] = // Array to get the inverted movements
	{
		Stp::NONE,
	
		// Single layer
		Stp::Up, Stp::U, Stp::U2,
		Stp::Dp, Stp::D, Stp::D2,
		Stp::Fp, Stp::F, Stp::F2,
		Stp::Bp, Stp::B, Stp::B2,
		Stp::Rp, Stp::R, Stp::R2,
		Stp::Lp, Stp::L, Stp::L2,
	
		// Double layer
		Stp::up, Stp::u, Stp::u2,
		Stp::dp, Stp::d, Stp::d2,
		Stp::fp, Stp::f, Stp::f2,
		Stp::bp, Stp::b, Stp::b2,
		Stp::rp, Stp::r, Stp::r2,
		Stp::lp, Stp::l, Stp::l2,
	
		// Middle layers
		Stp::Ep, Stp::E, Stp::E2,
		Stp::Sp, Stp::S, Stp::S2,
		Stp::Mp, Stp::M, Stp::M2,
	
		// Full cube
		Stp::xp, Stp::x, Stp::x2,
		Stp::yp, Stp::y, Stp::y2,
		Stp::zp, Stp::z, Stp::z2,
	
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 			// "("
		Stp::PARENTHESIS_CLOSE_1_REP,	// ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_symmetrical[] = // Array to get the symmetrical movements
	{
		Stp::NONE,
	
		// Single layer
		Stp::D, Stp::Dp, Stp::D2,
		Stp::U, Stp::Up, Stp::U2,
		Stp::B, Stp::Bp, Stp::B2,
		Stp::F, Stp::Fp, Stp::F2,
		Stp::L, Stp::Lp, Stp::L2,
		Stp::R, Stp::Rp, Stp::R2,
	
		// Double layer
		Stp::d, Stp::dp, Stp::d2,
		Stp::u, Stp::up, Stp::u2,
		Stp::b, Stp::bp, Stp::b2,
		Stp::f, Stp::fp, Stp::f2,
		Stp::l, Stp::lp, Stp::l2,
		Stp::r, Stp::rp, Stp::r2,
	
		// Middle layers
		Stp::E, Stp::Ep, Stp::E2,
		Stp::S, Stp::Sp, Stp::S2,
		Stp::M, Stp::Mp, Stp::M2,
	
		// Full cube
		Stp::x, Stp::xp, Stp::x2,
		Stp::y, Stp::yp, Stp::y2,
		Stp::z, Stp::zp, Stp::z2,
	
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_x[] = // Array for x turn
	{
		Stp::NONE,
	
		// Single layer movements
		Stp::B, Stp::Bp, Stp::B2,
		Stp::F, Stp::Fp, Stp::F2,
		Stp::U, Stp::Up, Stp::U2,
		Stp::D, Stp::Dp, Stp::D2,
		Stp::R, Stp::Rp, Stp::R2,
		Stp::L, Stp::Lp, Stp::L2,
	
		// Double layer movements
		Stp::b, Stp::bp, Stp::b2,
		Stp::f, Stp::fp, Stp::f2,
		Stp::u, Stp::up, Stp::u2,
		Stp::d, Stp::dp, Stp::d2,
		Stp::r, Stp::rp, Stp::r2,
		Stp::l, Stp::lp, Stp::l2,
	
		// Middle layer movements
		Stp::S, Stp::Sp, Stp::S2,
		Stp::Ep, Stp::E, Stp::E2,
		Stp::M, Stp::Mp, Stp::M2,
	
		// Full cube turns
		Stp::x, Stp::xp, Stp::x2,
		Stp::zp, Stp::z, Stp::z2,
		Stp::y, Stp::yp, Stp::y2,
	
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_xp[] = // Array for x' turn
	{
		Stp::NONE,
		
		// Single layer movements
		Stp::F, Stp::Fp, Stp::F2,
		Stp::B, Stp::Bp, Stp::B2,
		Stp::D, Stp::Dp, Stp::D2,
		Stp::U, Stp::Up, Stp::U2,
		Stp::R, Stp::Rp, Stp::R2,
		Stp::L, Stp::Lp, Stp::L2,
		
		// Double layer movements
		Stp::f, Stp::fp, Stp::f2,
		Stp::b, Stp::bp, Stp::b2,
		Stp::d, Stp::dp, Stp::d2,
		Stp::u, Stp::up, Stp::u2,
		Stp::r, Stp::rp, Stp::r2,
		Stp::l, Stp::lp, Stp::l2,
		
		// Middle layer movements
		Stp::Sp, Stp::S, Stp::S2,
		Stp::E, Stp::Ep, Stp::E2,
		Stp::M, Stp::Mp, Stp::M2,
		
		// Full cube turns
		Stp::x, Stp::xp, Stp::x2,
		Stp::z, Stp::zp, Stp::z2,
		Stp::yp, Stp::y, Stp::y2,
		
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_x2[] = // Array for x2 turn
	{
	 	Stp::NONE,
		 
		// Single layer movements
		Stp::D, Stp::Dp, Stp::D2,
		Stp::U, Stp::Up, Stp::U2,
		Stp::B, Stp::Bp, Stp::B2,
		Stp::F, Stp::Fp, Stp::F2,
		Stp::R, Stp::Rp, Stp::R2,
		Stp::L, Stp::Lp, Stp::L2,
		 
		// Double layer movements
		Stp::d, Stp::dp, Stp::d2,
		Stp::u, Stp::up, Stp::u2,
		Stp::b, Stp::bp, Stp::b2,
		Stp::f, Stp::fp, Stp::f2,
		Stp::r, Stp::rp, Stp::r2,
		Stp::l, Stp::lp, Stp::l2,
		
		// Middle layer movements
		Stp::Ep, Stp::E, Stp::E2,
		Stp::Sp, Stp::S, Stp::S2,
		Stp::M, Stp::Mp, Stp::M2,
		
		// Full cube turns
		Stp::x, Stp::xp, Stp::x2,
		Stp::yp, Stp::y, Stp::y2,
		Stp::zp, Stp::z, Stp::z2,
	
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_y[] = // Array for y turn
	{
		Stp::NONE,
	
		// Single layer movements
	    Stp::U, Stp::Up, Stp::U2,
	    Stp::D, Stp::Dp, Stp::D2,
	    Stp::L, Stp::Lp, Stp::L2,
	    Stp::R, Stp::Rp, Stp::R2,
	    Stp::F, Stp::Fp, Stp::F2,
	    Stp::B, Stp::Bp, Stp::B2,
	
	    // Double layer movements
	    Stp::u, Stp::up, Stp::u2,
	    Stp::d, Stp::dp, Stp::d2,
	    Stp::l, Stp::lp, Stp::l2,
	    Stp::r, Stp::rp, Stp::r2,
	    Stp::f, Stp::fp, Stp::f2,
	    Stp::b, Stp::bp, Stp::b2,
	
	    // Middle layers movememts
	    Stp::E, Stp::Ep, Stp::E2,
	    Stp::M, Stp::Mp, Stp::M2,
	    Stp::Sp, Stp::S, Stp::S2,
	
	    // Full cube turns
	    Stp::z, Stp::zp, Stp::z2,
	    Stp::y, Stp::yp, Stp::y2,
	    Stp::xp, Stp::x, Stp::x2,
	
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_yp[] = // Array for y' turn
	{
		Stp::NONE,
		
		// Single layer movements
		Stp::U, Stp::Up, Stp::U2,
		Stp::D, Stp::Dp, Stp::D2,
		Stp::R, Stp::Rp, Stp::R2,
		Stp::L, Stp::Lp, Stp::L2,
		Stp::B, Stp::Bp, Stp::B2,
		Stp::F, Stp::Fp, Stp::F2,
		
		// Double layer movements
		Stp::u, Stp::up, Stp::u2,
		Stp::d, Stp::dp, Stp::d2,
		Stp::r, Stp::rp, Stp::r2,
		Stp::l, Stp::lp, Stp::l2,
		Stp::b, Stp::bp, Stp::b2,
		Stp::f, Stp::fp, Stp::f2,
		
		// Middle layers movememts
		Stp::E, Stp::Ep, Stp::E2,
		Stp::Mp, Stp::M, Stp::M2,
		Stp::S, Stp::Sp, Stp::S2,
		
		// Full cube turns
		Stp::zp, Stp::z, Stp::z2,
		Stp::y, Stp::yp, Stp::y2,
		Stp::x, Stp::xp, Stp::x2,
		
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_y2[] = // Array for y2 turn
	{
		Stp::NONE,
		
		// Single layer movements
		Stp::U, Stp::Up, Stp::U2,
		Stp::D, Stp::Dp, Stp::D2,
		Stp::B, Stp::Bp, Stp::B2,
		Stp::F, Stp::Fp, Stp::F2,
		Stp::L, Stp::Lp, Stp::L2,
		Stp::R, Stp::Rp, Stp::R2,
		
		// Double layer movements
		Stp::u, Stp::up, Stp::u2,
		Stp::d, Stp::dp, Stp::d2,
		Stp::b, Stp::bp, Stp::b2,
		Stp::f, Stp::fp, Stp::f2,
		Stp::l, Stp::lp, Stp::l2,
		Stp::r, Stp::rp, Stp::r2,
		
		// Middle layers movememts
		Stp::E, Stp::Ep, Stp::E2,
		Stp::Sp, Stp::S, Stp::S2,
		Stp::Mp, Stp::M, Stp::M2,
		
		// Full cube turns
		Stp::xp, Stp::x, Stp::x2,
		Stp::y, Stp::yp, Stp::y2,
		Stp::zp, Stp::z, Stp::z2,
		
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_z[] = // Array for z turn
	{
		Stp::NONE,
	
	    // Single layer movements
		Stp::R, Stp::Rp, Stp::R2,
	    Stp::L, Stp::Lp, Stp::L2,
	    Stp::F, Stp::Fp, Stp::F2,
	    Stp::B, Stp::Bp, Stp::B2,
	    Stp::D, Stp::Dp, Stp::D2,
	    Stp::U, Stp::Up, Stp::U2,
	
	    // Double layer movements
	    Stp::r, Stp::rp, Stp::r2,
	    Stp::l, Stp::lp, Stp::l2,
	    Stp::f, Stp::fp, Stp::f2,
	    Stp::b, Stp::bp, Stp::b2,
	    Stp::d, Stp::dp, Stp::d2,
	    Stp::u, Stp::up, Stp::u2,
	
	    // Middle layers movememts
	    Stp::M, Stp::Mp, Stp::M2,
	    Stp::S, Stp::Sp, Stp::S2,
	    Stp::Ep, Stp::E, Stp::E2,
	
	    // Full cube turns
	    Stp::yp, Stp::y, Stp::y2,
	    Stp::x, Stp::xp, Stp::x2,
	    Stp::z, Stp::zp, Stp::z2,
	
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_zp[] = // Array for z' turn
	{
		Stp::NONE,
	
	    // Single layer movements
		Stp::L, Stp::Lp, Stp::L2,
	    Stp::R, Stp::Rp, Stp::R2,
	    Stp::F, Stp::Fp, Stp::F2,
	    Stp::B, Stp::Bp, Stp::B2,
	    Stp::U, Stp::Up, Stp::U2,
	    Stp::D, Stp::Dp, Stp::D2,
	
	    // Double layer movements 
	    Stp::l, Stp::lp, Stp::l2,
	    Stp::r, Stp::rp, Stp::r2,
	    Stp::f, Stp::fp, Stp::f2,
	    Stp::b, Stp::bp, Stp::b2,
	    Stp::u, Stp::up, Stp::u2,
	    Stp::d, Stp::dp, Stp::d2,
	
	    // Middle layers movememts
	    Stp::Mp, Stp::M, Stp::M2,
	    Stp::S, Stp::Sp, Stp::S2,
	    Stp::E, Stp::Ep, Stp::E2,
	
	    // Full cube turns
	    Stp::y, Stp::yp, Stp::y2,
	    Stp::xp, Stp::x, Stp::x2,
	    Stp::z, Stp::zp, Stp::z2,
	
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_z2[] = // Array for z2 turn
	{
		Stp::NONE,
	
	    // Single layer movements
		Stp::D, Stp::Dp, Stp::D2,
	    Stp::U, Stp::Up, Stp::U2,
	    Stp::F, Stp::Fp, Stp::F2,
	    Stp::B, Stp::Bp, Stp::B2,
	    Stp::L, Stp::Lp, Stp::L2,
	    Stp::R, Stp::Rp, Stp::R2,
	
	    // Double layer movements
	    Stp::d, Stp::dp, Stp::d2,
	    Stp::u, Stp::up, Stp::u2,
	    Stp::f, Stp::fp, Stp::f2,
	    Stp::b, Stp::bp, Stp::b2,
	    Stp::l, Stp::lp, Stp::l2,
	    Stp::r, Stp::rp, Stp::r2,
	
	    // Middle layers movememts
	    Stp::Ep, Stp::E, Stp::E2,
	    Stp::S, Stp::Sp, Stp::S2,
	    Stp::Mp, Stp::M, Stp::M2,
	
	    // Full cube turns
	    Stp::xp, Stp::x, Stp::x2,
	    Stp::yp, Stp::y, Stp::y2,
	    Stp::z, Stp::zp, Stp::z2,
	
		// Parentheses --limited to a maximum 9 repetitions--
		Stp::PARENTHESIS_OPEN, 		// "("
		Stp::PARENTHESIS_CLOSE_1_REP, // ")"
		Stp::PARENTHESIS_CLOSE_2_REP,	// ")2"
		Stp::PARENTHESIS_CLOSE_3_REP,	// ")3"
		Stp::PARENTHESIS_CLOSE_4_REP,	// ")4"
		Stp::PARENTHESIS_CLOSE_5_REP,	// ")5"
		Stp::PARENTHESIS_CLOSE_6_REP,	// ")6"
		Stp::PARENTHESIS_CLOSE_7_REP,	// ")7"
		Stp::PARENTHESIS_CLOSE_8_REP,	// ")8"
		Stp::PARENTHESIS_CLOSE_9_REP	// ")9"
	};
	
	const Stp Algorithm::m_sequence[][3] = // Array of equivalent three movements sequence
	{
		{ Stp::NONE, Stp::NONE, Stp::NONE }, // NONE
	
		// Single layer
		{ Stp::U, Stp::NONE, Stp::NONE },    // U
		{ Stp::Up, Stp::NONE, Stp::NONE },   // U'
		{ Stp::U2, Stp::NONE, Stp::NONE },   // U2
		{ Stp::D, Stp::NONE, Stp::NONE },    // D
		{ Stp::Dp, Stp::NONE, Stp::NONE },   // D'
		{ Stp::D2, Stp::NONE, Stp::NONE },   // D2
		{ Stp::F, Stp::NONE, Stp::NONE },    // F
		{ Stp::Fp, Stp::NONE, Stp::NONE },   // F'
		{ Stp::F2, Stp::NONE, Stp::NONE },   // F2
		{ Stp::B, Stp::NONE, Stp::NONE },    // B
		{ Stp::Bp, Stp::NONE, Stp::NONE },   // B'
		{ Stp::B2, Stp::NONE, Stp::NONE },   // B2
		{ Stp::R, Stp::NONE, Stp::NONE },    // R
		{ Stp::Rp, Stp::NONE, Stp::NONE },   // R'
		{ Stp::R2, Stp::NONE, Stp::NONE },   // R2
		{ Stp::L, Stp::NONE, Stp::NONE },    // L
		{ Stp::Lp, Stp::NONE, Stp::NONE },   // L'
		{ Stp::L2, Stp::NONE, Stp::NONE },   // L2
	
		// Double layer, adjacent layers, same direction
		{ Stp::D, Stp::y, Stp::NONE },       // u
		{ Stp::Dp, Stp::yp, Stp::NONE },     // u'
		{ Stp::D2, Stp::y2, Stp::NONE },     // u2
		{ Stp::U, Stp::yp, Stp::NONE },      // d
		{ Stp::Up, Stp::y, Stp::NONE },      // d'
		{ Stp::U2, Stp::y2, Stp::NONE },     // d2
		{ Stp::B, Stp::z, Stp::NONE },       // f
		{ Stp::Bp, Stp::zp, Stp::NONE },     // f'
		{ Stp::B2, Stp::z2, Stp::NONE },     // f2
		{ Stp::F, Stp::zp, Stp::NONE },      // b
		{ Stp::Fp, Stp::z, Stp::NONE },      // b'
		{ Stp::F2, Stp::z2, Stp::NONE },     // b2
		{ Stp::L, Stp::x, Stp::NONE },       // r
		{ Stp::Lp, Stp::xp, Stp::NONE },     // r'
		{ Stp::L2, Stp::x2, Stp::NONE },     // r2
		{ Stp::R, Stp::xp, Stp::NONE },      // l
		{ Stp::Rp, Stp::x, Stp::NONE },      // l'
		{ Stp::R2, Stp::x2, Stp::NONE },     // l2
	
		// Middle layers
		{ Stp::U, Stp::Dp, Stp::yp },        // E -> As layer D
		{ Stp::Up, Stp::D, Stp::y },         // E'
		{ Stp::U2, Stp::D2, Stp::y2 },       // E2
		{ Stp::Fp, Stp::B, Stp::z },         // S -> As layer F
		{ Stp::F, Stp::Bp, Stp::zp },        // S'
		{ Stp::F2, Stp::B2, Stp::z2 },       // S2
		{ Stp::R, Stp::Lp, Stp::xp },        // M -> As layer L
		{ Stp::Rp, Stp::L, Stp::x },         // M'
		{ Stp::R2, Stp::L2, Stp::x2 },       // M2
	
		// Full cube
		{ Stp::x, Stp::NONE, Stp::NONE },    // x
		{ Stp::xp, Stp::NONE, Stp::NONE },   // x'
		{ Stp::x2, Stp::NONE, Stp::NONE },   // x2
		{ Stp::y, Stp::NONE, Stp::NONE },    // y
		{ Stp::yp, Stp::NONE, Stp::NONE },   // y'
		{ Stp::y2, Stp::NONE, Stp::NONE },   // y2
		{ Stp::z, Stp::NONE, Stp::NONE },    // z
		{ Stp::zp, Stp::NONE, Stp::NONE },   // z'
		{ Stp::z2, Stp::NONE, Stp::NONE },   // z2
	
		// Parentheses
		{ Stp::PARENTHESIS_OPEN, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_1_REP, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_2_REP, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_3_REP, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_4_REP, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_5_REP, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_6_REP, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_7_REP, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_8_REP, Stp::NONE, Stp::NONE },
		{ Stp::PARENTHESIS_CLOSE_9_REP, Stp::NONE, Stp::NONE }
	};
	
	const Lyr Algorithm::m_layer[] = // Array for get movements layer
	{
		Lyr::NONE,
	
		// Single layer
		Lyr::U, Lyr::U, Lyr::U,
		Lyr::D, Lyr::D, Lyr::D,
		Lyr::F, Lyr::F, Lyr::F,
		Lyr::B, Lyr::B, Lyr::B,
		Lyr::R, Lyr::R, Lyr::R,
		Lyr::L, Lyr::L, Lyr::L,
	
		// Double layer
		Lyr::U, Lyr::U, Lyr::U,
		Lyr::D, Lyr::D, Lyr::D,
		Lyr::F, Lyr::F, Lyr::F,
		Lyr::B, Lyr::B, Lyr::B,
		Lyr::R, Lyr::R, Lyr::R,
		Lyr::L, Lyr::L, Lyr::L,
	
		// Middle layers
		Lyr::E, Lyr::E, Lyr::E,
		Lyr::S, Lyr::S, Lyr::S,
		Lyr::M, Lyr::M, Lyr::M,
	
		// Full cube
		Lyr::R, Lyr::R, Lyr::R,
		Lyr::U, Lyr::U, Lyr::U,
		Lyr::F, Lyr::F, Lyr::F,
	
		// Parentheses
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE
	};
	
	const Lyr Algorithm::m_layer_opposite[] = // Array for get movements opposite layer
	{
		Lyr::NONE,
	
		// Single layer
		Lyr::D, Lyr::D, Lyr::D,
		Lyr::U, Lyr::U, Lyr::U,
		Lyr::B, Lyr::B, Lyr::B,
		Lyr::F, Lyr::F, Lyr::F,
		Lyr::L, Lyr::L, Lyr::L,
		Lyr::R, Lyr::R, Lyr::R,
	
		// Double layer
		Lyr::D, Lyr::D, Lyr::D,
		Lyr::U, Lyr::U, Lyr::U,
		Lyr::B, Lyr::B, Lyr::B,
		Lyr::F, Lyr::F, Lyr::F,
		Lyr::L, Lyr::L, Lyr::L,
		Lyr::R, Lyr::R, Lyr::R,
	
		// Middle layers
		Lyr::E, Lyr::E, Lyr::E,
		Lyr::S, Lyr::S, Lyr::S,
		Lyr::M, Lyr::M, Lyr::M,
	
		// Full cube
		Lyr::R, Lyr::R, Lyr::R,
		Lyr::U, Lyr::U, Lyr::U,
		Lyr::F, Lyr::F, Lyr::F,
	
		// Parentheses
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE,
		Lyr::NONE
	};
	
	const Rng Algorithm::m_range[] = // Array for get movements range
	{
		Rng::NONE,
	
		// Single layer
		Rng::SINGLE, Rng::SINGLE, Rng::SINGLE,
		Rng::SINGLE, Rng::SINGLE, Rng::SINGLE,
		Rng::SINGLE, Rng::SINGLE, Rng::SINGLE,
		Rng::SINGLE, Rng::SINGLE, Rng::SINGLE,
		Rng::SINGLE, Rng::SINGLE, Rng::SINGLE,
		Rng::SINGLE, Rng::SINGLE, Rng::SINGLE,
	
		// Double layer
		Rng::DOUBLE, Rng::DOUBLE, Rng::DOUBLE,
		Rng::DOUBLE, Rng::DOUBLE, Rng::DOUBLE,
		Rng::DOUBLE, Rng::DOUBLE, Rng::DOUBLE,
		Rng::DOUBLE, Rng::DOUBLE, Rng::DOUBLE,
		Rng::DOUBLE, Rng::DOUBLE, Rng::DOUBLE,
		Rng::DOUBLE, Rng::DOUBLE, Rng::DOUBLE,
	
		// Middle layers
		Rng::INTERNAL, Rng::INTERNAL, Rng::INTERNAL,
		Rng::INTERNAL, Rng::INTERNAL, Rng::INTERNAL,
		Rng::INTERNAL, Rng::INTERNAL, Rng::INTERNAL,
	
		// Full cube
		Rng::TURN, Rng::TURN, Rng::TURN,
		Rng::TURN, Rng::TURN, Rng::TURN,
		Rng::TURN, Rng::TURN, Rng::TURN,
	
		// Parentheses
		Rng::PARENTHESES,
		Rng::PARENTHESES,
		Rng::PARENTHESES,
		Rng::PARENTHESES,
		Rng::PARENTHESES,
		Rng::PARENTHESES,
		Rng::PARENTHESES,
		Rng::PARENTHESES,
		Rng::PARENTHESES,
		Rng::PARENTHESES
	};
    
    // Half turn metric (HTM), also known as face turn metric (FTM)
    const uint Algorithm::m_HTM[] =
    {
		0u, // No movement

		// Single layer
		1u, 1u, 1u, // U, U', U2
		1u, 1u, 1u, // D, D', D2
		1u, 1u, 1u, // F, F', F2
		1u, 1u, 1u, // B, B', B2
		1u, 1u, 1u, // R, R', R2
		1u, 1u, 1u, // L, L', L2

		// Double layer
		1u, 1u, 1u, // u, u', u2
		1u, 1u, 1u, // d, d', d2
		1u, 1u, 1u, // f, f', f2
		1u, 1u, 1u, // b, b', b2
		1u, 1u, 1u, // r, r', r2
		1u, 1u, 1u, // l, l', l2  

		// Middle layers
		2u, 2u, 2u, // E, E', E2
		2u, 2u, 2u,	// S, S', S2
		2u, 2u, 2u,	// M, M', M2

		// Full cube turns
		0u, 0u, 0u, // x, x', x2
		0u, 0u, 0u,	// y, y', y2
		0u, 0u, 0u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};
    
    // Quarter turn metric (QTM)
    const uint Algorithm::m_QTM[] =
    {
		0u, // No movement

		// Single layer
		1u, 1u, 2u, // U, U', U2
		1u, 1u, 2u, // D, D', D2
		1u, 1u, 2u, // F, F', F2
		1u, 1u, 2u, // B, B', B2
		1u, 1u, 2u, // R, R', R2
		1u, 1u, 2u, // L, L', L2

		// Double layer
		1u, 1u, 2u, // u, u', u2
		1u, 1u, 2u, // d, d', d2
		1u, 1u, 2u, // f, f', f2
		1u, 1u, 2u, // b, b', b2
		1u, 1u, 2u, // r, r', r2
		1u, 1u, 2u, // l, l', l2  

		// Middle layers
		2u, 2u, 4u, // E, E', E2
		2u, 2u, 4u,	// S, S', S2
		2u, 2u, 4u,	// M, M', M2

		// Full cube turns
		0u, 0u, 0u, // x, x', x2
		0u, 0u, 0u,	// y, y', y2
		0u, 0u, 0u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};
    
    // Slice turn metric (STM)
    const uint Algorithm::m_STM[] =
    {
		0u, // No movement

		// Single layer
		1u, 1u, 1u, // U, U', U2
		1u, 1u, 1u, // D, D', D2
		1u, 1u, 1u, // F, F', F2
		1u, 1u, 1u, // B, B', B2
		1u, 1u, 1u, // R, R', R2
		1u, 1u, 1u, // L, L', L2

		// Double layer
		1u, 1u, 1u, // u, u', u2
		1u, 1u, 1u, // d, d', d2
		1u, 1u, 1u, // f, f', f2
		1u, 1u, 1u, // b, b', b2
		1u, 1u, 1u, // r, r', r2
		1u, 1u, 1u, // l, l', l2  

		// Middle layers
		1u, 1u, 1u, // E, E', E2
		1u, 1u, 1u,	// S, S', S2
		1u, 1u, 1u,	// M, M', M2

		// Full cube turns
		0u, 0u, 0u, // x, x', x2
		0u, 0u, 0u,	// y, y', y2
		0u, 0u, 0u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};
    
    // Quarter slice turn metric (QSTM)
    const uint Algorithm::m_QSTM[] =
    {
		0u, // No movement

		// Single layer
		1u, 1u, 2u, // U, U', U2
		1u, 1u, 2u, // D, D', D2
		1u, 1u, 2u, // F, F', F2
		1u, 1u, 2u, // B, B', B2
		1u, 1u, 2u, // R, R', R2
		1u, 1u, 2u, // L, L', L2

		// Double layer
		1u, 1u, 2u, // u, u', u2
		1u, 1u, 2u, // d, d', d2
		1u, 1u, 2u, // f, f', f2
		1u, 1u, 2u, // b, b', b2
		1u, 1u, 2u, // r, r', r2
		1u, 1u, 2u, // l, l', l2  

		// Middle layers
		1u, 1u, 2u, // E, E', E2
		1u, 1u, 2u,	// S, S', S2
		1u, 1u, 2u,	// M, M', M2

		// Full cube turns
		0u, 0u, 0u, // x, x', x2
		0u, 0u, 0u,	// y, y', y2
		0u, 0u, 0u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};
    
    // Execution turn metric (ETM)
    const uint Algorithm::m_ETM[] =
    {
		0u, // No movement

		// Single layer
		1u, 1u, 1u, // U, U', U2
		1u, 1u, 1u, // D, D', D2
		1u, 1u, 1u, // F, F', F2
		1u, 1u, 1u, // B, B', B2
		1u, 1u, 1u, // R, R', R2
		1u, 1u, 1u, // L, L', L2

		// Double layer
		1u, 1u, 1u, // u, u', u2
		1u, 1u, 1u, // d, d', d2
		1u, 1u, 1u, // f, f', f2
		1u, 1u, 1u, // b, b', b2
		1u, 1u, 1u, // r, r', r2
		1u, 1u, 1u, // l, l', l2  

		// Middle layers
		1u, 1u, 1u, // E, E', E2
		1u, 1u, 1u,	// S, S', S2
		1u, 1u, 1u,	// M, M', M2

		// Full cube turns
		1u, 1u, 1u, // x, x', x2
		1u, 1u, 1u,	// y, y', y2
		1u, 1u, 1u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};
    
    // Axial turn metric (ATM)
    const uint Algorithm::m_ATM[] =
    {
		0u, // No movement

		// Single layer
		1u, 1u, 1u, // U, U', U2
		1u, 1u, 1u, // D, D', D2
		1u, 1u, 1u, // F, F', F2
		1u, 1u, 1u, // B, B', B2
		1u, 1u, 1u, // R, R', R2
		1u, 1u, 1u, // L, L', L2

		// Double layer
		1u, 1u, 1u, // u, u', u2
		1u, 1u, 1u, // d, d', d2
		1u, 1u, 1u, // f, f', f2
		1u, 1u, 1u, // b, b', b2
		1u, 1u, 1u, // r, r', r2
		1u, 1u, 1u, // l, l', l2  

		// Middle layers
		1u, 1u, 1u, // E, E', E2
		1u, 1u, 1u,	// S, S', S2
		1u, 1u, 1u,	// M, M', M2

		// Full cube turns
		0u, 0u, 0u, // x, x', x2
		0u, 0u, 0u,	// y, y', y2
		0u, 0u, 0u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};
    
    // Pacelli turn metric (PTM)
    const uint Algorithm::m_PTM[] =
    {
		0u, // No movement

		// Single layer
		1u, 1u, 1u, // U, U', U2
		1u, 1u, 1u, // D, D', D2
		1u, 1u, 1u, // F, F', F2
		1u, 1u, 1u, // B, B', B2
		1u, 1u, 1u, // R, R', R2
		1u, 1u, 1u, // L, L', L2

		// Double layer
		0u, 0u, 0u, // u, u', u2
		0u, 0u, 0u, // d, d', d2
		0u, 0u, 0u, // f, f', f2
		0u, 0u, 0u, // b, b', b2
		0u, 0u, 0u, // r, r', r2
		0u, 0u, 0u, // l, l', l2  

		// Middle layers
		1u, 1u, 1u, // E, E', E2
		1u, 1u, 1u,	// S, S', S2
		1u, 1u, 1u,	// M, M', M2

		// Full cube turns
		0u, 0u, 0u, // x, x', x2
		0u, 0u, 0u,	// y, y', y2
		0u, 0u, 0u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};
    
    // 1.5 half turn metric (1.5HTM)
    const float Algorithm::m_15HTM[] =
    {
		0.0f, // No movement

		// Single layer
		1.0f, 1.0f, 1.5f, // U, U', U2
		1.0f, 1.0f, 1.5f, // D, D', D2
		1.0f, 1.0f, 1.5f, // F, F', F2
		1.0f, 1.0f, 1.5f, // B, B', B2
		1.0f, 1.0f, 1.5f, // R, R', R2
		1.0f, 1.0f, 1.5f, // L, L', L2

		// Double layer
		1.0f, 1.0f, 1.5f, // u, u', u2
		1.0f, 1.0f, 1.5f, // d, d', d2
		1.0f, 1.0f, 1.5f, // f, f', f2
		1.0f, 1.0f, 1.5f, // b, b', b2
		1.0f, 1.0f, 1.5f, // r, r', r2
		1.0f, 1.0f, 1.5f, // l, l', l2  

		// Middle layers
		2.0f, 2.0f, 3.0f, // E, E', E2
		2.0f, 2.0f, 3.0f, // S, S', S2
		2.0f, 2.0f, 3.0f, // M, M', M2

		// Full cube turns
		0.0f, 0.0f, 0.0f, // x, x', x2
		0.0f, 0.0f, 0.0f, // y, y', y2
		0.0f, 0.0f, 0.0f, // z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};
    
    // Outer block turn metric (OBTM)
    const uint Algorithm::m_OBTM[] =
    {
		0u, // No movement

		// Single layer
		1u, 1u, 1u, // U, U', U2
		1u, 1u, 1u, // D, D', D2
		1u, 1u, 1u, // F, F', F2
		1u, 1u, 1u, // B, B', B2
		1u, 1u, 1u, // R, R', R2
		1u, 1u, 1u, // L, L', L2

		// Double layer
		1u, 1u, 1u, // u, u', u2
		1u, 1u, 1u, // d, d', d2
		1u, 1u, 1u, // f, f', f2
		1u, 1u, 1u, // b, b', b2
		1u, 1u, 1u, // r, r', r2
		1u, 1u, 1u, // l, l', l2  

		// Middle layers
		2u, 2u, 2u, // E, E', E2
		2u, 2u, 2u,	// S, S', S2
		2u, 2u, 2u,	// M, M', M2

		// Full cube turns
		0u, 0u, 0u, // x, x', x2
		0u, 0u, 0u,	// y, y', y2
		0u, 0u, 0u,	// z, z', z2

		// Parentheses --limited to a maximum of 9 repetitions--
		0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u,	0u
	};
	
	// Array with allowed chars in an algorithm
	const std::array<char, 33u> Algorithm::m_chars = 
	{ 
		'U', 'D', 'F', 'B', 'R', 'L',
		'u', 'd', 'f', 'b', 'r', 'l', 'w',
		'E', 'S', 'M',
		'x', 'y', 'z', 'X', 'Y', 'Z',
		'\'', 
		'(', ')',
		'2', '3', '4', '5', '6', '7', '8', '9'
	};
	
	// Array with steps string representation
	const std::string Algorithm::m_strings[]
	{
		"", // No movement
	
		// Single layer
		"U", "U'", "U2",
		"D", "D'", "D2",
		"F", "F'", "F2",
		"B", "B'", "B2",
		"R", "R'", "R2",
		"L", "L'", "L2",
	
		// Double layer
		"u", "u'", "u2",
		"d", "d'", "d2",
		"f", "f'", "f2",
		"b", "b'", "b2",
		"r", "r'", "r2",
		"l", "l'", "l2",
	
		// Middle layers
		"E", "E'", "E2",
		"S", "S'", "S2",
		"M", "M'", "M2",
	
		// Full cube
		"x", "x'", "x2",
		"y", "y'", "y2",
		"z", "z'", "z2",
	
		// Parentheses --limited to a maximum of 9 repetitions--
		"(", ")", ")2", ")3", ")4", ")5", ")6", ")7", ")8", ")9"
	};
	
	// Array with metrics string representation
	const std::string Algorithm::metric_strings[] =
	{
		"Movs", "HTM", "QTM", "STM", "QSTM", "ETM", "ATM", "PTM", "1.5HTM", "OBTM"
	};

	// Array with all turns
	const std::array<Stp, 9u> Algorithm::Turns = { Stp::x, Stp::x2, Stp::xp, Stp::y, Stp::y2, Stp::yp, Stp::z, Stp::z2, Stp::zp };

	// Array with y turns
    const std::array<Stp, 4u> Algorithm::yTurns = { Stp::NONE, Stp::y, Stp::y2, Stp::yp };
	
	// Array with all single movements
	const std::array<Stp, 18u> Algorithm::SingleMovs = { Stp::U, Stp::U2, Stp::Up, Stp::D, Stp::D2, Stp::Dp,
                                                         Stp::F, Stp::F2, Stp::Fp, Stp::B, Stp::B2, Stp::Bp,
                                                         Stp::R, Stp::R2, Stp::Rp, Stp::L, Stp::L2, Stp::Lp };

    // Array with U slice movements
    const std::array<Stp, 4u> Algorithm::UMovs = { Stp::NONE, Stp::U, Stp::U2, Stp::Up };
	
	// Array with D slice movements
    const std::array<Stp, 4u> Algorithm::DMovs = { Stp::NONE, Stp::D, Stp::D2, Stp::Dp };

	// Array with E slice movements
	const std::array<Stp, 4u> Algorithm::EMovs = { Stp::NONE, Stp::E, Stp::E2, Stp::Ep };
	
	// Equal operator
	bool Algorithm::operator==(const Algorithm& rhalg) const
	{
		const uint size = GetSize();
		if (size != rhalg.GetSize()) return false;
		for (uint i = 0u; i < size; i++) if (Movs[i] != rhalg.At(i)) return false;
		return true;
	}

	// Not equal operator
	bool Algorithm::operator!=(const Algorithm& rhalg) const
	{
		const uint size = GetSize();
		if (size != rhalg.GetSize()) return true;
		for (uint i = 0u; i < size; i++) if (Movs[i] != rhalg.At(i)) return true;
		return false;
	}

	// Addition operator
	Algorithm Algorithm::operator+(const Algorithm& rhalg)
	{
		Algorithm Aux = *this;
		for (uint i = 0u; i < rhalg.GetSize(); i++) Aux.Append(rhalg.At(i));
		return Aux;
	}

	 // Step addition operator
	Algorithm Algorithm::operator+(const Stp rhalg)
	{
		Algorithm Aux = *this;
		Aux.Append(rhalg);
		return Aux;
	}
	
	// Erase single movement (step) in given position
	bool Algorithm::Erase(const uint pos) 
	{
        if (pos < GetSize())
        {
            Movs.erase(Movs.begin() + pos);
			return true;
        }
		return false;
    }

	// Erase movements (steps) in given range
	bool Algorithm::Erase(const uint pos, const uint len)
	{
		const uint poslen = pos + len;
		if (pos < GetSize() && poslen <= GetSize())
		{
			Movs.erase(Movs.begin() + pos, Movs.begin() + poslen);
			return true;
		}
		return false;
	}
    
	// Get the number of efective steps in the algorithm (parentheses are developed in the count)
	uint Algorithm::GetNumSteps() const
	{
		uint count = 0u;
		const uint size = GetSize();
		int iStep;

		for (uint i = 0; i < size; i++)
		{
			iStep = static_cast<int>(Movs[i]);
			if (m_range[iStep] != Rng::NONE && m_range[iStep] != Rng::PARENTHESES) count++;
			else if (Movs[i] == Stp::PARENTHESIS_OPEN)
			{
				int nesting = 1;
				Algorithm AlgPar; // Subalgorithm between parentheses
				while (nesting > 0)
				{
					if (++i == size) return 0u; // No count as parentheses are not ok
					iStep = static_cast<int>(Movs[i]);

					if (Movs[i] == Stp::PARENTHESIS_OPEN) nesting++;
					else if (m_range[iStep] == Rng::PARENTHESES) nesting--;

					if (nesting == 0)
						count += GetCloseParenthesisRepetitions(Movs[i]) * AlgPar.GetNumSteps(); // Recursive				
					else AlgPar.Append(Movs[i]);
				}
			}
		}

		return count;
	}

	// Searchs an algorithm inside the current algorithm, returns his position(s)
	bool Algorithm::Find(std::vector<uint>& Pos, const Algorithm& SubAlg) const
	{
		uint SizeAlg = GetSize(), SizeSubAlg = SubAlg.GetSize();
		bool Found;
		Pos.clear();
		
		if (SizeSubAlg > SizeAlg) return false;

		for (uint i = 0u; i <= SizeAlg - SizeSubAlg; i++)
		{
			Found = true;
			for (uint j = 0u; j < SizeSubAlg; j++) if (At(i + j) != SubAlg.At(j)) { Found = false; break; }
			if (Found)
			{
				Pos.push_back(i);
				i += SizeSubAlg - 1u;
			}
		}
		
		return !Pos.empty();
	}

	// Returns equivalent algorithm with the subalgorithm expressed with repetitions
	Algorithm Algorithm::Compress(const Algorithm& Alg, const Algorithm& RepAlg)
	{
		std::vector<uint> Pos;

		if (!Alg.Find(Pos, RepAlg)) return Alg;

		if (Pos.size() == 1u) return Alg;

		Algorithm ACompress;

		uint p = 0u;
		for (uint i = 0u; i < Alg.GetSize(); i++)
		{
			if (p < Pos.size() && i == Pos[p])
			{
				uint rep = 1u;
				for (uint pi = p; pi < Pos.size() - 1u; pi++)
				{
					if (Pos[static_cast<size_t>(pi) + 1u] - Pos[pi] == RepAlg.GetSize()) rep++;
					else break;
				}
				ACompress.Append(RepAlg);
				ACompress[ACompress.GetSize() - 1u] = GetCloseParenthesis(rep);
				i += RepAlg.GetSize() * rep - 1u;
				p += rep;
			}
			else ACompress.Append(Alg.At(i));
		}

		return ACompress;
	}

	// Check if algorithm parentheses are OK (final nesting == 0 and nesting always >= 0)
	bool Algorithm::CheckParentheses() const
	{
		int nesting = 0;

		for (const auto S : Movs)
		{
			if (S == Stp::PARENTHESIS_OPEN) nesting++;
			else if (S > Stp::PARENTHESIS_OPEN) nesting--; // Close parenthesis

			if (nesting < 0) return false; // Nesting must be always greater or equal than zero
		}

		return nesting == 0;
	}
	
	// Check if algorithm parentheses are OK (final nesting == 0 and nesting always >= 0) and returns by reference if there are parentheses
	bool Algorithm::CheckParentheses(bool& HasParentheses) const
	{ 
		int nesting = 0;
		HasParentheses = false;
		
		for (const auto S : Movs)
		{
			if (S == Stp::PARENTHESIS_OPEN) { HasParentheses = true; nesting++; }
			else if (S > Stp::PARENTHESIS_OPEN) { HasParentheses = true; nesting--; }
			
			if (nesting < 0) return false; // Nesting must be always greater or equal than zero
		}
		
		return nesting == 0;
	}
	
	// Check the parentheses in a string (final nesting == 0 and nesting always >= 0) - static
	bool Algorithm::CheckParentheses(const std::string& s)
	{
		int nesting = 0;
		
		for (const auto c : s)
		{
			if (c == '(') nesting++;
			else if (c == ')') nesting--;

			if (nesting < 0) return false;
		}
		
		return nesting == 0;
	}
	
	// Gets parentheses nesting (0 -> parentheses OK, >0 -> more open than close, <0 -> more close than open)
	int Algorithm::GetParenthesesNesting() const
	{ 
		int nesting = 0;
		
        for (const auto S : Movs)
		{
			if (S == Stp::PARENTHESIS_OPEN) nesting++;
			else if (S > Stp::PARENTHESIS_OPEN) nesting--;
		}
		
		return nesting;
	}
	
	// Join two steps, return true if the join has been done - static
	bool Algorithm::JoinSteps(const Stp S1, const Stp S2, Stp& SJ)
	{ 
		const int iS1 = static_cast<int>(S1), iS2 = static_cast<int>(S2);

        if (S1 < Stp::PARENTHESIS_OPEN && S2 < Stp::PARENTHESIS_OPEN && // Steps are not parentheses
            m_layer[iS1] == m_layer[iS2] && // Steps are in the same layer
            m_range[iS1] == m_range[iS2]) // Steps are in the same range
		{
			const int S1_n = (iS1 - 1) % 3, // Examples -> U: S1_n = 0 / U': S1_n = 1 / U2: S1_n = 2
			          S2_n = (iS2 - 1) % 3;
	
			if      ((S1_n == 1 && S2_n == 2) || (S1_n == 2 && S2_n == 1)) SJ = static_cast<Stp>(iS1 - S1_n);
			else if ((S1_n == 0 && S2_n == 2) || (S1_n == 2 && S2_n == 0)) SJ = static_cast<Stp>(iS1 - S1_n + 1);
			else if ((S1_n == 0 && S2_n == 0) || (S1_n == 1 && S2_n == 1)) SJ = static_cast<Stp>(iS1 - S1_n + 2);
			else SJ = Stp::NONE; // ((S1_n == 0 && S2_n == 1) || (S1_n == 1 && S2_n == 0) || (S1_n == 2 && S2_n == 2))
			return true;
		}
		return false;
	}
	
	// Join two steps with a middle step, return true if the join has been done - static
	bool Algorithm::JoinSteps(const Stp S1, const Stp SM, const Stp S2, Stp& SJ)
	{ 
		const int iS1 = static_cast<int>(S1), iSM = static_cast<int>(SM), iS2 = static_cast<int>(S2);

        if (S1 < Stp::PARENTHESIS_OPEN && SM < Stp::PARENTHESIS_OPEN && S2 < Stp::PARENTHESIS_OPEN && // Steps are not parentheses
            m_range[iS1] == Rng::SINGLE &&  m_range[iS2] == Rng::SINGLE && // Steps are in the single external layer range
			(m_range[iSM] == Rng::SINGLE || m_range[iSM] == Rng::DOUBLE) && // Middle step is in the single or double external layer range
			m_layer[iS1] == m_layer_opposite[iSM] && // Middle step has the opposite layer
            m_layer[iS1] == m_layer[iS2]) // Steps are in the same layer
		{
			const int S1_n = (iS1 - 1) % 3, // Examples -> U: S1_n = 0 / F': S1_n = 1 / R2: S1_n = 2
			          S2_n = (iS2 - 1) % 3;
	
			if      ((S1_n == 1 && S2_n == 2) || (S1_n == 2 && S2_n == 1)) SJ = static_cast<Stp>(iS1 - S1_n);
			else if ((S1_n == 0 && S2_n == 2) || (S1_n == 2 && S2_n == 0)) SJ = static_cast<Stp>(iS1 - S1_n + 1);
			else if ((S1_n == 0 && S2_n == 0) || (S1_n == 1 && S2_n == 1)) SJ = static_cast<Stp>(iS1 - S1_n + 2);
			else SJ = Stp::NONE; // ((S1_n == 0 && S2_n == 1) || (S1_n == 1 && S2_n == 0) || (S1_n == 2 && S2_n == 2))
			return true;
		}
		return false;
	}
	
	// Returns false if the algorithm is shrinked - if returns true apply shrink again
	bool Algorithm::Shrink()
	{ 
		bool changes = false;

		for (uint i = 0u; i < GetSize(); i++) if (Movs[i] == Stp::NONE) Erase(i--); // Remove empty steps

		if (GetSize() <= 1u) return false; // No more shrink needed

		Stp JoinedStep;
		uint index = 1u, index_prev = 0u, index_med;

		do // Shrink adjacent steps
		{
			if (JoinSteps(Movs[index_prev], Movs[index], JoinedStep))
            { // Steps have been joined
				if (JoinedStep != Stp::NONE) Insert(index + 1u, JoinedStep); // Add joined step
	
				// Delete steps that have been joined
				Erase(index);
				Erase(index_prev);
				changes = true;
			}
			else
            { // Steps have not been joined, try the next pair
				index++;
				index_prev++;
			}
		}
		while (index < GetSize());
	
		if (GetSize() <= 2u) return changes;
	
		index = 2u, index_prev = 0u, index_med = 1u;
	
		do // Shrink alternate steps
		{
			if (JoinSteps(Movs[index_prev], Movs[index_med], Movs[index], JoinedStep))
            {   // Steps have been joined
				if (JoinedStep != Stp::NONE) Insert(index + 1u, JoinedStep); // Add joined step
	
				// Delete steps that have been joined
				Erase(index);
				Erase(index_prev);
				changes = true;
			}
			else
            { // Steps have not been joined, try the next trio
				index++;
				index_med++;
				index_prev++;
			}
		}
		while (index < GetSize());
	
		return changes;
	}

	// Shrink last step only - returns true if shrink has been done
	bool Algorithm::ShrinkLast()
	{
		if (GetSize() > 1u)
		{
			Stp JoinedStep;
			uint index, index_prev, index_med;

			index = GetSize() - 1u;
			index_prev = index - 1u;

			if (JoinSteps(Movs[index_prev], Movs[index], JoinedStep)) // Adjacent steps
			{ // Stp have been joined
				if (JoinedStep != Stp::NONE) Insert(index + 1u, JoinedStep); // Add joined step

				// Delete steps that have been joined
				Erase(index);
				Erase(index_prev);
				return true;
			}
			else if (GetSize() > 2u) // Non adjacent steps
			{
				index_med = index_prev--;

				if (JoinSteps(Movs[index_prev], Movs[index_med], Movs[index], JoinedStep))
                { // Steps have been joined
					if (JoinedStep != Stp::NONE) Insert(index + 1u, JoinedStep); // Add joined step

					// Delete steps that have been joined
					Erase(index);
					Erase(index_prev);
					return true;
				}
			}
		}
		return false;
	}
	
	// Returns a clean algorithm string (removes not allowed chars, changes "2'" and "'2" for "2", 'Uw' notation to 'u' notation ) - static
	std::string Algorithm::CleanString(const std::string& s)
	{ 
		std::string clean_s;
		for (uint i = 0; i < s.length(); i++)
		{
			for (uint j = 0u; j < m_chars.size(); j++)
			{
				if (s[i] == m_chars[j])
				{ // Allowed char detected
					if (s[i] == 'X') clean_s.push_back('x'); // Turns in lower case
					else if (s[i] == 'Y') clean_s.push_back('y');
					else if (s[i] == 'Z') clean_s.push_back('z');
					else clean_s.push_back(m_chars[j]);
					break;
				}
			}
		}
		
		// "2'" & "'2" is the same than "2"
		size_t c_pos;
		while ((c_pos = clean_s.find("2'")) != std::string::npos)
			clean_s.erase(c_pos + 1, 1); // Erase '\'' char
		while ((c_pos = clean_s.find("'2")) != std::string::npos)
			clean_s.erase(c_pos, 1); // Erase '\'' char
			
		// Replace 'Uw' notation to 'u' notation for double layer movements
		while ((c_pos = clean_s.find("w")) != std::string::npos)
		{
			if (c_pos == 0) clean_s.erase(c_pos, 1); // First char is 'w', erase
			else
			{
				switch (clean_s[c_pos - 1])
				{
					case 'U': clean_s[c_pos - 1] = 'u'; clean_s.erase(c_pos, 1); break;
					case 'D': clean_s[c_pos - 1] = 'd'; clean_s.erase(c_pos, 1); break;
					case 'F': clean_s[c_pos - 1] = 'f'; clean_s.erase(c_pos, 1); break;
					case 'B': clean_s[c_pos - 1] = 'b'; clean_s.erase(c_pos, 1); break;
					case 'R': clean_s[c_pos - 1] = 'r'; clean_s.erase(c_pos, 1); break;
					case 'L': clean_s[c_pos - 1] = 'l'; clean_s.erase(c_pos, 1); break;
					default: clean_s.erase(c_pos, 1); break;
				}
			}
		}
		
		return clean_s;
	}
	
	 // Reads steps from plain text and append them to the algorithm
	bool Algorithm::Append(const std::string& ss)
	{
		const std::string s = CleanString(ss);
		if (s.length() == 0 || !CheckParentheses(s)) return false;
	
		Stp stp;
		uint pos = 0u, next_pos;
		while ((stp = GetStepFromString(s, pos, next_pos)) != Stp::NONE)
		{
			Append(stp);
			pos = next_pos;
		}
	
		return true;
	}
	
	// Append random movements (between initial step and end step) until get the given size (with shrink)
    void Algorithm::Append(const uint size, const Stp iStep, const Stp eStep)
	{
        if (iStep > eStep) return; // Not valid range of steps

        static std::random_device random_hardware; // Random numbers from hardware
        static std::mt19937 eng(random_hardware()); // Seed the random generator
		std::uniform_int_distribution<> distr(static_cast<int>(iStep), static_cast<int>(eStep)); // Define the range for allowed movements
	
		while (GetSize() < size) AppendShrink(static_cast<Stp>(distr(eng)));
	}
	
	// Insert given step in given position
	bool Algorithm::Insert(const uint pos, const Stp s)
	{
		const uint size = GetSize();
		if (pos < size) Movs.insert(Movs.begin() + pos, s);
		else if (pos == size) Append(s);
		else return false;

		return true;
	}

	// Surround the full algorithm with parentheses
	void Algorithm::InsertParentheses()
	{
		Insert(0u, Stp::PARENTHESIS_OPEN);
		Movs.push_back(Stp::PARENTHESIS_CLOSE_1_REP);
	}
	
	// Surround the given range in parentheses with the given repetitions
	bool Algorithm::InsertParentheses(const uint pos, const uint len, const uint rep)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);
		const Stp s_Aux = GetCloseParenthesis(rep);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses() && s_Aux != Stp::NONE)
		{
			Insert(pos + len, s_Aux);
			Insert(pos, Stp::PARENTHESIS_OPEN);
			return true;
		}

		return false;
	}

	// Get the step in the 'pos' positions inside the string 's' - static
	// 'next_pos' is the index of the following char after the current step
	// String should be a previous cleaned string and with parentheses checked
	Stp Algorithm::GetStepFromString(const std::string& s, uint pos, uint& next_pos)
	{
		const uint s_size = static_cast<uint>(s.length());

		if (s_size == 0u) { next_pos = 0u; return Stp::NONE; }
	
		uint pos_1, pos_2;
		
		while(pos < s_size)
		{
			pos_1 = pos + 1u; // Position increased in 1
			pos_2 = pos + 2u; // Position increased in 2
			
			switch (s[pos])
			{
				case 'U':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Up; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::U2; }
					}
					next_pos = pos_1; return Stp::U;
				case 'D':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Dp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::D2; }
					}
					next_pos = pos_1; return Stp::D;
				case 'F':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Fp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::F2; }
					}
					next_pos = pos_1; return Stp::F;
				case 'B':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Bp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::B2; }
					}
					next_pos = pos_1; return Stp::B;
				case 'R':
				if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Rp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::R2; }
					}
					next_pos = pos_1; return Stp::R;
				case 'L':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Lp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::L2; }
					}
					next_pos = pos_1; return Stp::L;
				case 'u':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::up; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::u2; }
					}
					next_pos = pos_1; return Stp::u;
				case 'd':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::dp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::d2; }
					}
					next_pos = pos_1; return Stp::d;
				case 'f':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::fp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::f2; }
					}
					next_pos = pos_1; return Stp::f;
				case 'b':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::bp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::b2; }
					}
					next_pos = pos_1; return Stp::b;
				case 'r':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::rp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::r2; }
					}
					next_pos = pos_1; return Stp::r;
				case 'l':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::lp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::l2; }
					}
					next_pos = pos_1; return Stp::l;
				case 'E':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Ep; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::E2; }
					}
					next_pos = pos_1; return Stp::E;
				case 'S':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Sp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::S2; }
					}
					next_pos = pos_1; return Stp::S;
				case 'M':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::Mp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::M2; }
					}
					next_pos = pos_1; return Stp::M;
				case 'x':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::xp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::x2; }
					}
					next_pos = pos_1; return Stp::x;
				case 'y':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::yp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::y2; }
					}
					next_pos = pos_1; return Stp::y;
				case 'z':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '\'') { next_pos = pos_2; return Stp::zp; }
						if (s[pos_1] == '2')  { next_pos = pos_2; return Stp::z2; }
					}
					next_pos = pos_1; return Stp::z;
				case '(': next_pos = pos_1; return Stp::PARENTHESIS_OPEN;
				case ')':
					if (pos_1 < s_size)
					{
						if (s[pos_1] == '2') { next_pos = pos_2; return Stp::PARENTHESIS_CLOSE_2_REP; }
						if (s[pos_1] == '3') { next_pos = pos_2; return Stp::PARENTHESIS_CLOSE_3_REP; }
						if (s[pos_1] == '4') { next_pos = pos_2; return Stp::PARENTHESIS_CLOSE_4_REP; }
						if (s[pos_1] == '5') { next_pos = pos_2; return Stp::PARENTHESIS_CLOSE_5_REP; }
						if (s[pos_1] == '6') { next_pos = pos_2; return Stp::PARENTHESIS_CLOSE_6_REP; }
						if (s[pos_1] == '7') { next_pos = pos_2; return Stp::PARENTHESIS_CLOSE_7_REP; }
						if (s[pos_1] == '8') { next_pos = pos_2; return Stp::PARENTHESIS_CLOSE_8_REP; }
						if (s[pos_1] == '9') { next_pos = pos_2; return Stp::PARENTHESIS_CLOSE_9_REP; }
					}
					next_pos = pos_1; return Stp::PARENTHESIS_CLOSE_1_REP;
				default: pos++; break;
			}
		}
		next_pos = pos;
		return Stp::NONE;
	}
	
	// Return current algorithm as plain text string
	std::string Algorithm::ToString(const char sep) const
	{
		std::string s;

        for (const auto step : Movs)
		{
			if (!s.empty() && s.back() == sep && m_range[static_cast<int>(step)] == Rng::PARENTHESES && step != Stp::PARENTHESIS_OPEN) s.pop_back();
			s += m_strings[static_cast<int>(step)];
			if (step != Stp::PARENTHESIS_OPEN) s.push_back(sep);
		}

		if (!s.empty() && s.back() == sep) s.pop_back();
		
		return s;
	}
	
	// Get the same algorithm with single or without parentheses (default) - steps inside parentheses developed
	// Should be used before applying the algorithm to a cube
	Algorithm Algorithm::GetDeveloped(const bool KeepSingleParentheses) const
	{
		Algorithm DA; // Developed algorithm
		
		if (!CheckParentheses()) return DA; // Return empty algorithm
	
		const uint size = GetSize();
		for (uint i = 0u; i < size; i++)
		{
			const int iStep = static_cast<int>(Movs[i]);
			if (m_range[iStep] != Rng::NONE && m_range[iStep] != Rng::PARENTHESES) DA.Append(Movs[i]);
			else if (Movs[i] == Stp::PARENTHESIS_OPEN)
			{
				int nesting = 1;
				Algorithm aux;
				while (nesting > 0 && i < size)
				{
					if (++i < size)
					{
						if (Movs[i] == Stp::PARENTHESIS_OPEN) nesting++;
						else if (m_range[static_cast<int>(Movs[i])] == Rng::PARENTHESES) nesting--;

						if (nesting == 0)
						{
							uint rep = GetCloseParenthesisRepetitions(Movs[i]);
							for (uint n = 0u; n < rep; n++)
							{
								if (KeepSingleParentheses) DA.Append(Stp::PARENTHESIS_OPEN);
								Algorithm AuxA = aux.GetDeveloped(KeepSingleParentheses);
								DA.Append(AuxA);
								if (KeepSingleParentheses) DA.Append(Stp::PARENTHESIS_CLOSE_1_REP);
							}
						}
						else aux.Append(Movs[i]);
					}
				}
			}
		}

		return DA;
	}
	 
	// Gets a sub-algorithm from an algorithm position with given length
	Algorithm Algorithm::GetSubAlgorithm(const uint pos, const uint len) const
	{
	 	Algorithm SA; // Sub-algorithm

	 	if ((pos + len) <= GetSize()) for (uint n = pos; n < pos + len; n++) SA.Append(Movs[n]);
		
		return SA;
	}
	
	 // Gets the same algorithm with single layer movements and turns and no parentheses
	Algorithm Algorithm::GetSimplified() const
	{
		Algorithm SA; // Simplified algorithm
		int iStep;
		
		if (!CheckParentheses()) return SA; // Return empty algorithm

		const uint size = GetSize();

		for (uint i = 0u; i < size; i++)
		{
			iStep = static_cast<int>(Movs[i]);

			if (m_range[iStep] != Rng::PARENTHESES)
			{
				if (m_sequence[iStep][0] != Stp::NONE) SA.Append(m_sequence[iStep][0]);
				if (m_sequence[iStep][1] != Stp::NONE) SA.Append(m_sequence[iStep][1]);
				if (m_sequence[iStep][2] != Stp::NONE) SA.Append(m_sequence[iStep][2]);
			}

			if (Movs[i] == Stp::PARENTHESIS_OPEN)
			{
				int nesting = 1;
				Algorithm aux;
				while (nesting > 0 && ++i < size)
				{
					iStep = static_cast<int>(Movs[i]);

					if (Movs[i] == Stp::PARENTHESIS_OPEN) nesting++;
					else if (m_range[iStep] == Rng::PARENTHESES) nesting--;

					if (nesting == 0)
						for (int n = 0; n < iStep - static_cast<int>(Stp::PARENTHESIS_OPEN); n++)
						{
							Algorithm aux_s = aux.GetSimplified();
							SA.Append(aux_s);
						}
					else aux.Append(Movs[i]);
				}
			}
		}
	
		return SA;
	}

	// Transform the algorithm applying the given turn
	void Algorithm::TransformTurn(const Stp T)
	{
		switch (T)
		{
		case Stp::x: Transform_x(); break;
		case Stp::xp: Transform_xp(); break;
		case Stp::x2: Transform_x2(); break;
		case Stp::y: Transform_y(); break;
		case Stp::yp: Transform_yp(); break;
		case Stp::y2: Transform_y2(); break;
		case Stp::z: Transform_z(); break;
		case Stp::zp: Transform_zp(); break;
		case Stp::z2: Transform_z2(); break;
		default: break;
		}
	}
	
	// Transform the algorithm applying an x turn to the specified range of steps
	bool Algorithm::Transform_x(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_x[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::xp);
			Insert(pos, Stp::x);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying an x' turn to the specified range of steps
	bool Algorithm::Transform_xp(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_xp[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::x);
			Insert(pos, Stp::xp);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying an x2 turn to the specified range of steps
	bool Algorithm::Transform_x2(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_x2[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::x2);
			Insert(pos, Stp::x2);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying an y turn to the specified range of steps
	bool Algorithm::Transform_y(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_y[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::yp);
			Insert(pos, Stp::y);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying an y' turn to the specified range of steps
	bool Algorithm::Transform_yp(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_yp[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::y);
			Insert(pos, Stp::yp);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying an y2 turn to the specified range of steps
	bool Algorithm::Transform_y2(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_y2[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::y2);
			Insert(pos, Stp::y2);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying a z turn to the specified range of steps
	bool Algorithm::Transform_z(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_z[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::zp);
			Insert(pos, Stp::z);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying a z' turn to the specified range of steps
	bool Algorithm::Transform_zp(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_zp[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::z);
			Insert(pos, Stp::zp);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying a z2 turn to the specified range of steps
	bool Algorithm::Transform_z2(const uint pos, const uint len)
	{
		const Algorithm Aux = GetSubAlgorithm(pos, len);

		if (Aux.GetSize() > 0u && Aux.CheckParentheses())
		{
			for (uint n = pos; n < pos + len; n++) Movs[n] = m_z2[static_cast<int>(Movs[n])];
			Insert(pos + len, Stp::z2);
			Insert(pos, Stp::z2);

			return true;
		}

		return false;
	}

	// Transform the algorithm applying the given turn to the specified range of steps
	bool Algorithm::TransformTurn(const Stp T, const uint pos, const uint len)
	{
		switch (T)
		{
		case Stp::x: return Transform_x(pos, len);
		case Stp::xp: return Transform_xp(pos, len);
		case Stp::x2: return Transform_x2(pos, len);
		case Stp::y: return Transform_y(pos, len);
		case Stp::yp: return Transform_yp(pos, len);
		case Stp::y2: return Transform_y2(pos, len);
		case Stp::z: return Transform_z(pos, len);
		case Stp::zp: return Transform_zp(pos, len);
		case Stp::z2: return Transform_z2(pos, len);
		default: return false;
		}
	}
	
	// Changes the steps order
	bool Algorithm::TransformReverse()
	{
		if (!CheckParentheses()) return false;
		
		const uint size = GetSize();
		
		if (size <= 1u) return true;
		
		Stp AuxStep;

        for (uint n = 0u; n < size / 2u; n++)
        {
            AuxStep = Movs[n];
			Movs[n] = Movs[size - 1u - n];
			Movs[size - 1u - n] = AuxStep;
    	}
    	
		// Check if there are parentheses and move them to the right position in the algorithm
		int nest;
		uint j;

		std::vector<bool> ParenthesesControl;

		for (uint i = 0u; i < size; i++) ParenthesesControl.push_back(false); // all false by default

		for (uint i = 0u; i < size - 1u; i++)
		{
			if (m_range[static_cast<int>(Movs[i])] == Rng::PARENTHESES && Movs[i] != Stp::PARENTHESIS_OPEN && !ParenthesesControl[i])
			{
				nest = 1;
				for (j = i + 1u; j < size; j++)
				{
					if (m_range[static_cast<int>(Movs[j])] == Rng::PARENTHESES && Movs[j] != Stp::PARENTHESIS_OPEN) nest++;
					else if (Movs[j] == Stp::PARENTHESIS_OPEN) if (--nest == 0) break;
				}
				AuxStep = Movs[i];
				Movs[i] = Movs[j];
				Movs[j] = AuxStep;
				ParenthesesControl[i] = true;
				ParenthesesControl[j] = true;
			}
		}

		return true;
	}

	// Changes every movement for the invert one
    void Algorithm::TransformInvert() { for (auto &S : Movs) S = m_inverted[static_cast<int>(S)]; }

	// Changes the steps order and every movement for the invert one
	bool Algorithm::TransformReverseInvert()
	{
		TransformInvert();
		return TransformReverse();
	}
	
	// Changes every movement for the symmetrical one
    void Algorithm::TransformSymmetrical() { for (auto& S : Movs) S = m_symmetrical[static_cast<int>(S)]; }

	// Change M to M' and M' to M
	void Algorithm::TransformInvertMSteps()
	{
		for (auto& S : Movs) if (S == Stp::M || S == Stp::Mp) S = m_inverted[static_cast<int>(S)];
	}

	// Change E to E' and E' to E
	void Algorithm::TransformInvertESteps()
	{
		for (auto& S : Movs) if (S == Stp::E || S == Stp::Ep) S = m_inverted[static_cast<int>(S)];
	}

	// Change S to S' and S' to S
	void Algorithm::TransformInvertSSteps()
	{
		for (auto& S : Movs) if (S == Stp::S || S == Stp::Sp) S = m_inverted[static_cast<int>(S)];
	}

    // Remove x, y and z turns from the algorithm (maintaining the functionality)
    // Algorithm must be simplified.
    Algorithm Algorithm::GetWithoutTurns() const
    {
		Algorithm Aux = GetDeveloped(false);
        uint size = Aux.GetSize();
		
        for (int n = size - 1; n >= 0; n--)
        {
            if (m_range[static_cast<int>(Aux[n])] == Rng::TURN)
            {
                switch (Aux[n])
                {
                    case Stp::x:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_xp[static_cast<int>(Aux[i])];
                        break;
                    case Stp::xp:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_x[static_cast<int>(Aux[i])];
                        break;
                    case Stp::x2:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_x2[static_cast<int>(Aux[i])];
                        break;

                    case Stp::y:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_yp[static_cast<int>(Aux[i])];
                        break;
                    case Stp::yp:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_y[static_cast<int>(Aux[i])];
                        break;
                    case Stp::y2:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_y2[static_cast<int>(Aux[i])];
                        break;

                    case Stp::z:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_zp[static_cast<int>(Aux[i])];
                        break;
                    case Stp::zp:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_z[static_cast<int>(Aux[i])];
                        break;
                    case Stp::z2:
                        for (uint i = n + 1; i < size; i++) Aux[i] = m_z2[static_cast<int>(Aux[i])];
                        break;
						
                    default: break;
                }
                Aux.Erase(n); // Clear turn movement
				size--;
            }
        }
        return Aux;
    }

	// Returns the algorithm with cancellations applied
	Algorithm Algorithm::GetCancellations() const 
	{
		Algorithm ACanc, ADev = GetDeveloped();
		while (ADev.Shrink());

		uint i;
		for (i = 0u; i < ADev.GetSize() - 2u; i++)
		{
			bool AllTurns = Algorithm::IsTurn(ADev.At(i)) && Algorithm::IsTurn(ADev.At(i + 1u)) && Algorithm::IsTurn(ADev.At(i + 2u));
			if (SameAxisSteps(ADev.At(i), ADev.At(i + 1u)) || AllTurns)
			{
				if (SameAxisSteps(ADev.At(i), ADev.At(i + 2u)) || AllTurns) // Test three steps cancellations
				{
					bool found = false;
					const Algorithm A3 = ADev.GetSubAlgorithm(i, 3u);
					for (size_t n = 0u; n < Cancellations3.size(); n += 2u)
					{
						if (A3 == Cancellations3[n])
						{
							ACanc += Cancellations3[n + 1u];
							i += 2u;
							found = true;
							break;
						}
					}
					if (!found) // Test two steps cancellations
					{
						const Algorithm A2 = ADev.GetSubAlgorithm(i, 2u);
						for (size_t n = 0u; n < Cancellations2.size(); n += 2u)
						{
							if (A2 == Cancellations2[n])
							{
								ACanc += Cancellations2[n + 1u];
								found = true;
								i++;
								break;
							}
						}
						if (!found) ACanc.Append(ADev.At(i));
					}
				}
				else if (!AllTurns) // Test two steps cancellations
				{
					bool found = false;
					const Algorithm A2 = ADev.GetSubAlgorithm(i, 2u);
					for (size_t n = 0u; n < Cancellations2.size(); n += 2u)
					{
						if (A2 == Cancellations2[n])
						{
							ACanc += Cancellations2[n + 1u];
							found = true;
							i++;
							break;
						}
					}
					if (!found) ACanc.Append(ADev.At(i));
				}
			}
			else ACanc.Append(ADev.At(i));
		}

		if (i == ADev.GetSize() - 2u) // Two steps left
		{
			if (SameAxisSteps(ADev.At(ADev.GetSize() - 2u), ADev.At(ADev.GetSize() - 1u))) // Test the last two steps
			{
				bool found = false;
				const Algorithm A2 = ADev.GetSubAlgorithm(ADev.GetSize() - 2u, 2u);
				for (size_t n = 0u; n < Cancellations2.size(); n += 2u)
				{
					if (A2 == Cancellations2[n])
					{
						ACanc += Cancellations2[n + 1u];
						found = true;
						break;
					}
				}
				if (!found)
				{
					ACanc.Append(ADev.At(ADev.GetSize() - 2u));
					ACanc.Append(ADev.At(ADev.GetSize() - 1u));
				}
			}
			else
			{
				ACanc.Append(ADev.At(ADev.GetSize() - 2u));
				ACanc.Append(ADev.At(ADev.GetSize() - 1u));
			}
		}
		else if (i == ADev.GetSize() - 1u) // One step left
		{
			ACanc.Append(ADev.At(ADev.GetSize() - 1u));
		}

		while (ACanc.Shrink());

		return ACanc;
	}

	// Returns the same algorithm with turns for getting most comfortable (subjective) movements (lower score = best)
	Algorithm Algorithm::GetRegrip() const
	{
		uint MinScore = GetSubjectiveScore(), MinScoreIndex = static_cast<uint>(Turns.size());
	
		if (MinScore == 0u) return Algorithm("");

		for (uint i = 0u; i < static_cast<uint>(Turns.size()); i++)
		{
			Algorithm RegripAlg = *this;
			RegripAlg.TransformTurn(Turns[i]); // No turns added
			uint RegripScore = RegripAlg.GetSubjectiveScore();
			
			if (RegripScore < MinScore)
			{
				MinScore = RegripScore;
				MinScoreIndex = i;
			}
		}

		Algorithm RegripAlg = *this;
		if (MinScoreIndex < static_cast<uint>(Turns.size())) RegripAlg.TransformTurn(Turns[MinScoreIndex], 0u, GetSize()); // Add turns
		return RegripAlg;
	}

	// Add given search steps group to a steps list - static
	bool Algorithm::AddToStepsList(const Sst SS, std::vector<Stp>& SL)
	{
		switch (SS)
		{
		case Sst::SINGLE_ALL:
			SL.push_back(Stp::U); SL.push_back(Stp::Up); SL.push_back(Stp::U2);
			SL.push_back(Stp::D); SL.push_back(Stp::Dp); SL.push_back(Stp::D2);
			SL.push_back(Stp::F); SL.push_back(Stp::Fp); SL.push_back(Stp::F2);
			SL.push_back(Stp::B); SL.push_back(Stp::Bp); SL.push_back(Stp::B2);
			SL.push_back(Stp::R); SL.push_back(Stp::Rp); SL.push_back(Stp::R2);
			SL.push_back(Stp::L); SL.push_back(Stp::Lp); SL.push_back(Stp::L2); return true;

		case Sst::SINGLE_U: SL.push_back(Stp::U); SL.push_back(Stp::Up); SL.push_back(Stp::U2); return true;
		case Sst::SINGLE_D: SL.push_back(Stp::D); SL.push_back(Stp::Dp); SL.push_back(Stp::D2); return true;
		case Sst::SINGLE_F: SL.push_back(Stp::F); SL.push_back(Stp::Fp); SL.push_back(Stp::F2); return true;
		case Sst::SINGLE_B: SL.push_back(Stp::B); SL.push_back(Stp::Bp); SL.push_back(Stp::B2); return true;
		case Sst::SINGLE_R: SL.push_back(Stp::R); SL.push_back(Stp::Rp); SL.push_back(Stp::R2); return true;
		case Sst::SINGLE_L: SL.push_back(Stp::L); SL.push_back(Stp::Lp); SL.push_back(Stp::L2); return true;

        case Sst::SINGLE_M: SL.push_back(Stp::M); SL.push_back(Stp::Mp); SL.push_back(Stp::M2); return true;
        case Sst::SINGLE_S: SL.push_back(Stp::S); SL.push_back(Stp::Sp); SL.push_back(Stp::S2); return true;
        case Sst::SINGLE_E: SL.push_back(Stp::E); SL.push_back(Stp::Ep); SL.push_back(Stp::E2); return true;
		
		case Sst::SINGLE_UR: SL.push_back(Stp::U); SL.push_back(Stp::Up); SL.push_back(Stp::U2);
                                     SL.push_back(Stp::R); SL.push_back(Stp::Rp); SL.push_back(Stp::R2); return true;

        case Sst::SINGLE_UM: SL.push_back(Stp::U); SL.push_back(Stp::Up); SL.push_back(Stp::U2);
                                     SL.push_back(Stp::M); SL.push_back(Stp::Mp); SL.push_back(Stp::M2); return true;

		case Sst::LATERAL_UD: SL.push_back(Stp::F); SL.push_back(Stp::Fp);
			SL.push_back(Stp::B); SL.push_back(Stp::Bp);
			SL.push_back(Stp::R); SL.push_back(Stp::Rp);
			SL.push_back(Stp::L); SL.push_back(Stp::Lp); return true;
		case Sst::LATERAL_FB: SL.push_back(Stp::U); SL.push_back(Stp::Up);
			SL.push_back(Stp::D); SL.push_back(Stp::Dp);
			SL.push_back(Stp::R); SL.push_back(Stp::Rp);
			SL.push_back(Stp::L); SL.push_back(Stp::Lp); return true;
		case Sst::LATERAL_RL: SL.push_back(Stp::U); SL.push_back(Stp::Up);
			SL.push_back(Stp::D); SL.push_back(Stp::Dp);
			SL.push_back(Stp::F); SL.push_back(Stp::Fp);
			SL.push_back(Stp::B); SL.push_back(Stp::Bp); return true;
		
        case Sst::ROUX_SB: // Steps for Roux method second block search
			SL.push_back(Stp::U); SL.push_back(Stp::U2); SL.push_back(Stp::Up);
			SL.push_back(Stp::R); SL.push_back(Stp::R2); SL.push_back(Stp::Rp);
			SL.push_back(Stp::M); SL.push_back(Stp::M2); SL.push_back(Stp::Mp);
            SL.push_back(Stp::r); SL.push_back(Stp::rp); return true;

        case Sst::LATERAL_SINGLE_FBp: // For sequence in second block Roux method
            SL.push_back(Stp::F); SL.push_back(Stp::Bp); return true;
			
		case Sst::PETRUS_EB: // Steps for Petrus method expanded block search
			SL.push_back(Stp::U); SL.push_back(Stp::U2); SL.push_back(Stp::Up);
			SL.push_back(Stp::R); SL.push_back(Stp::R2); SL.push_back(Stp::Rp);
			SL.push_back(Stp::F); SL.push_back(Stp::F2); SL.push_back(Stp::Fp);
			return true;
			
		case Sst::PETRUS_OR_U: // Petrus edges orientation return movements (for U movements)
			SL.push_back(Stp::Fp); SL.push_back(Stp::Sp); SL.push_back(Stp::B); 
			SL.push_back(Stp::fp); SL.push_back(Stp::b); return true;
			
		case Sst::PETRUS_OR_R: // Petrus edges orientation return movements (for R movements)
			SL.push_back(Stp::F); SL.push_back(Stp::S); SL.push_back(Stp::Bp); 
			SL.push_back(Stp::f); SL.push_back(Stp::bp); return true;

		case Sst::PETRUS_UD_R: // Petrus lateral movements for B, R, R' and F'
			SL.push_back(Stp::B); SL.push_back(Stp::Fp);
			SL.push_back(Stp::R); SL.push_back(Stp::Rp); return true;

		case Sst::ZZ_LATERAL: // ZZ Lateral movements for F2L (R, R', L, L')
			SL.push_back(Stp::R); SL.push_back(Stp::Rp);
			SL.push_back(Stp::L); SL.push_back(Stp::Lp); return true;
			
		case Sst::ZZ_LATERAL2: // ZZ Lateral movements for F2L (R, R', R2, L, L', L2)
			SL.push_back(Stp::R2); SL.push_back(Stp::L2);
			SL.push_back(Stp::R); SL.push_back(Stp::Rp);
			SL.push_back(Stp::L); SL.push_back(Stp::Lp); return true;

		case Sst::YRURU_urUR: // YruRU movements for pEO Extension
			SL.push_back(Stp::U); SL.push_back(Stp::U2); SL.push_back(Stp::Up);
			SL.push_back(Stp::R); SL.push_back(Stp::R2); SL.push_back(Stp::Rp);
			SL.push_back(Stp::u); SL.push_back(Stp::u2); SL.push_back(Stp::up);
			SL.push_back(Stp::r); SL.push_back(Stp::r2); SL.push_back(Stp::rp); return true;

		case Sst::YRURU_rUR: // YruRU movements for EO-BF
			SL.push_back(Stp::U); SL.push_back(Stp::U2); SL.push_back(Stp::Up);
			SL.push_back(Stp::R); SL.push_back(Stp::R2); SL.push_back(Stp::Rp);
			SL.push_back(Stp::r); SL.push_back(Stp::r2); SL.push_back(Stp::rp); return true;

		case Sst::YRURU_UR: // YruRU movements for F2L
			SL.push_back(Stp::U); SL.push_back(Stp::U2); SL.push_back(Stp::Up);
			SL.push_back(Stp::R); SL.push_back(Stp::R2); SL.push_back(Stp::Rp); return true;

		case Sst::NAUTILUS_rRUM: // Moveset for Nautilus dFR
			SL.push_back(Stp::U); SL.push_back(Stp::U2); SL.push_back(Stp::Up);
			SL.push_back(Stp::R); SL.push_back(Stp::R2); SL.push_back(Stp::Rp);
			SL.push_back(Stp::M); SL.push_back(Stp::M2); SL.push_back(Stp::Mp);
            SL.push_back(Stp::r); SL.push_back(Stp::r2); SL.push_back(Stp::rp); return true;

		default: return false;
		}
	}

	// Returns true if the given steps turns slices around the same axis - static
	bool Algorithm::SameAxisSteps(const Stp S1, const Stp S2)
	{
		const Lyr L1 = m_layer[static_cast<int>(S1)],
		          L2 = m_layer[static_cast<int>(S2)];

		switch (L1)
		{
			case Lyr::U:
			case Lyr::D:
			case Lyr::E: return L2 == Lyr::U || L2 == Lyr::D || L2 == Lyr::E;
			case Lyr::F:
			case Lyr::B:
			case Lyr::S: return L2 == Lyr::F || L2 == Lyr::B || L2 == Lyr::S;
			case Lyr::R:
			case Lyr::L:
			case Lyr::M: return L2 == Lyr::R || L2 == Lyr::L || L2 == Lyr::M;
			default: return false;
		}
	}

	// Load a collection of cancellation algorithms from an XML file
	std::vector<Algorithm> Algorithm::LoadXMLCancellationsFile(const std::string& XMLFilename)
	{
		std::vector<Algorithm> Canc;

		XMLDocument canc_xml;

		if (canc_xml.LoadFile(XMLFilename.c_str()) != XML_SUCCESS) return Canc; // Read existing file

		XMLNode* pRoot = canc_xml.FirstChild();
		if (pRoot != nullptr)
		{
			XMLElement* pElementCalc = pRoot->FirstChildElement("Cancellation");
			while (pElementCalc != nullptr)
			{
					const char* s_Source = pElementCalc->Attribute("Source");
					const char* s_Target = pElementCalc->Attribute("Target");

					if (s_Source != nullptr && s_Target != nullptr)
					{
						Canc.push_back(Algorithm(s_Source));
						Canc.push_back(Algorithm(s_Target));
					}

				pElementCalc = pElementCalc->NextSiblingElement("Cancellation");
			}
		}
		return Canc;
	}
	
	// Half turn metric (HTM), also known as face turn metric (FTM)
	uint Algorithm::GetHTM() const
	{
        const Algorithm A = GetDeveloped();
        uint HTM = 0u;
        for (uint n = 0u; n < A.GetSize(); n++) HTM += m_HTM[static_cast<int>(A.At(n))];
        return HTM;
    }
    
    // Quarter turn metric (QTM)
    uint Algorithm::GetQTM() const
    {
        const Algorithm A = GetDeveloped();
        uint QTM = 0u;
        for (uint n = 0u; n < A.GetSize(); n++) QTM += m_QTM[static_cast<int>(A.At(n))];
        return QTM;
    }
    
    // Slice turn metric (STM)
    uint Algorithm::GetSTM() const
    {
        const Algorithm A = GetDeveloped();
        uint STM = 0u;
        for (uint n = 0u; n < A.GetSize(); n++) STM += m_STM[static_cast<int>(A.At(n))];
        return STM;
    }
    
    // Quarter slice turn metric (QSTM)
    uint Algorithm::GetQSTM() const
    {
        const Algorithm A = GetDeveloped();
        uint QSTM = 0u;
        for (uint n = 0u; n < A.GetSize(); n++) QSTM += m_QSTM[static_cast<int>(A.At(n))];
        return QSTM;
    }
    
    // Execution turn metric (ETM)
    uint Algorithm::GetETM() const
    {
        const Algorithm A = GetDeveloped();
        uint ETM = 0u;
        for (uint n = 0u; n < A.GetSize(); n++)
        {
            if (IsTurn(A.At(n)) && IsTurn(A.At(n + 1u)))
            {
                ETM++;
                n++;
                continue;
            }
            if (IsSingleMov(A.At(n)) && (OppositeSteps(A.At(n), A.At(n + 1u)) ||
                                         OppositeSteps(A.At(n), InvertedStep(A.At(n + 1u)))))
            {
                ETM++;
                n++;
                continue;
            }
            ETM += m_ETM[static_cast<int>(A.At(n))];
        }
        return ETM;
    }
    
    // Axial turn metric (ATM)
    uint Algorithm::GetATM() const
    {
        const Algorithm A = GetDeveloped();
        uint ATM = 0u;
        for (uint n = 0u; n < A.GetSize(); n++)
        {
            if (IsSingleMov(A.At(n)) && (OppositeSteps(A.At(n), A.At(n + 1u)) ||
                                         OppositeSteps(A.At(n), InvertedStep(A.At(n + 1u)))))
            {
                ATM++;
                n++;
                continue;
            }
            ATM += m_ATM[static_cast<int>(A.At(n))];
        }
        return ATM;
    }
    
    // Pacelli turn metric (PTM)
    uint Algorithm::GetPTM() const
    {
        const Algorithm A = GetDeveloped();
        uint PTM = 0u;
        for (uint n = 0u; n < A.GetSize(); n++)
        {
            if (IsSingleMov(A.At(n)) && (OppositeSteps(A.At(n), A.At(n + 1u)) ||
                                         OppositeSteps(A.At(n), InvertedStep(A.At(n + 1u)))))
            {
                PTM++;
                n++;
                continue;
            }
            PTM += m_PTM[static_cast<int>(A.At(n))];
        }
        return PTM;
    }
    
    // 1.5 half turn metric (1.5HTM)	
    float Algorithm::Get15HTM() const
    {
        const Algorithm A = GetDeveloped();
        float HTM15 = 0u;
        for (uint n = 0u; n < A.GetSize(); n++) HTM15 += m_15HTM[static_cast<int>(A.At(n))];
        return HTM15;
    }
    
    // Outer block turn metric (OBTM)
    uint Algorithm::GetOBTM() const
    {
        const Algorithm A = GetDeveloped();
        uint OBTM = 0u;
        for (uint n = 0u; n < A.GetSize(); n++) OBTM += m_OBTM[static_cast<int>(A.At(n))];
        return OBTM;
    }

    // Get algorithm metric
    float Algorithm::GetMetric(const Metrics M) const
    {
        switch (M)
        {
        case Metrics::HTM: return static_cast<float>(GetHTM());
        case Metrics::QTM: return static_cast<float>(GetQTM());
        case Metrics::STM: return static_cast<float>(GetSTM());
        case Metrics::QSTM: return static_cast<float>(GetQSTM());
        case Metrics::ATM: return static_cast<float>(GetATM());
        case Metrics::ETM: return static_cast<float>(GetETM());
        case Metrics::PTM: return static_cast<float>(GetPTM());
        case Metrics::HTM15: return Get15HTM();
        case Metrics::OBTM: return static_cast<float>(GetOBTM());
        default: return static_cast<float>(GetNumSteps());
        }
    }
}
