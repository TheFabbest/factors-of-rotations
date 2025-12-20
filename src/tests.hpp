#ifndef TESTS_HPP
#define TESTS_HPP

#include <functional>
#include <math.h>
#include <chrono>
#include "SAIS.hpp"
#include "utils.hpp"
#include "naiveSuffix.hpp"

void aux_PrintArray(const unsigned long *array, const unsigned long length, const string array_name) {
    cout << array_name << ": ";
    for (unsigned long i = 0; i < length; ++i) {
        cout << array[i] << " ";
    }
    cout << endl;
}

void aux_test_verbose(const char * const testName, const char * const input_word, const char * const message, const bool do_print = false) {
    if (do_print) {
        cout << testName << " [" << input_word << "] " << message << endl;
    }
}

char *generateRandomString(const unsigned long SIZE, const char MIN_ALPHABET, const char MAX_ALPHABET) {
    const unsigned long ALPHABET_SIZE = MAX_ALPHABET - MIN_ALPHABET + 1;
    char *word = new char [SIZE+1];

    srand(time(0));
    for (unsigned long i = 0; i < SIZE; ++i){
        word[i] = MIN_ALPHABET + rand() % ALPHABET_SIZE;
    }
    word[SIZE] = '\0';
    return word;
}

void testForEachWordOfLength(const unsigned long length, const function<void (char[], unsigned long)> func) {
    char *word = new char[length+1];
    for (unsigned long i = 0; i < length; ++i) {
        word[i] = 'A';
    }
    word[length] = '\0';
    
    int limit = pow(length, length);
    for (int i = 0; i < limit; ++i)
    {
        func(word, length);
        int j = length-1;
        while(j < length) {
            ++word[j];
            if (word[j] > 'A'+length-1) {
                word[j]='A';
                j--;
            }
            else {
                break;
            }
        }
    }
    delete[] word;
}

void testForEachWordOfLength(const unsigned long length, const function<void (char[], unsigned long, bool)> func) {
    testForEachWordOfLength(length, [func](char w[], unsigned long l) { func(w, l, false); } );
}

void testRandomEdgeChars(const unsigned long SIZE){
    static const char MIN_CH = 'A';
    static const char MAX_CH = 'E';
    static const unsigned long ul_alphabet[] = {0, LONG_MAX, ULONG_MAX-2, ULONG_MAX-1, ULONG_MAX};

    const char * const word = generateRandomString(SIZE, MIN_CH, MAX_CH);
    unsigned long * const input_as_long = new unsigned long [SIZE];
    for (unsigned long i = 0; i < SIZE; ++i){
        input_as_long[i] = ul_alphabet[word[i]-MIN_CH];
    }
    
    unsigned long* const SA_optimal = new unsigned long [SIZE];
    optimalSuffixArray(input_as_long, SA_optimal, SIZE);
    
    const unsigned long * const suffix_array_SAIS = buildSuffixArray(word, SIZE);
    
    // look for differences
    for (unsigned long i = 0; i < SIZE; ++i) {
        if (SA_optimal[i] != suffix_array_SAIS[i]) {
            cout << "ERROR for string " << word << endl;
            cout << "ERROR at " << i << " - SA_optimal was '" << SA_optimal[i] << "' while SA_SAIS was '" << suffix_array_SAIS[i] << "'" << endl;
            aux_PrintArray(SA_optimal, SIZE, "SA_optimal");
            aux_PrintArray(suffix_array_SAIS, SIZE, "SA_SAIS");
            cin.get();
        }
    }

    delete[] word;
    delete[] input_as_long;
    delete[] SA_optimal;
    delete[] suffix_array_SAIS;
}

