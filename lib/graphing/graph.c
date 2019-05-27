#include <stdio.h>
#include <ncurses.h>
#include <config.h>
#include <inttypes.h>

#include "graph.h"
#include "graphdata.h"

void graph_findRange(graphdata_handle_t data, int64_t* highest, int64_t* lowest)
{
  graphdata_cursor_t cursor = graphdata_current_head(data) + 1;
  *highest = -1;
  *lowest = -1;

  while(!graphdata_cursor_at_end(data, cursor)) {
    if(graphdata_read_datapoint(data, cursor) == -1) {
      graphdata_advance_cursor(data, &cursor);
      continue;
    }
    if(*highest == -1) {
      *highest = graphdata_read_datapoint(data, cursor);
    }
    if(*lowest == -1) {
      *lowest = graphdata_read_datapoint(data, cursor);
    }

    if(graphdata_read_datapoint(data, cursor) > *highest) {
      *highest = graphdata_read_datapoint(data, cursor);
    } else if(graphdata_read_datapoint(data, cursor) < *lowest) {
      *lowest = graphdata_read_datapoint(data, cursor);
    }

    graphdata_advance_cursor(data, &cursor);
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
  graphdata_cursor_t cursor = graphdata_current_head(data) + 1;

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

  for (int x = 10; !graphdata_cursor_at_end(data, cursor); graphdata_advance_cursor(data, &cursor)) {
    graphdata_cursor_t next = cursor;
    graphdata_advance_cursor(data, &cursor);

    int64_t nextValueY    = config->rows - ((int)(graphdata_read_datapoint(data, next) - lowest))/bytesPerDivision;
    int64_t currentValueY = config->rows - ((int)(graphdata_read_datapoint(data, cursor) - lowest))/bytesPerDivision;

    wmove(window, currentValueY, x);

    if (nextValueY == currentValueY || graphdata_read_datapoint(data, next) == -1) {
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

    if (graphdata_cursor_at_end(data, next)) {
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
