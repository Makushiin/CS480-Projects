// Name: Aya Abdul Hasan
// REDID: 824080459

//Name : Maxine Mayor
//RedID : 825551461

#ifndef TREE_H
#define TREE_H

#define CONSTANTS 128

using namespace std;

const int N = CONSTANTS;

// Tree node 
struct Tree {
    Tree *child[N];
    bool EndofNode;
};

// initialize functions
Tree *getNode(void);
void insert(Tree *root, const char* word);
void insertHelper(Tree* imHere, const char* word, int START = 0);
bool search(Tree* root, const char* word);
void deleteTree(Tree* root);



#endif
