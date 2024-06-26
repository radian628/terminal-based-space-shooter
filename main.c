#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <asm/termbits.h>

#include "dynarray.h"
#include "input.h"
#include "display.h"
#include "game.h"
#include "config.h"
#include "level.h"


int main( int argc, char *argv[] ) {

  game game;
  game_loop_result result; 
  game_init(&game);
  if(argc > 1) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    parseFile(argv[1], &game);
    game.level_progress -= w.ws_row;
  }else{
    printf("No level specified.\n");
    exit(1);
  }
  disable_echo_and_canonical();
  init_screen();
  // repeatedly poll for input
  while (1) {
    print_screen(game.level->width, GAME_HEIGHT, &game);
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