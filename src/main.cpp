#include <iostream>
#include "sorter/sorter.h"


int main(int argc, char** argv) {
    if (argc < 6) {
        std::cerr << "There should be 5 arguments: RAM size in MB, block size in KB, input file, output file and directory for temporary files. Terminating.\n";
        return -1;
    }

    size_t ram_size = std::stoi(argv[1]);
    size_t block_size = std::stoi(argv[2]);
    std::string in = argv[3];
    std::string out = argv[4];
    std::string tmp_dir = argv[5];

    // decreasing RAM limit for external sort to avoid RAM excess 
    size_t ram_size_for_sorting = ram_size * 3 / 4;

    ExternalMemorySorter sorter(ram_size_for_sorting * (1 << 20), block_size * (1 << 10), in, out, tmp_dir);
    sorter.Sort();

    return 0;
}