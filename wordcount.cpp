#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

using namespace std;

// Function to normalize a word
// - convert to lowercase
// - remove non-alphanumeric characters
string normalize(const string& input) {
    string result;

    for (char c : input) {
        if (isalnum(c)) {
            result += tolower(c);
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

    string word;

    while (file >> word) {
        string cleanWord = normalize(word);

        if (!cleanWord.empty()) {
            cout << cleanWord << "\n";
        }
    }

    return 0;
}
