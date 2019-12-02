//
// Created by armando on 11/15/19.
//

#ifndef NAIVEBAYESCPP_WEBHANDLER_H
#define NAIVEBAYESCPP_WEBHANDLER_H

#include "NewsItem.h"
#include <unordered_set>
#include <string>
#include <vector>
#include <map>

//! Uses the libcurl library to interface with newsapi.org to obtain the leading news
class WebHandler {
  std::string url = "https://newsapi.org/v2/top-headlines?";
  std::unordered_set<std::string> _stop_words;
  bool _stop_words_init = false;
  std::string _key;

  std::string Call(const std::string &uri);
  std::string Call(const std::map<std::string, std::string> &args);
  void getStopWords();

public:
  /*!
   * Constructs the WebHandler class
   * @param key The api key for the newsapi.org site
   */
  explicit WebHandler(std::string key);

  /*!
   * Sends query to newsapi for the top 20 of class collection
   * @param collection The class
   * @return A vector of NewsItem objects all of class collection
   */
  std::vector<NewsItem> sendQuery(const std::string &collection);

  /*!
   * Send queries to newsapi for the top 20 per class in vector of classes collection
   * @param collections The classes
   * @return A vector of NewsItem objects
   */
  std::vector<NewsItem> sendQueries(const std::vector<std::string> &collections);

  /*!
   * Set the newapi.org api key
   * @param key newsapi key
   */
  void setKey(const std::string &key);

  /*!
   * Gets the newsapi.org api key
   * @return The news api key
   */
  std::string getKey();
};

#endif //NAIVEBAYESCPP_WEBHANDLER_H
