#include "game.h"

#include <algorithm>
#include <iostream>

Game::Game(Agent *p1, Agent *p2) {
    white = p1;
    black = p2;
    side = Side::White;
    state = GameState(0);
    print();
    // std::vector<Move> validMoves = state.getLegalMoves();
    handleTurn(white->requestMove(state));
}

void Game::handleTurn(Move move) {
    /*std::vector<Move> validMoves = state.getLegalMoves();
    if (std::find(validMoves.begin(), validMoves.end(), move) == validMoves.end()) {
        print();
        handleTurn(getCurrentAgent()->requestMove(state));
        return;
    }*/
    state.processMove(move);
    // Change turns
    if (side == Side::White) {
        side = Side::Black;
    } else {
        side = Side::White;
    }
    print();
    /* Start debug */
    std::vector<Move> moves = state.getLegalMoves();
    std::cout << moves.size();
    for (Move move : moves) {
        std::cout << " " << Square::toString(move.origin) << Square::toString(move.destination);
    }
    std::cout << std::endl;
    /* end debug */
    if (false) {
        delete this;
        return;
    }
    if (side == Side::White) {
        handleTurn(white->requestMove(state));
    } else {
        handleTurn(black->requestMove(state));
    }
}

void Game::print() const {
    state.getBoard().print();
}

Agent *Game::getCurrentAgent() const {
    if (side == Side::White) {
        return white;
    } else {
        return black;
    }
}

Game::~Game() {
    delete white;
    delete black;
}