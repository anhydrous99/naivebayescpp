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
  string key = "&apikey=" + apikey;
  CURL *curl;
  CURLcode res;
  string readBuffer;
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, (url + uri).c_str());
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
  return Call(url);
}

WebHandler::WebHandler(std::string key) : apikey(std::move(key)) {}

std::vector<NewsItem> WebHandler::getTop(int n) {
  if (!fs::exists("stop_words.txt") || fs::is_directory("stop_words.txt"))
    throw std::runtime_error("Error: stop_words.txt is not at the binary directory.\n");
  vector<NewsItem> items;

  // Get stop words into a vector
  ifstream stop_words_stream("stop_words.txt");
  vector<string> stop_words;
  string line;
  while (getline(stop_words_stream, line))
    stop_words.push_back(line);

  regex expr("([^\\W_0123456789])+");
  const int max_per_call = 100;
  for (int i = 0, j = 0; i < n; i += max_per_call, j++) {
    string contents;
    int rm = n - (i * max_per_call);
    map<string, string> args;
    args["country"] = "us";
    args["pageSize"] = to_string(min(rm, max_per_call));
    args["page"] = to_string(j);
    contents = Call(args);
    auto json_parse = json::parse(contents);
    if (!json_parse.contains("status"))
      throw runtime_error("Error: could parser json or could access internet");
    if (json_parse["status"].get<string>() == "error")
      throw runtime_error(json_parse["code"].get<string>() + json_parse["message"].get<string>());
    auto arr = json_parse["articles"];
    for (const auto &element :  arr) {
      NewsItem itm;
      itm.collection = "unknown";
      itm.path = element["url"].get<string>();
      itm.contents = element["content"].get<string>();
      itm.contents += " " + element["description"].get<string>();
      itm.contents += " " + element["title"].get<string>();

      // Count words
      auto regit = sregex_iterator(itm.contents.begin(), itm.contents.end(), expr);
      auto end = sregex_iterator();
      while (regit != end) {
        auto word = (*regit++).str();
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
  return items;
}
