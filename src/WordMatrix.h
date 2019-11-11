//
// Created by armando on 11/10/19.
//

#ifndef NAIVEBAYESCPP_WORDMATRIX_H
#define NAIVEBAYESCPP_WORDMATRIX_H

#include "Item.h"
#include <Eigen/Core>
#include <ostream>
#include <vector>
#include <map>

typedef Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> MatrixXi;

class WordMatrix {
    MatrixXi word_count;
    std::map<std::string, unsigned> classes;
    std::map<std::string, unsigned> words;

    template <typename T, int N, int M>
    void print_matrix(std::ostream &ostr, const Eigen::Matrix<T, N, M> &mat);
public:
    explicit WordMatrix(const std::vector<Item> &items);
    WordMatrix(const MatrixXi &word_count, const std::map<std::string, unsigned> &classes,
            const std::map<std::string, unsigned> &words);
    unsigned getTotalWords();
    unsigned getClassTotal(const std::string &cls);
    unsigned getWordTotal(const std::string &word);
    unsigned getClassTotal(const std::vector<std::string> &clss);
    unsigned getWordTotal(const std::vector<std::string> &words);
    unsigned getCount(const std::string &clss, const std::string &word);
    unsigned getCount(const std::vector<std::string> &cls, const std::string &word);
    unsigned getCount(const std::string &cls, const std::vector<std::string> &wrds);
    unsigned getCount(const std::vector<std::string> &clss, std::vector<std::string> &wrds);
    WordMatrix block(const std::vector<std::string> &clss, const std::vector<std::string> &wrds);
    WordMatrix block(const std::vector<std::string> &clss);
    void printProbabilities(std::ostream &ostr);
    void printFrequency(std::ostream &ostr);
    void printProbabilities();
    void printFrequency();
};

// ---- Template Function Implementations ----

template <typename T, int N, int M>
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

#endif //NAIVEBAYESCPP_WORDMATRIX_H
