#include "chess-simulator.h"
// disservin's lib. drop a star on his hard work!
// https://github.com/Disservin/chess-library
#include "chess.hpp"
#include <random>
using namespace ChessSimulator;

int getBoardScore(chess::Board& board, bool isWhite);
int getMaterialScore(const chess::Board& board, bool isWhite);
int getMobilityScore(const chess::Board& board, bool isWhite);
int minmaxMove(chess::Movelist moves, int depth, bool isMaximizing, bool isWhite, std::string fen, chess::Move& bestMove);

std::string ChessSimulator::Move(std::string fen) {
  // create your board based on the board string following the FEN notation
  // search for the best move using minimax / monte carlo tree search /
  // alpha-beta pruning / ... try to use nice heuristics to speed up the search
  // and have better results return the best move in UCI notation you will gain
  // extra points if you create your own board/move representation instead of
  // using the one provided by the library

  // here goes a random movement
  chess::Board board(fen);
  chess::Movelist moves;
  chess::movegen::legalmoves(moves, board);
  if(moves.size() == 0)
    return "";

  // get random move
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, moves.size() - 1);
  chess::Move move;

  minmaxMove(moves, 1, true, true, fen, move);

  return chess::uci::moveToUci(move);
}

int minmaxMove(chess::Movelist moves, int depth, bool isMaximizing, bool isWhite, std::string fen, chess::Move& bestMove)
{
    chess::Board board(fen);

    if (depth == 0)
    {
        //determine the board score

        //1. compare materials

        int materialScore = getBoardScore(board, isWhite);

        return materialScore;
    }

    if (isMaximizing)
    {
        int maxValue = -INFINITY;
        for (chess::Move move : moves)
        {
            chess::Board tempBoard(board);
            std::string uciMove = chess::uci::moveToUci(move);

            tempBoard.makeMove(move);
            chess::Movelist newMoves;
            chess::movegen::legalmoves(newMoves, tempBoard);

            int evaluation = minmaxMove(newMoves, depth - 1, false, !isWhite, tempBoard.getFen(), bestMove);
            maxValue = fmax(maxValue, evaluation);

            if (maxValue == evaluation)
            {
                bestMove = move;
            }
        }

        return maxValue;
    }
    else
    {
        int minValue = INFINITY;

        for (chess::Move move : moves)
        {
            chess::Board tempBoard(board);
            std::string uciMove = chess::uci::moveToUci(move);

            tempBoard.makeMove(move);
            chess::Movelist newMoves;
            chess::movegen::legalmoves(newMoves, tempBoard);

            int evaluation = minmaxMove(newMoves, depth - 1, true, !isWhite, tempBoard.getFen(), bestMove);
            minValue = fmin(minValue, evaluation);

            if (minValue == evaluation)
            {
                bestMove = move;
            }
        }

        return minValue;
    }
}

int getBoardScore(chess::Board& board, bool isWhite)
{
    //determining the score of the board based on materials
    int materialScore = getMaterialScore(board, isWhite);

    //2. compare development

    //3. compare central control

    //4. king safety

    //5.compare pawn structure

    return materialScore;
}

int getMaterialScore(const chess::Board& board, bool isWhite)
{
    int materialScore = 0;

    if (isWhite)
    {
        materialScore += 999 * (board.pieces(chess::PieceType::KING, chess::Color::WHITE).count() - board.pieces(chess::PieceType::KING, chess::Color::BLACK).count());
        materialScore += 9 * (board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE).count() - board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK).count());
        materialScore += 5 * (board.pieces(chess::PieceType::ROOK, chess::Color::WHITE).count() - board.pieces(chess::PieceType::ROOK, chess::Color::BLACK).count());
        materialScore += 3 * (board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE).count() - board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK).count());
        materialScore += 3 * (board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE).count() - board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK).count());
        materialScore += 1 * (board.pieces(chess::PieceType::PAWN, chess::Color::WHITE).count() - board.pieces(chess::PieceType::PAWN, chess::Color::BLACK).count());
    }
    else
    {
        materialScore += 999 * (board.pieces(chess::PieceType::KING, chess::Color::BLACK).count() - board.pieces(chess::PieceType::KING, chess::Color::WHITE).count());
        materialScore += 9 * (board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK).count() - board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE).count());
        materialScore += 5 * (board.pieces(chess::PieceType::ROOK, chess::Color::BLACK).count() - board.pieces(chess::PieceType::ROOK, chess::Color::WHITE).count());
        materialScore += 3 * (board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK).count() - board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE).count());
        materialScore += 3 * (board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK).count() - board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE).count());
        materialScore += 1 * (board.pieces(chess::PieceType::PAWN, chess::Color::BLACK).count() - board.pieces(chess::PieceType::PAWN, chess::Color::WHITE).count());
    }

    return materialScore;
}

int getMobilityScore(const chess::Board& board, bool isWhite)
{
    auto fen = board.getFen();



    return -1;
}