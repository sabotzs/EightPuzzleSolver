#include "State.h"
#include <cstdio>
#include <random>
#include <algorithm>

State::State() {
	std::generate(board, board + BOARD_SIZE, [n = 0]() mutable {return n++; });
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(board, board + BOARD_SIZE, gen);
}

State::State(u8 arr[BOARD_SIZE]) {
	std::copy(arr, arr + BOARD_SIZE, board);
}

State& State::operator=(const State& other) {
	if (this != &other) {
		std::copy(other.board, other.board + BOARD_SIZE, board);
	}
	return *this;
}

bool State::operator==(const State& other) const {
	return std::equal(board, board + BOARD_SIZE, other.board);
}

bool State::is_goal() const {
	return std::is_sorted(board, board + BOARD_SIZE);
}

bool State::is_solvable() const {
	size_t inversions = 0;
	for (size_t i = 0; i < BOARD_SIZE; ++i) {
		for (size_t j = i + 1; j < BOARD_SIZE; ++j) {
			if (board[i] && board[j] && board[i] > board[j]) {
				++inversions;
			}
		}
	}
	return !(inversions % 2);
}

std::list<State> State::generate_moves() const {
	std::list<State> result;
	size_t empty_pos = std::find(board, board + BOARD_SIZE, 0) - board;

	if (empty_pos < 6)
		result.push_back(move_up());
	if (empty_pos > 2)
		result.push_back(move_down());
	if (empty_pos % 3 != 0)
		result.push_back(move_right());
	if (empty_pos % 3 != 2)
		result.push_back(move_left());

	return result;
}

size_t State::manhattan_distance() const {
	size_t distance = 0;
	for (size_t i = 0; i < BOARD_SIZE; ++i) {
		if (board[i] != 0) {
			int diff = i - board[i];
			distance += std::abs(diff) / 3 + std::abs(diff) % 3;
		}
	}
	return distance;
}

size_t State::not_in_place_count() const {
	size_t distance = 0;
	for (size_t i = 0; i < BOARD_SIZE; ++i) {
		if (board[i] != i && board[i] != 0) {
			++distance;
		}
	}
	return distance;
}

State State::move_up() const {
	return move_tile(3);
}

State State::move_down() const {
	return move_tile(-3);
}

State State::move_left() const {
	return move_tile(1);
}

State State::move_right() const {
	return move_tile(-1);
}

State State::move_tile(int indent) const {
	u8 new_board[BOARD_SIZE];
	size_t empty_pos = std::find(board, board + BOARD_SIZE, 0) - board;
	std::copy(board, board + BOARD_SIZE, new_board);
	std::swap(new_board[empty_pos], new_board[empty_pos + indent]);
	return State(new_board);
}
