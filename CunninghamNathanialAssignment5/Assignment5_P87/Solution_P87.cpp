/* ---------------------------------------------------------------------------
** I attest that this following code represents my own work and is subject to
** the plagiarism policy found in the course syllabus.
**
** Class: 	CSC 242
** Assignment: 	P8.7
** File: 	CunninghamNathanialAssignment5.cpp
** Description: Encrypts or decrypts a file using a random monoalphabetic cipher
**              generated from a keyword per P8.7 (Horstmann, Big C++: Late Objects).
**
** Author: 	Cunningham, Nathanial
** Date: 	08/28/2025
** -------------------------------------------------------------------------*/

#include <iostream>      // (Ch.1/2) Basic I/O streams
#include <fstream>       // (Ch.8) File stream classes ifstream/ofstream
#include <string>        // (Ch.2/3) std::string for text handling
#include <vector>        // (Ch.7) std::vector for dynamic arrays
#include <cctype>        // (Ch.4) character classification and conversion

using namespace std;

// --------------------------- Function Declarations ---------------------------
// (Team Member: Nathanial Cunningham)
// (Ch.5) Function prototypes to organize program structure.
string dedupeKeyword(const string& raw);                         // Remove duplicates, keep order
string buildKeyAlphabet(const string& keywordDedupe);            // Append remaining letters (Z..A) not in keyword
vector<char> buildMappingEncrypt(const string& keyAlphabet);     // Map 'A'..'Z' -> keyAlphabet
vector<char> buildMappingDecrypt(const string& keyAlphabet);     // Map keyAlphabet -> 'A'..'Z'
char transformChar(char ch, const vector<char>& mapping);        // Apply mapping to a single letter
bool parseArgs(int argc, char* argv[], bool& decryptMode, string& keyword,
    string& inPath, string& outPath);                 // Parse CLI flags
int processFile(const string& inPath, const string& outPath, const vector<char>& mapping); // Stream transform

// --------------------------- Helper Utilities -------------------------------

// (Team Member: Relly Bodden)
// (Ch.4) Character handling; (Ch.3) string building
string dedupeKeyword(const string& raw)
{
    string upper;
    upper.reserve(raw.size());
    // Keep only alphabetic letters and convert to uppercase (Ch.4 cctype)
    for (char c : raw) {
        if (isalpha(static_cast<unsigned char>(c))) {
            upper.push_back(static_cast<char>(toupper(static_cast<unsigned char>(c))));
        }
    }

    string result;
    // Track seen letters (simple array for A..Z)
    bool seen[26] = { false };
    for (char c : upper) {
        int idx = c - 'A';
        if (idx >= 0 && idx < 26 && !seen[idx]) {
            seen[idx] = true;
            result.push_back(c);
        }
    }
    return result; // e.g., "FEATHER" -> "FEATHR"
}

// (Team Member: Relly Bodden)
// (Ch.3/7) strings and simple loops; (Spec P8.7): Append remaining letters in reverse order (Z..A)
string buildKeyAlphabet(const string& keywordDedupe)
{
    bool used[26] = { false };
    for (char c : keywordDedupe) {
        if (c >= 'A' && c <= 'Z') used[c - 'A'] = true;
    }

    string key = keywordDedupe;
    for (char c = 'Z'; c >= 'A'; --c) {
        if (!used[c - 'A']) key.push_back(c);
        if (c == 'A') break; // prevent wraparound
    }
    return key; // key now length 26
}

// (Team Member: Tyler Drake)
// (Ch.7) Build mapping table for encryption: A..Z -> keyAlphabet
vector<char> buildMappingEncrypt(const string& keyAlphabet)
{
    vector<char> map(26);
    for (int i = 0; i < 26; ++i) {
        map[i] = keyAlphabet[i];
    }
    return map;
}

// (Team Member: Tyler Drake)
// (Ch.7/5) Inverse mapping for decryption: keyAlphabet -> A..Z
vector<char> buildMappingDecrypt(const string& keyAlphabet)
{
    vector<char> map(26);
    for (int i = 0; i < 26; ++i) {
        char k = keyAlphabet[i];      // ciphertext letter for plaintext 'A'+i
        int idx = k - 'A';
        if (idx >= 0 && idx < 26) {
            map[idx] = static_cast<char>('A' + i);
        }
    }
    return map;
}

// (Team Member: Nathanial Cunningham)
// (Ch.4) Conditional logic; (Ch.5) functions
char transformChar(char ch, const vector<char>& mapping)
{
    if (isalpha(static_cast<unsigned char>(ch))) {
        bool isLower = islower(static_cast<unsigned char>(ch));
        char up = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
        int idx = up - 'A';
        if (idx >= 0 && idx < 26) {
            char out = mapping[idx];
            return isLower ? static_cast<char>(tolower(static_cast<unsigned char>(out))) : out;
        }
    }
    return ch; // Non-letters unchanged
}

// (Team Member: Tyler Drake)
// (Ch.8) File streams
int processFile(const string& inPath, const string& outPath, const vector<char>& mapping)
{
    ifstream in(inPath.c_str(), ios::binary);
    if (!in) {
        cerr << "Error: cannot open input file: " << inPath << endl;
        return 2;
    }
    ofstream out(outPath.c_str(), ios::binary);
    if (!out) {
        cerr << "Error: cannot open output file: " << outPath << endl;
        return 3;
    }

    char ch;
    while (in.get(ch)) {
        out.put(transformChar(ch, mapping));
    }
    return 0;
}

// (Team Member: Nathanial Cunningham)
// (Ch.5) Argument parsing
bool parseArgs(int argc, char* argv[], bool& decryptMode, string& keyword,
    string& inPath, string& outPath)
{
    if (argc < 5) return false;

    decryptMode = false;
    keyword.clear();
    inPath.clear();
    outPath.clear();

    int i = 1;
    for (; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-e") {
            decryptMode = false;
        }
        else if (arg == "-d") {
            decryptMode = true;
        }
        else if (arg.rfind("-k", 0) == 0) { // starts with "-k"
            keyword = arg.substr(2);
            if (keyword.empty()) {
                if (i + 1 < argc) {
                    keyword = argv[++i];
                }
                else {
                    return false;
                }
            }
        }
        else {
            if (inPath.empty()) inPath = arg;
            else if (outPath.empty()) outPath = arg;
            else return false;
        }
    }

    if (keyword.empty() || inPath.empty() || outPath.empty()) return false;
    return true;
}

// (Team Member: Whole Team — integration)
// (Ch.1–8) Main: ties together parsing, key generation, and streaming transform
int main(int argc, char* argv[])
{
    bool decrypt = false;
    string keyword, inFile, outFile;

    if (!parseArgs(argc, argv, decrypt, keyword, inFile, outFile)) {
        cerr << "Usage: crypt (-e | -d) -kKEYWORD <input> <output>\n"
            << "Example: crypt -d -kFEATHER encrypt.txt output.txt\n";
        return 1;
    }

    string deduped = dedupeKeyword(keyword);
    if (deduped.empty()) {
        cerr << "Error: keyword must contain at least one alphabetic character.\n";
        return 1;
    }

    string keyAlphabet = buildKeyAlphabet(deduped);

    vector<char> mapping = decrypt ? buildMappingDecrypt(keyAlphabet)
        : buildMappingEncrypt(keyAlphabet);

    int rc = processFile(inFile, outFile, mapping);
    if (rc != 0) return rc;

    cout << (decrypt ? "Decryption" : "Encryption")
        << " complete using keyword \"" << deduped << "\".\n";
    return 0;
}
