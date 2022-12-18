#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cinttypes>

namespace utils
{
    template <typename T>
    class BlockReader {
    public:
        BlockReader(size_t block_size, const std::string& filepath) : _block_size(block_size), _buffer(block_size / sizeof(T)) {
            try {
                _in.open(filepath, std::ios::in | std::ios::binary);
            }
            catch(const std::ios_base::failure& fail) {
                std::cout << fail.what() << std::endl;
                return;
            }
            _in.seekg (0, _in.end);
            _left_to_read = _in.tellg();
            _in.seekg (0, _in.beg);
        }

        std::vector<T>& Read() {
            _data_size = (_block_size < _left_to_read) ? _block_size : _left_to_read;
            _in.read(reinterpret_cast<char*>(&(_buffer[0])), _data_size);
            _left_to_read -= _data_size;
            return _buffer;  
        }

        std::vector<T>& GetBuffer(){
            return _buffer;
        }

        size_t GetDataSize() {
            return _data_size / sizeof(T);
        }

        bool IsEOF() {
            return _left_to_read == 0;
        }
    private:
        std::vector<T> _buffer;
        const size_t _block_size;
        size_t _data_size;
        std::ifstream _in;
        size_t _left_to_read;
    };


    template <typename T>
    class BlockWriter {
    public:
        BlockWriter(const std::string& filepath) {
            try {
                _out.open(filepath, std::ios::out | std::ios::binary);
            }
            catch(const std::ios_base::failure& fail) {
                std::cout << fail.what() << std::endl;
                return;
            }
        }

        void Write(const std::vector<T>& data, size_t count) {
            _out.write(reinterpret_cast<const char*>(&(data[0])), count * sizeof(T));
            _out << std::flush;
        }
    private:
        std::ofstream _out;
    };

    using BlockWriterUint32 = BlockWriter<uint32_t>;
    using BlockReaderUint32 = BlockReader<uint32_t>;
}