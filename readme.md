# Hadean Compression Example
This code compresses or decompresses a file.

# Compiling
`make all`

# Running
`bin/compress test_input | bin/decompress test_output`

# Testing
`./test.sh` - should output the original test inputs and the inputs after being compressed and decompressed

# Design Choices
The compression works by making a list of the unique symbols in the input, labelling them 0-n, then packing them into log2(n) output bits.
Currently the code only reads a byte at a time, this is slower than reading ~4k bytes and processing them at once, but it's only a proof of concept.
The algorithm doesn't use a huffman coding because all of the symbols are assumed to have equal probabilities.
I assume the input file contains *only* a small set of unique symbols, not *mostly* the small set with others interspersed.
Also, the requirement to access elements in a constant time kind of implies a fixed length encoding.
For alphabets which are a power of two, and as the input file grows to infinity, the compression ratio will be almost perfect, because the overhead of the symbol table is constant ish, grows as log2(unique_symbols).
It can't handle arbitrarily large streams because there is no terminal symbol, the length is encoded as a uint32 number in the header.
