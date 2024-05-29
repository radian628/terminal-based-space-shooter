#include "game.h"
#include "config.h"
#include "level.h"

#include <math.h>

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

vec2 ivec2_to_vec2(ivec2 v) {
  vec2 v2;
  v2.x = (double)v.x;
  v2.y = (double)v.y;
  return v2;
}

ivec2 vec2_to_ivec2(vec2 v) {
  ivec2 v2;
  v2.x = (int)v.x;
  v2.y = (int)v.y;
  return v2;
}

vec2 dir_to_vec2(dir dir) {
  vec2 v2 = ivec2_to_vec2(dir_to_ivec2(dir));
  v2.y *= 0.5;
  return v2;
}

vec2 scalar_mul(vec2 v, double s) {
  v.x *= s;
  v.y *= s;
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

vec2 add_vec2(vec2 a, vec2 b) {
  vec2 v;
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

int is_position_intersecting_level(game *game, ivec2 pos, int ignore_top) {
  level *level = game->level;
  int x = pos.x;
  int y = pos.y - (int)game->level_progress;
  if (
    x < 0 || x >= level->width
    || (pos.y < 0 && !ignore_top) || pos.y > GAME_HEIGHT
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

int is_vec2_pos_intersecting_level(game *game, vec2 pos, int ignore_top) {
  return is_position_intersecting_level(
    game, vec2_to_ivec2(pos), ignore_top 
  );
}

int is_player_intersecting_level(game *game) {
  return is_position_intersecting_level(game, game->player.pos, 0);
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

  // enemy e;
  // e.type = FOUR_DIRECTIONS;
  // e.pos.x = 20;
  // e.pos.y = 10;
  // e.time_until_fire = 0.3;
  // e.hitpoints = 3;
  // e.damage_animation_frames_remaining = 0;
  // da_append(game->enemies, &e);
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

    if (is_position_intersecting_level(game, pp->pos, 1)) {
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
    ep->pos = add_vec2(ep->pos, ep->vel);

    // collision with player
    if ((int)ep->pos.x == game->player.pos.x 
    && (int)ep->pos.y == game->player.pos.y) {
      ep->alive = 0;
      game->player.hitpoints -= ep->damage;
      continue;
    }

    if (is_vec2_pos_intersecting_level(game, ep->pos, 1)) {
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

void update_enemy(game *game, enemy *e) {
  if (e->type == FOUR_DIRECTIONS) {
    if (e->time_until_fire < 0) {
      e->time_until_fire = 0.6;
      for (size_t i = 0; i < 4; i++) {
        enemy_projectile proj;
        proj.pos = ivec2_to_vec2(e->pos);
        proj.vel = scalar_mul(
          dir_to_vec2(DIRS[i]),
          0.4
        );
        proj.size = 1;
        proj.alive = 1;
        proj.damage = 4;
        da_append(game->enemy_projectiles, &proj);
      }
    }
  } else if (e->type == DOWN_SHOOTER) {
    if (e->time_until_fire < 0) {
      e->time_until_fire = 0.3;
      enemy_projectile proj;
      proj.pos = ivec2_to_vec2(e->pos);
      proj.vel.x = 0.0;
      proj.vel.y = 0.5;
      proj.size = 1;
      proj.alive = 1;
      proj.damage = 3;
      da_append(game->enemy_projectiles, &proj);
    }
    if (e->time_until_move < 0) {
      e->time_until_move = 0.17;
      e->pos = add(e->pos, dir_to_ivec2(e->dir));
    }

    if (is_position_intersecting_level(game, e->pos, 1)) {
      e->dir = e->dir == LEFT ? RIGHT : LEFT;
      e->pos = add(e->pos, dir_to_ivec2(e->dir));
    }
  } else if (e->type == FOLLOWER) {
    if (e->time_until_fire < 0) {
      e->time_until_fire = 0.4;
      enemy_projectile proj;
      proj.pos = ivec2_to_vec2(e->pos);
      proj.vel.x = e->pos.x - game->player.pos.x;
      proj.vel.y = e->pos.y - game->player.pos.y;
      double mag = sqrt(proj.vel.x * proj.vel.x + proj.vel.y * proj.vel.y);
      proj.vel.x /= -mag;
      proj.vel.y /= -mag;
      proj.vel.x *= 0.6;
      proj.vel.y *= 0.6;
      proj.size = 1;
      proj.alive = 1;
      proj.damage = 5;
      da_append(game->enemy_projectiles, &proj);
    }
  }
  
  e->time_until_fire -= 1.0 / 60.0;
  e->time_until_move -= 1.0 / 60.0;
  e->damage_animation_frames_remaining--;
}

void update_enemies(game *game) {
  da_iterate(game->enemies, enemy, e) {
    update_enemy(game, e);
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