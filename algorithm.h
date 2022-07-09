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

#include <string>
#include <array>
#include <vector>

#include "cube_definitions.h"

namespace grcube3
{
	class Algorithm
	{
	public:
		Algorithm() { }; // Empty algorithm constructor
		Algorithm(const uint n) { Append(n); } // Random algorithm constructor
		Algorithm(const char* s) { const std::string ss(s); Append(ss); } // Algorithm from chars array
		Algorithm(const std::string& s) { Append(s); } // Algorithm from string constructor
		~Algorithm() { } // Destructor
		
		Stp& operator[](const uint pos) { return Movs[pos]; } // Return step at position (read/write)
        Stp At(const uint pos) const { return pos < Movs.size() ? Movs[pos] : Stp::NONE; } // Return step at position (read only)
		bool operator==(const Algorithm&) const; // Equal operator
		bool operator!=(const Algorithm&) const; // Not equal operator
		Algorithm operator+(const Algorithm&); // Algorithm addition operator
		Algorithm operator+(const Stp); // Step addition operator
        Algorithm& operator+=(const Algorithm& rhalg)	{ Append(rhalg); return *this; } // Add algorithm operator
        Algorithm& operator+=(const Stp rhstp) { Append(rhstp); return *this; } // Add step operator
		
		void Clear() { Movs.clear(); } // Clear algorithm
		bool Erase(const uint); // Erase single movement (step) in given position
		bool Erase(const uint, const uint); // Erase movements (steps) in given range
		void EraseLast() { Movs.pop_back(); } // Erase last movememnt
		void EraseFirst() { Movs.erase(Movs.begin()); } // Erase first movement
        bool Empty() const { return Movs.empty() || (Movs.size() == 1u && Movs[0] == Stp::NONE); } // Algorithm is empty
	
		uint GetSize() const { return static_cast<uint>(Movs.size()); } // Get the algorithm length (number of steps in the list)
		uint GetNumSteps() const; // Get the number of steps in the algorithm (parentheses are developed in the count)
		
		bool Find(std::vector<uint>&, const Algorithm&) const; // Searchs a algorithm inside the current algorithm, returns his position(s)
		static Algorithm Compress(const Algorithm&, const Algorithm&); // Returns equivalent algorithm with the subalgorithm expressed with repetitions
		
		// Get metrics
        uint GetHTM() const; // Half turn metric (HTM), also known as face turn metric (FTM)
        uint GetQTM() const; // Quarter turn metric (QTM)
        uint GetSTM() const; // Slice turn metric (STM)
        uint GetQSTM() const; // Quarter slice turn metric (QSTM)
        uint GetETM() const; // Execution turn metric (ETM)
        uint GetATM() const; // Axial turn metric (ATM)
        uint GetPTM() const; // Pacelli turn metric (PTM)
        float Get15HTM() const; // 1.5 half turn metric (1.5HTM)	
        uint GetOBTM() const; // Outer block turn metric (OBTM)
        float GetMetric(const Metrics = Metrics::Movements) const; // Get algorithm metric
		
		int GetParenthesesNesting() const; // Gets parentheses nesting - 0 for parentheses OK
		
		// Gets a subjective score in function of algorithm movements (higher score, worst algorithm)
		uint GetSubjectiveScore() const 
		{ 
			uint s = 0u; 
			for (const auto m : Movs) s += m_scores[static_cast<int>(m)];
			return s;
		}

		Stp First() const { return Movs.empty() ? Stp::NONE : Movs.front(); }  // Get the algorithm first step
		Stp Last() const { return Movs.empty() ? Stp::NONE : Movs.back(); } // Get the algorithm last step
		Stp Penultimate() const { return GetSize() < 2u ? Stp::NONE : Movs[GetSize() - 2u]; } // Gets the penultimate step in the algorithm
		Stp PenultimateInverted() const { return GetSize() < 2u ? Stp::NONE : m_inverted[static_cast<int>(Movs[GetSize() - 2u])]; } // Gets the inverted penultimate step in the algorithm

		// Returns true if given step is in the opposite layer of the last algorithm step
		bool OppositeLayerLastStep(const Stp S) const { return m_layer[static_cast<int>(S)] == m_layer_opposite[static_cast<int>(Last())]; }
		
		bool CheckParentheses() const; // Check if algorithm parentheses are OK
		bool CheckParentheses(bool&) const; // Check if algorithm parentheses are OK and returns by reference if there are parentheses
	
		void Append(const Stp stp) { Movs.push_back(stp); } // Append given step at the end of the algorithm (no shrink)
		bool AppendShrink(const Stp stp) { Movs.push_back(stp);  return ShrinkLast(); } // Append given step at the end of the algorithm and shrink it
		// Append given algorithm at the end of the algorithm (no shrinking)
		void Append(const Algorithm& A) { for (uint n = 0u; n < A.GetSize(); n++) Append(A.At(n)); }
		bool Append(const std::string&); // Append given text algorithm at the end of the algorithm (no shrink)
        void Append(const uint, const Stp = Stp::U, const Stp = Stp::L2); // Append random movements (by default U, D, F, B, R ,L) until get the given size (with shrink)

