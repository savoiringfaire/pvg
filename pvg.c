#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <locale.h>

#include "graphdata.h"
#include "graph.h"

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
    graph_print(&data);

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