void testOneRandom(const unsigned long SIZE){
    char *word = generateRandomString(SIZE, 'A', 'Z');
    unsigned long *input_as_long = new unsigned long [SIZE];
    char_to_ulong_array(word, input_as_long, SIZE);
    
    unsigned long* SA_optimal = new unsigned long [SIZE];
    optimalSuffixArray(input_as_long, SA_optimal, SIZE);
    unsigned long *SA_naive = buildSuffixArray(word, SIZE);

    // look for differences
    for (unsigned long i = 0; i < SIZE; ++i) {
        if (SA_optimal[i] != SA_naive[i]) {
            cout << "ERROR for string " << word << endl;
            cout << "ERROR at " << i << " - SA_optimal was '" << SA_optimal[i] << "' while SA_native was '" << SA_naive[i] << "'" << endl;
            aux_PrintArray(SA_optimal, SIZE, "SA_optimal");
            aux_PrintArray(SA_naive, SIZE, "SA_naive");
            cin.get();
        }
    }
    
    delete[] SA_optimal;
    delete[] SA_naive;
    delete[] word;
    delete[] input_as_long;
}



void testFactorsLyn(const char input_word[], const unsigned long word_length, const bool verbose=false) {
    char * const word = new char[word_length+1];
    const unsigned long least = least_rotation(input_word, word_length);
    rotate_copy(input_word, input_word+least, input_word+word_length, word);
    word[word_length] = '\0';
    
    // this test skips periodic words
    if (duval(string(word)).size()!=1){
        aux_test_verbose("testFactorsLyn", input_word, "skipping test on periodic word", verbose);
        return;
    }

    unsigned long *SA = new unsigned long[word_length];
    optimalSuffixArray(word, SA, word_length);
    unsigned long *rank = new unsigned long[word_length];
    rankArrayFromSA(SA, word_length, rank);
    unsigned long *Lyn = new unsigned long[word_length];
    LongestLyndon(word, word_length, rank, Lyn);

    Node* rightTree = RightLyndonTree(word, word_length, rank, Lyn);

    vector<Node*> factors_in_suffixes, factors_in_prefixes;
    RightChildren(rightTree, factors_in_suffixes);

    string factors_from_right_tree = "";
    for (Node* factor: factors_in_suffixes) {
        factors_from_right_tree += factor->ToString("");
        factors_from_right_tree += ", ";
    }
    
    string factors_from_lyn = "";
    for (int i = 1; i < word_length; ++i) {
        int len = Lyn[i];
        for (int j = i; j < i+len; ++j) {
            factors_from_lyn += word[j];
        }
        factors_from_lyn += ", ";
    }

    if (factors_from_lyn != factors_from_right_tree) {
        cout << "ERROR: " << word << endl;
        cout << factors_from_lyn << endl;
        cout << factors_from_right_tree << endl;
        cin.get();
    }

    delete[] SA;
    delete[] rank;
    delete[] word;
    delete[] Lyn;
    delete rightTree;
}

void testFactorsLynS(const char input_word[], const unsigned long word_length, const bool verbose = false) {
    char * const word = new char[word_length+1];
    const unsigned long least = least_rotation(input_word, word_length);
    rotate_copy(input_word, input_word+least, input_word+word_length, word);
    word[word_length] = '\0';
    
    // this test skips periodic words
    if (duval(string(word)).size()!=1){
        aux_test_verbose("testFactorsLynS", input_word, "skipping test on periodic word", verbose);
        return;
    }

    unsigned long *LynS = new unsigned long[word_length];
    LyndonSuffixTable(word, word_length, LynS);

    Node** roots = new Node* [word_length];
    Node* leftTree = LeftLyndonTree(word, word_length, LynS, roots);
    
    vector<Node*> factors_in_prefixes;
    LeftChildren(leftTree, factors_in_prefixes);
    string factors_from_left_tree = "";
    for (Node* factor: factors_in_prefixes) {
        factors_from_left_tree += factor->ToString("");
        factors_from_left_tree += ", ";
    }
    
    string factors_from_lynS = "";
    for (unsigned long i = 0; i < word_length-1; ++i) {
        unsigned long len = LynS[i];
        for (int j = i-len+1; j <= i; ++j) {
            factors_from_lynS += word[j];
        }
        factors_from_lynS += ", ";
    }

    if (factors_from_lynS != factors_from_left_tree) {
        cout << "ERROR: " << word << endl;
        cout << factors_from_lynS << endl;
        cout << factors_from_left_tree << endl;
        cin.get();
    }
    
    if (verbose) {
        cout << factors_from_left_tree << endl;
        LeftChildrenWithPrefixNumber(leftTree);
        PrintPrefixesFactorsFromLynSWithCorrespondingPrefix(word, word_length, LynS);
    }
    delete leftTree;
    delete[] roots;
    delete[] word;
    delete[] LynS;
}

