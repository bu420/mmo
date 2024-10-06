SRC := src/main.c src/net.c

mmo: src/main.c
	clang-19 -std=c23 -Wall -Wextra -o build/mmo ${SRC}