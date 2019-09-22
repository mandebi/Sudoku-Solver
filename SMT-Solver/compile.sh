#!/bin/bash

SRC=$2
OUTPUT=$1

echo "SRC=$SRC"
echo "OUTPUT=$OUTPUT"

gcc -o $OUTPUT  -Wall $SRC utilities.c libz3.so -lpthread -w
