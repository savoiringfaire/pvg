#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <locale.h>

#include "graphdata.h"

#define BUFFER_SIZE 100
#define UPDATE_RATE 1000
#define GRAPH_ROWS 20
#define GRAPH_COLUMNS 20

pthread_mutex_t byteCountMutex = PTHREAD_MUTEX_INITIALIZER;
int byteCount = 0;
volatile int dataFinished = false;

void *forwardAndCount(void* vargp)
{
  char buffer[BUFFER_SIZE];
  int bytesRead;

  while ((bytesRead = fread(&buffer, 1, BUFFER_SIZE, stdin)) != 0)
  {
    fwrite(&buffer, 1, bytesRead, stdout);

    pthread_mutex_lock(&byteCountMutex);
    byteCount += bytesRead;
    pthread_mutex_unlock(&byteCountMutex);
  }

  dataFinished = true;
}

void findRange(GraphData* data, int* highest, int* lowest)
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

void readabledatarate(int bytespersecond, char* buf)
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

void printGraph(GraphData* data)
{
  erase();

  int pointer = data->head + 1;

  int highest;
  int lowest;

  findRange(data, &highest, &lowest);
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
    readabledatarate(bytesPerDivision*(GRAPH_ROWS - i), buf);
    printw("%-10s", buf);
  }

  refresh();
}

int main(int _argc, char ** _argv)
{
  setlocale(LC_ALL, "");
  newterm(NULL, stderr, stdin);

  pthread_t thread;
  pthread_create(&thread, NULL, forwardAndCount, NULL);
  erase();

  int lastByteCount = 0;
  GraphData data;

  graphdata_initialize(&data, COLS);

  while(!dataFinished)
  {
    printGraph(&data);

    pthread_mutex_lock(&byteCountMutex);

    int bytesPerSecond = (byteCount - lastByteCount) / (UPDATE_RATE / 1000);
    graphdata_addDataPoint(&data, bytesPerSecond);

    int megabytesPerSecond = bytesPerSecond/1024/1024;

    lastByteCount = byteCount;

    pthread_mutex_unlock(&byteCountMutex);

    usleep(UPDATE_RATE * 1000);
  }

  endwin();
}
