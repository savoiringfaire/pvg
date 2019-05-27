#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <stdint.h>
#include <pthread.h>

typedef struct graphdata_t graphdata_t;

typedef graphdata_t* graphdata_handle_t;
typedef uint64_t graphdata_cursor_t;
typedef int64_t graphdata_datapoint_t;

void graphdata_advance_cursor(graphdata_handle_t, graphdata_cursor_t*);
graphdata_datapoint_t graphdata_read_datapoint(graphdata_handle_t, graphdata_cursor_t);
int  graphdata_cursor_at_end(graphdata_handle_t, graphdata_cursor_t);
graphdata_cursor_t  graphdata_current_head(graphdata_handle_t);
void graphdata_add_datapoint(graphdata_handle_t, int64_t);
void graphdata_initialize(graphdata_handle_t, int);
void graphdata_destroy(graphdata_handle_t);

#endif
