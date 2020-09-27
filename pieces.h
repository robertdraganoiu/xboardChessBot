#ifndef PIECES_H
#define PIECES_H

#include <iostream>
#include <ios>
#include <string>
#include "utils.h"
#include <vector>

using namespace std;

#define PAWN_VALUE 100
#define KNIGHT_VALUE 320
#define BISHOP_VALUE 330
#define ROOK_VALUE 500
#define QUEEN_VALUE 900
#define KING_VALUE 999


struct Coords {
	int start_x;
	int start_y;
	int next_x;
	int next_y;
	PieceType promotion = NONE;

	Coords() { }

	Coords (int start_x, int start_y, int next_x, int next_y) {
		this->start_x = start_x;
		this->start_y = start_y;
		this->next_x = next_x;
		this->next_y = next_y;
	}

	void print() {
    	cerr << "start: " << start_x << " " << start_y << endl;
		cerr << "next: " << next_x << " " << next_y << endl;
		cerr << "promotion: " << promotion << endl;
	}
};

class Piece {
protected:
	Sides side;
	int x;
	int y;
	PieceType piece_type;
	int value;
	bool hasMoved = false;
	
public:
	Piece(Sides side, int x, int y) : side(side), x(x), y(y) { }

	int get_x();
	void set_x(int x);
	int get_y();
	void set_y(int y);
	bool has_moved();
	void set_has_moved(bool status);
	PieceType get_piece_type();
	void set_piece_type(PieceType type);
	Sides get_side();
	int get_value();
	void set_value(int value);

	bool operator<(Piece& aux) {
		return value > aux.get_value();
	}
};

class Pawn : public Piece {
	public:
	Pawn(Sides side, int x, int y) : Piece(side, x, y) { 
		piece_type = PAWN;
		this->value = PAWN_VALUE;
	}
};

class Rook : public Piece {
	public:
	Rook(Sides side, int x, int y) : Piece(side, x, y) { 
		piece_type = ROOK;
		this->value = ROOK_VALUE;
	}
};

class Knight : public Piece {
	public:
	Knight(Sides side, int x, int y) : Piece(side, x, y) { 
		piece_type = KNIGHT;
		this->value = KNIGHT_VALUE;
	}
};

class Bishop : public Piece {
	public:
	Bishop(Sides side, int x, int y) : Piece(side, x, y) { 
		piece_type = BISHOP;
		this->value = BISHOP_VALUE;
	}
};

class Queen : public Piece {
	public:
	Queen(Sides side, int x, int y) : Piece(side, x, y) { 
		piece_type = QUEEN;
		this->value = QUEEN_VALUE;
	}
};

class King : public Piece {
	public:
	King(Sides side, int x, int y) : Piece(side, x, y) { 
		piece_type = KING;
		this->value = KING_VALUE;
	}
};

#endif