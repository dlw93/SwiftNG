CC=gcc
CFLAGS=-std=c99 -lsqlite3 -lpthread -ldl
LFLAGS=-lm -lsqlite3 -lpthread -ldl
SRC=lib/sqlite/sqlite3.c lib/cJSON/cJSON.c src/iterator.c src/vertex.c src/array.c src/ngram.c src/ngramfns.c src/hashmap.c src/hashfns.c src/graph.c src/index.c src/args.c src/main.c
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
