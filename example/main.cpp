#include "jsonator.h"

#include <fstream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;
    }
    std::fstream fileStream(argv[1]);
    mblet::Jsonator jsonator;
    jsonator.readStream(fileStream);
    jsonator.dump(2);
    jsonator.dump(0);
    return 0;
}