#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <fstream>
#include <ios>
#include <string>
#include "utils.h"
#include "pieces.h"
#include <stack>
#include <unordered_map>
#include <algorithm>
#define NUM_SQUARES 8
#define NUM_START_PIECES 32

using namespace std;

struct MoveInfo {
	bool piece_taken = false;
	Coords coords;
	bool was_first_move = false;
	Piece* piece_moved;
};

struct Boardcell {
	bool hasPiece;
	Piece* piece;
};

class Board {
	Sides bot_side;
	Boardcell board[NUM_SQUARES + 1][NUM_SQUARES + 1];
	unordered_map<Piece*, bool> pieces_status;
	vector<Piece*> pieces_vec;
	stack<MoveInfo> moves;
	stack<Piece*> dead_pieces;
	int queens_alive = 2;
	
	
public:
	Board(Sides side);
	~Board();

	// getters
	const unordered_map<Piece*, bool>& get_pieces_status();
	const stack<Piece*>& get_dead_pieces();
	const int get_alive_queens();
	const int get_score(Sides side);

	// move checks for each piece
	vector<Coords> check_all_moves(Sides side);
	void check_move(Piece& piece, vector<Coords>& moves);
	void pawn_move(Piece& piece, vector<Coords>& moves);
	void rook_move(Piece& piece, vector<Coords>& moves);
	void bishop_move(Piece& piece, vector<Coords>& moves);
	void knight_move(Piece& piece, vector<Coords>& moves);
	void queen_move(Piece& piece, vector<Coords>& moves);
	void king_move(Piece& piece, vector<Coords>& moves);
	bool safe_to_castle(Sides side, Coords coords);
	
	// king safety
	int king_safe_lanes(Piece* king);

	// check functions for move
	bool is_attacked(Piece& piece, Sides side);
	bool is_check(Sides side);


	// move functions
	void move_piece(Coords move_coords);
	void undo_move();


	void print() {
		ofstream out("lot_bot.out");
		for(int i = 1; i <= NUM_SQUARES; ++i) {
			for(int j = 1; j <= NUM_SQUARES; ++j) {
				if(board[i][j].hasPiece) {
					out << " " << board[i][j].piece->get_value() << "." << board[i][j].piece->get_side() << " ";
					//out << "(" << board[i][j].piece->get_x() << "," << board[i][j].piece->get_y() << ") ";
				} else {
					out << " 000.0 ";
				}
			}
			out << endl << endl;
		}
		out.close();
	}
};


#endif