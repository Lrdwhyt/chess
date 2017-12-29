#include "game.h"

#include <iostream>

Game::Game(Agent *p1, Agent *p2)
{
    white = p1;
    black = p2;
    side = Side::White;
    print();
    handleTurn(white->requestMove());
}

void Game::handleTurn(Move m)
{
    if (false)
    { // Check if move is valid
        return;
    }
    state.processMove(m);
    // Change turns
    if (side == Side::White)
    {
        side = Side::Black;
    }
    else
    {
        side = Side::White;
    }
    print();
    if (false)
    {
        delete this;
        return;
    }
    if (side == Side::White)
    {
        handleTurn(white->requestMove());
    }
    else
    {
        handleTurn(black->requestMove());
    }
}

void Game::print()
{
    state.getBoard().print();
}

Game::~Game()
{
    delete white;
    delete black;
}