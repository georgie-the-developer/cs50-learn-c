#!/bin/bash
docker run --rm -it -v "$PWD":/app -w /app project1 gdb ./bin/main
