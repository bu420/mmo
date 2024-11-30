CC := clang-19
CFLAGS := -std=gnu23 -Werror -Wall -Wextra -Wconversion

SRCS := src/main.c src/net.c src/arr.c
OBJS := $(SRCS:.c=.o)

DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O3 -DNDEBUG

#
# Release build.
#

release: build/release/mmo

build/release/mmo: $(addprefix build/release/, $(OBJS))
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -lm $^ -o build/release/mmo

build/release/%.o: %.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $(CFLAGS) $(RELEASE_FLAGS) -Iinc $< -o $@

#
# Debug build.
#

debug: build/debug/mmo

build/debug/mmo: $(addprefix build/debug/, $(OBJS))
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -lm $^ -o build/debug/mmo

build/debug/%.o: %.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $(CFLAGS) $(DEBUG_FLAGS) -Iinc $< -o $@

#
# Expand macros.
#

expand: $(addprefix build/debug_expanded/, $(SRCS))

build/debug_expanded/%.c: %.c
	@mkdir -p $(shell dirname $@)
	$(CC) -E $(DEBUG_FLAGS) -Iinc $< > $@

#
# Expanded macros debug build.
#

debug-expanded: build/debug_expanded/mmo

build/debug_expanded/mmo: $(addprefix build/debug_expanded/, $(OBJS))
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -lm $^ -o build/debug_expanded/mmo

build/debug_expanded/%.o: build/debug_expanded/%.c
	$(CC) -c $(CFLAGS) $(DEBUG_FLAGS) -Iinc $< -o $@

#
# Client build.
#

client:
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) src/client.c -o bin/client
