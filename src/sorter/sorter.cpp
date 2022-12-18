#include "sorter.h"
#include <filesystem>
#include <algorithm>
#include <cstdio>

ExternalMemorySorter::ExternalMemorySorter(size_t ram_size, size_t block_size, const std::string& in, 
                                            const std::string& out, const std::string& tmp_dir): 
                                            _ram_size(ram_size), _block_size(block_size), _in(in), _out(out), _tmp_dir(tmp_dir) {
                                            }

void ExternalMemorySorter::Sort() {
    namespace fs = std::filesystem;

    fs::create_directories(_tmp_dir);

    FirstStep();
    while (_sorted_parts.size() >= _ram_size / _block_size ) {
        std::string out_path = fs::path(_tmp_dir) / std::to_string(_tmp_files_cnt++);
        std::cout << out_path;
        std::cout << std::flush;
        MergeStep(out_path);
    }

    MergeStep(_out);
}

void ExternalMemorySorter::FirstStep() {
    namespace fs = std::filesystem;

    utils::BlockReaderUint32 reader(_ram_size, _in);
    auto& read_buffer = reader.GetBuffer();
    
    while (!reader.IsEOF()) {
        reader.Read();
        size_t data_size = reader.GetDataSize();
        if (data_size == 0) {
            break;
        }
        std::sort(read_buffer.begin(), read_buffer.begin() + data_size);

        std::string out_path = fs::path(_tmp_dir) / std::to_string(_tmp_files_cnt++);
        utils::BlockWriterUint32 writer(out_path);
        writer.Write(read_buffer, data_size);
        _sorted_parts.push(std::make_pair(data_size, out_path));
    }
}

void ExternalMemorySorter::MergeStep(const std::string& out) {
    // number of arrays to merge
    int k = _ram_size / _block_size - 1;
    std::vector<std::string> used_files(k);
    std::vector<utils::BlockReaderUint32> readers;
    std::vector<uint32_t> read_buffers_sizes;
    utils::BlockWriterUint32 writer(out);
    // heap with an element of the array as a key and a pair (number of array, index) as a value
    using pr_q_elem_t = std::tuple<uint32_t, size_t, size_t>;
    std::priority_queue<pr_q_elem_t, std::vector<pr_q_elem_t>, std::greater<pr_q_elem_t>> pr_q;
    
    size_t total_size = 0;
    for (int i = 0; i < k && !_sorted_parts.empty(); ++i) {
        auto [file_size, file_name] = _sorted_parts.top();
        total_size += file_size;

        readers.emplace_back(_block_size, file_name);
        readers.back().Read();
        read_buffers_sizes.push_back(readers.back().GetDataSize());
        pr_q.push(std::make_tuple(readers.back().GetBuffer()[0], i, 0));

        _sorted_parts.pop();
        used_files[i] = file_name;
    }

    size_t written = 0;
    std::vector<uint32_t> write_buffer(_block_size / sizeof(uint32_t));
    while(!pr_q.empty()) {
        auto [val, array_num, idx] = pr_q.top();
        pr_q.pop();

        write_buffer[written++] = val;
        // if the write buffer is full, dump it into the disk
        if (written == _block_size / sizeof(uint32_t)) {
            writer.Write(write_buffer, written);
            written = 0;
        }

        int next_idx = idx + 1;
        // check if it is time to read the next block
        if (next_idx == read_buffers_sizes[array_num]) {
            readers[array_num].Read();
            auto data_size = readers[array_num].GetDataSize();
            read_buffers_sizes[array_num] = data_size;
            if (data_size != 0) {
                next_idx = 0;
            } else {
                next_idx = -1;
            }
        }
        // push the next element of that array into the heap
        if(next_idx != -1) {
            pr_q.push(std::make_tuple(readers[array_num].GetBuffer()[next_idx], array_num, next_idx));
        }
    }

    // write remains of data
    writer.Write(write_buffer, written);

    // remove used files from disk
    for (const auto& file_name: used_files) {
        std::remove(file_name.c_str());
    }

    // add new file to the heap
    _sorted_parts.push(std::make_pair(total_size, out));
}