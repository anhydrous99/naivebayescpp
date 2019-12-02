#include <iostream>
#include <cxxopts.hpp>
#include <chrono>
#include <stdexcept>

#include "Parser.h"
#include "utils.h"

using namespace std;
using namespace chrono;
typedef high_resolution_clock hrc;


float test_battery(const vector<NewsItem> &to_test, WordMatrix classifier, const string &test_name) {
    auto t1 = hrc::now();
    size_t correct = 0;
    for (const NewsItem &itm : to_test) {
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
            ("part_one", "Run this project's first part according to assignment")
            ("part_two", "Run this project's second part according to assignment")
            ("run_online", "Run the naive bayes classifier on newapi.org's data")
            ("f,full_path", "Path to full newsgroup dataset (Only when running second part), will use data at "
                            "--path when not set", cxxopts::value<string>())
            ("optimizer_iterations", "Number of iterations to run the optimizer",
             cxxopts::value<size_t>()->default_value("100"))
            ("test_latex", "Print latex when testing only when running part 1")
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
    if (arg_results.count("part_one") != 0) {
        part1(newgroup_path, (arg_results.count("test_latex") != 0),
              (arg_results.count("save_csv") != 0));
    } else if (arg_results.count("part_two") != 0) {
        // ******************* RUN OFFLINE TESTS ********************

        // 1. The classifier is trained on the mini group benchmarked against the full group
        hrc::time_point t1, t2;
        Parser mini_newsgroup_parser;
        try {
            cout << "Parsing\n";
            t1 = hrc::now();
            mini_newsgroup_parser = Parser(arg_results["path"].as<string>());
            mini_newsgroup_parser.shuffle();
            t2 = hrc::now();
            cout << "Parse time: " << duration_cast<milliseconds>(t2 - t1).count() << " ms\n";
        } catch (const runtime_error &e) {
            cerr << e.what() << endl;
            return EXIT_FAILURE;
        }

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
        float test1_accuracy, test2_accuracy, test3_accuracy;
        if (arg_results.count("full_path") == 0) {
            test1_accuracy = test_battery(mini_newsgroup_parser.get_items_of_classes(mat_test1.getClasses()).
                    get_items(), mat_test1, "5 class, 25 most frequent words, 10 text files per class");
            test2_accuracy = test_battery(mini_newsgroup_parser.get_items_of_classes(mat_test2.getClasses()).
                    get_items(), mat_test2, "5 class, 25 most frequent words, 20 text files per class");
            test3_accuracy = test_battery(mini_newsgroup_parser.get_items_of_classes(mat_test3.getClasses()).
                    get_items(), mat_test3, "10 class, 50 most frequent words, 10 text files per class");
        } else {
            Parser new_parser(arg_results["full_path"].as<string>());
            test1_accuracy = test_battery(new_parser.get_items_of_classes(mat_test1.getClasses()).get_items(),
                                          mat_test1, "5 class, 25 most frequent words, 10 text files per class");
            test2_accuracy = test_battery(new_parser.get_items_of_classes(mat_test2.getClasses()).get_items(),
                                          mat_test2, "5 class, 25 most frequent words, 20 text files per class");
            test3_accuracy = test_battery(new_parser.get_items_of_classes(mat_test3.getClasses()).get_items(),
                                          mat_test3, "10 class, 50 most frequent words, 10 text files per class");
        }

        // Display results
        cout << " Test 1 - accuracy " << test1_accuracy << endl;
        cout << " Test 2 - accuracy " << test2_accuracy << endl;
        cout << " Test 3 - accuracy " << test3_accuracy << endl;
    } else if (arg_results.count("run_online") == 1) {
        // TODO: run online benchmarks
        // 1. Create re-classifier to reclassify according to online classifications
        // 2. Train on re-classified mini-newsgroup data and inference online data
        // 3. Train on online data and benchmark the mini and full newsgroup items
    } else {
        cout << options.help();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
