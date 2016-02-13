CC=gcc
CFLAGS=-std=c99
LFLAGS=-lm
SRC=edu/humboldt/wbi/iterator.c edu/humboldt/wbi/vertex.c edu/humboldt/wbi/array.c edu/humboldt/wbi/ngram.c edu/humboldt/wbi/graph.c edu/humboldt/wbi/treeset.c edu/humboldt/wbi/quadtree.c edu/humboldt/wbi/index.c main.c
OBJ=$(SRC:%.c=obj/%.o)
BIN=swfcmp

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o swfcmp

obj/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -R obj
