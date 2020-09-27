#include "pieces.h"

// getters and setters
int Piece::get_x() { return x; }
void Piece::set_x(int x) { this->x = x; }

int Piece::get_y() { return y; }
void Piece::set_y(int y) { this->y = y; }

bool Piece::has_moved() { return hasMoved; }
void Piece::set_has_moved(bool status) { this->hasMoved = status; };

PieceType Piece::get_piece_type() { return piece_type; }
void Piece::set_piece_type(PieceType type) { this->piece_type = type; }

Sides Piece::get_side() { return side; }
int Piece::get_value() { return value; }
void Piece::set_value(int value) { this->value = value; }