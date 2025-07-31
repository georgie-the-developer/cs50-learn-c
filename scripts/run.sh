#!/bin/bash
docker run --rm -it -v "$PWD":/app -w /app --memory=512m project1 ./bin/main ./files/speller/texts/austen.txt