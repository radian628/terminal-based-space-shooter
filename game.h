#pragma once
#include "dynarray.h"
#include "level.h"

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
};
typedef struct player_projectile player_projectile;

struct game {
  player player;
  dynarray *player_projectiles;
  level *level;
};
typedef struct game game;

int run_game_loop(game *game, dynarray *input);