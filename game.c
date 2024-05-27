#include "game.h"

void run_game_loop(game *game, dynarray *input) {
    game->player.movement_timer -= 1.0 / 60.0;

    for (char *c = da_start(input); c != da_end(input); c++) {
      switch (*c) {
        case 'w':
          game->player.dir = UP;
          break;
        case 'a':
          game->player.dir = LEFT;
          break;
        case 's':
          game->player.dir = DOWN;
          break;
        case 'd':
          game->player.dir = RIGHT;
          break;
      }
    }

    if (game->player.movement_timer < 0.0) {
      switch (game->player.dir) {
        case UP:
          game->player.pos.y--;
          game->player.movement_timer = 0.2;
          break;
        case DOWN:
          game->player.pos.y++;
          game->player.movement_timer = 0.2;
          break;
        case LEFT:
          game->player.pos.x--;
          game->player.movement_timer = 0.1;
          break;
        case RIGHT:
          game->player.pos.x++;
          game->player.movement_timer = 0.1;
          break;
      }
    }
}