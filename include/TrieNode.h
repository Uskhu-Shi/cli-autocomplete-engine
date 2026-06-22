#ifndef TRIE_NODE_H
#define TRIE_NODE_H

#include <unordered_map>
#include <string>

using namespace std;

// Represents a single node in the Trie.
// Each node holds links to children (keyed by character),
// a flag marking end-of-word, the frequency count for that word
// (only meaningful when isEndOfWord == true), and the full word
// cached at the terminal node so we don't have to reconstruct it
// by walking back up the tree.
class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    int frequency;
    string word;

    TrieNode();
    ~TrieNode();
};

#endif