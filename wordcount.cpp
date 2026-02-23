#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_map>

using namespace std;

string normalize(const string& input) {
    string result;

    for (char c : input) {
        if (isalnum((unsigned char)c)) {
            result += (char)tolower((unsigned char)c);
        }
    }

    return result;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <text_file>\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cout << "Error: cannot open file\n";
        return 1;
    }

    unordered_map<string, int> counts;

    string word;
    while (file >> word) {
        string cleanWord = normalize(word);
        if (!cleanWord.empty()) {
            counts[cleanWord]++; // increment count
        }
    }

    // Print results (order is not guaranteed with unordered_map)
    for (auto& pair : counts) {
        cout << pair.first << ": " << pair.second << "\n";
    }

    return 0;
}