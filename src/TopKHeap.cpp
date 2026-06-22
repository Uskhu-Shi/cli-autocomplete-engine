#include "TopKHeap.h"
#include <queue>
#include <algorithm>

using namespace std;

vector<pair<string, int>> TopKHeap::getTopK(
    const vector<pair<string, int>>& candidates,
    int k
) {
    // Min-heap keyed on frequency: smallest frequency sits at the top,
    // so when the heap exceeds size k we pop the weakest candidate.
    auto cmp = [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second; // reverse order -> min-heap
    };
    priority_queue<pair<string, int>, vector<pair<string, int>>, decltype(cmp)> minHeap(cmp);

    for (const auto& candidate : candidates) {
        minHeap.push(candidate);
        if ((int)minHeap.size() > k) {
            minHeap.pop();
        }
    }

    vector<pair<string, int>> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top());
        minHeap.pop();
    }

    // Heap pops smallest-first, so reverse to present highest frequency first.
    reverse(result.begin(), result.end());
    return result;
}