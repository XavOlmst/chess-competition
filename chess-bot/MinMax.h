//
// Created by xavier.olmstead on 4/18/2024.
//

#ifndef CHESS_MINMAX_H
#define CHESS_MINMAX_H


#include "chess.hpp"

class MinMax {
public:
    static int getBoardScore(chess::Board& board);
    static int getMaterialScore(const chess::Board& board);
    static int getMobilityScore(const chess::Board& board);
    static int getKingSafety(const chess::Board& board);
    static int minmaxMove(int depth, bool isMaximizing, chess::Board& board, chess::Move& bestMove, const chess::Movelist& initialMoves,
                   int alpha = std::numeric_limits<int>::min(), int beta = std::numeric_limits<int>::max());
};


#endif //CHESS_MINMAX_H
