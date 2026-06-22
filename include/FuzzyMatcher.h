#ifndef FUZZY_MATCHER_H
#define FUZZY_MATCHER_H

#include <vector>
#include <string>
#include <utility>

using namespace std;

// Provides typo-tolerant matching via Levenshtein edit distance.
// Used as a fallback when prefix search returns zero results,
// e.g. user typed "helo" and the dictionary only has "hello".
class FuzzyMatcher {
public:
    // Classic DP edit distance, O(L1 * L2) time and space.
    static int editDistance(const string& a, const string& b);

    // Scans the full candidate word list and returns every word
    // within maxDistance of the input, sorted by closeness
    // (smallest edit distance first).
    static vector<pair<string, int>> findClosest(
        const string& word,
        const vector<string>& dictionaryWords,
        int maxDistance
    );
};

#endif