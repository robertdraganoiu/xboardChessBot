#include "board.h"

using namespace std;

bool pieces_comparator(Piece* p1, Piece* p2) {
	return p1->get_value() < p2->get_value();
}

Board::Board(Sides side) {
	// add pawns
	bot_side = side;
	for(int i = 1; i < NUM_SQUARES + 1; ++i) {
		board[2][i].piece = new Pawn(BLACK, 2, i);
		board[7][i].piece = new Pawn(WHITE, 7, i);
	}

	// black pieces
	board[1][1].piece = new Rook(BLACK, 1, 1);
	board[1][2].piece = new Knight(BLACK, 1, 2);
	board[1][3].piece = new Bishop(BLACK, 1, 3);
	board[1][4].piece = new Queen(BLACK, 1, 4);
	board[1][5].piece = new King(BLACK, 1, 5);
	board[1][6].piece = new Bishop(BLACK, 1, 6);
	board[1][7].piece = new Knight(BLACK, 1, 7);
	board[1][8].piece = new Rook(BLACK, 1, 8);

	// white pieces
	board[8][1].piece = new Rook(WHITE, 8, 1);
	board[8][2].piece = new Knight(WHITE, 8, 2);
	board[8][3].piece = new Bishop(WHITE, 8, 3);
	board[8][4].piece = new Queen(WHITE, 8, 4);
	board[8][5].piece = new King(WHITE, 8, 5);
	board[8][6].piece = new Bishop(WHITE, 8, 6);
	board[8][7].piece = new Knight(WHITE, 8, 7);
	board[8][8].piece = new Rook(WHITE, 8, 8);

	// status for boardcells
	for(int i = 1; i <= NUM_SQUARES; ++i) {
		for(int j = 1; j <= NUM_SQUARES; ++j) {
			if(i < 3 || i > 6) {
				board[i][j].hasPiece = true;
			} else {
				board[i][j].hasPiece = false;
			}
		}
	}

	// add pieces to map
	for(int i = 1; i <= NUM_SQUARES; ++i) {
		pieces_status[board[1][i].piece] = true;
		pieces_status[board[2][i].piece] = true;
		pieces_status[board[7][i].piece] = true;
		pieces_status[board[8][i].piece] = true;

    	pieces_vec.push_back(board[1][i].piece);
		pieces_vec.push_back(board[2][i].piece);
		pieces_vec.push_back(board[7][i].piece);
		pieces_vec.push_back(board[8][i].piece);			
    }

	// sort pieces_vec by ascending value
	sort(pieces_vec.begin(), pieces_vec.end(), pieces_comparator);
}

Board::~Board() {
	for(pair<Piece*, bool> p : pieces_status) {
		delete p.first;
	}
}

// getters
const unordered_map<Piece*, bool>& Board::get_pieces_status() {
	return pieces_status;
}

const stack<Piece*>& Board::get_dead_pieces() {
	return dead_pieces;
}

const int Board::get_alive_queens() {
	return queens_alive;
}

const int Board::get_score(Sides side) {
	int score = 0;
	for(int i = 0; i < pieces_vec.size(); ++i) {
		if(pieces_vec[i]->get_side() != side || !pieces_status[pieces_vec[i]]) {
			continue;
		}
		score += pieces_vec[i]->get_value();
	}
	return score;
}

