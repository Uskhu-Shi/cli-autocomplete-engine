#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <string>
#include <utility>
#include "TrieNode.h"

using namespace std;

// Trie supporting word insertion with frequency counting,
// prefix-based retrieval of all matching words, and exact
// existence checks. Insertion and prefix-walk are both O(L)
// where L is word/prefix length.
class Trie {
private:
    TrieNode* root;

    // Recursively collects every complete word stored beneath
    // a given node, paired with its frequency.
    void collectWords(TrieNode* node, vector<pair<string, int>>& results) const;

public:
    Trie();
    ~Trie();

    // Inserts a word into the Trie. If it already exists,
    // increments its frequency instead of duplicating it.
    void insert(const string& word);

    // Returns true if the exact word exists in the Trie.
    bool exists(const string& word) const;

    // Returns all complete words stored under the given prefix,
    // each paired with its frequency count.
    vector<pair<string, int>> getWordsWithPrefix(const string& prefix) const;

    // Returns every word currently stored in the Trie (used by
    // the fuzzy matcher as the candidate pool when prefix search
    // finds nothing).
    vector<string> getAllWords() const;

    // Needed by the Serializer to walk the tree for binary I/O,
    // and to swap in a freshly-loaded tree after deserialization.
    TrieNode* getRoot() const;
    void setRoot(TrieNode* newRoot);

    // Transfers ownership of the root pointer to the caller and
    // replaces it internally with a fresh empty node, WITHOUT
    // deleting the original tree. Used when moving a fully-built
    // tree from a temporary Trie into another one without freeing
    // the data in the process (avoids a double-free/dangling pointer).
    TrieNode* releaseRoot();
};

#endif