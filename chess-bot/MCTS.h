//
// Created by xavier.olmstead on 4/15/2024.
//

#ifndef CHESS_MCTS_H
#define CHESS_MCTS_H

#include <random>
#include "chess.hpp"

namespace xoxo {

    const double DEFAULT_VALUE = -0.3;

    class Node {
    public:
        Node(chess::Board b, chess::Move* m, Node* p) : board(b), us(b.sideToMove()), move(m), parent(p), visits(0), wins(0) {}

        chess::Board board;
        chess::Color us;
        chess::Move* move;
        Node* parent;
        std::vector<Node*> children;
        int visits = 0;
        int wins = 0;

        Node* selectChild();
        void expand();
        int simulate();
        void backpropagate(int result);
        double getRaveScore();
    };

    class MCTS {
    public:
        chess::Board board;
        Node* root;

        MCTS(const chess::Board* b) : board(*b), root(new Node(board, nullptr, nullptr)) {}

        void search(int iterations);

        Node *selectNode();

        chess::Move* getBestMove();
    };

} // xoxo

#endif //CHESS_MCTS_H
