#pragma once
#include "dynarray.h"

enum dir {
  UP, DOWN, LEFT, RIGHT
};
typedef enum dir dir;

struct ivec2 {
  int x;
  int y;
};
typedef struct ivec2 ivec2;

struct player {
  ivec2 pos;
  dir dir;
  double movement_timer;
  double projectile_timer;
};
typedef struct player player;

struct player_projectile {
  ivec2 pos;
  int alive;
};
typedef struct player_projectile player_projectile;

enum enemy_type {
  FOUR_DIRECTIONS
};
typedef enum enemy_type enemy_type;

struct enemy {
  ivec2 pos;
  enemy_type type;
  double time_until_fire;
  int hitpoints;
  int damage_animation_frames_remaining;
};
typedef struct enemy enemy;

struct enemy_projectile {
  ivec2 pos;
  ivec2 vel;
  size_t size;
  double movement_interval;
  double time_until_move;
};
typedef struct enemy_projectile enemy_projectile;

struct game {
  player player;
  dynarray *player_projectiles;

  dynarray *enemies;
  dynarray *enemy_projectiles;
};
typedef struct game game;

int run_game_loop(game *game, dynarray *input);

void game_init(game *game);