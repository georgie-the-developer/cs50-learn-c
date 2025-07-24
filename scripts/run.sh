#!/bin/bash
docker run --rm -it -v "$PWD":/app -w /app --memory=512m project1 ./bin/main