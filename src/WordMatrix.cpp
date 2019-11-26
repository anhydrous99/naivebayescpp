//
// Created by armando on 11/10/19.
//

#include "WordMatrix.h"
#include "utils.h"
#include <queue>
#include <iostream>
#include <numeric>
#include <set>
#include <cmath>
#include <limits>

using namespace std;

WordMatrix::WordMatrix(MatrixXi wc, std::map<std::string, size_t> clss,
                       std::map<std::string, size_t> wrds) :
    _word_count(move(wc)),
    _classes(move(clss)),
    _words(move(wrds)) {
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
  size_t i = 0;
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

size_t WordMatrix::getTotalWords() {
  return _word_count.sum();
}

size_t WordMatrix::getClassTotal(const std::string &cls) {
  unsigned index = _classes[cls];
  return _word_count.row(index).sum();
}

size_t WordMatrix::getWordTotal(const std::string &word) {
  unsigned index = _words[word];
  return _word_count.col(index).sum();
}

size_t WordMatrix::getClassTotal(const std::vector<std::string> &clss) {
  Eigen::Matrix<unsigned, Eigen::Dynamic, 1> count(clss.size());
  for (size_t i = 0; i < clss.size(); i++) {
    count[i] = getClassTotal(clss[i]);
  }
  return count.sum();
}

size_t WordMatrix::getWordTotal(const std::vector<std::string> &wrds) {
  Eigen::Matrix<size_t, Eigen::Dynamic, 1> count(wrds.size());
  for (size_t i = 0; i < wrds.size(); i++) {
    count[i] = getWordTotal(wrds[i]);
  }
  return count.sum();
}

size_t &WordMatrix::getCount(const std::string &cls, const std::string &word) {
  unsigned i = _classes[cls];
  unsigned j = _words[word];
  return _word_count(i, j);
}

size_t WordMatrix::getCount(const std::vector<std::string> &clss, const std::string &word) {
  return accumulate(clss.begin(), clss.end(), 0, [&](size_t a, const string &b) -> size_t {
    return a + getCount(b, word);
  });
}

size_t WordMatrix::getCount(const std::string &cls, const std::vector<std::string> &wrds) {
  return accumulate(wrds.begin(), wrds.end(), 0, [&](size_t a, const string &b) -> size_t {
    return a + getCount(cls, b);
  });
}

size_t WordMatrix::getCount(const std::vector<std::string> &clss, const std::vector<std::string> &wrds) {
  return accumulate(clss.begin(), clss.end(), 0, [&](size_t a, const string &b) -> size_t {
    return a + getCount(b, wrds);
  });
}

WordMatrix WordMatrix::block(const std::vector<std::string> &clss, const std::vector<std::string> &wrds) {
  size_t n = clss.size(), m = wrds.size();
  MatrixXi new_mat(n, m);
  map<string, size_t> new_class_map;
  map<string, size_t> new_word_map;

  for (size_t i = 0; i < m; i++)
    new_word_map[wrds[i]] = i;

  for (size_t i = 0; i < n; i++) {
    new_class_map[clss[i]] = i;
    for (size_t j = 0; j < m; j++) {
      new_mat(i, j) = getCount(clss[i], wrds[j]);
    }
  }

  return WordMatrix(new_mat, new_class_map, new_word_map);
}

WordMatrix WordMatrix::block(const std::vector<std::string> &clss) {
  map<string, size_t> new_class_map;
  map<string, size_t> new_word_map;

  for (size_t i = 0; i < clss.size(); i++)
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

WordMatrix WordMatrix::prune_classes(size_t n) {
  random_device rd;
  mt19937 g(rd());
  vector<string> clss = getClasses();
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

WordMatrix WordMatrix::getMostFrequent(size_t n) {
  size_t new_n = n / _classes.size();
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

    for (size_t i = 0; i < new_n; i++) {
      used.push_back(queue.top());
      queue.pop();
    }
  }
  vector<string> clss = getClasses();
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
  _word_probability = MatrixXd::Zero(_classes.size(), _words.size());
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
  calculate_probability();
  map<string, size_t> wc = itm.word_count;
  // Remove words we are not considering
  vector<string> to_delete;
  for (const auto &wc_pair : wc) {
    if (_words.find(wc_pair.first) == _words.end())
      to_delete.push_back(wc_pair.first); // Deleting here might cause a miss-aligned iterator
  }
  for (const string &st : to_delete)
    wc.erase(st);

  // Get class count
  size_t n_classes = _classes.size();
  VectorXd class_count = VectorXd::Zero(n_classes);
  for (const auto &class_pair : _classes) {
    class_count[class_pair.second] = static_cast<double>(getClassTotal(class_pair.first));
  }
  //class_count.normalize();
  class_count /= class_count.sum();

  // Start Calculating probabilities
  VectorXd class_prob = VectorXd::Zero(n_classes);
  for (const auto &wc_pair : wc) {
    unsigned index = _words[wc_pair.first];
    for (const auto &cls_pair : _classes) {
      double current_wp = pow(_word_probability(cls_pair.second, index), wc_pair.second);
      double &clsp = class_prob(cls_pair.second);
      clsp = (clsp == 0) ? class_count[cls_pair.second] * current_wp : clsp * current_wp;
    }
  }
  // Normalize
  class_prob /= class_prob.sum();
  // Get argmax
  size_t argmax = 0;
  double mx = numeric_limits<double>::max();
  for (size_t i = 0; i < n_classes; i++) {
    if (mx > class_prob(i)) {
      mx = class_prob(i);
      argmax = i;
    }
  }
  auto found = find_if(_classes.begin(), _classes.end(), [&](const auto &cls_pair) {
    return cls_pair.second == argmax;
  });
  return found->first;
}

vector<string> WordMatrix::getWords() {
  vector<string> words;
  transform(_words.begin(), _words.end(), back_inserter(words), [](const auto &word_pair) {
    return word_pair.first;
  });
  return words;
}

vector<string> WordMatrix::getClasses() {
  vector<string> classes;
  transform(_classes.begin(), _classes.end(), back_inserter(classes), [](const auto &cls_pair) {
    return cls_pair.first;
  });
  return classes;
}

size_t WordMatrix::word_index(const std::string &word) {
  return _words[word];
}

size_t WordMatrix::class_index(const std::string &cls) {
  return _classes[cls];
}