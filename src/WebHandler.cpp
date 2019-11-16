//
// Created by armando on 11/15/19.
//

#include <iostream>
#include <curl/curl.h>

#include "WebHandler.h"

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string WebHandler::Call(const std::string& uri) {
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
    for (const auto& arg_pair : args) {
        uri += ((first) ? "" : "&") + arg_pair.first + "=" + arg_pair.second;
        if (first)
            first = false;
    }
    Call(url);
}

WebHandler::WebHandler(std::string key) : apikey(std::move(key)) {}