// move functions
void Board::pawn_move(Piece& piece, vector<Coords>& moves) {
	// if pawn was promoted, treat it as queen
	if(piece.get_piece_type() == QUEEN) {
		queen_move(piece, moves);
		return;
	}

	int x = piece.get_x(), y = piece.get_y();
	int index[2] = {7, 2};
	int add[4] = {x + 1, x - 1, x + 2, x - 2};
	int en_passant_x[2] = {5, 4};
	bool attack_cond[2] = {y > 1, y < 8};
	int attack_next_y[2] = {y - 1, y + 1};
	Sides sides[2] = {BLACK, WHITE};

	for (int i = 0; i < 2; ++i) {
		if (piece.get_side() != sides[i]) { continue; }

		// one step move if possible
		if (!board[add[i]][y].hasPiece) {
			Coords move_coords(x, y, add[i], y);
			// promotion case
			if(x == index[i]) {
				move_coords.promotion = QUEEN;
			}
			moves.push_back(move_coords);
		}
		// first move can have 2 steps
		if (x == index[1 - i]) {
			if (!board[add[i]][y].hasPiece && !board[add[i + 2]][y].hasPiece) {
				moves.push_back(Coords(x, y, add[i + 2], y));
			}
		}

		// if possible attack a piece
		for(int j = 0; j < 2; ++j) {
			if(attack_cond[j]) {
				if(board[add[i]][attack_next_y[j]].hasPiece && board[add[i]][attack_next_y[j]].piece->get_side() == sides[1 - i]) {
					// base attack case
					Coords move_coords(x, y, add[i], attack_next_y[j]);
					// promotion case
					if(x == index[i]) {
						move_coords.promotion = QUEEN;
					}
					moves.push_back(move_coords);
				} else if(!this->moves.empty() && x == en_passant_x[i] && board[x][attack_next_y[j]].hasPiece 
					&& board[x][attack_next_y[j]].piece->get_piece_type() == PAWN && board[x][attack_next_y[j]].piece->get_side() == sides[1 - i]) {
					// en passant case
					// check last move to see if there was a pawn who moved 2 squares	
					MoveInfo last_move = this->moves.top();
					Piece* attacked_piece = board[x][attack_next_y[j]].piece;
					if(last_move.piece_moved != attacked_piece || abs(last_move.coords.next_x - last_move.coords.start_x) < 2) {
						continue;
					}
					moves.push_back(Coords(x, y, add[i], attack_next_y[j]));
				}
			}
		}
		break;
	}
	
}

void Board::rook_move(Piece& piece, vector<Coords>& moves) {
	int x = piece.get_x(), y = piece.get_y();

	int cond[4] = {x - 8, 1 - x, y - 8, 1 - y};  // condition array
	int x_change[4] = {1, -1, 0, 0};		
	int y_change[4] = {0, 0, 1, -1};		// array for checking if the move is valid

	// Check all moves
	for (int i = 0; i < 4; ++i) {
		int new_x = piece.get_x();
		int new_y = piece.get_y();
		while (cond[i] < 0) {
			new_x += x_change[i];
			new_y += y_change[i];
			cond[i]++;
			if (!board[new_x][new_y].hasPiece) {
				moves.push_back(Coords(x, y, new_x, new_y));
				continue;
			}

			if(board[new_x][new_y].piece->get_side() != piece.get_side()) { moves.push_back(Coords(x, y, new_x, new_y)); }
			break;
		}
	}
}

void Board::bishop_move(Piece& piece, vector<Coords>& moves) {
	int x = piece.get_x(), y = piece.get_y();

	int condA[4] = {x - 8, 1 - x, x - 8, 1 - x};
	int condB[4] = {y - 8, y - 8, 1 - y, 1 - y};
	int x_change[4] = {1, -1, 1, -1};
	int y_change[4] = {1, 1, -1, -1};	// array for checking if the move is valid


	// Check all moves
	for (int i = 0; i < 4; ++i) {
		int new_x = piece.get_x();
		int new_y = piece.get_y();
		while (condA[i] < 0 && condB[i] < 0) {
			new_x += x_change[i];
			new_y += y_change[i];
			condA[i]++;
			condB[i]++;
			if (!board[new_x][new_y].hasPiece) {
				moves.push_back(Coords(x, y, new_x, new_y));
				continue;
			}

			if(board[new_x][new_y].piece->get_side() != piece.get_side()) { moves.push_back(Coords(x, y, new_x, new_y)); }
			break;
		}
	}
	
}

void Board::knight_move(Piece& piece, vector<Coords>& moves) {
	int x = piece.get_x(), y = piece.get_y();

	int x_change[8] = {2, 2, -2, -2, 1, -1, 1, -1};
	int y_change[8] = {1, -1, 1, -1, 2, 2, -2, -2};	// array for checking if the move is valid

	// Check all moves, return only the valid ones
	for (int i = 0; i < 8; ++i) {
		int new_x = x + x_change[i];
		int new_y = y + y_change[i];
		if (new_x >= 1 && new_x <= 8 && new_y >= 1 && new_y <= 8) {
			if (!board[new_x][new_y].hasPiece || board[new_x][new_y].piece->get_side() != piece.get_side()) {
				moves.push_back(Coords(x, y, new_x, new_y));
			}
		}
	}
}

