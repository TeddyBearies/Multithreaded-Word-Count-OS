#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <cstdlib>
#include <vector>

using namespace std;

struct Segment {
    long long start;
    long long end;
};

string normalize(const string& input) {
    string result;

    for (char c : input) {
        if (isalnum((unsigned char)c)) {
            result += (char)tolower((unsigned char)c);
        }
    }

    return result;
}

long long getFileSize(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        return -1;
    }
    return (long long)file.tellg();
}

vector<Segment> buildSegments(long long fileSize, int N) {
    vector<Segment> segments;
    long long chunkSize = fileSize / N;

    long long start = 0;
    for (int i = 0; i < N; i++) {
        long long end = (i == N - 1) ? fileSize : (start + chunkSize);
        segments.push_back({start, end});
        start = end;
    }

    return segments;
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <text_file> <N>\n";
        cout << "N is the number of segments/threads (must be >= 1)\n";
        return 1;
    }

    string filename = argv[1];

    int N = atoi(argv[2]);
    if (N < 1) {
        cout << "Error: N must be >= 1\n";
        return 1;
    }

    long long fileSize = getFileSize(filename);
    if (fileSize < 0) {
        cout << "Error: cannot open file\n";
        return 1;
    }

    cout << "Using N = " << N << "\n";
    cout << "File size = " << fileSize << " bytes\n";

    vector<Segment> segments = buildSegments(fileSize, N);

    cout << "Segments:\n";
    for (int i = 0; i < (int)segments.size(); i++) {
        cout << "  [" << i << "] start=" << segments[i].start
             << " end=" << segments[i].end << "\n";
    }

    // Keep existing single-thread counting for now
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: cannot open file\n";
        return 1;
    }

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