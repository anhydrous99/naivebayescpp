#include <iostream>
#include <cxxopts.hpp>
#include <chrono>

#include "Parser.h"
#include "utils.h"

using namespace std;
using namespace chrono;
typedef high_resolution_clock hrc;


float test_battery(const vector<NewsItem> &to_test, WordMatrix classifier, const string &test_name) {
  auto t1 = hrc::now();
  size_t correct = 0;
  for (const NewsItem& itm : to_test) {
    string classified = classifier.predict(itm);
    if (itm.collection == classified)
      correct++;
  }
  auto t2 = hrc::now();
  cout << "Test " << test_name << ": " << duration_cast<milliseconds>(t2 - t1).count() << " ms\n";
  return static_cast<float>(correct) / static_cast<float>(to_test.size());
}

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
      ("f,full_path", "Path to full newsgroup dataset", cxxopts::value<string>())
      ("optimizer_iterations", "Number of iterations to run the optimizer",
              cxxopts::value<size_t>()->default_value("100"))
      ("classify_trained", "Run classifier on data it was trained on");
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
    if (arg_results.count("full_path") == 1 || arg_results.count("classify_trained") == 1) {

      // ******************* RUN OFFLINE TESTS ********************

      // 1. The classifier is trained on the mini group benchmarked against the full group
      cout  << "Parsing\n";
      auto t1 = hrc::now();
      Parser mini_newsgroup_parser(arg_results["path"].as<string>());
      auto t2 = hrc::now();
      cout << "Parse time: " << duration_cast<milliseconds>(t2 - t1).count() << " ms\n";

      cout << "Optimizing\n";
      size_t n_iterations = arg_results["optimizer_iterations"].as<size_t>();
      t1 = hrc::now();
      Parser parsed_test1 = optimizer(mini_newsgroup_parser, 5, 10, 25, n_iterations);
      Parser parsed_test2 = optimizer(mini_newsgroup_parser, 5, 20, 25, n_iterations);
      Parser parsed_test3 = optimizer(mini_newsgroup_parser, 10, 10, 50, n_iterations);
      t2 = hrc::now();
      cout << "Optimization time: " << duration_cast<milliseconds>(t2 - t1).count() << " ms\n";

      // Create word matrices
      cout << "Create word matrices\n";
      t1 = hrc::now();
      WordMatrix mat_test1 = parsed_test1.getMatrix().getMostFrequent(25);
      WordMatrix mat_test2 = parsed_test2.getMatrix().getMostFrequent(25);
      WordMatrix mat_test3 = parsed_test3.getMatrix().getMostFrequent(50);
      t2 = hrc::now();
      cout << "Word matrix creation time: " << duration_cast<milliseconds>(t2 - t1).count() << " ms\n";

      // Run tests
      cout << "Running test batteries\n";
      float test1_accuracy = test_battery(parsed_test1.get_items(), mat_test1,
          "5 class, 25 most frequent words, 10 text files per class");
      float test2_accuracy = test_battery(parsed_test2.get_items(), mat_test2,
          "5 class, 25 most frequent words, 20 text files per class");
      float test3_accuracy = test_battery(parsed_test3.get_items(), mat_test3,
          "10 class, 50 most frequent words, 10 text files per class");

      // Display results
      cout << " Test 1 - accuracy " << test1_accuracy << endl;
      cout << " Test 2 - accuracy " << test2_accuracy << endl;
      cout << " Test 3 - accuracy " << test3_accuracy << endl;
    } else {
      // TODO: run online benchmarks
      // 1. Create re-classifier to reclassify according to online classifications
      // 2. Train on re-classified mini-newsgroup data and inference online data
      // 3. Train on online data and benchmark the mini and full newsgroup items
    }
  }

  return EXIT_SUCCESS;
}
