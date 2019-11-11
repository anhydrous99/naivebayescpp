//
// Created by armando on 11/10/19.
//

#ifndef NAIVEBAYESCPP_PARSER_H
#define NAIVEBAYESCPP_PARSER_H

#include "Item.h"
#include "WordMatrix.h"
#include <vector>

class Parser {
    std::vector<Item> items;
public:
    explicit Parser(const std::string &path);
    WordMatrix getMatrix();
};


#endif //NAIVEBAYESCPP_PARSER_H
