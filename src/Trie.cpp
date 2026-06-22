#include "Trie.h"

using namespace std;

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    delete root; // TrieNode destructor recursively frees children
}

void Trie::insert(const string& word) {
    if (word.empty()) return;

    TrieNode* current = root;
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = new TrieNode();
        }
        current = current->children[c];
    }

    if (current->isEndOfWord) {
        current->frequency += 1; // word seen again -> bump frequency
    } else {
        current->isEndOfWord = true;
        current->frequency = 1;
        current->word = word;
    }
}

bool Trie::exists(const string& word) const {
    if (word.empty()) return false;

    TrieNode* current = root;
    for (char c : word) {
        auto it = current->children.find(c);
        if (it == current->children.end()) return false;
        current = it->second;
    }
    return current->isEndOfWord;
}

void Trie::collectWords(TrieNode* node, vector<pair<string, int>>& results) const {
    if (node == nullptr) return;

    if (node->isEndOfWord) {
        results.push_back({node->word, node->frequency});
    }

    for (auto& pair : node->children) {
        collectWords(pair.second, results);
    }
}

vector<pair<string, int>> Trie::getWordsWithPrefix(const string& prefix) const {
    vector<pair<string, int>> results;

    TrieNode* current = root;
    for (char c : prefix) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return results; // prefix not found at all -> empty result
        }
        current = it->second;
    }

    collectWords(current, results);
    return results;
}

vector<string> Trie::getAllWords() const {
    vector<pair<string, int>> withFreq;
    collectWords(root, withFreq);

    vector<string> words;
    words.reserve(withFreq.size());
    for (auto& p : withFreq) {
        words.push_back(p.first);
    }
    return words;
}

TrieNode* Trie::getRoot() const {
    return root;
}

void Trie::setRoot(TrieNode* newRoot) {
    delete root; // free the old empty/previous tree first
    root = newRoot;
}

TrieNode* Trie::releaseRoot() {
    TrieNode* old = root;
    root = new TrieNode(); // leave this Trie in a valid, empty, usable state
    return old;
}