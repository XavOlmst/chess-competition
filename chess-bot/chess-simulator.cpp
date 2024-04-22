#include "chess-simulator.h"
#include "MCTS.h"
// disservin's lib. drop a star on his hard work!
// https://github.com/Disservin/chess-library
#include "chess.hpp"
#include <random>
#include <map>
#include "MCTS.h"
using namespace ChessSimulator;



std::string ChessSimulator::Move(std::string fen) {
	// create your board based on the board string following the FEN notation
	// search for the best move using minimax / monte carlo tree search /
	// alpha-beta pruning / ... try to use nice heuristics to speed up the search
	// and have better results return the best move in UCI notation you will gain
	// extra points if you create your own board/move representation instead of
	// using the one provided by the library

	chess::Board board(fen);
	chess::Movelist moves;
    chess::Move move{};

    chess::movegen::legalmoves(moves, board);
    if (moves.empty())
        return "";

    if(board.sideToMove() == chess::Color::BLACK)
    {
        xoxo::MCTS mcts(&board);

        mcts.search(5);

        move = *mcts.getBestMove();

        auto piece = board.at(move.from());

        //failsafe in case error
        if (moves.find(move) != -1 && piece != chess::Piece::NONE)
            return chess::uci::moveToUci(move);
    }

    // get random move
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, moves.size() - 1);

    move = moves[dist(gen)];

    return chess::uci::moveToUci(move);
}