// i+Lyn[i] is basically the smallest index j>i for which rank[i] > rank[j]
void propertyTest1(const char word[], unsigned long word_length) {
    unsigned long *SA = new unsigned long[word_length];
    optimalSuffixArray(word, SA, word_length);
    unsigned long *rank = new unsigned long[word_length];
    rankArrayFromSA(SA, word_length, rank);
    unsigned long *Lyn = new unsigned long[word_length];
    LongestLyndon(word, word_length, rank, Lyn);
    for (int i = 0; i < word_length; ++i) {
        for (int j = i+1; j < i+Lyn[i]; ++j) {
            if (rank[i] > rank[j]) {
                cout << "ERROR: " << word << endl;
                cout << i << " " << j << endl;
                cout << rank[i] << " > " << rank[j] << endl;
                cin.get();
            }
        }
        if (i+Lyn[i] < word_length && rank[i] < rank[i+Lyn[i]]) {
            cout << "ERROR: " << word << endl;
            cout << i << " " << i+Lyn[i] << endl;
            cout << rank[i] << " < " << rank[i+Lyn[i]] << endl;
            cin.get();
        }
    }

    delete[] SA;
    delete[] rank;
    delete[] Lyn;
}

// for each factor beginning at index i of each rotation of the word it is true that rank[i] < rank[j] for each j in the factor
void propertyTest2(const char input_word[], unsigned long word_length, const bool verbose = false) {
    // this test skips periodic words
    if (duval(string(input_word)).size()!=1){
        aux_test_verbose("propertyTest2", input_word, "skipping test on periodic word", verbose);
        return;
    }
    char *word = new char[word_length+1];
    word[word_length] = '\0';
    unsigned long *SA = SAIS(input_word, word_length, getAlphabetSize(word,word_length))+1;
    unsigned long *rank = new unsigned long[word_length];
    rankArrayFromSA(SA, word_length, rank);
    for (unsigned long r = 0; r < word_length; ++r) {
        rotate_copy(input_word, input_word+r, input_word+word_length, word);
        vector<string> factors = duval(string(word));
        long i = 0;
        for (string factor: factors) {
            for (long j = 1; j < factor.length(); ++j) {
                long index_first = (word_length+i+r)%word_length;
                long index_second = (index_first+j)%word_length;
                if (rank[index_first] > rank[index_second]) {
                    cout << "ERROR: " << word << endl;
                    for (unsigned long k = 0; k < word_length; ++k) cout << rank[k] << " ";
                    cout << index_first << " " << index_second << endl;
                    cout << rank[index_first] << " > " << rank[index_second] << endl;
                    cin.get();
                }
            }
            i += factor.length();
        }
    }
    delete[] (SA-1);
    delete[] rank;
    delete[] word;
}

void testOptimalSuffixArray(const char input_word[], const unsigned long word_length) {
    // get alphabet size
    const unsigned long alphabet_size = getAlphabetSize(input_word, word_length);

    // calculate suffix array naively
    unsigned long *SA_optimal = new unsigned long[word_length];
    unsigned long *input_as_long = new unsigned long [word_length];
    char_to_ulong_array(input_word, input_as_long, word_length);

    optimalSuffixArray(input_as_long, SA_optimal, word_length);
    
    // calculate naively
    unsigned long *SA = buildSuffixArray(input_word, word_length);

    // look for differences
    for (unsigned long i = 0; i < word_length; ++i) {
        if (SA[i] != SA_optimal[i]) {
            cout << "-------------------------" << endl;
            cout << "ERROR: " << input_word << endl;
            cout << "SA[" << i << "] = " << SA[i] << ", SA_optimal[" << i << "] = " << SA_optimal[i] << endl;
            aux_PrintArray(SA, word_length, "SA");
            aux_PrintArray(SA_optimal, word_length, "SA_optimal");

            cout << "SUFFIXES:" << endl;
            cout << string(input_word+SA[i], word_length - SA[i]) << endl;
            cout << string(input_word+SA_optimal[i], word_length - SA_optimal[i]) << endl;
            cin.get();
        }
    }
    delete[] SA;
    delete[] SA_optimal;
    delete[] input_as_long;
}

