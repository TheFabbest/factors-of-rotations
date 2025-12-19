#include <iostream>
#include "utils.hpp"
#include "test.hpp"
using namespace std;

void showTrees(const char* input_word, const unsigned long word_length) {
    char *word = new char[word_length+1];
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

    cout << "Working on word: " << word << endl;
    cout << "Left Lyndon Tree: " << endl;
    leftTree->Print();
    cout << endl;
    separator();
    cout << "Right Lyndon Tree: " << endl;
    rightTree->Print();
    cout << endl;
}

int main() {
    const char word[] = "BABBABAABBAABABABAABBAABAAABABABAABBAAABAAABABABAABBAAABAAABABABAABAAABAAABABABAAABAAABABAAABA";
    const unsigned long word_length = (sizeof(word) / sizeof(char)) - 1;
    
    testRandom();
    executionTimeOptimalSuffixArray(100000, 100);

    cout << endl << "Done." << endl;
    return 0;
}