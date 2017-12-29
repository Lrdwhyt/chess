#include "game.h"

#include <iostream>

Game::Game(Agent *p1, Agent *p2)
{
    white = p1;
    black = p2;
    turn = 1;
    side = Side::White;
    board = Board::startingPosition();
    print();
    handleTurn(white->requestMove());
}

void Game::handleTurn(Move m)
{
    if (false)
    { // Check if move is valid
        return;
    }
    int piece = board.at(m.origin);
    board.clearSquare(m.origin);
    board.updateSquare(m.destination, piece);
    // Change turns
    if (side == Side::White)
    {
        side = Side::Black;
    }
    else
    {
        side = Side::White;
        ++turn;
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
    board.print();
}

Game::~Game()
{
    delete white;
    delete black;
}