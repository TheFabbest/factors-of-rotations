#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string.h>
#include <stack>
#include <functional>
#include <fstream>
#include <algorithm>
#include "tree.hpp"
#include "SAIS.hpp"
#include "duval.hpp"
#include "leastRotation.hpp"
#include "computeStructures.hpp"
using namespace std;

unsigned long getAlphabetSize(const char * const string, const unsigned long length) {
    char max_char = string[0];
    char min_char = string[0];
    for (unsigned long i = 0; i < length; ++i) {
        if (string[i] > max_char) {
            max_char = string[i];
        }
        if (string[i] < min_char) {
            min_char = string[i];
        }
    }
    return max_char - min_char + 1;
}

char mapping(char c){
    return c - 96;
}

char* readFile(const char* filename, unsigned long &size){
    static const unsigned long MAX = 15;

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file.\n";
        return nullptr;
    }

    size = file.tellg();
    if (size > MAX) size = MAX;
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size + 1]; // +1 for null terminator
    if (!file.read(buffer, size)) {
        std::cerr << "Failed to read file.\n";
        delete[] buffer;
        return nullptr;
    }
    buffer[size] = '\0'; // Null-terminate the string
    return buffer;
}

void PrintAllFactorsNaive(const char *input_word, const unsigned long word_length) {
    cout << "Computing factors naively for " << input_word << ": ";
    char* rotation = new char[word_length+1];
    for (unsigned long i = 0; i < word_length; ++i) {
        rotate_copy(input_word, input_word+i, input_word+word_length, rotation);
        for (string s : duval(string(rotation))) {
            cout << s << ", ";
        }
    }
    cout << endl;
}

void PrintAllFactors(const char *input_word, const unsigned long word_length) {
    // find smallest rotation
    char word[word_length+1];
    const unsigned long rot = least_rotation(input_word, word_length);
    rotate_copy(input_word, input_word+rot, input_word+word_length, word);
    word[word_length] = '\0';
    // check if it's periodic (naively)
    const vector<string> factors = duval(string(word));
    if (factors.size() == 1) {
        const unsigned long alphabet_size = getAlphabetSize(word, word_length);
        // rank array
        unsigned long *SA = SAIS(word, word_length, alphabet_size)+1;
        unsigned long *rank = rankArrayFromSA(SA, word_length);

        // Lyndon Table
        unsigned long Lyn[word_length];
        LongestLyndon(word, word_length, rank, Lyn);

        // Lyndon Suffix Table
        unsigned long LynS[word_length];
        LyndonSuffixTable(word, word_length, LynS);

        // loop (Lyndon Suffix Table)
        cout << "Factors from left tree: " << endl;
        for (unsigned long i = 0; i < word_length-1; ++i) {
            unsigned long len = LynS[i];
            for (unsigned long j = i-len+1; j <= i; ++j) {
                cout << word[j];
            }
            cout << ", ";
        }
        
        // loop (Lyndon Table)
        cout << endl;
        cout << "Factors from right tree: " << endl;
        for (unsigned long i = 1; i < word_length; ++i) {
            unsigned long len = Lyn[i];
            for (unsigned long j = i; j < i+len; ++j) {
                cout << word[j];
            }
            cout << ", ";
        }
        cout << endl;

        delete[] (SA-1);
    }
    else {
        cout << endl << word << " is periodic and its factors are: ";
        for (string f: factors) {
            cout << f << ", ";
        }
        cout << endl;

        unsigned long first_factor_length = factors[0].length();
        PrintAllFactors(factors[0].c_str(), first_factor_length);
    }
}

#endif