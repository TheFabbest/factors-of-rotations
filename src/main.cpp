/*
    it is a simple observation that the Lyndon factorization
    of a suffix of w is exactly the sequence of maximal right nodes of the
    right Lyndon tree that are contained in the suffix, and the Lyndon factorization
    of a prefix of w is exactly the sequence of maximal left nodes of the left Lyndon
    tree that are contained in the prefix. See Fig 2 for an example.
    Both trees can be computed in linear time [4,2]. It is not difficult to see that
    the changes in the sequences, and thus the sizes of the Lyndon factorizations
    can be computed in total linear time for each of the suffixes and prefixes, by a
    left-to-right traversal on the trees.
*/

#include <iostream>
#include "utils.hpp"
#include "test.hpp"
using namespace std;

int main() {
    // some strings give wrong results on both SAIS and "naiveSuffix" but not on optimalSuffix (suffix at 97 is smaller than at 22):
    //testOptimalSuffixArray("BPILRZDACCJUKCMIRSPDPZUFOJNJMTIOLSZCREEUINQVREDJWUOOVKVJWKUIGEYRWZVQEBMMRCJIGOTEIIUFSPQOAKZGQXZPXUFBX", 101);
    // CHHRKOOFWWSZFOEOHDXAPGXCOTEVWQEAXNUJEIOAHIZMXDDGIAGZIDBXWIUVYZXXORJSCZTJIUVHAYNKBTKLYNKXVFUVGRVULENND is wrong
    testOptimalSuffixArray("CHHRKOOFWWSZFOEOHDXAPGXCOTEVWQEAXNUJEIOAHIZMXDDGIAGZIDBXWIUVYZXXORJSCZTJIUVHAYNKBTKLYNKXVFUVGRVULENND", 101);
    //CHHRKOOFWWSZFOEOHDXAPGXCOTEVWQEAXNUJEIOAHIZMXDDGIAGZIDBXWIUVYZXXORJSCZTJIUVHAYNKBTKLYNKXVFUVGRVULENND
    //01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
    //IUVHAYNKBTKLYNKXVFUVGRVULENND
    //IUVYZXXORJSCZTJIUVHAYNKBTKLYNKXVFUVGRVULENND
    return 0;
    routineTestForWorkingWithOptimalSuffix();
    cout << "DONE" << endl;
    
    // const char word[] = "AABABBBAABABBC";
    // const unsigned long word_length = (sizeof(word) / sizeof(char)) - 1;
    // PrintAllFactors(word, word_length);
    // cout << endl;
    // PrintAllFactorsNaive(word, word_length);
}