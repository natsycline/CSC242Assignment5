/* ---------------------------------------------------------------------------
** I attest that this following code represents my own work and is subject to
** the plagiarism policy found in the course syllabus.
**
** Class:     CSC 242
** Assignment:P8.2
** File:      CunninghamNathanialAssignment5.cpp
** Description: Spell checker that loads a UNIX-style dictionary file into a
**              vector<string>, then scans a target text file and prints any
**              words not found in the dictionary (case/punctuation normalized).
**
** Author:    Cunningham, Nathanial
** Date:      08/28/2025
** -------------------------------------------------------------------------*/

#include <iostream>   // Ch. 3: Input/Output streams
#include <fstream>    // Ch. 8: File streams
#include <string>     // Ch. 7: Strings
#include <vector>     // Ch. 6: Vectors
#include <algorithm>  // Ch. 7: Algorithms (sort, binary_search)
#include <cctype>     // Ch. 5: Character testing (isalpha, tolower)

using namespace std;

/* ---------------------------------------------------------------------------
   TEAM BREAKDOWN (each member owns at least one function/segment)

   - (Relly Bodden)  normalize_word
       Uses string and loops (Ch. 5–7).
   - (Tyler Drake)   load_dictionary
       Uses file streams and vectors (Ch. 6, 8).
   - (Nathanial Cunningham)  contains_word, check_spelling_in_file, main
       Uses binary_search, vector traversal, file I/O (Ch. 3, 6, 7, 8).
   -------------------------------------------------------------------------*/

   // (Relly Bodden) ------------------------------------------------------------
   // normalize_word: strips punctuation, lowercases letters
   // Concepts: strings (Ch. 7), loops & conditionals (Ch. 5)
string normalize_word(const string& raw)
{
    string provisional;

    // Loop through each character (Ch. 5: for loop)
    for (size_t i = 0; i < raw.size(); ++i)
    {
        unsigned char ch = static_cast<unsigned char>(raw[i]);

        if (isalpha(ch)) // Ch. 5: character testing
        {
            provisional.push_back(static_cast<char>(tolower(ch))); // Ch. 7: string building
        }
        else if (ch == '\'' || ch == '-')
        {
            bool prev_is_alpha = (i > 0) && isalpha(static_cast<unsigned char>(raw[i - 1]));
            bool next_is_alpha = (i + 1 < raw.size()) && isalpha(static_cast<unsigned char>(raw[i + 1]));
            if (prev_is_alpha && next_is_alpha)
            {
                provisional.push_back(ch);
            }
        }
    }

    // Trim leading/trailing punctuation (Ch. 7: substr)
    size_t start = 0;
    while (start < provisional.size() && (provisional[start] == '\'' || provisional[start] == '-'))
    {
        ++start;
    }
    if (start >= provisional.size()) return "";

    size_t end = provisional.size();
    while (end > start && (provisional[end - 1] == '\'' || provisional[end - 1] == '-'))
    {
        --end;
    }

    return provisional.substr(start, end - start);
}

// (Tyler Drake) -------------------------------------------------------------
// load_dictionary: reads file, normalizes words, stores in vector
// Concepts: file streams (Ch. 8), vectors (Ch. 6), algorithms (Ch. 7)
bool load_dictionary(const string& dict_path, vector<string>& words)
{
    ifstream dict_in(dict_path.c_str()); // Ch. 8: opening a file stream
    if (!dict_in)
    {
        cerr << "Error: Could not open dictionary file: " << dict_path << endl;
        return false;
    }

    words.clear(); // Ch. 6: clear vector

    string w;
    while (dict_in >> w) // Ch. 8: reading tokens from a file
    {
        for (size_t i = 0; i < w.size(); ++i)
        {
            unsigned char ch = static_cast<unsigned char>(w[i]);
            if (isalpha(ch)) w[i] = static_cast<char>(tolower(ch));
        }

        string norm = normalize_word(w);
        if (!norm.empty())
        {
            words.push_back(norm); // Ch. 6: append to vector
        }
    }

    sort(words.begin(), words.end()); // Ch. 7: STL algorithm
    words.erase(unique(words.begin(), words.end()), words.end()); // Ch. 7: remove duplicates
    return true;
}

// (Nathanial Cunningham) ----------------------------------------------------
// contains_word: binary search in sorted vector
// Concepts: STL algorithm (Ch. 7), vector (Ch. 6)
bool contains_word(const vector<string>& words, const string& target)
{
    return binary_search(words.begin(), words.end(), target);
}

// (Nathanial Cunningham) ----------------------------------------------------
// check_spelling_in_file: opens file, normalizes tokens, prints unknowns
// Concepts: file streams (Ch. 8), vectors (Ch. 6), string processing (Ch. 7)
void check_spelling_in_file(const string& filename, const vector<string>& dict_words)
{
    ifstream fin(filename.c_str()); // Ch. 8: file input
    if (!fin)
    {
        cerr << "Error: Could not open file to check: " << filename << endl;
        return;
    }

    vector<string> already_reported; // Ch. 6: vector to track duplicates

    string token;
    while (fin >> token) // Ch. 8: reading words
    {
        string norm = normalize_word(token); // Ch. 7: string processing
        if (norm.empty()) continue;

        if (!contains_word(dict_words, norm))
        {
            if (!already_reported.empty())
            {
                if (binary_search(already_reported.begin(), already_reported.end(), norm))
                {
                    continue;
                }
            }
            already_reported.insert(lower_bound(already_reported.begin(), already_reported.end(), norm), norm); // Ch. 7: STL insert with binary search
            cout << norm << endl; // Ch. 3: output
        }
    }
}

// (Nathanial Cunningham) ----------------------------------------------------
// main: orchestrates program flow
// Concepts: I/O (Ch. 3), file streams (Ch. 8), vectors (Ch. 6)
int main()
{
    cout << "Spell Checker (P8.2) — Chapter 8 Streams\n";

    string dict_path = "/usr/share/dict/words"; // Default dictionary
    string file_to_check;

    cout << "Enter path to dictionary [/usr/share/dict/words]: ";
    {
        string input;
        getline(cin, input); // Ch. 3: string input
        if (!input.empty())
        {
            dict_path = input;
        }
    }

    cout << "Enter path to the text file to check: ";
    getline(cin, file_to_check);

    vector<string> dictionary_words;
    if (!load_dictionary(dict_path, dictionary_words)) // Ch. 8: file load
    {
        cerr << "Aborting: failed to load dictionary.\n";
        return 1;
    }

    check_spelling_in_file(file_to_check, dictionary_words); // Driver call
    return 0;
}
