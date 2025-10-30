#ifndef TREE_HPP
#define TREE_HPP

#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

class Node
{
public:
    Node *left = nullptr;
    Node *right = nullptr;
    char value;

    Node()
    {
        value = 0;
    }

    Node(char n)
    {
        value = n;
    }

    ~Node()
    {
        if (left != nullptr) delete left;
        if (right != nullptr) delete right;
    }

    // see visualizer http://brenocon.com/parseviz/
    void Print()
    {
        if (left == nullptr && right == nullptr)
        {
            cout << '(' << value << ')';
        }
        else
        {
            cout << "(node ";

            if (left != nullptr)
            {
                left->Print();
                cout << " ";
            }
            else
            {
                cout << "null ";
            }

            if (right != nullptr)
            {
                right->Print();
                cout << ")";
            }
            else
            {
                cout << "null)";
            }
        }
    }

    void PrintString()
    {
        if (left == nullptr && right == nullptr)
        {
            cout << value;
        }
        else
        {
            if (left != nullptr)
            {
                left->PrintString();
            }
            if (right != nullptr)
            {
                right->PrintString();
            }
        }
    }

    string ToString(string s)
    {
        if (left == nullptr && right == nullptr)
        {
            return s + value;
        }
        else
        {
            if (left != nullptr)
            {
                s = left->ToString(s);
            }
            if (right != nullptr)
            {
                s = right->ToString(s);
            }
            return s;
        }
    }
};

// quando chiamato su un right lyndon tree restituisce il vettore di nodi rappresentanti
// tutti i fattori di Lyndon che appariranno nelle fattorizzazioni dei suffissi
void RightChildren(const Node* tree, vector<Node*> &output) {
    if (tree == nullptr) return;
    
    RightChildren(tree->left, output);

    if (tree->right != nullptr) {
        output.push_back(tree->right);
    }

    RightChildren(tree->right, output);
}

// quando chiamato su un left lyndon tree restituisce il vettore di nodi rappresentanti
// tutti i fattori di Lyndon che appariranno nelle fattorizzazioni dei prefissi
void LeftChildren(const Node* tree, vector<Node*> &output) {
    if (tree == nullptr) return;

    if (tree->left != nullptr) {
        LeftChildren(tree->left, output);
        output.push_back(tree->left);
    }

    LeftChildren(tree->right, output);
}

// each node returns its own factorization as a string and prints the factors of its left children, preceded by the factors found in the prefixes up to this node
string LeftChildrenWithPrefixNumber(const Node* tree, int my_length_as_a_prefix = 0, string old_factors = "") {
    if (tree == nullptr) return "";
    if (tree->left == nullptr && tree->right == nullptr) {
        return string(1, tree->value);
    }

    string me = "";
    if (tree->left != nullptr) {
        me += LeftChildrenWithPrefixNumber(tree->left, my_length_as_a_prefix, old_factors);
        my_length_as_a_prefix += me.size();
        cout << "Prefix of length " << my_length_as_a_prefix << " has factors: " << old_factors << me << endl;
    }
    me += LeftChildrenWithPrefixNumber(tree->right, my_length_as_a_prefix, old_factors + me + ", ");

    return me;
}


Node* LeftLyndonTree(const char* const word, const unsigned long length, unsigned long* const LynS, Node** roots) {
    LynS[0] = 1;
    unsigned long per = 1;
    unsigned long index_for_comparison = 0;
    roots[0] = new Node(word[0]);
    Node *q = roots[0];
    for (unsigned long j = 1; j < length; ++j){
        roots[j] = new Node(word[j]);
        if (word[j] != word[index_for_comparison]) {
            LynS[j] = j+1;
            per = j+1;
            index_for_comparison=0;
        }
        else {
            LynS[j] = LynS[index_for_comparison];
            index_for_comparison = (index_for_comparison+1) % per;
        }

        unsigned long l = 1;
        unsigned long k = j-1;
        while (l < LynS[j]) {
            q = new Node(word[l-1]);
            q->left = roots[k];
            q->right = roots[j];
            
            roots[j] = q;
            l += LynS[k];
            k -= LynS[k];
        }
    }
    return q;
}


Node* RightLyndonTree(const char* const word, const unsigned long length, const unsigned long * const rank, const unsigned long * const Lyn){
    Node* nodes[length]; // potrei anche inizializzare qui
    unsigned long j = length-1;
    nodes[j] = new Node(word[j]);
    for (unsigned long i = length-1; i > 0; --i) {
        nodes[i-1] = new Node(word[i-1]);

        while (rank[i-1] < rank [j]) {
            Node *father = new Node();
            father->left = nodes[i-1];
            father->right = nodes[j];
            nodes[i-1] = father;
            j+=Lyn[j];
            if (j == length) {
                break;
            }
        }
        j = i-1;
    }
    return nodes[0];
}

#endif