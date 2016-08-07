CC=gcc
CFLAGS=-std=c99 -lsqlite3 -lpthread -ldl
LFLAGS=-lm -lsqlite3 -lpthread -ldl
SRC=lib/sqlite/sqlite3.c lib/cJSON/cJSON.c iterator.c vertex.c array.c ngram.c ngramfns.c hashmap.c hashfns.c graph.c index.c args.c main.c
OBJ=$(SRC:%.c=obj/%.o)
BIN=SwiftNG

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o SwiftNG $(LFLAGS)

obj/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	-rm -R obj
