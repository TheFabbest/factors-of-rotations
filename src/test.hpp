#ifndef TEST_HPP
#define TEST_HPP

#include <functional>
#include <math.h>
#include "SAIS.hpp"
#include "utils.hpp"
#include "naiveSuffix.hpp"
#include "optimalSuffixArray.hpp"

void testForEachWordOfLength(const unsigned long length, const function<void (char[], unsigned long)> func) {
    char word[length+1];
    for (unsigned long i = 0; i < length; ++i) {
        word[i] = 'A';
    }
    word[length] = '\0';
    
    int limit = pow(length, length);
    int print_step = pow(length, length-1);
    print_step=1;
    for (int i = 0; i < limit; ++i)
    {
        if (i % print_step == 0) cout << word << endl;
        func(word, length);
        int j = length-1;
        while(true) {
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
}

void testOneBig(){
    const unsigned long SIZE = 1000;
    const unsigned long MIN_ALPHABET = 'A';
    const unsigned long MAX_ALPHABET = 'Z';
    const unsigned long ALPHABET_SIZE = MAX_ALPHABET - MIN_ALPHABET + 1;
    
    char *word = new char [SIZE+1];

    srand(time(0));
    for (unsigned long i = 0; i < SIZE; ++i){
        word[i] = MIN_ALPHABET + rand() % ALPHABET_SIZE;
    }
    word[SIZE] = '\0';

    unsigned long *SA = SAIS(word, SIZE, ALPHABET_SIZE)+1;
    unsigned long *SA_naive = buildSuffixArray(word, SIZE);

    // look for differences
    for (unsigned long i = 0; i < SIZE; ++i) {
        if (SA[i] != SA_naive[i]) {
            cout << "ERROR" << endl;
            int n;
            cin >> n;
        }
    }
    

    delete[] (SA-1);
    delete[] SA_naive;
    delete[] word;
}



void testFactorsLyn(const char input_word[], const unsigned long word_length) {
    char word[word_length+1];
    unsigned long least = least_rotation(input_word, word_length);
    rotate_copy(input_word, input_word+least, input_word+word_length, word);
    word[word_length] = '\0';
    
    if (duval(string(word)).size()!=1){
        for (string s : duval(string(word))) {
            cout << s << ", ";
        }
        cout << "skipping" << endl;
        return;
    }

    unsigned long *SA = SAIS(word, word_length, getAlphabetSize(input_word, word_length))+1;
    unsigned long *rank = rankArrayFromSA(SA, word_length);

    unsigned long Lyn[word_length];
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
        int n;
        cin >> n;
    }

    delete[] (SA-1);
    delete[] rank;
}

void testFactorsLynS(const char input_word[], const unsigned long word_length) {
    char word[word_length+1];
    unsigned long least = least_rotation(input_word, word_length);
    rotate_copy(input_word, input_word+least, input_word+word_length, word);
    word[word_length] = '\0';
    
    if (duval(string(word)).size()!=1){
        for (string s : duval(string(word))) {
            cout << s << ", ";
        }
        cout << "skipping" << endl;
        return;
    }

    unsigned long LynS[word_length];
    LyndonSuffixTable(word, word_length, LynS);

    Node* roots[word_length];
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
        int n;
        cin >> n;
    }

    delete leftTree;
}

// i+Lyn[i] is basically the smallest index j>i for which rank[i] > rank[j]
void propertyTest1(const char word[], unsigned long word_length) {
    unsigned long *SA = SAIS(word, word_length, getAlphabetSize(word, word_length))+1;
    unsigned long *rank = rankArrayFromSA(SA, word_length);
    unsigned long Lyn[word_length];
    LongestLyndon(word, word_length, rank, Lyn);
    for (int i = 0; i < word_length; ++i) {
        for (int j = i+1; j < i+Lyn[i]; ++j) {
            if (rank[i] > rank[j]) {
                cout << "ERROR: " << word << endl;
                cout << i << " " << j << endl;
                cout << rank[i] << " > " << rank[j] << endl;
                int n;
                cin >> n;
            }
        }
        if (i+Lyn[i] < word_length && rank[i] < rank[i+Lyn[i]]) {
            cout << "ERROR: " << word << endl;
            cout << i << " " << i+Lyn[i] << endl;
            cout << rank[i] << " < " << rank[i+Lyn[i]] << endl;
            int n;
            cin >> n;
        }
    }

    delete[] (SA-1);
    delete[] rank;
}

