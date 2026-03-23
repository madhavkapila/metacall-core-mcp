CC = gcc

INCLUDES = -I vendor/cjson -I src -I /gnu/include -I /usr/local/include
LIB_PATHS = -L /gnu/lib -L /usr/local/lib

CFLAGS = -Wall -Wextra -std=c11 $(INCLUDES)
LDFLAGS = $(LIB_PATHS) -lmetacall

SRC = src/main.c src/value_convert.c vendor/cjson/cJSON.c

all: metacall-mcp

metacall-mcp: $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f metacall-mcp

.PHONY: all clean