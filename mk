#!/bin/zsh

cd "$(dirname "$(readlink -f "$0")")"

for i in *.md
  Markdown.pl $i > $i.html

cd rf_protocol
echo "const char* YCDEF_rf_protocol=" > rf_protocol.ycdef.h
sed 's/"/\\"/g;s/^/"/g;s/$/"/g' rf_protocol.h >> rf_protocol.ycdef.h
echo ";" >> rf_protocol.ycdef.h
gcc -c -Wall -Werror -fpic rf_protocol.c
gcc -shared -o librf_protocol.so rf_protocol.o
gcc -o test -lrf_protocol -L. test.c rf_protocol.h

