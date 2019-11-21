//
// Created by armando on 11/10/19.
//

#include "Parser.h"
#include "filesystem.h"
#include <regex>
#include <set>
#include <random>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <streambuf>
#include <stdexcept>

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
  vector<string> stop_words;
  string line;
  while (getline(stop_words_stream, line))
    stop_words.push_back(line);

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
        std::string word = (*regit++).str();
        // If word is a stop word ignore
        if (std::find(stop_words.begin(), stop_words.end(), word) != stop_words.end())
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

void Parser::prune_per_class(unsigned long max_per_classes) {
  random_device r;
  mt19937 gen(r());

  vector<unsigned long> indices(items.size());
#ifdef __linux__
  iota(indices.begin(), indices.end(), 0);
#elif __WIN32
  for (int i = 0; i < indices.size(); i++)
      indices[i] =i;
#endif
  shuffle(indices.begin(), indices.end(), gen);

  map<string, unsigned long> item_count;
  vector<NewsItem> new_items;
  for (const auto &itm : items) {
    auto itr = item_count.find(itm.collection);
    if (itr != item_count.end()) {
      if (itr->second != max_per_classes) {
        new_items.push_back(itm);
        itr->second++;
      }
    } else {
      item_count[itm.collection];
    }
  }
  items = new_items;
}