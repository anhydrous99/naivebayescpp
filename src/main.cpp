#include "Parser.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Error: not enough arguments\n";
        return EXIT_FAILURE;
    }
    // Parse files
    cout << "Parsing text files!\n";
    Parser p((string(argv[1])));

    // Create a word matrix
    cout << "Generating word matrix!\n";
    WordMatrix mat = p.getMatrix();
    cout << "Total words: " << mat.getTotalWords() << endl;
    WordMatrix mat1 = mat.getMostFrequent(25);
    mat1.printFrequency();
    return EXIT_SUCCESS;
}

// TODO: create a way to use only a certain number of text files
// TODO: create test cases