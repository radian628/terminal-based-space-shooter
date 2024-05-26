#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
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
  // printf("Hello world!\n");
  char test[4] = "abcd";
  struct termios mode;

  // ansi escape seq to clear the terminal (ignore keyboard spam at the end)
  printf("\x1B[1;1H\x1B[2J");
  // int tty_file = open("/dev/tty", O_RDWR, 0);
  const char *msg = "haha stupid terminal get overwritten!";
  write(1, msg, strlen(msg));
  fflush(stdout);
  printScreen(80, 30);

  tcgetattr(0, &mode);
  mode.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(0, TCSANOW, &mode);

  // for (int i = 0; i < 4; i++) {
  //   read(0, test + i, 1);
  // }
  // printf("\ngot string: %s\n", test);

  while (1) {
    dynarray *input = get_all_of_stdin();

    if (da_size(input))
      write(1, da_start(input), da_size(input));

    if (((char *)da_start(input))[0] == 'x') {
      break;
    }
    free(input);
  }

  mode.c_lflag |= ECHO | ICANON;
  tcsetattr(0, TCSANOW, &mode);
}