void testForSize(const unsigned long test_size) {
    testForEachWordOfLength(test_size, testFactorsLyn);
    testForEachWordOfLength(test_size, testFactorsLynS);
    testForEachWordOfLength(test_size, propertyTest1);
    testForEachWordOfLength(test_size, propertyTest2);
    testForEachWordOfLength(test_size, testOptimalSuffixArray);
}

void loopRandomTests(const unsigned long TESTS, const unsigned long SIZE){
    cout << "Testing " << TESTS << " random strings of length " << SIZE << endl;
    for (unsigned long t = 0; t < TESTS; ++t){
        testOneRandom(SIZE);
    }
}

void executionTimeOptimalSuffixArray(const unsigned long SIZE, const unsigned long TESTS, const char MIN_ALPHABET_CHAR = 'A', const char MAX_ALPHABET_CHAR = 'Z'){
    const unsigned long MIN_ALPHABET = static_cast<unsigned long>(MIN_ALPHABET_CHAR);
    const unsigned long MAX_ALPHABET = static_cast<unsigned long>(MAX_ALPHABET_CHAR);
    const unsigned long ALPHABET_SIZE = MAX_ALPHABET - MIN_ALPHABET + 1;
    cout << "Performance tests for Optimal Suffix Array on alphabet " << MIN_ALPHABET_CHAR << "-" << MAX_ALPHABET_CHAR << ":" << endl;

    unsigned long *input_as_long = new unsigned long [SIZE];

    srand(time(0));
    for (unsigned long i = 0; i < SIZE; ++i){
        input_as_long[i] = MIN_ALPHABET + rand() % ALPHABET_SIZE;
    }

    unsigned long* SA_optimal = new unsigned long [SIZE];

    auto start = chrono::high_resolution_clock::now();
    for (unsigned long t = 0; t < TESTS; ++t){
        optimalSuffixArray(input_as_long, SA_optimal, SIZE);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Optimal Suffix Array for size " << SIZE << " over " << TESTS << " tests took " << elapsed.count() << " seconds." << endl;

    delete[] SA_optimal;
    delete[] input_as_long;
}

void testAll() {
    const unsigned long EXHAUSTIVE_LIMIT = 5;
    const unsigned long NUMBER_OF_TESTS_FOR_EDGE_CASES = 200;
    const unsigned long TEST_SIZE_FOR_EDGE_CASES = 100000;
    
    for (int size = 1; size <= EXHAUSTIVE_LIMIT; ++size) {
        cout << "Performing exhaustive test on all words of size " << size << endl;
        testForSize(size);
    }

    cout << endl << "Performing random tests for optimal in-place suffix array construction..." << endl;
    loopRandomTests(10000, 10);
    loopRandomTests(10000, 101);
    loopRandomTests(1000, 10000);
    loopRandomTests(100, 100000);

    cout << endl << "Performing " << NUMBER_OF_TESTS_FOR_EDGE_CASES << " tests on random strings with edge characters of length 100001..." << endl;
    for (int i = 0; i < NUMBER_OF_TESTS_FOR_EDGE_CASES; ++i) {
        testOneRandom(TEST_SIZE_FOR_EDGE_CASES);
    }

    cout << endl << "Performance tests for optimal suffix array construction" << endl;
    executionTimeOptimalSuffixArray(100000, 100, 'A', 'Z');
    executionTimeOptimalSuffixArray(100000, 100, 'A', 'B');

    cout << "All tests completed." << endl;
}

#endif