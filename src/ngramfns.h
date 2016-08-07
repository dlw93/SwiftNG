#ifndef NGRAMFNS_H
#define NGRAMFNS_H

#include "array.h"
#include "graph.h"

TArray *path_ngrams(TGraph *g, int n);

TArray *neighbourhood_ngrams(TGraph *g, int n);

#endif // NGRAMFNS_H