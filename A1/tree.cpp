#include "tree.h"

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
}

void Trie::insert(const std::string& word) {
    TrieNode* current = root;
    for (char c : word) {
        if (!isValidCharacter(c)) {
            continue; // Skip invalid characters
        }
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = new TrieNode();
        }
        current = current->children[c];
    }
    current->isEndOfWord = true;
}

bool Trie::search(const char* strBeingSearched) {
    TrieNode* current = root;
    while (*strBeingSearched) {
        char c = *strBeingSearched;
        if (!isValidCharacter(c)) {
            return false; // Invalid character found in search string
        }
        if (current->children.find(c) == current->children.end()) {
            return false; // Substring not found
        }
        current = current->children[c];
        ++strBeingSearched;
    }
    return current->isEndOfWord; // Check if it's the end of a valid word
}
