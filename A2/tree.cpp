// Name: Aya Abdul Hasan
// REDID: 824080459

//Name : Maxine Mayor
//RedID : 825551461

#include "tree.h"
#include <iostream>

using namespace std;

//Delete tree after every line for memory
void deleteTree(Tree* root) {
    if (root == nullptr) {
        return; }
    for (int i = 0; i < N; i++) {
        deleteTree(root->child[i]); }
    delete root;
}

// create new node, set all child nodes to Null. 
Tree *getNode(void) {
    Tree *node = new Tree;
    node->EndofNode = false;
    for (int i = 0; i < N; i++) {
        node->child[i] = NULL; }
    node->child['\0'] = NULL;
    return node;
}

// insert word into tree by calling the insert Helper for each substring
void insert(Tree* root, const char* word) {
    for (int i = 0; word[i] != '\0'; i++) {
        insertHelper(root, word, i);
    }
}

// helper function for insert that will create the tree
void insertHelper(Tree* imHere, const char* word, int START) {

    // when the word is not completed
    for (int i = START; word[i] != '\0'; i++) {
        char charNow = word[i];
        
        // check that only the valid characters are being passed through.
        if ( (charNow >= 'a' && charNow <= 'z')|| 
             (charNow >= 'A' && charNow <= 'Z') ||
              charNow == '\''|| 
              charNow == '-' ||
              charNow == '_' || 
              charNow == ' ' ) {

            // indexes of each character
            int head = (charNow >= 'a' && charNow <= 'z') ? charNow - 'a' :
                        (charNow >= 'A' && charNow <= 'Z') ? charNow - 'A' :
                        (charNow == ' ') ? 27 :
                        (charNow == '\'') ? 28 :
                        (charNow == '-') ? 29 :
                        (charNow == '_') ? 30 : -1;
        

            // if child node does not exist, create one
            if (!imHere->child[head]) {
                imHere->child[head] = new Tree();
            }

            // move to next child node
            imHere = imHere->child[head];
        }

        // once word is inserted, set to true
        imHere->EndofNode = true;
    }
}

bool search(Tree *root, const char* word) {

    //set search start to root
    Tree *imHere = root;

    for (int i = 0; word[i] != '\0'; i++) {
        char charNow = word[i];

        // check that only the valid characters are being passed through.
        if ( (charNow >= 'a' && charNow <= 'z')|| 
             (charNow >= 'A' && charNow <= 'Z') ||
              charNow == '\''|| 
              charNow == '-' ||
              charNow == '_' || 
              charNow == ' ' ) {

            // indexes of each character
            int head = (charNow >= 'a' && charNow <= 'z') ? charNow - 'a' :
                        (charNow >= 'A' && charNow <= 'Z') ? charNow - 'A' :
                        (charNow == ' ') ? 27 :
                        (charNow == '\'') ? 28 :
                        (charNow == '-') ? 29 :
                        (charNow == '_') ? 30 : -1;

            // if child does not exist, exit with false.
            if (!imHere->child[head]) {
                return false; }

            // move to next child
            imHere = imHere->child[head];
        } 

        else {
            //skip to next word in vocab
            return false;
        }
    }

    // check if last node represents the end of the word, return true.
    return (imHere != NULL && imHere->EndofNode);
}