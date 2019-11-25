//
// Created by armando on 11/10/19.
//

#ifndef NAIVEBAYESCPP_PARSER_H
#define NAIVEBAYESCPP_PARSER_H

#include "NewsItem.h"
#include "WordMatrix.h"
#include <vector>

//! The Parser that parses newsgroup data
class Parser {
  std::vector<NewsItem> items;
public:

    /*!
     * Constructs the parser, parses the data into an intermediary format.
     * @param path The path to the newgroup root path
     */
  explicit Parser(const std::string &path);

  /*!
   * Returns the data in a word matrix, useful for analysis and classification
   * @return A WordMatrix object
   */
  WordMatrix getMatrix();

  /*!
   * Gets the classes available in the data
   * @return The classes in a vector of strings
   */
  std::vector<std::string> get_classes();

  /*!
   * Get the NewsItems from the parser
   * @return A vector of NewsItem objects
   */
  std::vector<NewsItem> get_items();

  /*!
   * Gets the maximum on a per class basis
   * @param max_per_classes The max
   */
  void prune_per_class(unsigned long max_per_classes);

  /*!
   * This function returns a Parser object with only those items of classes provided
   * @param classes The classes as a vector of strings
   * @return The resulting Parser with only the classes provided
   */
  Parser get_items_of_classes(const std::vector<std::string> &classes);

  /*!
   * Prunes the number of classes randomly to n
   * @param n The number of classes in the resulting WordMatrix
   * @return A WordMatrix with n classes
   */
  Parser prune_classes(size_t n);
};


#endif //NAIVEBAYESCPP_PARSER_H
