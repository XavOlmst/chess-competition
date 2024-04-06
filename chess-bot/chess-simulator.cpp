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
int getMobilityScore(const chess::Board& board);
int getKingSafety(const chess::Board& board);
int minmaxMove(int depth, bool isMaximizing, chess::Board& board, chess::Move& bestMove, int alpha = std::numeric_limits<int>::min()
	, int beta = std::numeric_limits<int>::max(), bool firstTest = true);

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
    chess::Move move{};


	if (board.sideToMove() == chess::Color::WHITE)
	{
		minmaxMove(3, true, board, move);
	}
	else
	{
        chess::movegen::legalmoves(moves, board);
        if (moves.empty())
            return "";

        // get random move
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, moves.size() - 1);


		move = moves[dist(gen)];
	}

    return chess::uci::moveToUci(move);
}

int minmaxMove(int depth, bool isMaximizing, chess::Board& board, chess::Move& bestMove, int alpha, int beta, bool firstTest)
{
	if (depth == 0)
	{
		//determine the board score

		int boardScore = 0;
		boardScore = getBoardScore(board);

		return boardScore;
	}

	chess::Movelist moves;
	chess::movegen::legalmoves(moves, board);

	if (isMaximizing)
	{
		int maxValue = std::numeric_limits<int>::min();
		chess::Board tempBoard(board);
		for (chess::Move move : moves)
		{
			tempBoard.makeMove(move);

			int evaluation = minmaxMove(depth - 1, false, tempBoard, bestMove, alpha, beta, false);

			maxValue = fmax(maxValue, evaluation);
			alpha = fmax(alpha, evaluation);

			if (beta <= alpha)
			{
				tempBoard.unmakeMove(move);
				break;
			}

			if (maxValue == evaluation && firstTest)
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

			int evaluation = minmaxMove(depth - 1, true, tempBoard, bestMove, alpha, beta, false);

			minValue = fmin(minValue, evaluation);
			beta = fmin(beta, evaluation);

			if (beta <= alpha)
			{
				tempBoard.unmakeMove(move);
				break;
			}

			if (minValue == evaluation && firstTest)
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

	int mobilityScore = getMobilityScore(board);

	//4. king safety
	int kingSafety = getKingSafety(board);

	//5.compare pawn structure

	return materialScore + mobilityScore + kingSafety;
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
		return -materialScore;
	}
	else
	{
		return materialScore;
	}
}

int getMobilityScore(const chess::Board& board)
{
	int mobilityScore = 0;

	if (board.sideToMove() == chess::Color::BLACK)
	{
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				int index = y + (x << 3);

				if (chess::Piece piece = board.at(chess::Square(index)); piece != chess::Piece::NONE)
				{
					switch (piece)
					{
					case 0:
						mobilityScore += pawnValues[index];

						if ((x >= 3 && x <= 4) && (y >= 3 && y <= 4))
						{
							mobilityScore += 10;
						}

						break;
					case 1:
						mobilityScore += knightValues[index];

						if ((x >= 2 && x <= 5) && (y >= 2 && y <= 5))
						{
							mobilityScore += 20;
						}

						break;
					case 2:
						mobilityScore += bishopValues[index];

						if ((x >= 2 && x <= 5) && (y >= 2 && y <= 5))
						{
							mobilityScore += 30;
						}

						break;
					case 3:
						mobilityScore += rookValues[index];
						break;
					case 4:
						mobilityScore += queenValues[index];
						break;
					case 5:
						mobilityScore += kingValues[index];
						break;
					default:
						break;
					}
				}
			}
		}
	}
	else
	{
		for (int x = 7; x >= 0; x--)
		{
			for (int y = 7; y >= 0; y--)
			{
                int index = y + (x << 3);

				if (chess::Piece piece = board.at(chess::Square(index)); piece != chess::Piece::NONE)
				{
					switch (piece)
					{
					case 0:
						mobilityScore += pawnValues[index];

						if ((x >= 3 && x <= 4) && (y >= 3 && y <= 4))
						{
							mobilityScore += 10;
						}

						break;
					case 1:
						mobilityScore += knightValues[index];

						if ((x >= 2 && x <= 5) && (y >= 2 && y <= 5))
						{
							mobilityScore += 20;
						}

						break;
					case 2:
						mobilityScore += bishopValues[index];

						if ((x >= 2 && x <= 5) && (y >= 2 && y <= 5))
						{
							mobilityScore += 30;
						}

						break;
					case 3:
						mobilityScore += rookValues[index];
						break;
					case 4:
						mobilityScore += queenValues[index];
						break;
					case 5:
						mobilityScore += kingValues[index];
						break;
					default:
						break;
					}
				}
			}
		}
	}
	

	return mobilityScore;
}

int getKingSafety(const chess::Board& board)
{
	int kingSafetyScore = 0;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			int index = i + (j << 3);

			auto piece = board.at(chess::Square(index));
			if (piece == chess::Piece::WHITEKING) {
				//whiteKingFound = true;
				// Bonus score for safe position of the white king
				if (chess::Square(index).back_rank(chess::Square(index), chess::Color::WHITE))
					kingSafetyScore += 50;
			}
			else if (piece == chess::Piece::BLACKKING) {
				//whiteKingFound = true;
				// Bonus score for safe position of the white king
				if (chess::Square(index).back_rank(chess::Square(index), chess::Color::BLACK))
					kingSafetyScore -= 50;
			}
		}
	}

	if (board.sideToMove() == chess::Color::BLACK)
	{
		return kingSafetyScore;
	}
	else
	{
		return -kingSafetyScore;
	}
}