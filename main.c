#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "dynarray.h"
#include "input.h"
#include "display.h"
#include "game.h"


int main() {
  game game;
  game_init(&game);

  disable_echo_and_canonical();
  init_screen();
  // repeatedly poll for input
  while (1) {
    print_screen(80, 30, &game);
    usleep(1000000 / 60);

    // get all input from stdin
    dynarray *input = get_all_of_stdin();

    // run game loop
    if(run_game_loop(&game, input)) {
      free(input);
      break;
    }

    free(input);
  }
  close_screen();

  enable_echo_and_canonical();
}