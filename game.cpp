#include "game.h"

#include <iostream>

Game::Game()
{
    turn = 1;
    side = Side::White;
    board = Board::startingPosition();
}

void Game::handleTurn(Move m)
{
    if (side == Side::White)
    {
        side = Side::Black;
    }
    else
    {
        ++turn;
    }
}

void Game::print()
{
    board.print();
}

void Game::handleRaw(std::string s)
{
    if (s.length() >= 4)
    {
        std::string originSquareString = s.substr(0, 2);
        std::string destSquareString = s.substr(2, 2);
        int destSquare, originSquare;
        try
        {
            originSquare = Square::fromString(originSquareString);
            destSquare = Square::fromString(destSquareString);
        }
        catch (std::runtime_error err)
        {
            std::cout << err.what() << std::endl;
            return;
        }
        int piece = board.at(originSquare);
        board.clearSquare(originSquare);
        board.updateSquare(destSquare, piece);
        print();
    }
}