#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <inttypes.h>
#include <argp.h>

#include "lib/graphing/graphdata.h"
#include "lib/graphing/graph.h"

#define BUFFER_SIZE 100
#define UPDATE_RATE 100 // Milliseconds

pthread_mutex_t byteCountMutex = PTHREAD_MUTEX_INITIALIZER;
int64_t byteCount = 0;
volatile int shouldExit = false;

const char* argp_program_version = "0.5";
const char* argp_program_bug_address = "<marcus@hhra.me>";

/* The options we understand. */
static struct argp_option options[] = {
  {"rows",   'r', "ROWS", OPTION_ARG_OPTIONAL,
   "Number of rows in the graph, defaults to 20" },
  {"cols",   'c', "COLS", OPTION_ARG_OPTIONAL,
   "Number of columns in the graph, defaults to the full width of the terminal" },
  {"rate",   'u', "SECONDS", OPTION_ARG_OPTIONAL,
   "The number of seconds between readings" },
  { 0 }
};

struct arguments
{
	int rows;
	int cols;
	int rate;
};

/* Program documentation. */
static char doc[] =
  "A graphical replacement to the popular pv utility on linux. Designed to be drop-in (apart from arguments) will display a curses graph of the data rate going through a pipe.";

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'r':
      if ( arg == NULL ||
	   (arguments->rows = atoi(arg)) == 0) {
        argp_usage(state);
      }
      break;
    case 'c':
      if ( arg == NULL ||
	   (arguments->cols = atoi(arg)) == 0) {
        argp_usage(state);
      }
      break;
    case 'u':
      if ( arg == NULL ||
	   (arguments->rate = atoi(arg)) == 0) {
        argp_usage(state);
      }
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argument parser.  The options, parser, and
   args_doc fields are zero because we have neither options or
   arguments; doc and argp_program_bug_address will be
   used in the output for ‘--help’, and the ‘--version’
   option will print out argp_program_version. */
static struct argp argp = {options, parse_opt, 0, doc };

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

int main(int argc, char ** argv) {
  setlocale(LC_ALL, "");

  struct arguments arguments;

  /* Default values. */
  arguments.rows = 20;
  arguments.cols = COLS;

  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  newterm(NULL, stderr, stdin);
  signal(SIGINT, intHandler);

  pthread_t thread;
  pthread_create(&thread, NULL, forwardAndCount, NULL);
  erase();

  int64_t lastByteCount = 0;
  graphdata_handle_t data;
  GraphConfig config;

  config.rows = arguments.rows;
  config.columns = arguments.cols;

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
    float secondsPassed = (float)arguments.rate;
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

    usleep(arguments.rate * 1000000); // Milliseconds into microseconds.
  }

  fclose(fp);

  endwin();
}
