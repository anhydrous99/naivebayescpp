//
// Created by armando on 11/10/19.
//

#include "Parser.h"
#include <algorithm>
#include <iostream>
#include <streambuf>
#include <stdexcept>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace std;

static string read_file(const fs::path& p) {
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
            Item itm;
            // Read file to string
            string txt = read_file(tmp);
            itm.path = tmp.string();
            itm.contents = txt;

            // Parse header
            istringstream stream(txt);
            while (getline(stream, line)) {
                if (line.empty())
                    break;
                int colon_index = line.find(':');
                string name = line.substr(0, colon_index);
                string contents = line.substr(colon_index + 1);
                itm.headers[name] = contents;
            }

            // Get classification
            itm.collection = itm.headers["Newsgroups"];

            // Count words
            boost::regex expr{"([^\\W_0123456789])+"};
            boost::regex_token_iterator<std::string::iterator> regit{txt.begin(), txt.end(), expr};
            boost::regex_token_iterator<std::string::iterator> end;
            while (regit != end) {
                std::string word = (*regit++);
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