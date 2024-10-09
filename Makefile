SRC := src/core.c src/net.c src/main.c

mmo:
	clang-19 -std=c23 -Iinc -Wall -Wextra -o bin/mmo ${SRC}