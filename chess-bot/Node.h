//
// Created by xavier.olmstead on 4/15/2024.
//

#ifndef CHESS_NODE_H
#define CHESS_NODE_H

#include <random>
#include "chess.hpp"

namespace xoxo {

    class Node {
    public:
        Node(chess::Board b, chess::Move* m, Node* p) : board(b), move(m), parent(p), visits(0), wins(0) {}

        chess::Board board;
        chess::Move* move;
        Node* parent;
        std::vector<Node*> children;
        int visits = 0;
        int wins = 0;

        Node* selectChild();


        void expand();

        int simulate();

        void backpropagate(int result);
    };

} // xoxo

#endif //CHESS_NODE_H
