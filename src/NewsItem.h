//
// Created by armando on 11/10/19.
//

#ifndef NAIVEBAYESCPP_NEWSITEM_H
#define NAIVEBAYESCPP_NEWSITEM_H

#include <string>
#include <map>

//! Contains information for a single news item
struct NewsItem {
  std::string path; //! holds the path to the information, whether a file path or web path
  std::string collection; //! The classification of the news item set as empty or unknown when unknown
  std::map<std::string, size_t> word_count; //! The count (per word) of the contents of the news item
  std::string contents; //! The full contents of the news item
};

#endif //NAIVEBAYESCPP_NEWSITEM_H
