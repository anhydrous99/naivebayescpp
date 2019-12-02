//
// Created by armando on 11/15/19.
//

#include "WebHandler.h"
#include "filesystem.h"
#include <regex>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((string *) userp)->append((char *) contents, size * nmemb);
  return size * nmemb;
}

string WebHandler::Call(const std::string &uri) {
  string key = "&apikey=" + _key;
  CURL *curl;
  CURLcode res;
  string readBuffer;
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, (url + uri + key).c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl/1.0");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    // Check for errors
    if (res != CURLE_OK)
      cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);

    // always cleanup
    curl_easy_cleanup(curl);
  }
  return readBuffer;
}

string WebHandler::Call(const std::map<std::string, std::string> &args) {
  std::string uri;
  bool first = true;
  for (const auto &arg_pair : args) {
    uri += ((first) ? "" : "&") + arg_pair.first + "=" + arg_pair.second;
    if (first)
      first = false;
  }
  return Call(uri);
}

void WebHandler::getStopWords() {
    if (!_stop_words_init) {
        if (!fs::exists("stop_words.txt") || fs::is_directory("stopwords.txt"))
            throw runtime_error("Error: stop_words.txt is not at the binary directory.\n");
        ifstream stop_words_stream("stop_words.txt");
        string line;
        while (getline(stop_words_stream, line))
            _stop_words.insert(line);
        _stop_words_init = true;
    }
}

vector<NewsItem> WebHandler::sendQuery(const string &collection) {
    getStopWords();
    vector<NewsItem> output;
    map<string, string> args;
    args["pageSize"] = "40";
    args["q"] = collection.substr(collection.rfind('.') + 1);

    regex expr("([^\\W_0123456789])+");
    string contents = Call(args);
    auto json_parser = json::parse(contents);
    if (!json_parser.contains("status"))
        throw runtime_error("Error: could not parse json or could not access internet");
    if (json_parser["status"].get<string>() == "error")
        throw runtime_error(json_parser["code"].get<string>() + json_parser["message"].get<string>());
    auto arr = json_parser["articles"];
    for (const auto &element : arr) {
        NewsItem itm;
        itm.collection = collection;
        itm.path = (element["url"] != nullptr) ? " " + element["url"].get<string>() : " ";
        itm.contents = (element["title"] != nullptr) ? " " + element["title"].get<string>() : " ";
        itm.contents += (element["description"] != nullptr) ? " " + element["description"].get<string>() : " ";
        itm.contents += (element["content"] != nullptr) ? " " + element["content"].get<string>() : " ";

        // Count words
        auto regit = sregex_iterator(itm.contents.begin(), itm.contents.end(), expr);
        auto end = sregex_iterator();
        while (regit != end) {
            auto word = (*regit++).str();
            if (find(_stop_words.begin(), _stop_words.end(), word) != _stop_words.end())
                continue;
            auto map_itr = itm.word_count.find(word);
            if (map_itr == itm.word_count.end())
                itm.word_count[word] = 1;
            else
                map_itr->second++;
        }
        output.push_back(itm);
    }
    return output;
}

std::vector<NewsItem> WebHandler::sendQueries(const vector<string> & collections) {
    vector<NewsItem> output;
    for (const string &cls : collections) {
        vector<NewsItem> tmp = sendQuery(cls);
        output.insert(output.end(), tmp.begin(), tmp.end());
    }
    return output;
}

WebHandler::WebHandler(std::string key) : _key(std::move(key)) {}

WebHandler::WebHandler() {
    ifstream t("webconfig.json");
    if (!t.is_open())
        throw runtime_error("Error: couldn't find webconfig.json in binary path.\n");
    string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
    auto json_parser = json::parse(str);
    if (json_parser.count("key") != 1)
        throw runtime_error("Error: key not in json file.\n");
    _key = json_parser["key"].get<string>();
}

void WebHandler::setKey(const std::string &key) { _key = key; }

string WebHandler::getKey() { return _key; }
