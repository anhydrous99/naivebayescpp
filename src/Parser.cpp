//
// Created by armando on 11/10/19.
//

#include "utils.h"
#include "Parser.h"
#include "filesystem.h"
#include <regex>
#include <set>
#include <unordered_set>
#include <random>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <streambuf>
#include <stdexcept>
#include <utility>

using namespace std;

static string read_file(const fs::path &p) {
  ifstream t(p.string());
  string str;

  // Pre-allocate to prevent thrashing
  t.seekg(0, ios::end);
  str.reserve(t.tellg());
  t.seekg(0, ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
  return str;
}

Parser::Parser(const string &path) {
  if (!fs::exists(path) || !fs::is_directory(path))
    throw std::runtime_error("Error: path doesn't exist.\n");
  if (!fs::exists("stop_words.txt") || fs::is_directory("stop_words.txt"))
    throw std::runtime_error("Error: stop_words.txt is not at the binary directory.\n");

  // Get stop words into a vector
  ifstream stop_words_stream("stop_words.txt");
  unordered_set<string> stop_words;
  string line;
  while (getline(stop_words_stream, line))
    stop_words.insert(line);

  // Recursively iterate through the directory looking for files
  fs::recursive_directory_iterator it{path};
  while (it != fs::recursive_directory_iterator{}) {
    fs::path tmp = (*it++).path();
    if (fs::exists(tmp) && !fs::is_directory(tmp)) {
      NewsItem itm;
      // Read file to string
      string txt = read_file(tmp);
      itm.path = tmp.string();
      itm.contents = txt;

      // Get classification
      itm.collection = tmp.parent_path().filename().string();

      if (itm.collection == "mini_newsgroups")
        continue;

      // Count words
      regex expr("([^\\W_0123456789])+");
      auto regit = sregex_iterator(txt.begin(), txt.end(), expr);
      auto end = sregex_iterator();
      while (regit != end) {
        string word = (*regit++).str();
        // If word is a stop word ignore
        if (stop_words.find(word) != stop_words.end())
          continue;
        // Add to map of words
        auto map_itr = itm.word_count.find(word);
        if (map_itr == itm.word_count.end())
          itm.word_count[word] = 1;
        else
          map_itr->second++;
      }
      items.push_back(itm);
    }
  }
}

Parser::Parser(std::vector<NewsItem> itms) : items(std::move(itms)) {}

WordMatrix Parser::getMatrix() {
  return WordMatrix(items);
}

vector<string> Parser::get_classes() {
  set<string> preoutput;
  for (const auto &itm : items) {
    preoutput.insert(itm.collection);
  }
  vector<string> output(preoutput.begin(), preoutput.end());
  return output;
}

vector<NewsItem> Parser::get_items() {
  return items;
}

void Parser::prune_per_class(size_t max_per_classes) {
  random_device r;
  prune_per_class(r(), max_per_classes);
}

void Parser::prune_per_class(uint_fast32_t random_seed, size_t max_per_classes) {
  mt19937 gen(random_seed);
  vector<string> classes = get_classes();
  vector<NewsItem> new_items;
  for (const string& cls : classes) {
      vector<NewsItem> tmp_itms;
      copy_if(items.begin(), items.end(), back_inserter(tmp_itms), [&](const NewsItem &itm) {
         return itm.collection == cls;
      });
      tmp_itms = sample(tmp_itms.begin(), tmp_itms.end(), max_per_classes, gen);
      new_items.insert(new_items.end(), tmp_itms.begin(), tmp_itms.end());
  }
  items = new_items;
}

Parser Parser::get_items_of_classes(const std::vector<std::string> &classes) {
  Parser new_parser = *this;
  for (const auto &itm : items) {
    bool found = false;
    for (const auto &cls : classes) {
      if (itm.collection == cls)
        found = true;
    }
    if (!found)
      new_parser.items.erase(remove(new_parser.items.begin(), new_parser.items.end(), itm), new_parser.items.end());
  }
  return new_parser;
}

Parser Parser::prune_classes(size_t n) {
  Parser new_parser = *this;
  random_device rd;
  mt19937 g(rd());
  vector<string> classes = get_classes();
  vector<string> new_classes = sample(classes.begin(), classes.end(), n, g);
  for (const auto &itm : items) {
    if (find(new_classes.begin(), new_classes.end(), itm.collection) == new_classes.end())
      new_parser.items.erase(remove(new_parser.items.begin(), new_parser.items.end(), itm), new_parser.items.end());
  }
  return new_parser;
}

void Parser::shuffle() {
    random_device rd;
    mt19937 g(rd());
    std::shuffle(items.begin(), items.end(), g);
}