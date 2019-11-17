#include <iostream>
#include <cxxopts.hpp>

#include "utils.h"

using namespace std;

int main(int argc, char **argv) {
  cxxopts::Options options("naivebayescpp", "A NaiveBayes Classifier");
  options.add_options()
      ("p,path", "Path to mini_newsgroup dataset", cxxopts::value<string>())
      ("save_csv", "Save a CSV File of the results")
      ("h,help", "Displays help");
  options.add_options("part_1")
      ("run_first_part", "Run first part's Code")
      ("test_latex", "Print latex when testing");
  options.add_options("part_2")
      ("run_second_part", "Run second part's Code")
      ("f,full_path", "Path to full newsgroup dataset", cxxopts::value<string>());
  auto arg_results = options.parse(argc, argv);
  if (arg_results.arguments().empty() || arg_results.count("help") != 0) {
    cout << options.help({"", "part_1", "part_2"});
    return EXIT_SUCCESS;
  }
  if (arg_results.count("path") == 0) {
    cout << options.help();
    return EXIT_FAILURE;
  }
  string newgroup_path = arg_results["path"].as<string>();
  if (arg_results.count("run_first_part") != 0) {
    part1(newgroup_path, (arg_results.count("test_latex") != 0),
          (arg_results.count("save_csv") != 0));
  }

  return EXIT_SUCCESS;
}
