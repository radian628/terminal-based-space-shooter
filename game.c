#include "game.h"
#include "config.h"

dir key_to_dir(char key) {
  switch (key) {\
    case 'w': return UP;
    case 'a': return LEFT;
    case 's': return DOWN;
    case 'd': return RIGHT;
    default: return -1;  
  }
}

ivec2 dir_to_ivec2(dir dir) {
  ivec2 v;
  v.x = dir == LEFT ? -1 : (dir == RIGHT) ? 1 : 0;
  v.y = dir == UP ? -1 : (dir == DOWN) ? 1 : 0;
  return v;
}

ivec2 add(ivec2 a, ivec2 b) {
  ivec2 v;
  v.x = a.x + b.x;
  v.y = a.y + b.y;
  return v;
}

int run_game_loop(game *game, dynarray *input) {
  game->player.movement_timer -= 1.0 / 60.0;
  game->player.projectile_timer -= 1.0 / 60.0;

  int should_player_fire = 0;

  for (char *c = da_start(input); c != da_end(input); c++) {
    if (*c == ' ') {
      should_player_fire = 1;
      continue;
    }

    if (*c == 'x') {
      return 1;
    }

    dir dir = key_to_dir(*c);
    if (dir != -1) {
      game->player.dir = dir;
      continue;
    }
  }

  if (should_player_fire == 1 && game->player.projectile_timer < 0) {
    game->player.projectile_timer = 1.0 / PLAYER_FIRE_SPEED;
    player_projectile proj;
    proj.pos = game->player.pos;
    da_append(game->player_projectiles, &proj);
  }

  if (game->player.movement_timer < 0.0) {
    game->player.pos = add(game->player.pos, dir_to_ivec2(
      game->player.dir
    ));
    switch (game->player.dir) {
      case UP:
      case DOWN:
        game->player.movement_timer = 2.0 / PLAYER_SPEED;
        break;
      case LEFT:
      case RIGHT:
        game->player.movement_timer = 1.0 / PLAYER_SPEED;
        break;
    }
  }

  for (
    player_projectile *pp = da_start(game->player_projectiles);
    pp != da_end(game->player_projectiles);
    pp++  
  ) {
    pp->pos.y--;
  }
  
  return 0;
}