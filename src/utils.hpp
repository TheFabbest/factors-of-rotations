#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string.h>
#include <stack>
#include <functional>
#include <fstream>
#include <algorithm>
#include "tree.hpp"
#include "optimalSuffixArray.hpp"
#include "duval.hpp"
#include "leastRotation.hpp"
#include "computeStructures.hpp"
using namespace std;

void aux_PrintArray(const unsigned long * const array, const unsigned long length)
{
    for (unsigned long i = 0; i < length; ++i) {
        if (i) cout << ", ";
        cout << array[i];
    }
    cout << endl;
}

void aux_PrintArray(const unsigned long * const array, const unsigned long length, const string array_name) {
    cout << array_name << ": ";
    aux_PrintArray(array, length);
}

void separator() {
    cout << "-----------------------------------" << endl;
}

char* readFile(const string filename, const unsigned long max_size, unsigned long& size) {

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file.\n";
        return nullptr;
    }

    size = file.tellg() / sizeof(char);
    if (size > max_size) size = max_size;
    file.seekg(0, std::ios::beg);
    
    char *output_buffer = new char[(size * sizeof(char)) + 1];
    if (!file.read(output_buffer, size * sizeof(char))) {
        std::cerr << "Failed to read file.\n";
        return nullptr;
    }
    output_buffer[size * sizeof(char)] = '\0'; // Null-terminate the string
    return output_buffer;
}

unsigned long* readFile(const string filename, const unsigned long bytes_per_char, const unsigned long max_chars, unsigned long& size) {

    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        cerr << "Failed to open file.\n";
        return nullptr;
    }
    size = file.tellg() / bytes_per_char;
    if (size > max_chars && max_chars > 0) size = max_chars;
    file.seekg(0, ios::beg);
    unsigned long *output_buffer = new unsigned long[size * sizeof(unsigned long)];
    for (unsigned long i = 0; i < size; ++i) {
        unsigned long char_value = 0;
        file.read(reinterpret_cast<char*>(&char_value), bytes_per_char);
        output_buffer[i] = char_value;
    }
    return output_buffer;
}

string_view GetLastFactorOfPrefix(const char* const word, unsigned long prefix_length, const unsigned long* const LynS) {
    const unsigned long len = LynS[prefix_length-1];
    return string_view(word + prefix_length - len, len);
}

string_view GetLastFactorOfSuffix(const char* const word, unsigned long suffix_start, const unsigned long* const Lyn) {
    const unsigned long len = Lyn[suffix_start];
    return string_view(word + suffix_start, len);
}

void PrintPrefixesFactorsFromLynS(const char* const word, const unsigned long word_length, const unsigned long* const LynS) {
    for (unsigned long i = 0; i < word_length-1; ++i) {
        if (i) cout << ", ";
        cout << GetLastFactorOfPrefix(word, i+1, LynS);
    }
    cout << endl;
}

void PrintSuffixesFactorsFromLyn(const char* const word, const unsigned long word_length, const unsigned long* const Lyn) {
    for (unsigned long i = word_length; i > 1; --i) {
        if (i < word_length) cout << ", ";
        cout << GetLastFactorOfSuffix(word, i-1, Lyn);
    }
    cout << endl;
}


void PrintPrefixesFactorsFromLynSWithCorrespondingPrefix(const char * const word,
                               const unsigned long word_length,
                               const unsigned long* const LynS) {
    cout << "Exact factorization of each prefix, from LynS: " << endl;

    for (unsigned long prefix_len = 1; prefix_len < word_length; ++prefix_len) {
        cout << "Prefix " << string_view(word, prefix_len) << ": ";
        
        vector<string_view> factors;
        unsigned long j = prefix_len;
        while (j > 0) {
            string_view last = GetLastFactorOfPrefix(word, j, LynS);
            factors.insert(factors.end(), last);
            j -= last.size();
        }

        for (unsigned long k = 0; k < factors.size(); ++k) {
            if (k) cout << ", ";
            cout << factors[k];
        }
        cout << '\n';
    }
    cout << '\n';
}

