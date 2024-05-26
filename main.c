#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include "dynarray.h"
#include "input.h"

int main() {
  // printf("Hello world!\n");
  char test[4] = "abcd";
  struct termios mode;

  // ansi escape seq to clear the terminal (ignore keyboard spam at the end)
  printf("\x1B[1;1H\x1B[2J asdffasdfasdasfasdd");
  // int tty_file = open("/dev/tty", O_RDWR, 0);
  const char *msg = "haha stupid terminal get overwritten!";
  write(1, msg, strlen(msg));
  fflush(stdout);

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