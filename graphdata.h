#ifndef GRAPHDATA_H
#define GRAPHDATA_H

typedef struct GraphData GraphData;
struct GraphData {
  int head;
  int tail;
  int length;
  int *readings;
};

void graphdata_incrementPointer(GraphData*, int*);
int  graphdata_pointerAtEnd(GraphData*, int*);
void graphdata_addDataPoint(GraphData*, int);
void graphdata_initialize(GraphData*, int);
void graphdata_destroy(GraphData*);

#endif
