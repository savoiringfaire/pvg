#ifndef GRAPH_H
#define GRAPH_H

#include "graphdata.h"

typedef struct GraphConfig GraphConfig;
struct GraphConfig {
    int rows;
    int columns;
};

void graph_print(graphdata_handle_t, GraphConfig*, WINDOW*);
void graph_readableDataRate(int64_t, char*);
void graph_findRange(graphdata_handle_t, int64_t*, int64_t*);

#endif
