#include "Serializer.h"
#include <iostream>

using namespace std;

// Binary file layout:
// [int32 version]
// then recursively, per node:
//   [char marker]        '1' if this node is end-of-word, '0' otherwise
//   [int32 frequency]    only meaningful if marker == '1'
//   [int32 wordLength][char bytes...]   only present if marker == '1'
//   [int32 childCount]
//   for each child:
//     [char key]
//     <recursive node data>

void Serializer::writeNode(ofstream& out, TrieNode* node) {
    char marker = node->isEndOfWord ? '1' : '0';
    out.write(&marker, sizeof(marker));

    if (node->isEndOfWord) {
        out.write(reinterpret_cast<const char*>(&node->frequency), sizeof(node->frequency));

        int wordLength = static_cast<int>(node->word.size());
        out.write(reinterpret_cast<const char*>(&wordLength), sizeof(wordLength));
        out.write(node->word.data(), wordLength);
    }

    int childCount = static_cast<int>(node->children.size());
    out.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));

    for (auto& pair : node->children) {
        char key = pair.first;
        out.write(&key, sizeof(key));
        writeNode(out, pair.second);
    }
}

TrieNode* Serializer::readNode(ifstream& in) {
    TrieNode* node = new TrieNode();

    char marker;
    in.read(&marker, sizeof(marker));
    node->isEndOfWord = (marker == '1');

    if (node->isEndOfWord) {
        in.read(reinterpret_cast<char*>(&node->frequency), sizeof(node->frequency));

        int wordLength = 0;
        in.read(reinterpret_cast<char*>(&wordLength), sizeof(wordLength));

        string word(wordLength, ' ');
        in.read(&word[0], wordLength);
        node->word = word;
    }

    int childCount = 0;
    in.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));

    for (int i = 0; i < childCount; i++) {
        char key;
        in.read(&key, sizeof(key));
        TrieNode* child = readNode(in);
        node->children[key] = child;
    }

    return node;
}

bool Serializer::saveToBinary(TrieNode* root, const string& path) {
    ofstream out(path, ios::binary | ios::trunc);
    if (!out.is_open()) {
        cerr << "Serializer: could not open file for writing -> " << path << endl;
        return false;
    }

    int version = FORMAT_VERSION;
    out.write(reinterpret_cast<const char*>(&version), sizeof(version));

    writeNode(out, root);

    out.close();
    return true;
}

TrieNode* Serializer::loadFromBinary(const string& path) {
    ifstream in(path, ios::binary);
    if (!in.is_open()) {
        return nullptr; // no existing index yet -> caller should index from text
    }

    int version = 0;
    in.read(reinterpret_cast<char*>(&version), sizeof(version));

    if (version != FORMAT_VERSION) {
        cerr << "Serializer: index file version mismatch (found "
             << version << ", expected " << FORMAT_VERSION << ")" << endl;
        in.close();
        return nullptr;
    }

    TrieNode* root = readNode(in);
    in.close();
    return root;
}