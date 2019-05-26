#include <stdio.h>
#include <ncurses.h>
#include <config.h>
#include <inttypes.h>

#include "graph.h"
#include "graphdata.h"

void graph_findRange(graphdata_handle_t data, int64_t* highest, int64_t* lowest)
{
  int pointer = graphdata_current_head(data) + 1;
  *highest = -1;
  *lowest = -1;

  while(!graphdata_pointerAtEnd(data, &pointer)) {
    if(data->readings[pointer] == -1) {
      graphdata_incrementPointer(data, &pointer);
      continue;
    }
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

void graph_readableDataRate(int64_t bytespersecond, char* buf)
{
  int i = 0;

  const char* units[] = {"bps", "kbps", "mbps", "gbps", "tbps"};

  int64_t bitspersecond = bytespersecond * 8;

  while(bitspersecond > 1024 &&
        i <= 3) { // Magic number which must match the above units[] array length minus 1
    bitspersecond /= 1024;
    i++;
  }

  sprintf(buf, "%" PRIu64 " %s", bitspersecond, units[i]);
}

void graph_print(graphdata_handle_t data, GraphConfig* config, WINDOW* window)
{
  int pointer = data->head + 1;

  int64_t highest;
  int64_t lowest;

  graph_findRange(data, &highest, &lowest);
  if(highest == -1 || lowest == -1)
    return;

  int64_t range = highest-lowest;
  int64_t bytesPerDivision = (int64_t) range/(config->rows-1);

  werase(window);

  if(bytesPerDivision == 0)
    return;

  for (int x = 10; !graphdata_pointerAtEnd(data, &pointer); graphdata_incrementPointer(data, &pointer)) {
    //if (data->readings[pointer] == -1)
    //{
    //  x++;
    //  continue;
    //}

    int next = pointer;
    graphdata_incrementPointer(data, &next);

    int64_t nextValueY    = config->rows - ((int)(data->readings[next] - lowest))/bytesPerDivision;
    int64_t currentValueY = config->rows - ((int)(data->readings[pointer] - lowest))/bytesPerDivision;

    wmove(window, currentValueY, x);

    if (nextValueY == currentValueY || data->readings[next] == -1) {
      wprintw(window, "─");
    } else {
      if (nextValueY < currentValueY){
        wprintw(window, "╯");
        wmove(window, (nextValueY), x);
        wprintw(window, "╭");
        for(int i = nextValueY+1; i<currentValueY; i++) {
          wmove(window, ((i)), x);
          wprintw(window, "│");
        }
      }
      else {
        wprintw(window, "╮");
        wmove(window, (nextValueY), x);
        wprintw(window, "╰");
        for(int i = currentValueY+1; i<nextValueY; i++) {
          wmove(window, ((i)), x);
          wprintw(window, "│");
        }
      }
    }

    x++;

    if (graphdata_pointerAtEnd(data, &next)) {
      break;
    }
  }

  // Print the 10 character long x-axis
  char buf[100];
  for(int i = 0; i <= config->rows; i++) {
    wmove(window, i, 0);
    graph_readableDataRate(lowest + bytesPerDivision*(config->rows - i), buf);
    wprintw(window, "%-10s", buf);
  }

  wrefresh(window);
}
