#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string>
#include <fstream>
#include "TrieNode.h"

using namespace std;

// Handles converting the in-memory Trie into a binary file on disk
// (so it persists across runs) and reconstructing it back into
// memory later. This is what avoids re-reading and re-tokenizing
// every .txt document on every program start: once indexed, the
// Trie is cached as a binary blob and just reloaded directly.
class Serializer {
private:
    static const int FORMAT_VERSION = 1;

    // Recursively writes a single node and all its children.
    static void writeNode(ofstream& out, TrieNode* node);

    // Recursively reads a single node and all its children.
    static TrieNode* readNode(ifstream& in);

public:
    // Saves the Trie rooted at `root` to a binary file at `path`.
    // Returns true on success.
    static bool saveToBinary(TrieNode* root, const string& path);

    // Loads a Trie from a binary file at `path`.
    // Returns nullptr if the file doesn't exist or is invalid.
    static TrieNode* loadFromBinary(const string& path);
};

#endif