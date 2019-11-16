//
// Created by armando on 11/10/19.
//

#ifndef NAIVEBAYESCPP_NEWSITEM_H
#define NAIVEBAYESCPP_NEWSITEM_H

#include <string>
#include <map>

struct NewsItem {
    std::string path;
    std::string collection;
    std::map<std::string, unsigned> word_count;
    std::string contents;
};

#endif //NAIVEBAYESCPP_NEWSITEM_H
