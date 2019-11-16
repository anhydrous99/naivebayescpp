//
// Created by armando on 11/15/19.
//

#ifndef NAIVEBAYESCPP_WEBHANDLER_H
#define NAIVEBAYESCPP_WEBHANDLER_H

#include "NewsItem.h"
#include <string>
#include <vector>
#include <map>

class WebHandler {
    std::string url = "https://newsapi.org/v2/top-headlines?";
    std::string apikey;

    std::string Call(const std::string& uri);
    std::string Call(const std::map<std::string, std::string>& args);
public:
    explicit WebHandler(std::string key);
    std::vector<NewsItem> getTop(int n);
};

#endif //NAIVEBAYESCPP_WEBHANDLER_H
