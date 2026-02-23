#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <cstdlib>

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

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <text_file> <N>\n";
        cout << "N is the number of segments/threads (must be >= 1)\n";
        return 1;
    }

    string filename = argv[1];

    int N = atoi(argv[2]); // simple parsing for now
    if (N < 1) {
        cout << "Error: N must be >= 1\n";
        return 1;
    }

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: cannot open file\n";
        return 1;
    }

    // We will use N later when we add segmentation/threads
    cout << "Using N = " << N << "\n";

    unordered_map<string, int> counts;

    string word;
    while (file >> word) {
        string cleanWord = normalize(word);
        if (!cleanWord.empty()) {
            counts[cleanWord]++;
        }
    }

    for (auto& pair : counts) {
        cout << pair.first << ": " << pair.second << "\n";
    }

    return 0;
}