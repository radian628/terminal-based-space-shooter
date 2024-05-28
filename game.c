#include "game.h"
#include "config.h"
#include "level.h"

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

int between(int x, int lo, int hi) {
  return x >= lo && x < hi;
}

int in_rect(int test_x, int test_y, int x, int y, int w, int h) {
  return between(test_x, x, x + w) && between(test_y, y, y + h);
}

int is_position_intersecting_level(game *game, ivec2 pos) {
  level *level = game->level;
  int x = pos.x;
  int y = pos.y - (int)game->level_progress;
  if (
    x < 0 || x >= level->width
    || pos.y < 0 || pos.y > GAME_HEIGHT
  ) {
    return 1;
  }

  if (
    y < 0 || y >= level->height
  ) {
    return 0;
  }

  int i = y * level->width + x;

  char block = level->statics_map[i];

  // odd -> takes up space
  return block != 0;
}

int is_player_intersecting_level(game *game) {
  return is_position_intersecting_level(game, game->player.pos);
}

void game_init(game *game) {
  game->player.pos.x = 40;
  game->player.pos.y = GAME_HEIGHT / 2;
  game->player.hitpoints = 20;
  game->player.dir = RIGHT;
  game->player.movement_timer = 0.1;
  game->player_projectiles = da_create(sizeof(player_projectile));
  game->enemies = da_create(sizeof(enemy));
  game->enemy_projectiles = da_create(sizeof(enemy_projectile));
  game->level_progress = 0.0;

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

game_loop_result update_player(game *game, dynarray *input) {
  if (game->player.pos.y >= GAME_HEIGHT)
    game->player.hitpoints = 0;

  game->player.movement_timer -= 1.0 / 60.0;
  game->player.projectile_timer -= 1.0 / 60.0;

  int should_player_fire = 0;

  for (char *c = da_start(input); c != da_end(input); c++) {
    if (*c == ' ') {
      should_player_fire = 1;
      continue;
    }

    if (*c == 'x') {
      return EXIT;
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

  // make sure we're not intersecting a wall   
  while (is_player_intersecting_level(game)) {
    game->player.pos.y++;
  }

  if (game->player.movement_timer < 0.0) {
    ivec2 old_pos = game->player.pos;
    game->player.pos = add(game->player.pos, dir_to_ivec2(
      game->player.dir
    ));
    if (is_player_intersecting_level(game)) {
      game->player.pos = old_pos;
    }
    while (is_player_intersecting_level(game)) {
      game->player.pos.y++;
    }
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

  return NORMAL;
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
      int hit = in_rect(
        pp->pos.x, pp->pos.y,
        e->pos.x - 2, e->pos.y - 1, 5, 3
      );
      if (hit) {
        e->hitpoints--;
        e->damage_animation_frames_remaining = 6;
        pp->alive = 0;
        break;
      }
    }

    if (is_position_intersecting_level(game, pp->pos)) {
      pp->alive = 0;
    }
  }

  da_filter(
    game->player_projectiles, 
    filter_dead_player_projectiles,
    NULL
  );
}

int filter_dead_enemy_projectiles(
  void *eproj_void,
  size_t index,
  void *context
) {
  return ((enemy_projectile *)eproj_void)->alive;
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

    // collision with player
    if (ep->pos.x == game->player.pos.x 
    && ep->pos.y == game->player.pos.y) {
      ep->alive = 0;
      game->player.hitpoints -= ep->damage;
      continue;
    }

    if (is_position_intersecting_level(game, ep->pos)) {
      ep->alive = 0;
    }
  }

  da_filter(
    game->enemy_projectiles,
    filter_dead_enemy_projectiles,
    NULL
  );
}

int filter_dead_enemies(
  void *enemy_void,
  size_t index,
  void *context
) {
  return ((enemy *)enemy_void)->hitpoints > 0;
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
        proj.alive = 1;
        proj.damage = 4;
        da_append(game->enemy_projectiles, &proj);
      }
    }
    e->time_until_fire -= 1.0 / 60.0;
    e->damage_animation_frames_remaining--;
  }

  da_filter(
    game->enemies,
    filter_dead_enemies,
    NULL
  );
}

game_loop_result run_game_loop(game *game, dynarray *input) {
  if (update_player(game, input) == EXIT) return EXIT;
  update_player_projectiles(game);
  update_enemy_projectiles(game);
  update_enemies(game);

  int old_progress = (int)game->level_progress;
  game->level_progress += SCROLL_SPEED / 60.0 * 0.5;
  int advanced = old_progress != (int)game->level_progress;

  if (advanced) {
    da_iterate(game->enemies, enemy, e) {
      e->pos.y++;
    }
    da_iterate(game->enemy_projectiles, enemy_projectile, e) {
      e->pos.y++;
    }
  }

  return game->player.hitpoints <= 0 ? DEATH : NORMAL;  
}