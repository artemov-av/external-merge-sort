# External merge sorter
## About
Sorts large files on disk by successively merging sorted parts and dumping them on disk.\
Algorithm is trying to minimize disk operations: it uses k-way merging and chooses the smallest file parts to merge.\
Input file is interpreted as a sequence of 4 byte unsigned integers.
## Usage
```
./ExternalMergeSorter <ram_size> <block_size> <input_file> <output_file> <tmp_dir>
```
where RAM size is in MB, block size is in KB.
