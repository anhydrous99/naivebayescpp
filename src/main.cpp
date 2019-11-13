#include "Parser.h"
#include <iostream>
#include <random>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Error: not enough arguments\n";
        return EXIT_FAILURE;
    }
    // Parse files
    cout << "Parsing text files!\n";

    // Create parsers
    Parser p((string(argv[1])));
    Parser parsed_test1 = p;
    Parser parsed_test2 = p;
    Parser parsed_test3 = p;

    cout << "Randomly choosing categories!\n";

    // Init random generator
    std::random_device rd;
    std::mt19937 g(rd());

    // Randomly choose classes/categories
    vector<string> classes(p.get_classes());
    auto c_begin = classes.begin();
    auto c_end = classes.end();
    shuffle(c_begin, c_end, g);
    vector<string> test1_classes(c_begin, (c_begin + 5));
    shuffle(c_begin, c_end, g);
    vector<string> test2_classes(c_begin, (c_begin + 5));
    shuffle(c_begin, c_end, g);
    vector<string> test3_classes(c_begin, (c_begin + 10));

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

    // Create sub-word matrices of classes
    WordMatrix submat_test1 = mat_test1.block(test1_classes);
    WordMatrix submat_test2 = mat_test2.block(test2_classes);
    WordMatrix submat_test3 = mat_test3.block(test3_classes);

    submat_test1 = submat_test1.getMostFrequent(25);
    submat_test2 = submat_test2.getMostFrequent(25);
    submat_test3 = submat_test3.getMostFrequent(50);

    // Create stringstreams for save to files and printing
    ostringstream test1_freq;
    ostringstream test2_freq;
    ostringstream test3_freq;
    ostringstream test1_prob;
    ostringstream test2_prob;
    ostringstream test3_prob;

    cout << "Calculating frequencies and probabilities\n";

    // Print frequencies for these sub-matrices
    submat_test1.printFrequency(test1_freq);
    submat_test2.printFrequency(test2_freq);
    submat_test3.printFrequency(test3_freq);

    // Print probabilites for the sub-matrices
    submat_test1.printProbabilities(test1_prob);
    submat_test2.printProbabilities(test2_prob);
    submat_test3.printProbabilities(test3_prob);

    cout << "Frequencies & Probabilites: \n";
    string test1_freq_str = test1_freq.str();
    string test2_freq_str = test2_freq.str();
    string test3_freq_str = test3_freq.str();
    string test1_prob_str = test1_prob.str();
    string test2_prob_str = test2_prob.str();
    string test3_prob_str = test3_prob.str();

    cout << test1_freq_str << endl <<
    test2_freq_str << endl <<
    test3_freq_str << endl <<
    test1_prob_str << endl <<
    test2_prob_str << endl <<
    test3_prob_str;

    return EXIT_SUCCESS;
}

// TODO: create test cases