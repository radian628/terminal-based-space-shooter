#pragma once

struct ivec2 {
  int x;
  int y;
}
typedef struct ivec2 ivec2;

struct player {
  ivec2 pos;
}
typedef struct player player;

struct game {
  player player;
}
typedef struct game game;