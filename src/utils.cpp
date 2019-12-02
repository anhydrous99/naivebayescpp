//
// Created by Armando Herrera on 11/15/19.
//

#include "utils.h"
#include <iostream>
#include <fstream>
#include <random>
#include <future>
#include "Parser.h"
#include "WordMatrix.h"

using namespace std;

void print_to_file(const string &to_print, const string &path) {
  ofstream ostrm(path);
  ostrm << to_print;
}

void part1(const string &newgroup_path, bool print_latex, bool save_csv) {
  // Parse files
  cout << "Parsing text files!\n";

  // Create parsers
  Parser p(newgroup_path);
  Parser parsed_test1 = p;
  Parser parsed_test2 = p;
  Parser parsed_test3 = p;

  cout << "Randomly choosing categories!\n";

  // Init random generator
  random_device rd;
  mt19937 g(rd());

  // Randomly choose _classes/categories
  vector<string> classes(p.get_classes());
  auto bitr = classes.begin();
  auto eitr = classes.end();
  vector<string> test1_classes = sample(bitr, eitr, 5, g);
  vector<string> test2_classes = sample(bitr, eitr, 5, g);
  vector<string> test3_classes = sample(bitr, eitr, 10, g);

  cout << "Prunning number of text files per class!\n";

  // Prune parsers to k specified files
  parsed_test1.prune_per_class(10);
  parsed_test2.prune_per_class(20);
  parsed_test3.prune_per_class(10);

  cout << "Creating a WordMatrix!\n";

  // Create word matrices
  WordMatrix mat_test1 = parsed_test1.getMatrix();
  WordMatrix mat_test2 = parsed_test2.getMatrix();
  WordMatrix mat_test3 = parsed_test3.getMatrix();

  // Create sub-word matrices of _classes
  WordMatrix submat_test1 = mat_test1.block(test1_classes);
  WordMatrix submat_test2 = mat_test2.block(test2_classes);
  WordMatrix submat_test3 = mat_test3.block(test3_classes);

  submat_test1 = submat_test1.getMostFrequent(25);
  submat_test2 = submat_test2.getMostFrequent(25);
  submat_test3 = submat_test3.getMostFrequent(50);

  // Create stringstreams for save to files and printing
  ostringstream test1_oss;
  ostringstream test2_oss;
  ostringstream test3_oss;

  cout << "Calculating frequencies and probabilities\n";

  if (print_latex) {
    // Print frequencies for these sub-matrices
    submat_test1.printLatexFrequency(test1_oss);
    submat_test2.printLatexFrequency(test2_oss);
    submat_test3.printLatexFrequency(test3_oss);
  } else {
    submat_test1.printFrequency(test1_oss);
    submat_test2.printFrequency(test2_oss);
    submat_test3.printFrequency(test3_oss);
  }

  test1_oss << endl;
  test2_oss << endl;
  test3_oss << endl;

  if (print_latex) {
    // Print probabilites for the sub-matrices
    submat_test1.printLatexProbabilities(test1_oss);
    submat_test2.printLatexProbabilities(test2_oss);
    submat_test3.printLatexProbabilities(test3_oss);
  } else {
    submat_test1.printProbabilities(test1_oss);
    submat_test2.printProbabilities(test2_oss);
    submat_test3.printProbabilities(test3_oss);
  }

  test1_oss << endl;
  test2_oss << endl;
  test3_oss << endl;

  cout << "Frequencies & Probabilites: \n";
  string test1_str = test1_oss.str();
  string test2_str = test2_oss.str();
  string test3_str = test3_oss.str();

  if (!print_latex && save_csv) {
    print_to_file(test1_str + "\n" + test2_str + "\n" + test3_str, "AssignmentPart1.csv");
  }

  cout << test1_str << endl << test2_str << endl << test3_str;
}

Parser optimizer(const Parser &p, size_t n_classes, size_t n_textfiles, size_t most_frequent, size_t n_iterations) {
  // Init random number generator
  random_device rd;
  mt19937 gen(rd());

  // Create lambda function that will test accuracy
  auto test_lambda = [](uint_fast32_t random_seed, size_t n_clss, size_t n_tf, size_t mf, Parser p) {
    mt19937 gen(random_seed);
    vector<string> classes = p.get_classes();
    classes = sample(classes.begin(), classes.end(), n_clss, gen);
    p = p.get_items_of_classes(classes);
    vector<NewsItem> items = p.get_items();
    p.prune_per_class(gen(), n_tf);
    WordMatrix mat = p.getMatrix().getMostFrequent(mf);
    size_t correct = 0;
    for (const NewsItem &itm : items) {
      if (itm.collection == mat.predict(itm))
        correct++;
    }
    return static_cast<float>(correct) / static_cast<float>(items.size());
  };

  // Create lambda function that recreates steps from a seed
  auto get_lambda = [](uint_fast32_t random_seed, size_t n_clss, size_t n_tf, Parser p) {
    mt19937 gen(random_seed);
    vector<string> cls = p.get_classes();
    cls = sample(cls.begin(), cls.end(), n_clss, gen);
    p = p.get_items_of_classes(cls);
    p.prune_per_class(gen(), n_tf);
    return p;
  };

  cout << "Optimizer - queuing tasks...\n";
  vector<future<float>> futures; // Stores futures -- for asynchronous execution
  vector<uint_fast32_t> seeds; // Stores random seeds
  for (size_t i = 0; i < n_iterations; i++) {
    uint_fast32_t seed = gen();
    seeds.push_back(seed);
    futures.push_back(async(launch::async, test_lambda, seed, n_classes, n_textfiles, most_frequent, p));
  }

  float mx = 0; // Best accuracy
  uint_fast32_t max_seed = 0; // Seed of best accuracy
  cout << "Optimizer - queued all tasks.. waiting for all queued tasks to finish...\n";
  for (size_t i = 0; i < futures.size(); i++) {
    futures[i].wait(); // Wait for thread to finish
    if (i % 10 == 0)
      cout << "Optimizer - iteration : " << i << " - done\n";
    float current = futures[i].get(); // Get threads output
    if (current > mx) { // Check for maximum accuracy
      mx = current;
      max_seed = seeds[i];
    }
  }
  cout << "Optimizer - finished optimizing for " << n_iterations << " iterations\n";
  return get_lambda(max_seed, n_classes, n_textfiles, p); // Recreate best accuracy parser
}
