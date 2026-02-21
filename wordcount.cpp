#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <text_file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cout << "Error: cannot open file\n";
        return 1;
    }

    std::string word;
    while (file >> word) {
        std::cout << word << "\n";
    }

    return 0;
}
