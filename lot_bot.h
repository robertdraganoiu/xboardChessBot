#ifndef LOT_BOT_H
#define LOT_BOT_H

#include <iostream>
#include <algorithm>
#include "utils.h"
#include "board.h"
#include "pieces.h"

struct minimax_info {
    int max;
    Coords move_coords;
};

class LotBot {
private:
    Sides side;
    Board board;
    eval_matrices ev_matrices;
    int base_depth;

public:
    LotBot(Sides side) : side(side), board(side), ev_matrices() {
        srand(time(NULL)); 
        board.print();
    }

    void set_side(Sides side);
    Sides get_side();

    // transfer xboard move to board and reverse
    int parse_lin(char l);
    char parse_lin_reverse(int l);
    int parse_col(char c);
    char parse_col_reverse(int c);
    PieceType parse_to_piece_type(char c);
    string parse_from_piece_type(PieceType p);

    // get move from bot and send move back
    void usermove(string command);
    string botmove();

    // negamax functions
    int evaluate();
    minimax_info minimax(Sides current_side, int depth, int alpha, int beta);
};



#endif