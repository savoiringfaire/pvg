#ifndef GRAPH_H
#define GRAPH_H

#include "graphdata.h"

void graph_print(GraphData* data);
void graph_readableDataRate(int bytesPerSecond, char* buf);
void graph_findRange(GraphData* data, int* highest, int* lowest);

#endif
