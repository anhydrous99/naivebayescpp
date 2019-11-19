//
// Created by armando on 11/10/19.
//

#include "WordMatrix.h"
#include "utils.h"
#include <queue>
#include <iostream>
#include <set>

using namespace std;

WordMatrix::WordMatrix(const MatrixXi &wc, const std::map<std::string, unsigned> &clss,
                       const std::map<std::string, unsigned> &wrds) {
  _word_count = wc;
  _classes = clss;
  _words = wrds;
}

WordMatrix::WordMatrix(const std::vector<NewsItem> &items) {
  // Get list of words and _classes using set to find the size of the matrix
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
    _classes[cls] = i;
    i++;
  }
  i = 0;
  for (const string &word : word_list) {
    _words[word] = i;
    i++;
  }

  // Build matrix
  _word_count = MatrixXi::Zero(class_list.size(), word_list.size());
  for (const NewsItem &itm : items) {
    for (const auto &word_pair : itm.word_count) {
      _word_count(_classes[itm.collection], _words[word_pair.first]) += word_pair.second;
    }
  }
}

unsigned WordMatrix::getTotalWords() {
  return _word_count.sum();
}

unsigned WordMatrix::getClassTotal(const std::string &cls) {
  unsigned index = _classes[cls];
  return _word_count.row(index).sum();
}

unsigned WordMatrix::getWordTotal(const std::string &word) {
  unsigned index = _words[word];
  return _word_count.col(index).sum();
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

unsigned &WordMatrix::getCount(const std::string &cls, const std::string &word) {
  unsigned i = _classes[cls];
  unsigned j = _words[word];
  return _word_count(i, j);
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
  for (const auto &word_pair : _words) {
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

WordMatrix WordMatrix::prune_classes(unsigned long n) {
  random_device rd;
  mt19937 g(rd());
  vector<string> clss;
  for (const auto& cls_pair : _classes)
    clss.push_back(cls_pair.first);
  return block(sample(clss.begin(), clss.end(), n, g));
}

void WordMatrix::printFrequency(std::ostream &ostr) {
  print_matrix(ostr, _word_count);
}

void WordMatrix::printProbabilities(std::ostream &ostr) {
  calculate_probability();
  print_matrix(ostr, _word_probability);
}

void WordMatrix::printProbabilities() {
  printProbabilities(cout);
}

void WordMatrix::printFrequency() {
  printFrequency(cout);
}

WordMatrix WordMatrix::getMostFrequent(unsigned long n) {
  unsigned long new_n = n / _classes.size();
  vector<string> used;
  for (const auto &class_pair : _classes) {
    auto cmp = [&](const string &word1, const string &word2) {
      return getCount(class_pair.first, word1) < getCount(class_pair.first, word2);
    };

    priority_queue<string, vector<string>, decltype(cmp)> queue(cmp);
    for (const auto &word_pair : _words) {
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
  for (auto &cls_pair : _classes) {
    clss.push_back(cls_pair.first);
  }

  return block(clss, used);
}

void WordMatrix::printLatexProbabilities(std::ostream &ostr) {
  calculate_probability();
  print_latex(ostr, _word_probability);
}

void WordMatrix::printLatexFrequency(std::ostream &ostr) {
  print_latex(ostr, _word_count);
}

void WordMatrix::printLatexProbabilities() {
  printLatexProbabilities(cout);
}

void WordMatrix::printLatexFrequency() {
  printLatexFrequency(cout);
}

void WordMatrix::calculate_probability() {
  if (probability_calculated)
    return;
  _word_probability(_classes.size(), _words.size());
  for (const auto &class_pair : _classes) {
    double class_total = static_cast<double>(_word_count.row(class_pair.second).sum());
    for (const auto &word_pair : _words) {
      _word_probability(class_pair.second, word_pair.second) =
          static_cast<double>(_word_count(class_pair.second, word_pair.second)) / class_total;
    }
  }
  probability_calculated = true;
}

std::string WordMatrix::predict(const NewsItem &itm) {

  map<string, unsigned> wc = itm.word_count;
  // Remove words we are not considering
  vector<string> to_delete;
  for (const auto &wc_pair : wc) {
    if (_words.find(wc_pair.first) == _words.end())
      to_delete.push_back(wc_pair.first); // Deleting here might cause a miss-aligned iterator
  }
  for (const string &st : to_delete)
    wc.erase(st);


  // Start Calculating probabilities
  unsigned long n_classes = _classes.size();
  VectorXd word_prob = VectorXd::Zero(_words.size());
  for (const auto &wc_pair : wc) {
    unsigned index = _words[wc_pair.first];
    double &current = word_prob(index);
    for (unsigned long i = 0; i < n_classes; i++) {
      double &current_wp = _word_probability(i, index);
      current = (current == 0) ? current_wp : current * current_wp;
    }
  }

  // Get argmax's class
  int argmax = word_prob.maxCoeff();
  for (const auto &cls_pair : _classes)
    if (cls_pair.second == argmax)
      return cls_pair.first;
  return "";
}
