#include "chess-simulator.h"
// disservin's lib. drop a star on his hard work!
// https://github.com/Disservin/chess-library
#include "chess.hpp"
#include <random>
#include <map>
#include <map>
using namespace ChessSimulator;

int getBoardScore(chess::Board& board);
int getMaterialScore(const chess::Board& board);
int getMobilityScore(const chess::Board& board, bool isWhite);
int minmaxMove(int depth, bool isMaximizing, chess::Board& board, chess::Move& bestMove, int alpha, int beta);

int pawnValues[64] =
{
	 0,  0,  0,  0,  0,  0,  0,  0,
	50, 50, 50, 50, 50, 50, 50, 5,
	10, 10, 20, 30, 30, 20, 10, 10,
	 5,  5, 10, 25, 25, 10,  5,  5,
	 0,  0,  0, 20, 20,  0,  0,  0,
	 5, -5,-10,  0,  0,-10, -5,  5,
	 5, 10, 10,-20,-20, 10, 10,  5,
	 0,  0,  0,  0,  0,  0,  0,  0
};

int knightValues[64] =
{
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50
};

int bishopValues[64] =
{
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20
};

int rookValues[64] =
{
	 0,  0,  0,  0,  0,  0,  0,  0,
	 5, 10, 10, 10, 10, 10, 10,  5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	 0,  0,  0,  5,  5,  0,  0,  0
};

int queenValues[64] =
{
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5,  5,  5,  5,  0,-10,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	  0,  0,  5,  5,  5,  5,  0, -5,
	-10,  5,  5,  5,  5,  5,  0,-10,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
};

int kingValues[64] =
{
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	 20, 20,  0,  0,  0,  0, 20, 20,
	 20, 30, 10,  0,  0, 10, 30, 20
};

std::string ChessSimulator::Move(std::string fen) {
	// create your board based on the board string following the FEN notation
	// search for the best move using minimax / monte carlo tree search /
	// alpha-beta pruning / ... try to use nice heuristics to speed up the search
	// and have better results return the best move in UCI notation you will gain
	// extra points if you create your own board/move representation instead of
	// using the one provided by the library

	chess::Board board(fen);
	chess::Movelist moves;
	chess::movegen::legalmoves(moves, board);
	if (moves.size() == 0)
		return "";

	// get random move
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, moves.size() - 1);
	chess::Move move;

	move = moves[dist(gen)];

	if (board.sideToMove() == chess::Color::WHITE)
	{
		minmaxMove(2, true, board, move, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	}
	else
	{
		return chess::uci::moveToUci(move);
	}

	return chess::uci::moveToUci(move);
}

int minmaxMove(int depth, bool isMaximizing, chess::Board& board, chess::Move& bestMove, int alpha, int beta)
{
	chess::Movelist moves;
	chess::movegen::legalmoves(moves, board);

	if (depth == 0 || moves.size() == 0)
	{
		//determine the board score

		//1. compare materials

		int materialScore = 0;
		materialScore = getBoardScore(board);

		return materialScore;
	}



	if (isMaximizing)
	{
		int maxValue = std::numeric_limits<int>::min();
		chess::Board tempBoard(board);
		for (chess::Move move : moves)
		{
			tempBoard.makeMove(move);

			int evaluation = minmaxMove(depth - 1, false, tempBoard, bestMove, alpha, beta);

			maxValue = fmax(maxValue, evaluation);
			alpha = fmax(alpha, evaluation);

			if (beta <= alpha)
			{
				tempBoard.unmakeMove(move);
				break;
			}

			if (maxValue == evaluation)
			{
				bestMove = move;
			}

			tempBoard.unmakeMove(move);
		}

		return maxValue;
	}
	else
	{
		int minValue = std::numeric_limits<int>::max();
		chess::Board tempBoard(board);

		for (chess::Move move : moves)
		{
			tempBoard.makeMove(move);

			int evaluation = minmaxMove(depth - 1, true, tempBoard, bestMove, alpha, beta);

			minValue = fmin(minValue, evaluation);
			beta = fmin(beta, evaluation);

			if (beta <= alpha)
			{
				tempBoard.unmakeMove(move);
				break;
			}

			if (minValue == evaluation)
			{
				bestMove = move;
			}

			tempBoard.unmakeMove(move);
		}

		return minValue;
	}
}

int getBoardScore(chess::Board& board)
{
	//determining the score of the board based on materials
	int materialScore = getMaterialScore(board);

	//int mobilityScore = getMobilityScore(board, isWhite);

	//3. compare central control

	//4. king safety

	//5.compare pawn structure

	return materialScore;// +mobilityScore;
}

int getMaterialScore(const chess::Board& board)
{
	int materialScore = 0;

	materialScore += 99999 * (board.pieces(chess::PieceType::KING, chess::Color::WHITE).count() - board.pieces(chess::PieceType::KING, chess::Color::BLACK).count());
	materialScore += 900 * (board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE).count() - board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK).count());
	materialScore += 500 * (board.pieces(chess::PieceType::ROOK, chess::Color::WHITE).count() - board.pieces(chess::PieceType::ROOK, chess::Color::BLACK).count());
	materialScore += 330 * (board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE).count() - board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK).count());
	materialScore += 320 * (board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE).count() - board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK).count());
	materialScore += 100 * (board.pieces(chess::PieceType::PAWN, chess::Color::WHITE).count() - board.pieces(chess::PieceType::PAWN, chess::Color::BLACK).count());

	if (board.sideToMove() == chess::Color::WHITE)
	{
		return materialScore;
	}
	else
	{
		return -materialScore;
	}
}

int getMobilityScore(const chess::Board& board, bool isWhite)
{
	int mobilityScore = 0;

	if (isWhite)
	{
		for (int i = 0; i < 64; i++)
		{
			if (chess::Piece piece = board.at(chess::Square(i)); piece != chess::Piece::NONE)
			{
				switch (piece)
				{
				case 0:
					mobilityScore += pawnValues[i];
					break;
				case 1:
					mobilityScore += knightValues[i];
					break;
				case 2:
					mobilityScore += bishopValues[i];
					break;
				case 3:
					mobilityScore += rookValues[i];
					break;
				case 4:
					mobilityScore += queenValues[i];
					break;
				case 5:
					mobilityScore += kingValues[i];
					break;
				default:
					break;
				}
			}
		}
	}
	else
	{
		for (int i = 63; i >= 0; i--)
		{
			if (chess::Piece piece = board.at(chess::Square(i)); piece != chess::Piece::NONE)
			{
				switch (piece)
				{
				case 0:
					mobilityScore += pawnValues[i];
					break;
				case 1:
					mobilityScore += knightValues[i];
					break;
				case 2:
					mobilityScore += bishopValues[i];
					break;
				case 3:
					mobilityScore += rookValues[i];
					break;
				case 4:
					mobilityScore += queenValues[i];
					break;
				case 5:
					mobilityScore += kingValues[i];
					break;
				default:
					break;
				}
			}
		}
	}
	

	return mobilityScore;
}