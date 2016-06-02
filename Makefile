CC=gcc
CFLAGS=-std=c99 -lsqlite3 -lpthread -ldl
LFLAGS=-lm -lsqlite3 -lpthread -ldl
SRC=lib/sqlite/sqlite3.c edu/humboldt/wbi/iterator.c edu/humboldt/wbi/vertex.c edu/humboldt/wbi/array.c edu/humboldt/wbi/ngram.c edu/humboldt/wbi/graph.c main.c
OBJ=$(SRC:%.c=obj/%.o)
BIN=swfcmp

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o swfcmp $(LFLAGS)

obj/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	-rm -R obj
