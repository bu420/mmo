#!/bin/bash
# This script formats all .c and .h files recursively using clang-format.

find . \( -iname '*.h' -o -iname '*.c' \) -type f | xargs clang-format -i
