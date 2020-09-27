#include <stdlib.h>
#include <time.h>
#include "lot_bot.h"
#include <string.h>

void LotBot::set_side(Sides side) {  this->side = side; };
Sides LotBot::get_side() { return side; }

int LotBot::parse_lin(char l) {
    return NUM_SQUARES - (l - PARSE_LIN_OFF) + 1;
}

char LotBot::parse_lin_reverse(int l) {
    return NUM_SQUARES - l + 1 + PARSE_LIN_OFF;
}

int LotBot::parse_col(char c) {
    return c - PARSE_COL_OFF;
}

char LotBot::parse_col_reverse(int c) {
    return c + PARSE_COL_OFF;
}

PieceType LotBot::parse_to_piece_type(char c) {
    switch (c) {
        case 'q':
            return QUEEN;
        case 'k':
            return KNIGHT;
        case 'b':
            return BISHOP;
        case 'r':
            return ROOK;
    }
    cerr << "Error. Piece type " << c << " not compatible.\n";
    exit(-1);
}

string LotBot::parse_from_piece_type(PieceType p) {
    switch (p) {
        case QUEEN:
            return "q";
        case KNIGHT:
            return "k";
        case BISHOP:
            return "b";
        case ROOK:
            return "r";
    }
    cerr << "Error. Piece type " << p << " not compatible.\n";
    exit(-1);
}

int a = 10;


void LotBot::usermove(string command) {
    // separate move code from command
    string move_code = "";

    // check size for special commands such as pawn promotion
    int code_size = COMMAND_CODE_SIZE;
    if(command.size() > BASE_COMMAND_SIZE) {
        code_size++;
    }

    for(int i = command.size() - code_size; i < command.size(); ++i) {
        move_code += command[i];
    }

    // parse into coords struct
    Coords move_coords;
    move_coords.start_x = parse_lin(move_code[1]);
    move_coords.start_y = parse_col(move_code[0]);
    move_coords.next_x = parse_lin(move_code[3]);
    move_coords.next_y = parse_col(move_code[2]);

    // add promotion if necessary
    if(command.size() > BASE_COMMAND_SIZE) {
        move_coords.promotion = parse_to_piece_type(move_code[4]);
    }

    // move on internal board
    board.move_piece(move_coords);
    board.print();
}

string LotBot::botmove() {
    string move_code = "";
    
    // update depth based on situation
    base_depth = 4;
    
    if (board.get_dead_pieces().size() > 15) {
        cerr << "switched to endgame matrices\n";
        end_game_matrices mats;
        memcpy(&ev_matrices, &mats, sizeof(end_game_matrices));
    }

    // if (board.get_dead_pieces().size() > 20 && !board.get_alive_queens()) {
    //     cerr << "switched to endgame depth\n";
    //     base_depth = 6;
    // }

    // Sides opponent_side1;
    // side == WHITE ? opponent_side1 = BLACK : opponent_side1 = WHITE;
    // if (board.get_dead_pieces().size() > 25 || board.get_score(opponent_side1) - KING_VALUE <= QUEEN_VALUE) {
    //     cerr << "switched to endgame depth\n";
    //     base_depth = 6;
    // }

    // get best move
    minimax_info info = minimax(side, base_depth, INT32_MIN, INT32_MAX);
    Coords move_coords = info.move_coords;

    cerr << "MOVE SCORE: " << info.max << endl;
    // no moves => checkmate or draw
    if(info.max <= INT32_MIN / 2) {
        // check lower depth for best moves (maybe opponent bots are dumb and do not see forced mate)
        info = minimax(side, 2, INT32_MIN, INT32_MAX);
        move_coords = info.move_coords;
        if(info.max <= INT32_MIN / 2) {
            return "resign\n";
        }
    }

    // move on internal board
    board.move_piece(move_coords);
    board.print();

    // reverse parse to command
    move_code += parse_col_reverse(move_coords.start_y);
    move_code += parse_lin_reverse(move_coords.start_x);
    move_code += parse_col_reverse(move_coords.next_y);
    move_code += parse_lin_reverse(move_coords.next_x);
    
    // add promotion if necessary
    if(move_coords.promotion) {
        move_code += parse_from_piece_type(move_coords.promotion);
    }

    return "move " + move_code + "\n";
}


