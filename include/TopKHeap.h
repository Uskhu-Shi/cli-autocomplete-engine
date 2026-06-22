#ifndef TOP_K_HEAP_H
#define TOP_K_HEAP_H

#include <vector>
#include <string>
#include <utility>

using namespace std;

// Given a list of (word, frequency) candidates, returns the top K
// by frequency using a min-heap of size K. This is O(N log K)
// instead of sorting all N candidates, which matters once a prefix
// matches thousands of words in a large document set.
class TopKHeap {
public:
    static vector<pair<string, int>> getTopK(
        const vector<pair<string, int>>& candidates,
        int k
    );
};

#endif