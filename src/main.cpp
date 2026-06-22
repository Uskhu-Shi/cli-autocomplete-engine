#include <iostream>
#include <string>
#include <vector>
#include "Trie.h"
#include "Indexer.h"
#include "Serializer.h"
#include "TopKHeap.h"
#include "FuzzyMatcher.h"

#if defined(_WIN32)
    #include <direct.h>   // _mkdir
#else
    #include <sys/stat.h> 
    #include <sys/types.h>

#endif

using namespace std;

void ensureDirectoryExists(const string& path) {
#if defined(_WIN32)
    _mkdir(path.c_str());           // returns -1 if it already exists; that's fine
#else
    mkdir(path.c_str(), 0755);     
#endif
}

const string DOCS_DIR = "data/sample_docs";
const string INDEX_FILE = "index/trie_index.bin";
const int TOP_K = 5;
const int MAX_EDIT_DISTANCE = 2;

void buildIndexFromText(Trie& trie) {
    cout << "No existing index found. Building from text files in '"
         << DOCS_DIR << "'..." << endl;

    int filesProcessed = Indexer::indexDirectory(DOCS_DIR, trie);
    cout << "Processed " << filesProcessed << " file(s)." << endl;

    ensureDirectoryExists("index");
    if (Serializer::saveToBinary(trie.getRoot(), INDEX_FILE)) {
        cout << "Index persisted to '" << INDEX_FILE << "'." << endl;
    }
}

bool loadIndexFromBinary(Trie& trie) {
    TrieNode* root = Serializer::loadFromBinary(INDEX_FILE);
    if (root == nullptr) return false;

    trie.setRoot(root);
    cout << "Loaded existing index from '" << INDEX_FILE << "' (skipped re-parsing text)." << endl;
    return true;
}

void reindex(Trie& trie) {
    Trie fresh;
    buildIndexFromText(fresh);

    TrieNode* newRoot = fresh.releaseRoot();
    trie.setRoot(newRoot);
}

void printSuggestions(const vector<pair<string, int>>& results) {
    if (results.empty()) {
        cout << "  (no suggestions)" << endl;
        return;
    }
    for (auto& r : results) {
        cout << "  " << r.first << "  (freq: " << r.second << ")" << endl;
    }
}

void handleQuery(Trie& trie, const string& query) {
    vector<pair<string, int>> prefixMatches = trie.getWordsWithPrefix(query);

    if (!prefixMatches.empty()) {
        vector<pair<string, int>> top = TopKHeap::getTopK(prefixMatches, TOP_K);
        cout << "Autocomplete suggestions for \"" << query << "\":" << endl;
        printSuggestions(top);
        return;
    }

    cout << "No prefix matches for \"" << query << "\". Trying fuzzy match..." << endl;
    vector<string> allWords = trie.getAllWords();
    vector<pair<string, int>> fuzzyMatches = FuzzyMatcher::findClosest(query, allWords, MAX_EDIT_DISTANCE);

    if (fuzzyMatches.empty()) {
        cout << "  (no close matches found within edit distance " << MAX_EDIT_DISTANCE << ")" << endl;
        return;
    }

    cout << "Did you mean:" << endl;
    int shown = 0;
    for (auto& m : fuzzyMatches) {
        cout << "  " << m.first << "  (edit distance: " << m.second << ")" << endl;
        shown++;
        if (shown >= TOP_K) break;
    }
}

void printHelp() {
    cout << endl;
    cout << "Commands:" << endl;
    cout << "  <text>     - get autocomplete/fuzzy suggestions for <text>" << endl;
    cout << "  :reindex   - re-read all .txt files in data/sample_docs and rebuild the index" << endl;
    cout << "  :help      - show this message" << endl;
    cout << "  :quit      - exit the program" << endl;
    cout << endl;
}

int main() {
    cout << "Autocomplete + Fuzzy Search Engine" << endl;

    Trie trie;

    bool loaded = loadIndexFromBinary(trie);
    if (!loaded) {
        buildIndexFromText(trie);
    }

    printHelp();

    string input;
    while (true) {
        cout << "> ";
        if (!getline(cin, input)) break;

        if (input.empty()) continue;

        if (input == ":quit") {
            break;
        } else if (input == ":help") {
            printHelp();
        } else if (input == ":reindex") {
            reindex(trie);
        } else {
            vector<string> tokens = Indexer::tokenize(input);
            if (tokens.empty()) {
                cout << "  (please enter alphabetic text)" << endl;
                continue;
            }
            handleQuery(trie, tokens[0]);
        }
    }

    cout << "Goodbye." << endl;
    return 0;
}