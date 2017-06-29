all: bin/compress bin/decompress

bin/compress: src/compress.c
	clang -o $@ $< -O3

bin/decompress: src/decompress.c
	clang -o $@ $< -O3
