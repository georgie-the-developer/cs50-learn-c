#!/bin/bash
mkdir -p bin
docker build -t project1 .
docker run --rm -v "$PWD":/app -w /app project1 gcc -g src/main.c src/app/cs50/speller/speller.c src/app/cs50/speller/dictionary.c -o bin/main
