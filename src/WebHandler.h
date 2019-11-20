//
// Created by armando on 11/15/19.
//

#ifndef NAIVEBAYESCPP_WEBHANDLER_H
#define NAIVEBAYESCPP_WEBHANDLER_H

#include "NewsItem.h"
#include <string>
#include <vector>
#include <map>

//! Uses the libcurl library to interface with newsapi.org to obtain the leading news
class WebHandler {
  std::string url = "https://newsapi.org/v2/top-headlines?";
  std::string apikey;

  std::string Call(const std::string &uri);
  std::string Call(const std::map<std::string, std::string> &args);

public:
  /*!
   * Constructs the WebHandler class
   * @param key The api key for the newsapi.org site
   */
  explicit WebHandler(std::string key);

  /*!
   * Calls the newsapi and obtains the top n news articles
   * @param n The number of articles to get
   * @return A vector of n NewsItem objects
   */
  std::vector<NewsItem> getTop(size_t n);
};

#endif //NAIVEBAYESCPP_WEBHANDLER_H
