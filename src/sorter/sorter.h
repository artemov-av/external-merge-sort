#pragma once

#include <queue>
#include <string>
#include "../utils/block_io.h"

class ExternalMemorySorter {
public:
    ExternalMemorySorter(size_t ram_size, size_t block_size, const std::string& in, const std::string& out, const std::string& tmp_dir);

    void Sort();

private:
    size_t _ram_size;
    size_t _block_size;
    const std::string& _in;
    const std::string& _out;
    const std::string& _tmp_dir;
    size_t _tmp_files_cnt = 0;

    // heap for the sizes of the sorted file parts
    using file_heap_t = std::pair<size_t, std::string>;
    std::priority_queue<file_heap_t, std::vector<file_heap_t>, std::greater<file_heap_t>> _sorted_parts;

    // sorting ram-sized parts of input file and writing them to the disc
    void FirstStep();

    // merge a few sorted file parts and write into the disc
    void MergeStep(const std::string& out);
};