#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <stdint.h>
#include <pthread.h>

typedef struct GraphData GraphData;
struct GraphData {
  int head;
  int tail;
  int length;
  int64_t *readings;
};

void graphdata_incrementPointer(GraphData*, int*);
int  graphdata_pointerAtEnd(GraphData*, int*);
void graphdata_addDataPoint(GraphData*, int64_t);
void graphdata_initialize(GraphData*, int);
void graphdata_destroy(GraphData*);

#endif