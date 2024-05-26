#include "input.h"
#include "dynarray.h"

#include <unistd.h>
#include <stdio.h>
#include <termios.h>

dynarray *get_all_of_stdin() {
  dynarray *da = da_create(sizeof(char));

  char buffer[1];

  struct pollfd pfd;
  pfd.fd = 0;
  pfd.events = POLLIN;

  while (poll(&pfd, 1, 0)) {
    int c = getchar();
    char c_as_char = (char)c;

    da_append(da, (void *)(&c_as_char));
  }
  return da;
}

struct termios termios_mode;

void disable_echo_and_canonical() {
  tcgetattr(0, &termios_mode);
  termios_mode.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(0, TCSANOW, &termios_mode);
}

void enable_echo_and_canonical() {
  termios_mode.c_lflag |= ECHO | ICANON;
  tcsetattr(0, TCSANOW, &termios_mode);
}