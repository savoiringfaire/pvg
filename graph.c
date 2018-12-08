#include <stdio.h>
#include <ncurses.h>
#include <config.h>

#include "graph.h"
#include "graphdata.h"

void graph_findRange(GraphData* data, int* highest, int* lowest)
{
  int pointer = data->head + 1;
  *highest = -1;
  *lowest = -1;

  while(!graphdata_pointerAtEnd(data, &pointer)) {
    if(*highest == -1) {
      *highest = data->readings[pointer];
    }
    if(*lowest == -1) {
      *lowest = data->readings[pointer];
    }

    if(data->readings[pointer] > *highest) {
      *highest = data->readings[pointer];
    } else if(data->readings[pointer] < *lowest) {
      *lowest = data->readings[pointer];
    }

    graphdata_incrementPointer(data, &pointer);
  }
}

void graph_readableDataRate(int bytespersecond, char* buf)
{
  int i = 0;
  const char* units[] = {"bps", "kbps", "mbps", "gbps", "", "", "", "", "", ""};
  int bitspersecond = bytespersecond * 8;
  while(bitspersecond > 1024) {
    bitspersecond /= 1024;
    i++;
  }
  sprintf(buf, "%d %s", bitspersecond, units[i]);
}

void graph_print(GraphData* data)
{
  erase();

  int pointer = data->head + 1;

  int highest;
  int lowest;

  graph_findRange(data, &highest, &lowest);
  int range = highest-lowest;
  int bytesPerDivision = (int) range/GRAPH_ROWS;

  if(bytesPerDivision == 0)
    return;

  int x = 0;

  while(!graphdata_pointerAtEnd(data, &pointer)) {
    int y = GRAPH_ROWS;
    for(int i = 0; i < (int) (data->readings[pointer]/bytesPerDivision); i++) {
      move(y, x);
      printw("█");
      y--;
    }

    x++;
    graphdata_incrementPointer(data, &pointer);
  }

  // Print the 10 character long x-axis
  char buf[100];
  for(int i = 0; i <= GRAPH_ROWS; i++) {
    move(i, 0);
    graph_readableDataRate(bytesPerDivision*(GRAPH_ROWS - i), buf);
    printw("%-10s", buf);
  }

  refresh();
}
