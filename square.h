#ifndef SQUARE_H
#define SQUARE_H

#include <string>

#include "column.h"

class Square {
public:
    static int get(int, int);
    static int fromString(std::string str);
};

#endif