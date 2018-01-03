#include "game.h"

#include <iostream>

Game::Game(Agent *p1, Agent *p2) {
    white = p1;
    black = p2;
    side = Side::White;
    print();
    handleTurn(white->requestMove(state));
}

void Game::handleTurn(Move move) {
    if (state.isValidMove(move) == false) {
        print();
        handleTurn(getCurrentAgent()->requestMove(state));
        return;
    }
    state.processMove(move);
    // Change turns
    if (side == Side::White) {
        side = Side::Black;
    } else {
        side = Side::White;
    }
    print();
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