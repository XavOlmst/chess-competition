//
// Created by xavier.olmstead on 4/15/2024.
//

#ifndef CHESS_MCTS_H
#define CHESS_MCTS_H

#include <random>
#include <utility>
#include "chess.hpp"

namespace xoxo {

    const double DEFAULT_VALUE = -0.3;

    class Node {
    public:
        Node(chess::Board b, chess::Move* m, Node* p, chess::Color c);

        chess::Board board;
        chess::Color us;
        chess::Move* move;
        Node* parent;
        std::vector<Node*> children;
        int visits = 0;
        int wins = 0;
        std::string uciString;

        Node* selectChild();
        void expand();
        int simulate();
        void backPropagate(int result);
        double getRaveScore() const;
    };

    class MCTS {
    public:
        chess::Board board;
        Node* root;

        MCTS(const chess::Board* b) : board(*b), root(new Node(board, nullptr, nullptr, board.sideToMove())) {}

        void search(int iterations) const;

        Node *selectNode() const;

        Node* getBestNode();
    };

} // xoxo

#endif //CHESS_MCTS_H
