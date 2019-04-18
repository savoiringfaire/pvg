#include "graphdata.h"
#include <stdlib.h>
#include <stdbool.h>

void graphdata_incrementPointer(GraphData* data, int* pointer)
{
  if(*pointer >= data->length)
    {
      *pointer = 0;
      return;
    }

  (*pointer)++;
}

int graphdata_pointerAtEnd(GraphData* data, int* pointer)
{
  if((data->head == 0 &&
      *pointer == data->length-1) ||
     *pointer == data->head)
    {
      return true;
    }

  return false;
}

void graphdata_addDataPoint(GraphData* data, int64_t reading)
{
  if (data->head == data->length - 1) {
    data->head = 0;
  } else {
    (data->head)++;
  }

  data->readings[data->head] = reading;
}

void graphdata_initialize(GraphData* data, int elements)
{
  data->readings = (int64_t *) malloc(sizeof(int64_t) * elements);

  for (int i = 0; i < elements; i++) {
    data->readings[i] = -1;
  }

  data->head = 0;
  data->tail = 0;
  data->length = elements;
}

void graphdata_destroy(GraphData* data)
{
  free(data->readings);
  free(data);
}
