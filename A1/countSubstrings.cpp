#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include "tree.h"

using namespace std;

// Implement the helper functions
bool isValidCharacter(char c) {
    return (c >= 'a' && c <= 'z') || c == ' ' || c == '\'' || c == '-' || c == '_';
}

string toLower(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

string trimNewlines(const string& str) {
    size_t pos = str.find_last_not_of("\n\r");
    if (pos != string::npos) {
        return str.substr(0, pos + 1);
    }
    return str;
}

int main() {
    ifstream input_file("input.txt");
    if (!input_file.is_open()) {
        cerr << "Error: Unable to open input.txt" << endl;
        return 1;
    }

    Trie trie;

    string line;
    while (getline(input_file, line)) {
        string trimmedLine = trimNewlines(toLower(line));

        for (int i = 0; i < trimmedLine.length(); ++i) {
            string substring = "";
            for (int j = i; j < trimmedLine.length(); ++j) {
                char currentChar = trimmedLine[j];
                if (isValidCharacter(currentChar)) {
                    substring += currentChar;
                    trie.insert(substring);
                }
            }
        }

        vector<string> vocabulary;
        ifstream inFile("vocabulary.txt");
        if (inFile.is_open()) {
            string vocabWord;
            while (getline(inFile, vocabWord)) {
                string trimmedWord = trimNewlines(toLower(vocabWord));
                vocabulary.push_back(trimmedWord);
            }
            inFile.close();
        } else {
            cerr << "Error: Unable to open vocabulary.txt" << endl;
            return 1;
        }

        int foundCount = 0;
        for (const string& word : vocabulary) {
            if (trie.search(word.c_str())) {
                foundCount++;
            }
        }

        cout << foundCount << endl;

        trie = Trie();
    }

    input_file.close();

    return 0;
}
