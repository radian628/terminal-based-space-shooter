#pragma once
#include "dynarray.h"

struct level;
typedef struct level level;

enum game_loop_result {
  NORMAL, DEATH, EXIT
};
typedef enum game_loop_result game_loop_result;

enum dir {
  UP, DOWN, LEFT, RIGHT
};
typedef enum dir dir;

struct ivec2 {
  int x;
  int y;
};
typedef struct ivec2 ivec2;

struct vec2 {
  double x;
  double y;
};
typedef struct vec2 vec2;

struct player {
  ivec2 pos;
  dir dir;
  double movement_timer;
  double projectile_timer;
  int hitpoints;
};
typedef struct player player;

struct player_projectile {
  ivec2 pos;
  int alive;
};
typedef struct player_projectile player_projectile;

enum enemy_type {
  FOUR_DIRECTIONS,
  FOLLOWER,
  DOWN_SHOOTER
};
typedef enum enemy_type enemy_type;

struct enemy {
  ivec2 pos;
  enemy_type type;
  double time_until_fire;
  int hitpoints;
  int damage_animation_frames_remaining;
  dir dir;
  double time_until_move;
};
typedef struct enemy enemy;

struct enemy_projectile {
  vec2 pos;
  vec2 vel;
  size_t size;
  int damage;
  int alive;
};
typedef struct enemy_projectile enemy_projectile;

struct game {
  player player;
  dynarray *player_projectiles;
  level *level;
  dynarray *enemies;
  dynarray *enemy_projectiles;
  double level_progress;
};
typedef struct game game;

game_loop_result run_game_loop(game *game, dynarray *input);

void game_init(game *game);

int is_player_intersecting_level(game *game);