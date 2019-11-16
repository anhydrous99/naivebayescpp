//
// Created by armando on 11/10/19.
//

#include "WordMatrix.h"
#include <queue>
#include <iostream>
#include <set>

using namespace std;

WordMatrix::WordMatrix(const MatrixXi &wc, const std::map<std::string, unsigned> &clss,
                       const std::map<std::string, unsigned> &wrds) {
  word_count = wc;
  classes = clss;
  words = wrds;
}

WordMatrix::WordMatrix(const std::vector<NewsItem> &items) {
  // Get list of words and classes using set to find the size of the matrix
  set<string> class_list;
  set<string> word_list;
  for (const NewsItem &itm : items) {
    class_list.insert(itm.collection);
    for (const auto &word_pair : itm.word_count) {
      word_list.insert(word_pair.first);
    }
  }

  // Create index maps, maps strings to the matrix's indexes
  int i = 0;
  for (const string &cls : class_list) {
    classes[cls] = i;
    i++;
  }
  i = 0;
  for (const string &word : word_list) {
    words[word] = i;
    i++;
  }

  // Build matrix
  word_count = MatrixXi::Zero(class_list.size(), word_list.size());
  for (const NewsItem &itm : items) {
    for (const auto &word_pair : itm.word_count) {
      word_count(classes[itm.collection], words[word_pair.first]) += word_pair.second;
    }
  }
}

unsigned WordMatrix::getTotalWords() {
  return word_count.sum();
}

unsigned WordMatrix::getClassTotal(const std::string &cls) {
  unsigned index = classes[cls];
  return word_count.row(index).sum();
}

unsigned WordMatrix::getWordTotal(const std::string &word) {
  unsigned index = words[word];
  return word_count.col(index).sum();
}

unsigned WordMatrix::getClassTotal(const std::vector<std::string> &clss) {
  Eigen::Matrix<unsigned, Eigen::Dynamic, 1> count(clss.size());
  for (unsigned long i = 0; i < clss.size(); i++) {
    count[i] = getClassTotal(clss[i]);
  }
  return count.sum();
}

unsigned WordMatrix::getWordTotal(const std::vector<std::string> &wrds) {
  Eigen::Matrix<unsigned, Eigen::Dynamic, 1> count(wrds.size());
  for (unsigned long i = 0; i < wrds.size(); i++) {
    count[i] = getWordTotal(wrds[i]);
  }
  return count.sum();
}

unsigned WordMatrix::getCount(const std::string &cls, const std::string &word) {
  unsigned i = classes[cls];
  unsigned j = words[word];
  return word_count(i, j);
}

unsigned WordMatrix::getCount(const std::vector<std::string> &clss, const std::string &word) {
  unsigned count = 0;
  for (const string &cls : clss) {
    count += getCount(cls, word);
  }
  return count;
}

unsigned WordMatrix::getCount(const std::string &cls, const std::vector<std::string> &wrds) {
  unsigned count = 0;
  for (const string &word : wrds) {
    count += getCount(cls, word);
  }
  return count;
}

unsigned WordMatrix::getCount(const std::vector<std::string> &clss, std::vector<std::string> &wrds) {
  unsigned count = 0;
  for (const string &cls : clss) {
    count += getCount(cls, wrds);
  }
  return count;
}

WordMatrix WordMatrix::block(const std::vector<std::string> &clss, const std::vector<std::string> &wrds) {
  unsigned long n = clss.size(), m = wrds.size();
  MatrixXi new_mat(n, m);
  map<string, unsigned> new_class_map;
  map<string, unsigned> new_word_map;

  for (unsigned long i = 0; i < n; i++) {
    new_class_map[clss[i]] = i;
    for (unsigned long j = 0; j < m; j++) {
      if (i == 0)
        new_word_map[wrds[j]] = j;

      new_mat(i, j) = getCount(clss[i], wrds[j]);
    }
  }

  return WordMatrix(new_mat, new_class_map, new_word_map);
}

WordMatrix WordMatrix::block(const std::vector<std::string> &clss) {
  map<string, unsigned> new_class_map;
  map<string, unsigned> new_word_map;

  for (unsigned long i = 0; i < clss.size(); i++)
    new_class_map[clss[i]] = i;

  int i = 0;
  for (const auto &word_pair : words) {
    if (getCount(clss, word_pair.first) != 0) {
      new_word_map[word_pair.first] = i;
      i++;
    }
  }

  MatrixXi new_mat(new_class_map.size(), new_word_map.size());
  for (const auto &class_pair : new_class_map) {
    for (const auto &word_pair : new_word_map) {
      new_mat(class_pair.second, word_pair.second) = getCount(class_pair.first, word_pair.first);
    }
  }
  return WordMatrix(new_mat, new_class_map, new_word_map);
}

void WordMatrix::printFrequency(std::ostream &ostr) {
  print_matrix(ostr, word_count);
}

void WordMatrix::printProbabilities(std::ostream &ostr) {
  Eigen::MatrixXd prob_mat(classes.size(), words.size());
  for (const auto &class_pair : classes) {
    double class_total = static_cast<double>(word_count.row(class_pair.second).sum());
    for (const auto &word_pair : words) {
      prob_mat(class_pair.second, word_pair.second) =
          static_cast<double>(word_count(class_pair.second, word_pair.second)) / class_total;
    }
  }

  print_matrix(ostr, prob_mat);
}

void WordMatrix::printProbabilities() {
  printProbabilities(cout);
}

void WordMatrix::printFrequency() {
  printFrequency(cout);
}

WordMatrix WordMatrix::getMostFrequent(unsigned long n) {
  unsigned long new_n = n / classes.size();
  vector<string> used;
  for (const auto &class_pair : classes) {
    auto cmp = [&](const string &word1, const string &word2) {
      return getCount(class_pair.first, word1) < getCount(class_pair.first, word2);
    };

    priority_queue<string, vector<string>, decltype(cmp)> queue(cmp);
    for (const auto &word_pair : words) {
      if (find(used.begin(), used.end(), word_pair.first) == used.end()) {
        queue.push(word_pair.first);
      }
    }

    for (unsigned long i = 0; i < new_n; i++) {
      used.push_back(queue.top());
      queue.pop();
    }
  }

  vector<string> clss;
  for (auto &cls_pair : classes) {
    clss.push_back(cls_pair.first);
  }

  return block(clss, used);
}

void WordMatrix::printLatexProbabilities(std::ostream &ostr) {
  Eigen::MatrixXd prob_mat(classes.size(), words.size());
  for (const auto &class_pair : classes) {
    double class_total = static_cast<double>(word_count.row(class_pair.second).sum());
    for (const auto &word_pair : words) {
      prob_mat(class_pair.second, word_pair.second) =
          static_cast<double>(word_count(class_pair.second, word_pair.second)) / class_total;
    }
  }
  print_latex(ostr, prob_mat);
}

void WordMatrix::printLatexFrequency(std::ostream &ostr) {
  print_latex(ostr, word_count);
}

void WordMatrix::printLatexProbabilities() {
  printLatexProbabilities(cout);
}

void WordMatrix::printLatexFrequency() {
  printLatexFrequency(cout);
}
