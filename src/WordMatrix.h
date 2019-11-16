//
// Created by armando on 11/10/19.
//

#ifndef NAIVEBAYESCPP_WORDMATRIX_H
#define NAIVEBAYESCPP_WORDMATRIX_H

#include "NewsItem.h"
#include <Eigen/Core>
#include <ostream>
#include <vector>
#include <map>

typedef Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> MatrixXi;

//! Uses an Eigen matrix to store a words count per class and do arithmetic with
class WordMatrix {
  MatrixXi word_count;
  std::map<std::string, unsigned> classes;
  std::map<std::string, unsigned> words;

  template<typename T, int N, int M>
  void print_matrix(std::ostream &ostr, const Eigen::Matrix<T, N, M> &mat);
  template<typename T, int N, int M>
  void print_latex(std::ostream &ostr, const Eigen::Matrix<T, N, M> &mat);
public:
  /*!
   * Constructs the word matrix from a vector of NewsItem objects
   * @param items A vector of NewsItems
   */
  explicit WordMatrix(const std::vector<NewsItem> &items);

  /*!
   * Constructs the word matrix from raw data
   * @param word_count An Eigen Matrix representing the word count
   * @param classes A map of classes where the key is the class and the value is the column index of the matrix
   * @param words A map of words where the key is the word and the value is the row index of the matrix
   */
  WordMatrix(const MatrixXi &word_count, const std::map<std::string, unsigned> &classes,
             const std::map<std::string, unsigned> &words);
  /*!
   * Gets the total words in the data
   * @return The total words
   */
  unsigned getTotalWords();

  /*!
   * Gets the total words in a class
   * @param cls The class to sum the words for
   * @return The total words with class cls
   */
  unsigned getClassTotal(const std::string &cls);

  /*!
   * Gets the total number of occurrences of a word in all classes
   * @param word The word to sum for
   * @return The total number of occurrences of the word
   */
  unsigned getWordTotal(const std::string &word);

  /*!
   * Gets the total words for a set of classes
   * @param clss A vector of the classes to sum for
   * @return The total words present in the classes provided
   */
  unsigned getClassTotal(const std::vector<std::string> &clss);

  /*!
   * Gets the total number of occurrences of a set of words in all classes
   * @param words The words in vector of strings
   * @return The total number of occurrences for the set of words
   */
  unsigned getWordTotal(const std::vector<std::string> &words);

  /*!
   * Gets the number of occurrences of a word in a class
   * @param clss The class
   * @param word The word
   * @return The count
   */
  unsigned getCount(const std::string &clss, const std::string &word);

  /*!
   * Gets the number of occurences of a word in a set of classes
   * @param cls The set of classes in a vector of strings
   * @param word The word
   * @return The count
   */
  unsigned getCount(const std::vector<std::string> &cls, const std::string &word);

  /*!
   * Gets the number of occurences of a set of words in a class
   * @param cls The class
   * @param wrds The set of words in a vector of strings
   * @return The count
   */
  unsigned getCount(const std::string &cls, const std::vector<std::string> &wrds);

  /*!
   * Gets the number of occurences of a set of words in a set of classes
   * @param clss The set of classes in a vector of strings
   * @param wrds The set of words in a vector of strings
   * @return The count
   */
  unsigned getCount(const std::vector<std::string> &clss, std::vector<std::string> &wrds);

  /*!
   * Gets a submatrix of the matrix with the specified classes and words
   * @param clss The set of classes in a vector of strings
   * @param wrds The set of words in a vector of strings
   * @return A sub-WordMatrix
   */
  WordMatrix block(const std::vector<std::string> &clss, const std::vector<std::string> &wrds);

  /*!
   * Gets a submatrix of the matrix with the specified classes
   * @param clss The set of classes in a vector of strings
   * @return A sub-WordMatrix
   */
  WordMatrix block(const std::vector<std::string> &clss);

  /*!
   * Prints the probabilities of the words in an output stream
   * @param ostr The output stream
   */
  void printProbabilities(std::ostream &ostr);

  /*!
   * Prints the frequencies of the words in an output stream
   * @param ostr The output stream
   */
  void printFrequency(std::ostream &ostr);

  /*!
   * Prints the probabilities of the words to the terminal
   */
  void printProbabilities();

  /*!
   * Prints the frequencies of the words to the terminal
   */
  void printFrequency();

  /*!
   * Prints the probability of the words in an output stream in latex format
   * @param ostr The output stream
   */
  void printLatexProbabilities(std::ostream &ostr);

  /*!
   * Prints the frequencies of the words in an output stream in latex format
   * @param ostr The output stream
   */
  void printLatexFrequency(std::ostream &ostr);

  /*!
   * Prints the probabilities of the words to the terminal in latex format
   */
  void printLatexProbabilities();

  /*!
   * Prints the frequencies of the words to the terminal in latex format
   */
  void printLatexFrequency();

  /*!
   * Gets the n/n_classes most frequent words in each class that results in n total words
   * @param n The total words per class
   * @return A sub-matrix with only the n most frequent words as described above
   */
  WordMatrix getMostFrequent(unsigned long n);
};

// ---- Template Function Implementations ----

template<typename T, int N, int M>
void WordMatrix::print_matrix(std::ostream &ostr, const Eigen::Matrix<T, N, M> &mat) {
  ostr << "word,";
  for (const auto &cls_pair : classes) {
    ostr << cls_pair.first << ',';
  }
  ostr << '\n';
  for (const auto &word_pair : words) {
    ostr << word_pair.first << ',';
    for (const auto &cls_pair : classes) {
      ostr << mat(cls_pair.second, word_pair.second) << ',';
    }
    ostr << '\n';
  }
}

template<typename T, int N, int M>
void WordMatrix::print_latex(std::ostream &ostr, const Eigen::Matrix<T, N, M> &mat) {
  ostr << "\\begin{center}\n\\begin{tabular}{|| c ";
  for (unsigned long i = 0; i < classes.size(); i++)
    ostr << "c ";
  ostr << "||}\n\\hline\n";
  ostr << "word";
  for (const auto &cls_pair : classes)
    ostr << " & " << cls_pair.first;
  ostr << " \\\\ \n\\hline\\hline\n";
  for (const auto &word_pair : words) {
    ostr << word_pair.first;
    for (const auto &cls_pair : classes) {
      ostr << " & " << mat(cls_pair.second, word_pair.second);
    }
    ostr << " \\\\ \\hline \n";
  }
  ostr << "\\end{tabular}\\end{center}\n";
}

#endif //NAIVEBAYESCPP_WORDMATRIX_H