void Board::queen_move(Piece& piece, vector<Coords>& moves) {
	rook_move(piece, moves);
	bishop_move(piece, moves);
}

void Board::king_move(Piece& piece, vector<Coords>& moves) {
	int x = piece.get_x(), y = piece.get_y();

	int condA[8] = {1, 1, 1, -1, -1, -1, 0, 0};
	int condB[8] = {0, 1, -1, 0, 1, -1, 1, -1};

	// Check all moves, return only the valid ones
	for (int i = 0; i < 8; ++i) {
		int new_x = x + condA[i];
		int new_y = y + condB[i];

		if (new_x >= 1 && new_x <= 8 && new_y >= 1 && new_y <= 8) {
			if (!board[new_x][new_y].hasPiece || board[new_x][new_y].piece->get_side() != piece.get_side()) {
				moves.push_back(Coords(x, y, new_x, new_y));
			}
		}
	}
	
	// castle check
	if(!piece.has_moved()) {
		int castle_x[4] = {8, 8, 1, 1};
		int castle_y12[4] = {1, 8, 1, 8};
		int castle_y3[4] = {2, 7, 2, 7};
		int castle_y4[4] = {3, 6, 3, 6};
		int castle_move_y[4] = {3, 7, 3, 7};
		bool extra_cond[4] = {!board[8][4].hasPiece, true, !board[1][4].hasPiece, true};
		Sides castle_sides[4] = {WHITE, WHITE, BLACK, BLACK};

		for(int i = 0; i < 4; ++i) {
			// skip if not piece side
			if(castle_sides[i] != piece.get_side()) {
				continue;
			}
			// check squares
			if (board[castle_x[i]][castle_y12[i]].hasPiece && !board[castle_x[i]][castle_y12[i]].piece->has_moved()) {
                // verify safety of king passage squares
				if (!board[castle_x[i]][castle_y3[i]].hasPiece && !board[castle_x[i]][castle_y4[i]].hasPiece && extra_cond[i]) {
                    Coords coords(x, y, castle_x[i], castle_move_y[i]);
					if(safe_to_castle(piece.get_side(), coords)) {
                        moves.push_back(coords);
                    }
				}
			}
		}
	}
}

// check safety of king passage squares
bool Board::safe_to_castle(Sides side, Coords coords) {
	int squares_x = coords.start_x;
	int squares_y[2] = {coords.start_y, coords.start_y};

	// update second square to verify based castle side
	coords.start_y < coords.next_y ? squares_y[1]++ : squares_y[1]--;

	for(pair<Piece*, bool> piece_status : pieces_status) {
		if(piece_status.second && piece_status.first->get_side() != side && piece_status.first->get_piece_type() != KING) {
			// get enemy piece moves
			std::vector<Coords> aux_moves;
			check_move(*piece_status.first, aux_moves);

			// check if moves match piece position
			std::vector<Coords>::iterator it;
			for (it = aux_moves.begin(); it != aux_moves.end(); it++) {
				if (it->next_x == squares_x && (it->next_y == squares_y[0] || it->next_y == squares_y[1])) {
					return false;
				}
			}
		}
	}
	return true;
}

int Board::king_safe_lanes(Piece* king) {
	int x = king->get_x();
	int y = king->get_y();
	int side = king->get_side();
	int x_add;
	side == WHITE ? x_add = -1 : x_add = 1;
	int count = 0;
	for(int i = y - 1 ; i <= y + 1; ++i) {
		cout << i << " ";
		if(i > 8 && i < 1) {
			count++;
			continue;
		}
		bool lane_safe = false;
		for(int j = x_add; j != x_add * 3; j += x_add) {
			cout << "j ";
			if(x + j < 1 || x + j > 8) {
				lane_safe = true;
				continue;
			}
			if(board[x + j][i].hasPiece && board[x + j][i].piece->get_side() == side) {
				lane_safe = true;
				continue;
			}
		}
		if(lane_safe) {
			count++;
		}
		cout << endl;
	}
	return count;
}

