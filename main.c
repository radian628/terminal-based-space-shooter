#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "dynarray.h"
#include "input.h"
#include "display.h"
#include "game.h"


int main( int argc, char *argv[] ) {

  game game;
  game_loop_result result; 
  game_init(&game);
  if(argc > 1) {
    game.level = parseFile(argv[1]);
    game.level_progress
      = -(double)game.level->height;
  }else{
    printf("No level specified.\n");
    exit(1);
  }
  disable_echo_and_canonical();
  init_screen();
  // repeatedly poll for input
  while (1) {
    // Min height? 30 for now.
    print_screen(game.level->width, 30, &game);
    usleep(1000000 / 60);

    // get all input from stdin
    dynarray *input = get_all_of_stdin();

    // run game loop
    result = run_game_loop(&game, input);
    if (result != NORMAL) {
      da_free(input);
      break;
    }

    da_free(input);
  }
  close_screen();

  enable_echo_and_canonical();

  if (result == DEATH) {
    printf("You died!\n");
  }
}