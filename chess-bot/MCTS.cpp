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
        chess::Color ourSide = board.sideToMove();
        chess::Color theirSide = ourSide == chess::Color::WHITE ? chess::Color::BLACK : chess::Color::WHITE;


        materialScore += 99999 * (board.pieces(chess::PieceType::KING, ourSide).count() -
                board.pieces(chess::PieceType::KING, theirSide).count());

        materialScore += 900 * (board.pieces(chess::PieceType::QUEEN, ourSide).count() -
                board.pieces(chess::PieceType::QUEEN, theirSide).count());

        materialScore += 500 * (board.pieces(chess::PieceType::ROOK, ourSide).count() -
                board.pieces(chess::PieceType::ROOK, theirSide).count());

        materialScore += 330 * (board.pieces(chess::PieceType::BISHOP, ourSide).count() -
                board.pieces(chess::PieceType::BISHOP, theirSide).count());

        materialScore += 320 * (board.pieces(chess::PieceType::KNIGHT, ourSide).count() -
                board.pieces(chess::PieceType::KNIGHT, theirSide).count());

        materialScore += 100 * (board.pieces(chess::PieceType::PAWN, ourSide).count() -
                board.pieces(chess::PieceType::PAWN, theirSide).count());

        return materialScore;
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

    int getKingSafety(const chess::Board& board) {
        int kingIndex = board.kingSq(board.sideToMove()).index();
        int numAttackedSquares = 0;
        //for each square around the king
        for(int x = -1; x <= 1; x++)
        {
            for(int y = -1; y <= 1; y++)
            {
                chess::File curFile = chess::File(kingIndex % 8 + x);
                chess::Rank curRank = chess::Rank(kingIndex / 8 + y);

                if(!chess::Square::is_valid(curRank, curFile)) continue;

                chess::Square sq = chess::Square(curFile, curRank);


                if (board.isAttacked(sq, board.sideToMove()))
                {
                    numAttackedSquares++;
                }
            }
        }

        return numAttackedSquares;
    }

    int getBoardScore(chess::Board& board)
    {
        if(board.isGameOver().first == chess::GameResultReason::CHECKMATE)
        {
            return 10000;
        }
        //determining the score of the board based on materials
        int materialScore = getMaterialScore(board);
        //int mobilityScore = getMobilityScore(board);
        //5.compare pawn structure
        int kingSafety = getKingSafety(board) * 10;


        return kingSafety + materialScore;//materialScore;// + mobilityScore;// + kingSafety;
    }


    Node* Node::selectChild()
    {
        double best_score = -1.0;
        Node* best_child = nullptr;

        for(Node* child : children)
        {
            double exploitation_term = static_cast<double>(child->wins) / (child->visits + 0.000001f);
            double exploration_term = 2.0 * sqrt(log(static_cast<double>(visits)) / (child->visits + 0.000001f));

            double score = exploitation_term + getRaveScore()+ exploration_term;

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

        for (chess::Move move : moves) {
            chess::Board tempBoard(board);
            tempBoard.makeMove(move);

            Node* child = new Node(tempBoard, &move, this, us);
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
                return (color == us) ? -1 : 1;
            }
            else if(tempBoard.isGameOver().first != chess::GameResultReason::NONE)
            {
                return DEFAULT_VALUE;
            }

            for(chess::Move& move : moves)
            {
                tempBoard.makeMove(move);
                auto uciString = chess::uci::moveToUci(move);
                int boardEval = getBoardScore(tempBoard);
                move.setScore(boardEval);
                tempBoard.unmakeMove(move);
            }

            std::sort(moves.begin(), moves.end(), [](const chess::Move& a, const chess::Move& b)
                { return a.score() > b.score(); });

            //move->setScore(moves[0].score());
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

    Node::Node(chess::Board b, chess::Move *m, Node *p, chess::Color c){
        board = (std::move(b));
        us = (c);
        move = (m);
        parent = (p);
        visits = (0);
        wins = (0);

        if(move != nullptr)
            uciString = (chess::uci::moveToUci(*move));
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

    Node* MCTS::getBestNode() {
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

            if(score == best_score && child->children.size() < best_child->children.size())
            {
                best_child = child;
            }
        }

        if(best_child == nullptr)
            return nullptr;

        return best_child;
    }
} // xoxo