//
// Created by xavier.olmstead on 4/15/2024.
//

#include "Node.h"

namespace xoxo {

    Node* Node::selectChild()
    {
        float best_score = -1.0;
        Node* best_child = nullptr;

        for(Node* child : children)
        {
            double exploitation_term = 0;
            double exploration_term = 0;

            if (child->visits > 0)
            {
                exploitation_term = static_cast<double>(child->wins) / child->visits;
                exploration_term = sqrt(2.0 * log(static_cast<double>(visits)) / child->visits);
            }

            double score = exploitation_term + exploration_term;

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

        while(tempBoard.isGameOver().first == chess::GameResultReason::NONE)
        {
            chess::Movelist moves;
            chess::movegen::legalmoves(moves, board);

            if(moves.empty())
            {
                if(tempBoard.isGameOver().first == chess::GameResultReason::CHECKMATE)
                {
                    return (tempBoard.sideToMove() == chess::Color::WHITE) ? -1 : 1; //TODO: might need to swap these, not sure
                }
                else
                {
                    return 0;
                }
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, moves.size() - 1);

            tempBoard.makeMove(moves[dist(gen)]);
        }

        return 0;
    }

    void Node::backpropagate(int result)
    {
        visits++;

        if(result > 0)
            wins++;

        if(parent != nullptr)
        {
            parent->backpropagate(result);
        }
    }

} // xoxo