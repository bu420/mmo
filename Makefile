SRC := src/main.c src/net.c
LIB := -lm

mmo:
	clang-19 -O3 -std=c23 -Iinc -Wall -Wextra -Wconversion -o bin/mmo ${SRC} ${LIB}

debug:
	clang-19 -g -std=c23 -Iinc -Wall -Wextra -Wconversion -o bin/mmo ${SRC} ${LIB}