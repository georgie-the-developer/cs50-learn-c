#!/bin/bash
mkdir -p bin
docker build -t project1 .
docker run --rm -v "$PWD":/app -w /app project1 gcc -g src/main.c src/libs/cs50/cs50.c src/app/cs50/recover/recover.c -o bin/main