int LotBot::evaluate() {
    int sums[2];
    sums[0] = 0; // mysum
    sums[1] = 0; // opponent_sum

    for(pair<Piece*, bool> piece_status : board.get_pieces_status()) {
        // skip if piece is dead
        if(!piece_status.second) {
            continue;
        }
        
        int i = 0;
        // pick coresponding sum
        if(piece_status.first->get_side() != this->side) {
            i = 1;
        } else if (base_depth == 3 && board.is_attacked(*piece_status.first, side)) {
            continue;
        }

        // add to coresponding sum
        Sides piece_side = piece_status.first->get_side();
        sums[i] += piece_status.first->get_value();
        int pos_modifier = 0;
        int x = piece_status.first->get_x() - 1;
        int y = piece_status.first->get_y() - 1;
        switch(piece_status.first->get_piece_type()) {
            case PAWN:
                pos_modifier = (piece_side ==  WHITE) ? ev_matrices.eval_mat_pawn_w[x][y] : ev_matrices.eval_mat_pawn_b[x][y];
                break;
            case ROOK:
                pos_modifier = (piece_side ==  WHITE) ? ev_matrices.eval_mat_rook_w[x][y] : ev_matrices.eval_mat_rook_b[x][y];
                break;
            case BISHOP:
                pos_modifier = (piece_side ==  WHITE) ? ev_matrices.eval_mat_bishop_w[x][y] : ev_matrices.eval_mat_bishop_b[x][y];
                break; 
            case KNIGHT:
                pos_modifier = ev_matrices.eval_mat_knight[x][y];
                break; 
            case QUEEN:
                pos_modifier = ev_matrices.eval_mat_queen[x][y];
                break; 
            case KING:
                pos_modifier = (piece_side ==  WHITE) ? ev_matrices.eval_mat_king_w[x][y] : ev_matrices.eval_mat_king_b[x][y];
                int safety_score = board.king_safe_lanes(piece_status.first) * 50;
                sums[i] += safety_score;
                break; 
        }
        sums[i] += pos_modifier;
    }

    return sums[0] - sums[1];
}

minimax_info LotBot::minimax(Sides current_side, int depth, int alpha, int beta) {
    minimax_info current;
    current.max = INT32_MIN;
    // prepare next side
    Sides next_side;
    current_side == WHITE ? next_side = BLACK : next_side = WHITE;
    
    // evaluate current position if reached end
    if(depth == 0) {
        current.max = evaluate();
        return current;
    }

    // get all possible moves for current side
    std::vector<Coords> pos_moves = board.check_all_moves(current_side);

    // return min/max if checkmated
    if(!pos_moves.size()) {
        // verify who has no moves -> checkmate/stalemate
        if(current_side == side) {
            current.max = INT32_MIN;
        } else {
            current.max = INT32_MAX;
        }

        // update if only stalemated
        if(board.is_check(current_side) || board.is_check(next_side)) {
            current.max /= 2;
        }
        return current;
    }
    

    int chk = 0;
    for(Coords coords : pos_moves) {
        if (current_side == side) {
            board.move_piece(coords);
            
            // change side and compute score
            minimax_info next = minimax(next_side, depth - 1, alpha, beta);
            if(next.max > current.max) {
                current.max = next.max;
                current.move_coords = coords;
            }
            board.undo_move();

            // update aplpha
            if (alpha < current.max) {
                alpha = current.max;
            } 
        } else {
            board.move_piece(coords);
            
            // change side and compute score
            minimax_info next = minimax(next_side, depth - 1, alpha, beta);
            if(next.max <= current.max || current.max == INT32_MIN) {
                current.max = next.max;
                current.move_coords = coords;
            }
            board.undo_move();
            if(next.max < current.max) {
                current.max = next.max;
                current.move_coords = coords;
            }

            // update beta
            if (beta > current.max) {
                beta = current.max;
            }
        }
        if (beta <= alpha) {
            return current;
        }    
    }
    return current;
}
