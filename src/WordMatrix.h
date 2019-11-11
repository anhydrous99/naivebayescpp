//
// Created by armando on 11/10/19.
//

#ifndef NAIVEBAYESCPP_WORDMATRIX_H
#define NAIVEBAYESCPP_WORDMATRIX_H

#include "Item.h"
#include <Eigen/Core>
#include <vector>
#include <map>

typedef Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> MatrixXi;

class WordMatrix {
    MatrixXi word_count;
    std::map<std::string, unsigned> classes;
    std::map<std::string, unsigned> words;

public:
    explicit WordMatrix(const std::vector<Item> &items);
    WordMatrix(const MatrixXi &word_count, const std::map<std::string, unsigned> &classes,
            const std::map<std::string, unsigned> &words);
    unsigned getTotalWords();
    unsigned getClassTotal(const std::string &cls);
    unsigned getWordTotal(const std::string &word);
};


#endif //NAIVEBAYESCPP_WORDMATRIX_H