void PrintSuffixesFactorsFromLynWithCorrespondingSuffix(const char * const word,
                               const unsigned long word_length,
                               const unsigned long* const Lyn) {
    cout << "Exact factorization of each suffix, from Lyn: " << endl;

    for (unsigned long suffix_start = 1; suffix_start < word_length; ++suffix_start) {
        cout << "Suffix " << string_view(word + suffix_start, word_length - suffix_start) << ": ";
        
        vector<string_view> factors;
        unsigned long j = suffix_start;
        while (j < word_length) {
            string_view last = GetLastFactorOfSuffix(word, j, Lyn);
            factors.insert(factors.end(), last);
            j += last.size();
        }

        for (unsigned long k = 0; k < factors.size(); ++k) {
            if (k) cout << ", ";
            cout << factors[k];
        }
        cout << '\n';
    }
    cout << '\n';
}


void PrintAllFactorsNaive(const char * const input_word, const unsigned long word_length) {
    cout << "Naively computing all factors of all rotations, with repetition:" << endl;
    char* rotation = new char[word_length+1];
    for (unsigned long i = 0; i < word_length; ++i) {
        rotate_copy(input_word, input_word+i, input_word+word_length, rotation);
        rotation[word_length] = '\0';
        cout << "Factors of rotation " << rotation << ": ";
        for (string s : duval(string(rotation))) {
            cout << s << ", ";
        }
        cout << endl;
    }
    delete[] rotation;
}


// this function shows what we implemented, it is just a proof of concept
void PrintAllFactors(const char * const input_word, const bool verbose) {
    // allocate memory
    const unsigned long word_length = strlen(input_word);
    char* word = new char[word_length+1];

    // find smallest rotation
    const unsigned long rot = least_rotation(input_word, word_length);
    rotate_copy(input_word, input_word+rot, input_word+word_length, word);
    word[word_length] = '\0';

    if (verbose) {
        cout << "Working on smallest conjugate at index " << rot << ": " << word << endl;
    }

    // check if it's periodic
    const vector<string> factors = duval(string(word));
    if (factors.size() == 1) {
        // compute suffix array
        unsigned long * const SA = new unsigned long[word_length];
        optimalSuffixArray(word, SA, word_length);

        // rank array
        unsigned long * const rank = new unsigned long[word_length];
        rankArrayFromSA(SA, word_length, rank);

        // Lyndon Table
        unsigned long* const Lyn = new unsigned long[word_length];
        LongestLyndon(word, word_length, rank, Lyn);

        // Lyndon Suffix Table (LynS)
        unsigned long* const LynS = new unsigned long[word_length];
        LyndonSuffixTable(word, word_length, LynS);

        if (verbose) {
            separator();
            
            // print factors for each prefix
            cout << "All factors that appear in prefixes, from LynS: " << endl;
            PrintPrefixesFactorsFromLynS(word, word_length, LynS);

            // print factors for each prefix, with corresponding prefix
            PrintPrefixesFactorsFromLynSWithCorrespondingPrefix(word, word_length, LynS);
            
            separator();

            // print factors for each suffix
            cout << "All factors that appear in suffixes, from Lyn: " << endl;
            PrintSuffixesFactorsFromLyn(word, word_length, Lyn);

            // print factors for each suffix, with corresponding suffix
            PrintSuffixesFactorsFromLynWithCorrespondingSuffix(word, word_length, Lyn);
        }
        else {
            PrintPrefixesFactorsFromLynS(word, word_length, LynS);
            PrintSuffixesFactorsFromLyn(word, word_length, Lyn);
        }

        delete[] SA;
        delete[] word;
        delete[] rank;
        delete[] Lyn;
        delete[] LynS;
    }
    else {
        if (verbose) cout << endl << word << " is periodic and its primitive root is: " << factors[0] << endl;

        delete[] word;

        unsigned long first_factor_length = factors[0].length();
        PrintAllFactors(factors[0].c_str(), verbose);
    }
}

#endif