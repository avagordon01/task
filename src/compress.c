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
    if (argc == 2) {
        input_file = fopen(argv[1], "rb");
        if (!input_file) {
            fprintf(stderr, "error: failed to open input file %s\n", argv[1]);
            return 1;
        }
        output_file = stdout;
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
        fprintf(stderr, "usage: %s input_filename [output_filename]\n", argv[0]);
        fprintf(stderr, "if output_filename is omitted, %s will read from stdout\n", argv[0]);
        return 1;
    }



    //count the unique bytes in the input stream
    uint8_t unique_map[256] = {0};
    uint16_t unique_count = 0;
    uint8_t symbols_list[256] = {0};
    uint32_t length = 0;
    while (!feof(input_file)) {
        uint8_t byte = 0;
        if (fread(&byte, sizeof(uint8_t), 1, input_file) == 0)
            break;
        length++;
        if (unique_map[byte] == 0) {
            symbols_list[unique_count] = byte;
            unique_count++;
            unique_map[byte] = 1;
        }
    }



    //write a magic number, to "make sure" this is a compressed file
    uint16_t magic = 0xf00d;
    fwrite(&magic, sizeof(magic), 1, output_file);
    //create the symbol table to go at the head of the output file
    fwrite(&length, sizeof(length), 1, output_file);
    fwrite(&unique_count, sizeof(unique_count), 1, output_file);
    fwrite(&symbols_list, unique_count, 1, output_file);



    //compress the symbols into bits, write them to the output file
    uint32_t bits_per_symbol = int_ceil_log2(unique_count);
    fseek(input_file, 0, SEEK_SET);
    uint8_t output_byte;
    uint8_t output_bit = 0;
    while (!feof(input_file)) {
        uint8_t byte = 0;
        fread(&byte, sizeof(uint8_t), 1, input_file);
        uint8_t symbol;
        for (symbol = 0; symbol < unique_count; symbol++) {
            if (byte == symbols_list[symbol])
                break;
        }
        output_byte |= symbol << output_bit;
        output_bit += bits_per_symbol;
        if (output_bit >= 8) {
            while (fwrite(&output_byte, sizeof(uint8_t), 1, output_file) != 1) {};
            output_bit %= 8;
            output_byte = symbol >> (bits_per_symbol - output_bit);
        }
    }
    while (fwrite(&output_byte, sizeof(uint8_t), 1, output_file) != 1) {};
    return 0;
}
