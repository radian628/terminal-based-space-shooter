#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>

#include "dynarray.h"

int main() {
  // printf("Hello world!\n");
  char test[4] = "abcd";
  struct termios mode;

  // ansi escape seq to clear the terminal (ignore keyboard spam at the end)
  printf("\e[1;1H\e[2J asdffasdfasdasfasdd");
  // int tty_file = open("/dev/tty", O_RDWR, 0);
  const char *msg = "haha stupid terminal get overwritten!";
  write(1, msg, strlen(msg));
  fflush(stdout);

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