#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>

#include "graphdata.h"
#include "graph.h"

#define BUFFER_SIZE 100
#define UPDATE_RATE 1000 // Milliseconds
#define GRAPH_ROWS 20
#define GRAPH_COLUMNS 20

pthread_mutex_t byteCountMutex = PTHREAD_MUTEX_INITIALIZER;
int64_t byteCount = 0;
volatile int dataFinished = false;

void *forwardAndCount(void* vargp)
{
  char buffer[BUFFER_SIZE];
  int64_t bytesRead;

  while ((bytesRead = fread(&buffer, 1, BUFFER_SIZE, stdin)) != 0)
  {
    fwrite(&buffer, 1, bytesRead, stdout);

    pthread_mutex_lock(&byteCountMutex);
    byteCount += bytesRead;
    pthread_mutex_unlock(&byteCountMutex);
  }

  dataFinished = true;
}

void intHandler(int dummy) {
  endwin();
  exit(1);
}

int main(int _argc, char ** _argv) {
  setlocale(LC_ALL, "");
  newterm(NULL, stderr, stdin);
  //signal(SIGINT, intHandler);

  pthread_t thread;
  pthread_create(&thread, NULL, forwardAndCount, NULL);
  erase();

  int64_t lastByteCount = 0;
  GraphData data;
  GraphConfig config;

  config.rows = GRAPH_ROWS;
  config.columns = GRAPH_COLUMNS;

  graphdata_initialize(&data, COLS);
  WINDOW* win = newwin(0,0,0,0);

  while(!dataFinished)
  {
    pthread_mutex_lock(&byteCountMutex);

    int64_t bytesPerSecond = (byteCount - lastByteCount) / (UPDATE_RATE / 1000); // Milliseconds into seconds.
    graphdata_addDataPoint(&data, bytesPerSecond);

    lastByteCount = byteCount;

    pthread_mutex_unlock(&byteCountMutex);

    graph_print(&data, &config, win);


    usleep(UPDATE_RATE * 1000); // Milliseconds into microseconds.
  }

  endwin();
}
