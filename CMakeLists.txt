cmake_minimum_required(VERSION 3.3)

project(Graphs)

set(SOURCE_FILES main.c edu/humboldt/wbi/vertex.c edu/humboldt/wbi/vertex.h edu/humboldt/wbi/graph.h edu/humboldt/wbi/graph.c edu/humboldt/wbi/iterator.c edu/humboldt/wbi/iterator.h edu/humboldt/wbi/ngram.c edu/humboldt/wbi/ngram.h edu/humboldt/wbi/array.c edu/humboldt/wbi/array.h edu/humboldt/wbi/index.c edu/humboldt/wbi/index.h edu/humboldt/wbi/treeset.c edu/humboldt/wbi/treeset.h edu/humboldt/wbi/quadtree.c edu/humboldt/wbi/quadtree.h)
add_executable(Graphs ${SOURCE_FILES})
