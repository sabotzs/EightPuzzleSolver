#include "State.h"
#include "Utility.h"
#include <iostream>
#include <chrono>
#include <numeric>

//
// A function that generates the path from the initial state
// to the goal state by using the already generated parent table
// which stores the relations between the states
//
StateList generate_path(const State& initial, const State& goal, ParentTable& parent)
{
	StateList path{ goal };
	State current = goal;

	while (!(current == initial)) {
		current = parent[current];
		path.push_front(current);
	}
	return path;
}

//
// Polymorphic implementation of the Best First Search algorithm.
// It can use any kind of structure, that compares two states,
// based on their value from a heuristic function.
//
template <typename Comparator>
StateList best_first_search(const State& initial) {
	StatePriorityQueue<Comparator> queue;
	ParentTable parent;
	StateSet visited;

	queue.push(initial);
	visited.insert(initial);

	while (!queue.empty()) {
		State current = queue.top();
		queue.pop();

		if (current.is_goal()) {
			return generate_path(initial, current, parent);
		}

		auto successors = current.generate_moves();
		for (auto& succ : successors) {
			// If we haven't already visited the state
			if (visited.find(succ) == visited.end()) {
				parent[succ] = current;
				visited.insert(succ);
				queue.push(succ);
			}
		}
	}
	return StateList();
}

//
// Polimorphic implementation of the A* Search algorithm.
// It can use any kind of structure, that compares two states,
// based on their value from a heuristic function.
//
template <typename Comparator>
StateList a_star_search(const State& initial) {
	ParentTable parent;
	CostTable moves;
	Comparator cmp(&moves);
	StatePriorityQueue<Comparator> queue(cmp);

	moves[initial] = 0;
	queue.push(initial);

	while (!queue.empty()) {
		State current = queue.top();
		queue.pop();

		if (current.is_goal()) {
			return generate_path(initial, current, parent);
		}

		auto successors = current.generate_moves();
		size_t succ_depth = moves[current] + 1;
		for (auto& succ : successors) {
			if (moves.find(succ) == moves.end()) {
				moves[succ] = succ_depth;
				parent[succ] = current;
				queue.push(succ);
			}
			else if (moves[succ] > succ_depth) {
				moves[succ] = succ_depth;
				parent[succ] = current;
			}
		}
	}
	return StateList();
}

SearchResults run_search_on_state(Search search, const State& state) {
	auto start = std::chrono::high_resolution_clock::now();
	size_t moves = search(state).size() - 1;
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	return SearchResults(moves, duration.count());
}

std::pair<double, double> get_average(std::list<SearchResults>& list) {
	SearchResults sum = std::accumulate(list.begin(), list.end(), SearchResults(0, 0),
		[](SearchResults& p1, SearchResults& p2) {
			return SearchResults(p1.first + p2.first, p1.second + p2.second);
		});
	return std::make_pair(double(sum.first) / list.size(), double(sum.second) / list.size());
}

int main() {
	std::list<SearchResults> bf_manh;
	std::list<SearchResults> bf_nipc;
	std::list<SearchResults> as_manh;
	std::list<SearchResults> as_nipc;
	size_t COUNT_OF_TRIALS = 100;
	
	for (size_t trials_made = 0; trials_made < COUNT_OF_TRIALS; ++trials_made) {
		State state;
		while (!state.is_solvable()) {
			state = State();
		}
		bf_manh.push_back(run_search_on_state(best_first_search<ManhattanDistanceComparator>, state));
		bf_nipc.push_back(run_search_on_state(best_first_search<NotInPlaceCountComparator>, state));
		as_manh.push_back(run_search_on_state(a_star_search<ManhattanDistanceComparator>, state));
		as_nipc.push_back(run_search_on_state(a_star_search<NotInPlaceCountComparator>, state));
	}
	
	auto bf_manh_avg = get_average(bf_manh);
	std::cout << "Best-First search using Manhattan Distance:\nOn average: Moves: "
		<< bf_manh_avg.first << " | Time: " << bf_manh_avg.second << " ms\n";
	
	auto bf_nipc_avg = get_average(bf_nipc);
	std::cout << "Best-First search using Not In Place Count:\nOn average: Moves: "
		<< bf_nipc_avg.first << " | Time: " << bf_nipc_avg.second << " ms\n";
	
	auto as_manh_avg = get_average(as_manh);
	std::cout << "A* search using Manhattan Distance:\nOn average: Moves: "
		<< as_manh_avg.first << " | Time: " << as_manh_avg.second << " ms\n";
	
	auto as_nipc_avg = get_average(as_nipc);
	std::cout << "A* search using Not In Place Count:\nOn average: Moves: "
		<< as_nipc_avg.first << " | Time: " << as_nipc_avg.second << " ms\n";

	return 0;
}
