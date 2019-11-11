//
// Created by armando on 11/10/19.
//

#ifndef NAIVEBAYESCPP_ITEM_H
#define NAIVEBAYESCPP_ITEM_H

#include <string>
#include <map>

struct Item {
    std::string path;
    std::string collection;
    std::map<std::string, std::string> headers;
    std::map<std::string, unsigned> word_count;
    std::string contents;
};

#endif //NAIVEBAYESCPP_ITEM_H
