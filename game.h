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
};
typedef struct player player;

struct game {
  player player;
};
typedef struct game game;

int run_game_loop(game *game, dynarray *input);