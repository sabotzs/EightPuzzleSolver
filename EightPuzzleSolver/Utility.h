#ifndef UTILITY_HEADER_INCLUDED
#define UTILITY_HEADER_INCLUDED

#include "State.h"
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>

template <typename Comparator>
using StatePriorityQueue = std::priority_queue<State, std::vector<State>, Comparator>;

using StateList = std::list<State>;

using StateSet = std::unordered_set<State>;

using ParentTable = std::unordered_map<State, State>;

using CostTable = std::unordered_map<State, size_t>;

using Search = std::function<StateList(const State&)>;

//
// First component is the count of moves made
// Second component is the time for execution in milliseconds
//
using SearchResults = std::pair<size_t, long long>;

//
// Custom specialization of std::hash
// for hashing states, so they can be used
// as keys for some lookup tables
//
namespace std {
	template<> struct hash<State> {
		size_t operator()(const State& state) const {
			hash<u8> hasher;
			size_t seed = hasher(state.board[0]);
			for (size_t i = 1; i < BOARD_SIZE; ++i) {
				seed ^= hasher(state.board[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
			return seed;
		}
	};
}

//
// Struct for comparing the manhattan distances of two states.
// It stores a pointer to a table, so it can lookup at the depth
// of the states in the graph when performing the A* algorithm.
//
struct ManhattanDistanceComparator {
	CostTable* table;
	ManhattanDistanceComparator(CostTable* _table = nullptr)
		: table(_table) {}
	bool operator()(const State& first, const State& second) {
		size_t first_cost = first.manhattan_distance();
		size_t second_cost = second.manhattan_distance();
		if (table != nullptr) {
			first_cost += (*table)[first];
			second_cost += (*table)[second];
		}
		return first_cost > second_cost;
	}
};

//
// Struct for comparing the counts of tiles that are not in place
// of two states. It stores a pointer to a table, for the same reason
// as to why does the ManhattanDistanceComparator.
//
struct NotInPlaceCountComparator {
	CostTable* table;
	NotInPlaceCountComparator(CostTable* _table = nullptr)
		: table(_table) {}
	bool operator()(const State& first, const State& second) {
		size_t first_cost = first.not_in_place_count();
		size_t second_cost = second.not_in_place_count();
		if (table != nullptr) {
			first_cost += (*table)[first];
			second_cost += (*table)[second];
		}
		return first_cost > second_cost;
	}
};

#endif // !UTILITY_HEADER_DEFINED