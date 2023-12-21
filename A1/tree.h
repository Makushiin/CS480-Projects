#ifndef TREE_H
#define TREE_H

#include <string>
#include <unordered_map>

// Declare the TrieNode structure
struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
};

// Declare the Trie class
class Trie {
private:
    TrieNode* root;

public:
    Trie();
    ~Trie();
    void insert(const std::string& word);
    bool search(const char* strBeingSearched);
};

// Declare the helper functions
bool isValidCharacter(char c);
std::string toLower(const std::string& str);
std::string trimNewlines(const std::string& str);

#endif // TREE_H
