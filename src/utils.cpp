//
// Created by Armando Herrera on 11/15/19.
//

#include "utils.h"
#include <iostream>
#include <fstream>
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
