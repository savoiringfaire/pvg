#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <inttypes.h>

#include "graphdata.h"
#include "graph.h"

#define BUFFER_SIZE 100
#define UPDATE_RATE 100 // Milliseconds
#define GRAPH_ROWS 20
#define GRAPH_COLUMNS 20

pthread_mutex_t byteCountMutex = PTHREAD_MUTEX_INITIALIZER;
int64_t byteCount = 0;
volatile int shouldExit = false;

void *forwardAndCount(void* vargp)
{
  char buffer[BUFFER_SIZE];
  int64_t bytesRead;

  while ((bytesRead = fread(&buffer, 1, BUFFER_SIZE, stdin)) != 0 && !shouldExit)
  {
    fwrite(&buffer, 1, bytesRead, stdout);

    pthread_mutex_lock(&byteCountMutex);
    byteCount += bytesRead;
    pthread_mutex_unlock(&byteCountMutex);
  }

  shouldExit = true;
}

void intHandler(int dummy) {
  shouldExit = true;
}

int main(int _argc, char ** _argv) {
  setlocale(LC_ALL, "");
  newterm(NULL, stderr, stdin);
  signal(SIGINT, intHandler);

  pthread_t thread;
  pthread_create(&thread, NULL, forwardAndCount, NULL);
  erase();

  int64_t lastByteCount = 0;
  GraphData data;
  GraphConfig config;

  config.rows = GRAPH_ROWS;
  config.columns = COLS;

  graphdata_initialize(&data, COLS-10);
  WINDOW* win = newwin(0,0,0,0);

  FILE *fp;


  fp = fopen("/tmp/test.txt", "w+b");
  fprintf(fp, "This is testing for fprintf...\n");

  while(!shouldExit)
  {
    fprintf(fp, "locking mutex\n");
    pthread_mutex_lock(&byteCountMutex);

    fprintf(fp, "calculating BPS\n");
    float secondsPassed = (float)UPDATE_RATE / 1000.0;
    fprintf(fp, "Seconds: %f", secondsPassed);
    int64_t bytesSent = byteCount - lastByteCount;
    fprintf(fp, "Sent: %" PRIu64, bytesSent);
    int64_t bytesPerSecond = (byteCount - lastByteCount) / secondsPassed; // Milliseconds into seconds.
    fprintf(fp, "BPS: %" PRIu64 "\n", bytesPerSecond);

    fprintf(fp, "Adding datapoint\n");
    if (bytesPerSecond == 0) {
      graphdata_addDataPoint(&data, -1);
    } else {
      graphdata_addDataPoint(&data, bytesPerSecond);
    }

    fprintf(fp, "Setting lastbytecound\n");
    lastByteCount = byteCount;

    fprintf(fp, "unlocking mutex\n");
    pthread_mutex_unlock(&byteCountMutex);

    fprintf(fp, "printing graph\n");
    graph_print(&data, &config, win);

    usleep(UPDATE_RATE * 1000); // Milliseconds into microseconds.
  }

  fclose(fp);

  endwin();
}