void Board::check_move(Piece& piece, vector<Coords>& moves) {
	switch (piece.get_piece_type()) {
		case PAWN:
			pawn_move(piece, moves);
			break;
		case ROOK:
			rook_move(piece, moves);
			break;
		case BISHOP:
			bishop_move(piece, moves);
			break;
		case KNIGHT:
			knight_move(piece, moves);
			break;
		case QUEEN:
			queen_move(piece, moves);
			break;
		case KING:
			king_move(piece, moves);
			break;
	}
}

vector<Coords> Board::check_all_moves(Sides side) {
	vector<Coords> moves;
	if (side == bot_side) {
		for(int i = pieces_vec.size() - 1; i >= 0; --i) {
			if(pieces_status[pieces_vec[i]] && pieces_vec[i]->get_side() == side) {
				check_move(*pieces_vec[i], moves);
			}
		}
	} else {
		for(int i = 0; i < pieces_vec.size(); ++i) {
			if(pieces_status[pieces_vec[i]] && pieces_vec[i]->get_side() == side) {
				check_move(*pieces_vec[i], moves);
			}
		}
	}
    
	// filter moves in check
	vector<Coords> good_moves;
	for(int i = 0; i < moves.size(); ++i) {
		move_piece(moves[i]);
		if(!is_check(side)) {
			good_moves.push_back(moves[i]);
		}
		undo_move();
	}
	return good_moves;
}

bool Board::is_attacked(Piece& piece, Sides side) {
	for(pair<Piece*, bool> piece_status : pieces_status) {
		if(piece_status.second && piece_status.first->get_side() != side) {
			// get enemy piece moves
			std::vector<Coords> aux_moves;
			check_move(*piece_status.first, aux_moves);

			// check if moves match piece position
			std::vector<Coords>::iterator it;
			for (it = aux_moves.begin(); it != aux_moves.end(); it++) {
				if (it->next_x == piece.get_x() && it->next_y == piece.get_y()) {
					return true;
				}
				
			}
		}
	}
	return false;
}

bool Board::is_check(Sides side) {
	for (pair<Piece*, bool> piece : pieces_status) {
		if (piece.first->get_piece_type() == KING && piece.first->get_side() == side && piece.second == true) {
			return is_attacked(*piece.first, side);
		}
	}
	return false;
}

int get_type_value(PieceType type) {
	switch (type) {
		case ROOK:
			return ROOK_VALUE;
		case KNIGHT:
			return KNIGHT_VALUE;
		case BISHOP:
			return BISHOP_VALUE;
		case QUEEN:
			return QUEEN_VALUE;
		default:
            cerr << "[Error] Type value incompatible with promotion type.\n";
			exit(-1);
	}
}


