#include <iostream>
#include <cxxopts.hpp>

#include "utils.h"

using namespace std;

int main(int argc, char **argv) {
  cxxopts::Options options("naivebayescpp", "A NaiveBayes Classifier");
  options.add_options()
  ("p,path", "Path to newsgroup dataset")
  ("t,test", "Run newgroup tests")
  ("test_latex", "Print latex when testing")
  ("h,help", "Displays help");
  auto arg_results = options.parse(argc, argv);
  if (arg_results.arguments().empty() || arg_results.count("help") != 0) {
    cout << options.help();
    return EXIT_SUCCESS;
  }
  if (arg_results.count("path") == 0) {
    cout << options.help();
    return EXIT_FAILURE;
  }
  string newgroup_path = arg_results["path"].as<string>();
  if (arg_results.count("test") != 0) {
    test(newgroup_path, (arg_results.count("test_latex") != 0));
  }
  return EXIT_SUCCESS;
}
