#include "FuzzyMatcher.h"
#include <algorithm>

using namespace std;

int FuzzyMatcher::editDistance(const string& a, const string& b) {
    int n = a.size();
    int m = b.size();

    // dp[i][j] = edit distance between a[0..i) and b[0..j)
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

    for (int i = 0; i <= n; i++) dp[i][0] = i; // delete all of a
    for (int j = 0; j <= m; j++) dp[0][j] = j; // insert all of b

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1]; // characters match, no cost
            } else {
                int deleteCost = dp[i - 1][j] + 1;
                int insertCost = dp[i][j - 1] + 1;
                int replaceCost = dp[i - 1][j - 1] + 1;
                dp[i][j] = min({deleteCost, insertCost, replaceCost});
            }
        }
    }

    return dp[n][m];
}

vector<pair<string, int>> FuzzyMatcher::findClosest(
    const string& word,
    const vector<string>& dictionaryWords,
    int maxDistance
) {
    vector<pair<string, int>> matches;

    for (const string& candidate : dictionaryWords) {
        int dist = editDistance(word, candidate);
        if (dist <= maxDistance) {
            matches.push_back({candidate, dist});
        }
    }

    // Closest matches (smallest edit distance) first.
    sort(matches.begin(), matches.end(),
         [](const pair<string, int>& x, const pair<string, int>& y) {
             return x.second < y.second;
         });

    return matches;
}