		bool Insert(const uint, const Stp); // Insert given step in given position
		void InsertParentheses(); // Surround the full algorithm with parentheses
		bool InsertParentheses(const uint, const uint, const uint = 1u); // Surround the given range in parentheses with the given repetitions
		
		std::string ToString(const char = ' ') const; // Return current algorithm as plain text string
		
		Algorithm GetDeveloped(const bool = false) const; // Gets the same algorithm with single or without parentheses -default- (steps inside parentheses developed)
		Algorithm GetSimplified() const; // Gets the same algorithm with single layer movements and turns and no parentheses (should be used before applying the algorithm to a cube)
		Algorithm GetSubAlgorithm(const uint, const uint) const; // Gets a sub-algorithm from an algorithm position with given length
		Algorithm GetWithoutTurns() const; // Returns an algorithm removing x, y and z turns from the algorithm (maintaining the functionality)
		Algorithm GetCancellations() const; // Returns the algorithm with cancellations applied
		Algorithm GetRegrip() const; // Returns the same algorithm with turns for getting most comfortable (subjective) movements
		
		bool Shrink(); // Returns false if the algorithm is shrinked; if returns true apply shrink again
		bool ShrinkLast(); // Shrinks last step only

		void Transform_x() { for (auto& S : Movs) S = m_x[static_cast<int>(S)];	} // Transform the algorithm applying an x turn
		void Transform_xp() { for (auto& S : Movs) S = m_xp[static_cast<int>(S)]; } // Transform the algorithm applying an x' turn
		void Transform_x2() { for (auto& S : Movs) S = m_x2[static_cast<int>(S)]; } // Transform the algorithm applying an x2 turn
		void Transform_y() { for (auto& S : Movs) S = m_y[static_cast<int>(S)]; } // Transform the algorithm applying an y turn
		void Transform_yp() { for (auto& S : Movs) S = m_yp[static_cast<int>(S)]; } // Transform the algorithm applying an y' turn
		void Transform_y2() { for (auto& S : Movs) S = m_y2[static_cast<int>(S)]; } // Transform the algorithm applying an y2 turn
		void Transform_z() { for (auto& S : Movs) S = m_z[static_cast<int>(S)]; } // Transform the algorithm applying an z turn
		void Transform_zp() { for (auto& S : Movs) S = m_zp[static_cast<int>(S)]; } // Transform the algorithm applying an z' turn
		void Transform_z2() { for (auto& S : Movs) S = m_z2[static_cast<int>(S)]; } // Transform the algorithm applying an z2 turn

		void TransformTurn(const Stp); // Transform the algorithm applying the given turn

		bool Transform_x(const uint, const uint); // Transform the algorithm applying an x turn to the specified range of steps
		bool Transform_xp(const uint, const uint); // Transform the algorithm applying an x' turn to the specified range of steps
		bool Transform_x2(const uint, const uint); // Transform the algorithm applying an x2 turn to the specified range of steps
		bool Transform_y(const uint, const uint); // Transform the algorithm applying an y turn to the specified range of steps
		bool Transform_yp(const uint, const uint); // Transform the algorithm applying an y' turn to the specified range of steps
		bool Transform_y2(const uint, const uint); // Transform the algorithm applying an y2 turn to the specified range of steps
		bool Transform_z(const uint, const uint); // Transform the algorithm applying an z turn to the specified range of steps
		bool Transform_zp(const uint, const uint); // Transform the algorithm applying an z' turn to the specified range of steps
		bool Transform_z2(const uint, const uint); // Transform the algorithm applying an z2 turn to the specified range of steps

		// Transform the algorithm applying the given turn to the specified range of steps
		bool TransformTurn(const Stp, const uint, const uint);

		bool TransformReverse(); // Changes the steps order
		void TransformInvert(); // Changes every movement for the invert one
		bool TransformReverseInvert(); // Changes the steps order and every movement for the invert one
		void TransformSymmetrical(); // Changes every movement for the symmetrical one
		void TransformInvertMSteps(); // Change M to M' and M' to M
		void TransformInvertESteps(); // Change E to E' and E' to E
		void TransformInvertSSteps(); // Change S to S' and S' to S

		// Gets the close parenthesis step with given repetitions
		static Stp GetCloseParenthesis(const uint rep = 1u)
		{
			return (rep < 1u || rep > 9u) ?
				Stp::NONE : static_cast<Stp>(static_cast<uint>(Stp::PARENTHESIS_OPEN) + rep);
		}

		// Gets the repetitions of the given close parenthesis step
		static uint GetCloseParenthesisRepetitions(const Stp s)
		{
			return (s < Stp::PARENTHESIS_CLOSE_1_REP || s > Stp::PARENTHESIS_CLOSE_9_REP) ?
				0u : static_cast<uint>(s) - static_cast<uint>(Stp::PARENTHESIS_OPEN);
		}

