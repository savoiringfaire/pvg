#ifndef GRAPH_H
#define GRAPH_H

#include "graphdata.h"

typedef struct GraphConfig GraphConfig;
struct GraphConfig {
    int rows;
    int columns;
};

void graph_print(GraphData*, GraphConfig*, WINDOW*);
void graph_readableDataRate(uint64_t, char*);
void graph_findRange(GraphData*, uint64_t*, uint64_t*);

#endif
