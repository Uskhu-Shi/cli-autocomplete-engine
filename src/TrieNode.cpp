#include "TrieNode.h"

using namespace std;

TrieNode::TrieNode() : isEndOfWord(false), frequency(0), word("") {}

// Recursively delete all children to avoid memory leaks.
TrieNode::~TrieNode() {
    for (auto& pair : children) {
        delete pair.second;
    }
    children.clear();
}