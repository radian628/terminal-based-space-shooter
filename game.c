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

double dir_multiplier(dir dir) {
  if (dir == UP || dir == DOWN) return 2.0;
  return 1.0;
}

ivec2 add(ivec2 a, ivec2 b) {
  ivec2 v;
  v.x = a.x + b.x;
  v.y = a.y + b.y;
  return v;
}

void game_init(game *game) {
  game->player.pos.x = 0;
  game->player.pos.y = 0;
  game->player.dir = RIGHT;
  game->player.movement_timer = 0.1;
  game->player_projectiles = da_create(sizeof(player_projectile));
  game->enemies = da_create(sizeof(enemy));
  game->enemy_projectiles = da_create(sizeof(enemy_projectile));

  enemy e;
  e.type = FOUR_DIRECTIONS;
  e.pos.x = 20;
  e.pos.y = 10;
  e.time_until_fire = 0.3;
  e.hitpoints = 3;
  e.damage_animation_frames_remaining = 0;
  da_append(game->enemies, &e);
}

dir DIRS[4] = { UP, DOWN, LEFT, RIGHT };

int update_player(game *game, dynarray *input) {
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
    proj.alive = 1;
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

  return 0;
}

int filter_dead_player_projectiles(
  void *proj_void,
  size_t index,
  void *context
) {
  return ((player_projectile *)proj_void)->alive;
}

void update_player_projectiles(game *game) {
  da_iterate(
    game->player_projectiles, player_projectile, pp
  ) {
    pp->pos.y--;
    if (pp->pos.y < 0) pp->alive = 0;
    da_iterate(game->enemies, enemy, e) {
      if (e->pos.x == pp->pos.x && e->pos.y == pp->pos.y) {
        e->hitpoints--;
        e->damage_animation_frames_remaining = 6;
        pp->alive = 0;
        break;
      }
    }
  }

  da_filter(
    game->player_projectiles, 
    filter_dead_player_projectiles,
    NULL
  );
}

void update_enemy_projectiles(game *game) {
  da_iterate(
    game->enemy_projectiles, enemy_projectile, ep
  ) {
    if (ep->time_until_move < 0.0)  {
      ep->pos = add(ep->pos, ep->vel);
      ep->time_until_move = ep->movement_interval;
    }
    ep->time_until_move -= 1.0 / 60.0;
  }
}

void update_enemies(game *game) {
  da_iterate(game->enemies, enemy, e) {
    if (e->time_until_fire < 0) {
      e->time_until_fire = 0.6;
      for (size_t i = 0; i < 4; i++) {
        enemy_projectile proj;
        proj.pos = e->pos;
        proj.vel = dir_to_ivec2(DIRS[i]);
        proj.size = 1;
        proj.movement_interval = 0.025 * dir_multiplier(DIRS[i]);
        proj.time_until_move = 0.0;
        da_append(game->enemy_projectiles, &proj);
      }
    }
    e->time_until_fire -= 1.0 / 60.0;
    e->damage_animation_frames_remaining--;
  }
}

int run_game_loop(game *game, dynarray *input) {
  if (update_player(game, input)) return 1;
  update_player_projectiles(game);
  update_enemy_projectiles(game);
  update_enemies(game);
  
  return 0;
}