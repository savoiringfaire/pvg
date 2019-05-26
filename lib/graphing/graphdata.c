#include "graphdata.h"
#include <stdlib.h>
#include <stdbool.h>

struct graphdata_t {
  graphdata_cursor_t head;
  graphdata_cursor_t tail;
  int length;
  graphdata_datapoint_t *readings;
};

void graphdata_increment_cursor(graphdata_handle_t data, graphdata_cursor_t cursor)
{
  if(*cursor >= data->length - 1)
    {
      *cursor = 0;
      return;
    }

  (*cursor)++;
}

int graphdata_pointerAtEnd(graphdata_handle_t data, int* pointer)
{
  if((data->head == 0 &&
      *pointer == data->length-1) ||
     *pointer == data->head)
    {
      return true;
    }

  return false;
}

void graphdata_addDataPoint(graphdata_handle_t data, graphdata_datapoint_t reading)
{
  if (data->head == data->length - 1) {
    data->head = 0;
  } else {
    (data->head)++;
  }

  data->readings[data->head] = reading;
}

void graphdata_initialize(graphdata_handle_t data, int elements)
{
  data->readings = (graphdata_datapoint_t *) malloc(sizeof(graphdata_datapoint_t) * elements);

  for (int i = 0; i < elements; i++) {
    data->readings[i] = -1;
  }

  data->head = 0;
  data->tail = 0;
  data->length = elements;
}

int graphdata_current_head(graphdata_handle_t data) {
  return data->head;
}

void graphdata_destroy(graphdata_handle_t data)
{
  free(data->readings);
  free(data);
}
