#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <stdint.h>
#include <pthread.h>

typedef struct graphdata_t graphdata_t;

typedef graphdata_t* graphdata_handle_t;
typedef int* graphdata_cursor_t;
typedef int64_t graphdata_datapoint_t;

void graphdata_increment_cursor(graphdata_handle_t, graphdata_cursor_t);
void graphdata_read_data(graphdata_handle_t, graphdata_cursor_t);
int  graphdata_pointerAtEnd(graphdata_handle_t, graphdata_cursor_t);
int  graphdata_current_head(graphdata_handle_t);
void graphdata_addDataPoint(graphdata_handle_t, int64_t);
void graphdata_initialize(graphdata_handle_t, int);
void graphdata_destroy(graphdata_handle_t);

#endif
