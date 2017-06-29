#include <stdio.h>
#include <stdint.h>

uint32_t int_ceil_log2(uint32_t x) {
    if (x == 0)
        return 0;
    x--;
    int32_t log = 1;
    while (x >>= 1) log++;
    return log;
}

int main(int argc, char* argv[]) {
    //setup the input and output stream
    FILE *input_file, *output_file;
    if (argc == 1) {
        input_file = stdin;
        output_file = stdout;
    } else if (argc == 2) {
        input_file = stdin;
        output_file = fopen(argv[1], "wb");
        if (!output_file) {
            fprintf(stderr, "error: failed to open output file %s\n", argv[1]);
            return 1;
        }
    } else if (argc == 3) {
        input_file = fopen(argv[1], "rb");
        if (!input_file) {
            fprintf(stderr, "error: failed to open input file %s\n", argv[1]);
            return 1;
        }
        output_file = fopen(argv[2], "wb");
        if (!output_file) {
            fprintf(stderr, "error: failed to open output file %s\n", argv[2]);
            return 1;
        }
    } else {
        fprintf(stderr, "usage: %s [output_filename [input_filename]]\n", argv[0]);
        fprintf(stderr, "if either input_filename or output_filename is omitted, %s will read from stdin or stdout\n", argv[0]);
        return 1;
    }



    //read the magic number, to "make sure" this is a compressed file
    uint16_t magic;
    fread(&magic, sizeof(magic), 1, input_file);
    if (magic != 0xf00d) {
        fprintf(stderr, "input_file doesn't appear to be a compressed file\n");
        return 1;
    }
    //read the symbol table from the input file
    uint32_t length = 0;
    uint16_t unique_count = 0;
    uint8_t symbols_list[256] = {0};
    fread(&length, sizeof(length), 1, input_file);
    fread(&unique_count, sizeof(unique_count), 1, input_file);
    fread(&symbols_list, sizeof(uint8_t), unique_count, input_file);



    //decompress the symbols from bits
    uint32_t bits_per_symbol = int_ceil_log2(unique_count);
    uint8_t input_bit = 8;
    uint8_t input_byte = 0;
    uint32_t output_length = 0;
    while (!feof(input_file)) {
        uint8_t byte = 0;
        fread(&byte, sizeof(uint8_t), 1, input_file);

        while (input_bit < 8) {
            if (output_length >= length)
                break;
            uint8_t symbol = (input_byte >> input_bit) | (byte << (8 - input_bit));
            symbol = symbols_list[symbol & ((1 << bits_per_symbol) - 1)];
            while (fwrite(&symbol, sizeof(uint8_t), 1, output_file) != 1) {};
            output_length++;
            input_bit += bits_per_symbol;
        }
        input_bit %= 8;
        input_byte = byte;
    }
    return 0;
}
