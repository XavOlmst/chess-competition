//
// Created by xavier.olmstead on 4/15/2024.
//

#include "MCTS.h"

namespace xoxo {

    const double RAVE_FACTOR = 0.75;
    const double EPSILON = 0.00000001;

    int getMaterialScore(const chess::Board& board)
    {
        int materialScore = 0;

        materialScore += 99999 * (board.pieces(chess::PieceType::KING, chess::Color::WHITE).count() -
                board.pieces(chess::PieceType::KING, chess::Color::BLACK).count());

        materialScore += 900 * (board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE).count() -
                board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK).count());

        materialScore += 500 * (board.pieces(chess::PieceType::ROOK, chess::Color::WHITE).count() -
                board.pieces(chess::PieceType::ROOK, chess::Color::BLACK).count());

        materialScore += 330 * (board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE).count() -
                board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK).count());

        materialScore += 320 * (board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE).count() -
                board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK).count());

        materialScore += 100 * (board.pieces(chess::PieceType::PAWN, chess::Color::WHITE).count() -
                board.pieces(chess::PieceType::PAWN, chess::Color::BLACK).count());

        return board.sideToMove() == chess::Color::WHITE ? materialScore : -materialScore;
    }

    int getMobilityScore(const chess::Board& board) {
        int mobilityScore = 0;

        for(int i = 0; i < 64; i++) {
            auto piece = board.at(chess::Square(i));

            if(piece != chess::Piece::NONE) {
                chess::Movelist moves;
                chess::movegen::legalmoves(moves, board, piece.type());

                piece.color() == board.sideToMove() ? mobilityScore += moves.size() : mobilityScore -= moves.size();
            }
        }

        return mobilityScore;
    }

    int getBoardScore(chess::Board& board)
    {
        //determining the score of the board based on materials
        int materialScore = getMaterialScore(board);
        int mobilityScore = getMobilityScore(board);
        //5.compare pawn structure

        return materialScore + mobilityScore;// + kingSafety;
    }


    Node* Node::selectChild()
    {
        double best_score = -1.0;
        Node* best_child = nullptr;

        for(Node* child : children)
        {
            double exploitation_term = static_cast<double>(child->wins) / (child->visits + 0.000001f);
            double exploration_term = 2.0 * sqrt(log(static_cast<double>(visits)) / (child->visits + 0.000001f));

            double score = exploitation_term + exploration_term + getRaveScore();

            if(score > best_score)
            {
                best_child = child;
                best_score = score;
            }
        }

        return best_child;
    }

    void Node::expand()
    {
        chess::Movelist moves;
        chess::movegen::legalmoves(moves, board);

        for (chess::Move move: moves) {
            chess::Board tempBoard(board);
            tempBoard.makeMove(move);

            Node* child = new Node(tempBoard, &move, this);
            children.push_back(child);
        }
    }

    int Node::simulate()
    {
        chess::Board tempBoard(board);

        do
        {
            chess::Movelist moves;
            chess::movegen::legalmoves(moves, tempBoard);

            
            if(tempBoard.isGameOver().first == chess::GameResultReason::CHECKMATE)
            {
                auto color = tempBoard.sideToMove();
                return (color == us) ? -1 : 1; //TODO: might need to swap these, not sure
            }
            else if(tempBoard.isGameOver().first != chess::GameResultReason::NONE)
            {
                return DEFAULT_VALUE;
            }

            for(chess::Move& move : moves)
            {
                tempBoard.makeMove(move);
                int boardEval = getBoardScore(tempBoard);
                move.setScore(boardEval);
                tempBoard.unmakeMove(move);
            }

            std::sort(moves.begin(), moves.end(), [](const chess::Move& a, const chess::Move& b)
                { return a.score() > b.score(); });

            tempBoard.makeMove(moves[0]);

        } while (true);
    }

    void Node::backPropagate(int result)
    {
        Node* node = this;
        while(node != nullptr)
        {
            node->visits++;
            if(result > 0)
                node->wins++;

            node = node->parent;
        }
    }

    double Node::getRaveScore() const
    {
        if(parent == nullptr) return 0;

        double winRate = static_cast<double>(wins) / (visits + EPSILON);
        double parentWinRate = static_cast<double>(parent->wins) / (parent->visits + EPSILON);

        return ((RAVE_FACTOR * winRate) + ((1 - RAVE_FACTOR) * parentWinRate)) / (parent->visits + EPSILON);
    }

    void MCTS::search(int iterations) const {
        for(int i = 0; i < iterations; i++)
        {
            Node* node = selectNode();
            node->expand();
            int results = node->simulate();

            node->backPropagate(results);
        }
    }

    Node* MCTS::selectNode() const {
        Node* currentNode = root;
        while(!currentNode->children.empty())
        {
            currentNode = currentNode->selectChild();
        }

        return currentNode;
    }

    chess::Move* MCTS::getBestMove() {
        Node* best_child = nullptr;
        double best_score = -1;

        for(Node* child : root->children)
        {
            double score;

            score = static_cast<double>(child->wins) / (child->visits + EPSILON) + child->getRaveScore();

            if(score > best_score)
            {
                best_score = score;
                best_child = child;
            }
        }

        return best_child->move;
    }
} // xoxo