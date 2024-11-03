SRC := src/main.c src/net.c
LIB := -lm
WARN := -Wall -Wextra -Wconversion

mmo:
	clang-19 -O3 -std=c23 -Iinc ${WARN} -o bin/mmo ${SRC} ${LIB}

debug:
	clang-19 -g -std=c23 -Iinc ${WARN} -o bin/mmo ${SRC} ${LIB}

client:
	clang-19 -O3 -std=c23 ${WARN} -o bin/client src/client.c