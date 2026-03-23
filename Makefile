CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I vendor/cjson -I src
LDFLAGS = -lmetacall

SRC = src/main.c src/value_convert.c vendor/cjson/cJSON.c

all: metacall-mcp

metacall-mcp: $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f metacall-mcp

.PHONY: all clean