void Board::move_piece(Coords move_coords) {
	MoveInfo move;
	move.coords = move_coords;

	if(!board[move.coords.start_x][move.coords.start_y].hasPiece) {
		cerr << "[ERROR] Tried to move a piece that was not there\n";
		exit(-1);
	} else {
		move.piece_moved = board[move.coords.start_x][move.coords.start_y].piece;
	}

	// take piece if next square occupied
	if (board[move.coords.next_x][move.coords.next_y].hasPiece) {
		// update dead piece status
		pieces_status[board[move.coords.next_x][move.coords.next_y].piece] = false;

		// add to dead stack
		dead_pieces.push(board[move.coords.next_x][move.coords.next_y].piece);
		move.piece_taken = true;

		// if queen is dead, update
		if(dead_pieces.top()->get_piece_type() == QUEEN) {
			queens_alive--;
		}
	} else if(board[move.coords.start_x][move.coords.start_y].piece->get_piece_type() == PAWN && move_coords.start_y - move.coords.next_y != 0) {
		// en passant case
		// add to dead stack
		dead_pieces.push(board[move.coords.start_x][move.coords.next_y].piece);

		// update dead piece status
		pieces_status[board[move.coords.start_x][move.coords.next_y].piece] = false;
		board[move.coords.start_x][move.coords.next_y].hasPiece = false;
		board[move.coords.start_x][move.coords.next_y].piece = nullptr;
		move.piece_taken = true;
	}

	// update has moved status
	if(!board[move.coords.start_x][move.coords.start_y].piece->has_moved()) {
		board[move.coords.start_x][move.coords.start_y].piece->set_has_moved(true);
		move.was_first_move = true;
	}

	// update piece coords 
	board[move.coords.start_x][move.coords.start_y].piece->set_x(move.coords.next_x);
	board[move.coords.start_x][move.coords.start_y].piece->set_y(move.coords.next_y);
	
	// move piece
	board[move.coords.next_x][move.coords.next_y].piece = board[move.coords.start_x][move.coords.start_y].piece;
	board[move.coords.next_x][move.coords.next_y].hasPiece = true;
	board[move.coords.start_x][move.coords.start_y].hasPiece = false;
	board[move.coords.start_x][move.coords.start_y].piece = nullptr;

	// promote if pawn reached end of board
	if(move.coords.promotion != NONE) {
		board[move.coords.next_x][move.coords.next_y].piece->set_piece_type(move.coords.promotion);
		board[move.coords.next_x][move.coords.next_y].piece->set_value(get_type_value(move.coords.promotion));

		// if promotion is queen, update alive queens
		if(get_type_value(move.coords.promotion) == QUEEN) {
			queens_alive++;
		}
	}

	// move rook if castle
	if(board[move.coords.next_x][move.coords.next_y].piece->get_piece_type() == KING && abs(move.coords.next_y - move.coords.start_y) == 2) {
		Coords rook_move_coords;

		// find rook x
		if(board[move.coords.next_x][move.coords.next_y].piece->get_side() == WHITE) {
			rook_move_coords.start_x = 8;
			rook_move_coords.next_x = 8; 
		} else {
			rook_move_coords.start_x = 1;
			rook_move_coords.next_x = 1; 
		}
		
		// find rook ys
		if(move.coords.start_y < move.coords.next_y) {
			rook_move_coords.start_y = 8;
			rook_move_coords.next_y = 6;
		} else {
			rook_move_coords.start_y = 1;
			rook_move_coords.next_y = 4;
		}
		// move rook
		move_piece(rook_move_coords);
	}

	moves.push(move);
}

void Board::undo_move() {
	struct MoveInfo move = moves.top();
	moves.pop();
	int initial_x = move.coords.next_x, final_x = move.coords.start_x;
	int initial_y = move.coords.next_y, final_y = move.coords.start_y;

	// if was castle, revert rook move
	if(board[initial_x][initial_y].piece->get_piece_type() == KING && abs(initial_y - final_y) == 2) {
		undo_move();
	}

	// if was promotion, revert it
	if(move.coords.promotion != NONE) {
		// if promotion is queen, decrement alive queens
		if(board[initial_x][initial_y].piece->get_piece_type() == QUEEN) {
			queens_alive--;
		}

		board[initial_x][initial_y].piece->set_piece_type(PAWN);
		board[initial_x][initial_y].piece->set_value(PAWN_VALUE);
	}

	// move piece back
	board[final_x][final_y].hasPiece = true;
	board[final_x][final_y].piece = board[initial_x][initial_y].piece;
	board[initial_x][initial_y].hasPiece = false;
	board[initial_x][initial_y].piece = nullptr;

	// if was first move, revert status
	if(move.was_first_move) {
		board[final_x][final_y].piece->set_has_moved(false);
	}

	// revert piece coords
	board[final_x][final_y].piece->set_x(final_x);
	board[final_x][final_y].piece->set_y(final_y);

	// add taken piece back if needed
	if (move.piece_taken) {
		// add queen back in count if comes back to life
		if(dead_pieces.top()->get_piece_type() == QUEEN) {
			queens_alive++;
		}

		Piece* temp = dead_pieces.top();

		board[temp->get_x()][temp->get_y()].hasPiece = true;
		board[temp->get_x()][temp->get_y()].piece = temp;

		// update status
		pieces_status[board[temp->get_x()][temp->get_y()].piece] = true;
		dead_pieces.pop();
	}
}
