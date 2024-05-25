#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main() {
  // printf("Hello world!\n");
  char test[4] = "abcd";
  struct termios mode;

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