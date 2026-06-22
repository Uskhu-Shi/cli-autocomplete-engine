#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include "Trie.h"

using namespace std;

// Responsible for reading plain .txt documents from a directory,
// tokenizing them into clean lowercase words, and feeding those
// words into a Trie. This is the "real file handling" layer:
// the user only ever provides plain text, never anything binary.
class Indexer {
public:
    // Reads every .txt file in dirPath, tokenizes the contents,
    // and inserts each token into the given Trie. Returns the
    // number of files successfully processed.
    static int indexDirectory(const string& dirPath, Trie& trie);

    // Tokenizes a single line of text into lowercase, punctuation-
    // stripped words. Exposed publicly so it's unit-testable on
    // its own without needing real files on disk.
    static vector<string> tokenize(const string& line);
};

#endif