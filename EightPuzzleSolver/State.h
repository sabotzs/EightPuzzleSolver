#ifndef STATE_HEADER_INCLUDED
#define STATE_HEADER_INCLUDED

#include <list>

constexpr size_t BOARD_SIZE = 9;
using u8 = unsigned short;

class State {
	friend struct std::hash<State>;

	u8 board[BOARD_SIZE];
public:
	State();
	State(u8 arr[BOARD_SIZE]);
	State& operator=(const State& other);
	
	bool operator==(const State& other) const;
	bool is_goal() const;
	bool is_solvable() const;
	
	std::list<State> generate_moves() const;
	
	size_t manhattan_distance() const;
	size_t not_in_place_count() const;
private:
	State move_up() const;
	State move_down() const;
	State move_left() const;
	State move_right() const;
	State move_tile(int indent) const;
};

#endif // !STATE_HEADER_INCLUDED