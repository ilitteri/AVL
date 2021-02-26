#!/bin/bash
mkdir -p build

files=("${@:2}")
out=(${files[@]/%c/o})
out=(${out[@]/#/build/})

for ((i=0; i < ${#files[@]}; i++))
do
    cc -g -O2 -std=gnu18 -Wall -pedantic -Wformat=2 -Wshadow -Wpointer-arith -Wunreachable-code -Wconversion -Wno-sign-conversion -Wbad-function-cast --coverage -Werror -DTDA=avl -c -o ${out[i]} ${files[i]} || exit
done

cc -g -O2 -std=gnu18 -Wall -pedantic -Wformat=2 -Wshadow -Wpointer-arith -Wunreachable-code -Wconversion -Wno-sign-conversion -Wbad-function-cast --coverage -Werror -DTDA=avl \
    ${out[@]} -o build/pruebas_alu &&
./build/pruebas_alu &&
gcovr -r . -f "$1" && 
gcovr -r . -f "$1" --branches -s 