// for each factor beginning at index i of each rotation of the word it is true that rank[i] < rank[j] for each j in the factor
void propertyTest2(const char input_word[], unsigned long word_length) {
    if (duval(string(input_word)).size()!=1){
        cout << "skipping" << endl;
        return;
    }
    char word[word_length+1];
    word[word_length] = '\0';
    unsigned long *SA = SAIS(input_word, word_length, getAlphabetSize(word,word_length))+1;
    unsigned long *rank = rankArrayFromSA(SA, word_length);
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
                    int n;
                    cin >> n;
                }
            }
            i+= factor.length();
        }
    }
    delete[] (SA-1);
    delete[] rank;
}

void testSAIS(const char input_word[], unsigned long word_length) {

    // get alphabet size
    const unsigned long alphabet_size = getAlphabetSize(input_word, word_length);

    // calculate suffix array naively
    unsigned long *SA_naive = buildSuffixArray(input_word, word_length);

    // calculate SAIS
    unsigned long *SA = SAIS(input_word, word_length, alphabet_size)+1;

    // look for differences
    for (unsigned long i = 0; i < word_length; ++i) {
        if (SA[i] != SA_naive[i]) {
            cout << "-------------------------" << endl;
            cout << "ERROR: " << input_word << endl;
            cout << "SA[" << i << "] = " << SA[i] << ", SA_naive[" << i << "] = " << SA_naive[i] << endl;
            for (unsigned long j = 0; j < word_length; ++j) {
                cout << SA[j] << " ";
            }
            cout << endl;
            for (unsigned long j = 0; j < word_length; ++j) {
                cout << SA_naive[j] << " ";
            }
            cout << endl;
            int n;
            cin >> n;
        }
    }

    delete[] (SA-1);
    delete[] SA_naive;
}

void testOptimalSuffixArray(const char input_word[], const unsigned long word_length) {

    // get alphabet size
    const unsigned long alphabet_size = getAlphabetSize(input_word, word_length);

    // calculate suffix array naively
    unsigned long *SA_optimal = new unsigned long[word_length+1];
    optimalSuffixArray(input_word, SA_optimal, word_length);
    SA_optimal += 1;
    cout << "Optimal Suffix Array for " << input_word << " is done." << endl;
    
    // calculate naively
    unsigned long *SA = buildSuffixArray(input_word, word_length);

    // look for differences
    for (unsigned long i = 0; i < word_length; ++i) {
        if (SA[i] != SA_optimal[i]) {
            cout << "-------------------------" << endl;
            cout << "ERROR: " << input_word << endl;
            cout << "SA[" << i << "] = " << SA[i] << ", SA_optimal[" << i << "] = " << SA_optimal[i] << endl;
            for (unsigned long j = 0; j < word_length; ++j) {
                cout << SA[j] << " ";
            }
            cout << endl;
            for (unsigned long j = 0; j < word_length; ++j) {
                cout << SA_optimal[j] << " ";
            }
            cout << endl;
            int n;
            cin >> n;
        }
    }
    cout << "Optimal Suffix Array for " << input_word << " is correct." << endl;

    delete[] SA;
    delete[] (SA_optimal-1);
}

void testForSize(const unsigned long test_size) {
    cout << "testFactorsLyn" << endl;
    testForEachWordOfLength(test_size, testFactorsLyn);
    cout << "testFactorsLynS" << endl;
    testForEachWordOfLength(test_size, testFactorsLynS);
    cout << "propertyTest1" << endl;
    testForEachWordOfLength(test_size, propertyTest1);
    cout << "propertyTest2" << endl;
    testForEachWordOfLength(test_size, propertyTest2);
    cout << "testSAIS" << endl;
    testForEachWordOfLength(test_size, testSAIS);
    cout << "testOptimalSuffixArray" << endl;
    testForEachWordOfLength(test_size, testOptimalSuffixArray);
}

void testAll(){
    testForSize(5);
}

#endif