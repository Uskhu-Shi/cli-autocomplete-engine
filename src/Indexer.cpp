#include "Indexer.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

vector<string> Indexer::tokenize(const string& line) {
    vector<string> tokens;
    string current;

    for (char c : line) {
        if (isalpha(static_cast<unsigned char>(c))) {
            current += static_cast<char>(tolower(static_cast<unsigned char>(c)));
        } else {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

static bool hasTxtExtension(const string& filename) {
    const string ext = ".txt";
    if (filename.size() < ext.size()) return false;
    return filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0;
}

static bool isRegularFile(const string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) return false;
    return (info.st_mode & S_IFREG) != 0;
}

int Indexer::indexDirectory(const string& dirPath, Trie& trie) {
    int filesProcessed = 0;

    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        cerr << "Indexer: directory not found -> " << dirPath << endl;
        return filesProcessed;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;

        if (name == "." || name == "..") continue;
        if (!hasTxtExtension(name)) continue;

        string fullPath = dirPath + "/" + name;
        if (!isRegularFile(fullPath)) continue;

        ifstream file(fullPath);
        if (!file.is_open()) {
            cerr << "Indexer: could not open -> " << fullPath << endl;
            continue;
        }

        string line;
        while (getline(file, line)) {
            vector<string> words = tokenize(line);
            for (const string& w : words) {
                trie.insert(w);
            }
        }

        file.close();
        filesProcessed++;
        cout << "Indexed file: " << name << endl;
    }

    closedir(dir);
    return filesProcessed;
}