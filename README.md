# Autocomplete Engine (C++)

A Trie-based autocomplete and fuzzy search engine that indexes plain text documents, ranks suggestions by word frequency, and falls back to typo-tolerant matching when no exact prefix exists. Built in C++17 with no external dependencies.

---

## Objective

Most "Trie autocomplete" projects stop at toy dictionaries with no real ingestion, ranking, or persistence layer. This project was built to go further:

- Accept **real, arbitrary `.txt` documents** as input rather than a hardcoded word list
- Rank suggestions by **actual frequency** observed across those documents, not just alphabetically
- **Persist the index to disk** in a custom binary format, so the program doesn't re-parse every document on every run
- Tolerate **typos** via edit-distance fuzzy matching when no exact prefix match exists
- Demonstrate clean **OOP separation of concerns** — each data structure and responsibility lives in its own class, independently testable

The result is a small but complete pipeline: text in → indexed structure → persisted to disk → queried interactively from the command line.

---

## Data Structures & Algorithms Used

| Component | Data Structure / Algorithm | Why |
|---|---|---|
| Word storage & prefix lookup | **Trie** | O(L) insertion and prefix lookup, where L is word length — independent of dictionary size |
| Ranking suggestions | **Min-heap (Top-K selection)** | Retrieves the K highest-frequency matches in O(N log K) instead of sorting all N candidates |
| Typo tolerance | **Levenshtein edit distance (DP)** | Classic O(L1 × L2) dynamic programming table to measure how many single-character edits separate two words |
| Persistence | **Custom binary serialization** | Recursive pre-order tree write/read directly in binary, avoiding the cost of re-tokenizing source text on every run |
| File ingestion | **Directory traversal + tokenization** | Reads arbitrary `.txt` files, normalizes to lowercase, strips punctuation, and feeds clean tokens into the Trie |

---

## How It Works

```
First run:
  .txt files (data/sample_docs/)
       → Indexer reads & tokenizes (plain text)
       → words inserted into Trie (frequency counted per word)
       → Serializer writes Trie to index/trie_index.bin (binary)

Every later run:
  index/trie_index.bin exists?
       → loaded directly into memory (no text re-parsing)
       → ready to serve queries instantly

Query flow:
  user types a prefix
       → Trie prefix walk finds all matching words
       → Top-K heap ranks them by frequency
       → if NO prefix match exists at all:
             → fuzzy matcher scans the full word list
             → returns closest words within edit distance ≤ 2
```

The binary index file is the persistence layer — it's what lets the engine "remember" previously indexed documents without redoing the parsing work. The user only ever interacts with plain `.txt` files; the binary format is internal caching, invisible to them.

---

## Project Structure

```
autocomplete-engine/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── TrieNode.h        # Core node: children map, end-of-word flag, frequency, cached word
│   ├── Trie.h             # Insert, prefix search, existence check
│   ├── TopKHeap.h         # Min-heap based Top-K ranking
│   ├── FuzzyMatcher.h     # Levenshtein edit-distance DP + closest-match search
│   ├── Indexer.h          # Reads .txt files, tokenizes, feeds the Trie
│   └── Serializer.h       # Binary save/load of the Trie
├── src/
│   ├── TrieNode.cpp
│   ├── Trie.cpp
│   ├── TopKHeap.cpp
│   ├── FuzzyMatcher.cpp
│   ├── Indexer.cpp
│   ├── Serializer.cpp
│   └── main.cpp           # CLI driver tying everything together
├── data/
│   └── sample_docs/       # Sample .txt files for testing/demo
└── index/                  # Generated at runtime — holds trie_index.bin
```

---

## Build & Run

### Requirements
- C++17 compiler (g++, clang++, or MSVC)
- CMake 3.15+ (optional — direct g++ compilation also works)

### Using CMake
```bash
mkdir build && cd build
cmake ..
cmake --build .
./autocomplete       # or autocomplete.exe on Windows
```

### Direct compilation (no CMake needed)
```bash
g++ -std=c++17 -Wall -Wextra -Iinclude -o autocomplete src/*.cpp
./autocomplete
```

> **Windows note:** if your `g++` is older (e.g. MinGW GCC 8.x), `<filesystem>` may fail to compile due to a known toolchain bug. This project avoids `<filesystem>` entirely (uses `dirent.h` / `_mkdir`) specifically to stay portable across compiler versions.

---

## Usage

On first run, the engine indexes every `.txt` file in `data/sample_docs/` and saves the result to `index/trie_index.bin`:


```
Autocomplete + Fuzzy Search Engine 
No existing index found. Building from text files in 'data/sample_docs'...
Indexed file: doc1.txt
Indexed file: doc2.txt
Processed 2 file(s).
Index persisted to 'index/trie_index.bin'.
```

Then query interactively:

| Input | Behavior |
|---|---|
| `hel` | Prefix search → top-5 results ranked by frequency |
| `helo` (typo) | No prefix match → fuzzy fallback → "Did you mean: help, hello" |
| `:reindex` | Re-reads all `.txt` files and rebuilds the persisted index |
| `:help` | Shows available commands |
| `:quit` | Exits |

Example session:
<img width="1472" height="755" alt="image" src="https://github.com/user-attachments/assets/d121bba3-c155-47f3-9dd2-2f39abcd75e5" />

```
> hel
Autocomplete suggestions for "hel":
  hello  (freq: 4)
  help  (freq: 4)
  helicopter  (freq: 1)
  helsinki  (freq: 1)
  helping  (freq: 1)

> helo
No prefix matches for "helo". Trying fuzzy match...
Did you mean:
  help  (edit distance: 1)
  hello  (edit distance: 1)
```

On the **next** run, the binary index is loaded directly — no documents are re-read:
```
=== Autocomplete + Fuzzy Search Engine ===
Loaded existing index from 'index/trie_index.bin' (skipped re-parsing text).
```

To index your own documents: drop additional `.txt` files into `data/sample_docs/`, then run `:reindex` from inside the program (or delete `index/trie_index.bin` and restart).

---

## Design Notes

- **Why a Trie over a hash set?** A hash set can confirm a word exists in O(1), but it can't efficiently answer "give me all words starting with this prefix" — that requires scanning every key. A Trie answers prefix queries in O(L) regardless of dictionary size.
- **Why binary serialization instead of JSON/plain text?** Binary is smaller on disk and faster to parse back (no string-to-number parsing, no tokenizing), which matters specifically because the whole point of persistence here is to make the *second* run faster than the first.
- **Why a min-heap for Top-K instead of just sorting?** Sorting all candidates is O(N log N). A bounded min-heap of size K is O(N log K) — meaningfully cheaper once a prefix matches thousands of words in a large corpus.
- **Fuzzy matching is brute-force by design.** It computes full Levenshtein DP against every word in the dictionary, which is O(N × L1 × L2) and doesn't scale to very large vocabularies. A production system would pre-filter candidates (e.g., by length, or using a BK-tree / SymSpell-style deletion index) before running DP on survivors — noted here as a known, deliberate simplification rather than an oversight.

---

## Possible Extensions

- Pre-filter fuzzy-match candidates by length/first-letter before running full DP, to avoid O(N) scans on large vocabularies
- Multi-threaded ingestion for large document sets
- Expose the engine over HTTP (e.g. via a single-header library like cpp-httplib) with a minimal HTML/JS frontend for live demo purposes
- Per-document or per-source frequency weighting instead of a single global count
