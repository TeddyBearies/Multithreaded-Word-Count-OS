#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <cstdlib>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

struct Segment {
    long long start;
    long long end;
};

bool isDelimiter(char c) {
    return isspace((unsigned char)c) || !isalnum((unsigned char)c);
}

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
    if (!file.is_open()) return -1;
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

void adjustSegments(const string& filename, vector<Segment>& segments, long long fileSize) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return;

    // Adjust starts (except first)
    for (int i = 1; i < (int)segments.size(); i++) {
        long long pos = segments[i].start;
        if (pos >= fileSize) continue;

        file.clear();
        file.seekg(pos);

        char c;
        while (pos < fileSize && file.get(c)) {
            if (isDelimiter(c)) {
                segments[i].start = pos + 1;
                break;
            }
            pos++;
        }
    }

    // Adjust ends (except last)
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
                segments[i].end = pos; // exclusive end
                break;
            }
            pos++;
        }
    }

    segments[0].start = 0;
    segments.back().end = fileSize;
}

unordered_map<string, int> countSegment(const string& filename, long long start, long long end) {
    unordered_map<string, int> counts;

    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return counts;
    }

    file.seekg(start);

    string current;
    long long pos = start;

    char c;
    while (pos < end && file.get(c)) {
        if (isDelimiter(c)) {
            if (!current.empty()) {
                string cleanWord = normalize(current);
                if (!cleanWord.empty()) counts[cleanWord]++;
                current.clear();
            }
        } else {
            current += c;
        }
        pos++;
    }

    if (!current.empty()) {
        string cleanWord = normalize(current);
        if (!cleanWord.empty()) counts[cleanWord]++;
    }

    return counts;
}

void mergeCounts(unordered_map<string, int>& target, const unordered_map<string, int>& src) {
    for (auto& p : src) {
        target[p.first] += p.second;
    }
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
    adjustSegments(filename, segments, fileSize);

    cout << "Segments (adjusted):\n";
    for (int i = 0; i < (int)segments.size(); i++) {
        cout << "  [" << i << "] start=" << segments[i].start
             << " end=" << segments[i].end << "\n";
    }

    unordered_map<string, int> combinedCounts;
    mutex mergeMutex;
    vector<thread> workers;

    for (int i = 0; i < (int)segments.size(); i++) {
        workers.push_back(thread([&, i]() {
            unordered_map<string, int> segCounts =
                countSegment(filename, segments[i].start, segments[i].end);

            lock_guard<mutex> lock(mergeMutex);
            mergeCounts(combinedCounts, segCounts);
        }));
    }

    for (auto& t : workers) {
        t.join();
    }

    cout << "Final consolidated counts:\n";
    for (auto& pair : combinedCounts) {
        cout << pair.first << ": " << pair.second << "\n";
    }

    return 0;
}