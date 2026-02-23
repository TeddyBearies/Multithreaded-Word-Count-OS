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

bool isDelimiter(char c) {
    // delimiter = whitespace OR anything that is not a letter/digit
    return isspace((unsigned char)c) || !isalnum((unsigned char)c);
}

void adjustSegments(const string& filename, vector<Segment>& segments, long long fileSize) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return;

    // Adjust starts (except first segment)
    for (int i = 1; i < (int)segments.size(); i++) {
        long long pos = segments[i].start;
        if (pos >= fileSize) continue;

        file.clear();
        file.seekg(pos);

        char c;
        while (pos < fileSize && file.get(c)) {
            if (isDelimiter(c)) {
                // Move to the byte AFTER the delimiter
                segments[i].start = pos + 1;
                break;
            }
            pos++;
        }
    }

    // Adjust ends (except last segment)
    for (int i = 0; i < (int)segments.size() - 1; i++) {
        long long pos = segments[i].end;
        if (pos >= fileSize) {
            segments[i].end = fileSize;
            continue;
        }

        file.clear();
        file.seekg(pos);

        char c;
        while (pos < fileSize && file.get(c)) {
            if (isDelimiter(c)) {
                segments[i].end = pos; // end at delimiter byte (exclusive end logic)
                break;
            }
            pos++;
        }
    }

    // Force first start and last end
    segments[0].start = 0;
    segments.back().end = fileSize;
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

    cout << "Segments (raw):\n";
    for (int i = 0; i < (int)segments.size(); i++) {
        cout << "  [" << i << "] start=" << segments[i].start
             << " end=" << segments[i].end << "\n";
    }

    adjustSegments(filename, segments, fileSize);

    cout << "Segments (adjusted):\n";
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