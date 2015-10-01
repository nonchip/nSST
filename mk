#!/bin/zsh

cd "$(dirname "$(readlink -f "$0")")"

for i in *.md
  Markdown.pl $i > $i.html

cd rf_protocol
gcc -c -Wall -Werror -fpic rf_protocol.c
gcc -shared -o librf_protocol.so rf_protocol.o
gcc -o test test.c -L. -lrf_protocol

