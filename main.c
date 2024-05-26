#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include "dynarray.h"
#include "input.h"

void printScreen(int min_width, int min_height) {
  printf("\x1B[1;1H\x1B[2J");
  fflush(stdout);
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  if(w.ws_col < min_width || w.ws_row < min_height) {
    char msg[1024];
    sprintf(msg, "Term (%d x %d) Too Small", w.ws_col, w.ws_row);
    for(int i = 0; i < w.ws_row/2; i++) {
      printf("\n");
    }
    for(int i = 0; i < w.ws_col/2 - (strlen(msg)/2); i++) {
      printf(" ");
    }
    printf("%s", msg);
  }
  fflush(stdout);
}

int main() {

  // ansi escape seq to clear the terminal (ignore keyboard spam at the end)
  printf("\x1B[1;1H\x1B[2J");
  const char *msg = "haha stupid terminal get overwritten!";
  write(1, msg, strlen(msg));
  fflush(stdout);
  printScreen(80, 30);

  disable_echo_and_canonical();
  // repeatedly poll for input
  while (1) {

    // get all input from stdin
    dynarray *input = get_all_of_stdin();

    // write input to stdout if any exists
    if (da_size(input))
      write(1, da_start(input), da_size(input));

    // exit if user presses "x"
    if (((char *)da_start(input))[0] == 'x') {
      break;
    }
    free(input);
  }

  enable_echo_and_canonical();
}