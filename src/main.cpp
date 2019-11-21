#include <iostream>
#include <cxxopts.hpp>

#include "Parser.h"
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
  } else if (arg_results.count("run_second_part") != 0) {
    if (arg_results.count("full_path") == 1) {
      // TODO: run offline benchmarks
      // 1. The classifier is trained on the mini group benchmarked against the full group
      Parser mini_newsgroup_parser(arg_results["path"].as<string>());
      Parser parsed_test1 = mini_newsgroup_parser;
      Parser parsed_test2 = mini_newsgroup_parser;
      Parser parsed_test3 = mini_newsgroup_parser;
      // Prune parsers to k specified files
      parsed_test1.prune_per_class(10);
      parsed_test2.prune_per_class(20);
      parsed_test3.prune_per_class(10);
      // Create word matrices
      WordMatrix mat_test1 = parsed_test1.getMatrix().prune_classes(5).getMostFrequent(25);
      WordMatrix mat_test2 = parsed_test2.getMatrix().prune_classes(5).getMostFrequent(25);
      WordMatrix mat_test3 = parsed_test3.getMatrix().prune_classes(10).getMostFrequent(50);
      // Parse new group of news items
      Parser newsgroup_parser(arg_results["full_path"].as<string>());
      vector<NewsItem> to_clasifiy = newsgroup_parser.get_items();
    } else {
      // TODO: run online benchmarks
      // 1. Create re-classifier to reclassify according to online classifications
      // 2. Train on re-classified mini-newsgroup data and inference online data
      // 3. Train on online data and benchmark the mini and full newsgroup items
    }
  }

  return EXIT_SUCCESS;
}
