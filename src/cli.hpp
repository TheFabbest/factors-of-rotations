#ifndef CLI_HPP
#define CLI_HPP
#include <iostream>
#include <string.h>
#include "utils.hpp"
using namespace std;

void help() {
    cout << "Usage: ./program command [word] [-v|--verbose]" << endl;
    cout << "Commands:" << endl;
    cout << "  help                    : Show this help message." << endl;
    cout << "  conjugatefactors <word> : Print all conjugate factors of the given word." << endl;
    cout << "  showtrees <word>        : Show the Left and Right Lyndon Trees of the given word." << endl;
    cout << "  suffixarray <word>      : Compute and display the suffix array of the given word." << endl;
    cout << "  test                    : Run all tests." << endl;
    cout << "Flags:" << endl;
    cout << "  -v, --verbose          : Enable verbose output." << endl;
    cout << "Unknown flags will be ignored with a warning." << endl;
    cout << "Malformed commands will show this help message if required parameters are missing or wrong, unneeded parameters will be ignored with no warning." << endl;
    cout << "This program is designed by Fabrizio Apuzzo as part of its Bachelor's Thesis at University of Naples Federico II. Clarity, simplicity and fidelity were prioritized over performance." << endl;
}

void suffixArray(const char* const input_word, const bool verbose) {
    const unsigned long word_length = strlen(input_word);
    if (word_length == 0) {
        cout << "Please provide a non-empty word." << endl;
        return;
    }

    if (verbose) {
        cout << "Computing suffix array for the word: " << input_word << endl;
    }

    unsigned long* const SA = new unsigned long[word_length];
    optimalSuffixArray(input_word, SA, word_length);

    aux_PrintArray(SA, word_length, "Suffix Array");

    delete[] SA;
}

void showTrees(const char* const input_word, const bool verbose) {
    const unsigned long word_length = strlen(input_word);
    char * const word = new char[word_length+1];
    const unsigned long least = least_rotation(input_word, word_length);
    rotate_copy(input_word, input_word+least, input_word+word_length, word);
    word[word_length] = '\0';
    vector<string> factors = duval(string(word));
    if (factors.size()!=1){
        if (verbose) {
            cout << word << " is periodic, trees are not defined: showing trees for its primitive root: " << factors[0] << endl;
        }
        delete[] word;
        showTrees(factors[0].c_str(), verbose);
        return;
    }
    
    unsigned long *LynS = new unsigned long[word_length];
    unsigned long *Lyn = new unsigned long[word_length];
    unsigned long *rank = new unsigned long[word_length];
    unsigned long *SA = new unsigned long[word_length];
    Node** roots = new Node* [word_length];

    optimalSuffixArray(word, SA, word_length);
    rankArrayFromSA(SA, word_length, rank);
    LyndonSuffixTable(word, word_length, LynS);
    LongestLyndon(word, word_length, rank, Lyn);
    Node* leftTree = LeftLyndonTree(word, word_length, LynS, roots);
    Node* rightTree = RightLyndonTree(word, word_length, rank, Lyn);

    if (verbose) {
        cout << "Working on smallest conjugate: " << word << endl;
    }
    cout << "Left Lyndon Tree: " << endl;
    leftTree->Print();
    cout << endl;
    separator();
    cout << "Right Lyndon Tree: " << endl;
    rightTree->Print();
    cout << endl;

    delete[] word;
    delete[] LynS;
    delete[] Lyn;
    delete[] rank;
    delete[] SA;
    delete[] roots;
    delete leftTree;
    delete rightTree;
}

#endif // CLI_HPP