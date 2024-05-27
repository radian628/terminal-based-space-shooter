#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "dynarray.h"
#include "input.h"
#include "display.h"


int main() {
  disable_echo_and_canonical();
  init_screen();
  // repeatedly poll for input
  while (1) {
    print_screen(80, 30);

    // get all input from stdin
    dynarray *input = get_all_of_stdin();

    // write input to stdout if any exists
    //if (da_size(input))
      //write(1, da_start(input), da_size(input));

    // exit if user presses "x"
    if (da_has(input, "x")) {
      break;
    }
    //free(input);
  }

  enable_echo_and_canonical();
}