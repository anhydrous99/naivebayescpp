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
    std::vector<std::string> get_classes();
    void prune_per_class(unsigned long max_per_classes);
};


#endif //NAIVEBAYESCPP_PARSER_H
