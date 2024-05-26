#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>

#include "dynarray.h"

void printScreen(int min_width, int min_height) {
  printf("\e[1;1H\e[2J");
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
    printf(msg);
  }
  fflush(stdout);
}

int main() {
  // printf("Hello world!\n");
  char test[4] = "abcd";
  struct termios mode;

  printScreen(80, 30);

  tcgetattr(0, &mode);
  mode.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(0, TCSANOW, &mode);

  for (int i = 0; i < 4; i++) {
    read(0, test + i, 1);
  }
  printf("\ngot string: %s\n", test);

  mode.c_lflag |= ECHO | ICANON;
  tcsetattr(0, TCSANOW, &mode);
}