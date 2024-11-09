SRC := src/main.c src/net.c
LIB := -lm
WARN := -Wall -Wextra -Wconversion

mmo:
	clang-19 -std=gnu23 -O3 -DNDEBUG -Iinc ${WARN} ${SRC} ${LIB} -o bin/mmo

debug:
	clang-19 -std=gnu23 -g -Iinc ${WARN} ${SRC} ${LIB} -o bin/mmo

client:
	clang-19 -std=gnu23 -O3 -DNDEBUG ${WARN} src/client.c -o bin/client 

client-debug:
	clang-19 -std=gnu23 -g ${WARN} src/client.c -o bin/client