		static bool CheckParentheses(const std::string&); // Check the parentheses in a string (final nesting == 0 and nesting always >= 0)
		static std::string CleanString(const std::string&); // Returns a clean algorithm string (removes not allowed chars)
		static bool AddToStepsList(const Sst, std::vector<Stp>&); // Add given steps group to the steps list
		// Returns true if given steps are in opposite layers (and steps are single layer ones <- Commented for speed) 
		static bool OppositeSteps(const Stp S1, const Stp S2) {	return (m_layer[static_cast<int>(S1)] == m_layer_opposite[static_cast<int>(S2)]) /* &&
			                                                               (m_range[static_cast<int>(S1)] == Ranges::SINGLE)*/; }
		static std::string GetTextStep(const Stp s) { return m_strings[static_cast<uint>(s)]; }
        static Stp InvertedStep(const Stp s) { return m_inverted[static_cast<uint>(s)]; }

		static bool SameAxisSteps(const Stp, const Stp); // Returns true if the given steps turns around the same axis
		
		static Rng GetRange(const Stp s) { return m_range[static_cast<int>(s)]; }
		static Lyr GetLayer(const Stp s) { return m_layer[static_cast<int>(s)]; }
		
        static std::string GetMetricString(const Metrics m) { return metric_strings[static_cast<int>(m)]; }
        static std::string GetMetricValue(const float m)
        {
            std::string str = std::to_string (m);
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            str.erase(str.find_last_not_of('.') + 1, std::string::npos);
            str.erase(str.find_last_not_of(',') + 1, std::string::npos);
            return str;
        }
		
		static bool IsTurn(const Stp t) { return m_range[static_cast<int>(t)] == Rng::TURN; }
        static bool IsSingleMov(const Stp m) { return m_range[static_cast<int>(m)] == Rng::SINGLE; }
        static bool IsDoubleMov(const Stp m) { return m_range[static_cast<int>(m)] == Rng::DOUBLE; }
		static bool IsyTurn(const Stp s) { return (s == Stp::y) || (s == Stp::yp) || (s == Stp::y2); }
		static bool IsUMov(const Stp s) { return (s == Stp::U) || (s == Stp::Up) || (s == Stp::U2); }
		
		static bool JoinSteps(const Stp, const Stp, Stp&); // Join two steps, return true if the join has been done
		static bool JoinSteps(const Stp, const Stp, const Stp, Stp&); // Join two steps with a middle step, return true if the join has been done

		static uint GetCancellation2Size() { return static_cast<uint>(Cancellations2.size()); }
		static uint GetCancellation3Size() { return static_cast<uint>(Cancellations3.size()); }
		
		// static arrays with all turns, y turns, all single movements and U slice movements
		const static std::array<Stp, 9u> Turns;
		const static std::array<Stp, 4u> yTurns, UMovs, DMovs, EMovs;
		const static std::array<Stp, 18u> SingleMovs;

	private:
		std::vector<Stp> Movs; // Algorithm movements (steps)
	
		const static Stp m_x[]; // Array for x turn
		const static Stp m_xp[]; // Array for x' turn
		const static Stp m_x2[]; // Array for x2 turn
		const static Stp m_y[]; // Array for y turn
		const static Stp m_yp[]; // Array for y' turn
		const static Stp m_y2[]; // Array for y2 turn
		const static Stp m_z[]; // Array for z turn
		const static Stp m_zp[]; // Array for z' turn
		const static Stp m_z2[]; // Array for z2 turn

		const static Stp m_inverted[]; // Array to get the inverted movements
		const static Stp m_symmetrical[]; // Array to get the symmetrical movements
	
		const static Stp m_sequence[][3]; // Array of equivalent three movements sequence
	
		const static Lyr m_layer[]; // Array for get movements layer
		const static Lyr m_layer_opposite[]; // Array for get movements opposite layer
		
		const static Rng m_range[]; // Array for get movements range
        
        // Metrics
        const static uint m_HTM[]; // Half turn metric (HTM), also known as face turn metric (FTM)
        const static uint m_QTM[]; // Quarter turn metric (QTM)
        const static uint m_STM[]; // Slice turn metric (STM)
        const static uint m_QSTM[]; // Quarter slice turn metric (QSTM)
        const static uint m_ETM[]; // Execution turn metric (ETM)
        const static uint m_ATM[]; // Axial turn metric (ATM)
        const static uint m_PTM[]; // Pacelli turn metric (PTM)
        const static float m_15HTM[]; // 1.5 half turn metric (1.5HTM)	
        const static uint m_OBTM[]; // Outer block turn metric (OBTM)
	
		const static std::array<char, 33u> m_chars; // Array with allowed chars in an algorithm
	
		const static std::string m_strings[]; // Array with steps string representation
		
		const static std::string metric_strings[]; // Array with metrics string representation
		
		const static uint m_scores[]; // Array with subjective score for each movement

		const static std::vector<Algorithm> Cancellations2, Cancellations3; // Cancellations
		
		static Stp GetStepFromString(const std::string&, uint, uint&); // Get a step inside the string

		// Load a collection of cancelllation algorithms from an XML file
		static std::vector<Algorithm> LoadXMLCancellationsFile(const std::string&);
	};
}
