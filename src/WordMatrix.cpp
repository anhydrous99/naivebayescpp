//
// Created by armando on 11/10/19.
//

#include "WordMatrix.h"
#include <iostream>
#include <set>

using namespace std;

WordMatrix::WordMatrix(const MatrixXi &wc, const std::map<std::string, unsigned> &clss,
                       const std::map<std::string, unsigned> &wrds) {
    word_count = wc;
    classes = clss;
    words = wrds;
}

WordMatrix::WordMatrix(const std::vector<Item> &items) {
    // Get list of words and classes using set to find the size of the matrix
    set<string> class_list;
    set<string> word_list;
    for (const Item& itm : items) {
        class_list.insert(itm.collection);
        for (const auto& word_pair : itm.word_count) {
            word_list.insert(word_pair.first);
        }
    }

    // Create index maps, maps strings to the matrix's indexes
    int i = 0;
    for (const string& cls : class_list) {
        classes[cls] = i;
        i++;
    }
    i = 0;
    for (const string& word : word_list) {
        words[word] = i;
        i++;
    }

    // Build matrix
    word_count = MatrixXi::Zero(class_list.size(), word_list.size());
    for (const Item& itm : items) {
        for (const auto& word_pair : itm.word_count) {
            word_count(classes[itm.collection], words[word_pair.first]) += word_pair.second;
        }
    }
}

unsigned int WordMatrix::getTotalWords() {
    return word_count.sum();
}