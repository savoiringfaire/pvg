#include <stdio.h>
#include <ncurses.h>
#include <config.h>
#include <inttypes.h>

#include "graph.h"
#include "graphdata.h"

void graph_findRange(GraphData* data, uint64_t* highest, uint64_t* lowest)
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

void graph_readableDataRate(uint64_t bytespersecond, char* buf)
{
  int i = 0;

  const char* units[] = {"bps", "kbps", "mbps", "gbps", "tbps"};

  uint64_t bitspersecond = bytespersecond * 8;

  while(bitspersecond > 1024 &&
        i <= 4) { // Magic number which must match the above units[] array length minus 1
    bitspersecond /= 1024;
    i++;
  }

  sprintf(buf, "%" PRIu64 " %s", bitspersecond, units[i]);
}

void graph_print(GraphData* data, GraphConfig* config, WINDOW* window)
{
  werase(window);

  int pointer = data->head + 1;

  uint64_t highest;
  uint64_t lowest;

  graph_findRange(data, &highest, &lowest);
  uint64_t range = highest-lowest;
  uint64_t bytesPerDivision = (uint64_t) range/config->rows;

  if(bytesPerDivision == 0)
    return;

  int x = 0;

  while(!graphdata_pointerAtEnd(data, &pointer)) {
    int y = config->rows;
    for(int i = 0; i < (int) (data->readings[pointer]/bytesPerDivision); i++) {
      wmove(window, y, x);
      wprintw(window, "█");
      y--;
    }

    x++;
    graphdata_incrementPointer(data, &pointer);
  }

  // Print the 10 character long x-axis
  char buf[100];
  for(int i = 0; i <= config->rows; i++) {
    wmove(window, i, 0);
    graph_readableDataRate(bytesPerDivision*(config->rows - i), buf);
    wprintw(window, "%-10s", buf);
  }

  wrefresh(window);
}
