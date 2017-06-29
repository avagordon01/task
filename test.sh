#!/bin/bash
make
for i in tests/*; do
    echo "$i: diff input output:"
    diff --color=auto <(cat $i | hexdump) <(bin/compress $i | bin/decompress | hexdump)
    #echo "$i: compressed:"
    #bin/compress $i | hexdump
done
