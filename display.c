#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "game.h"
#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

enum color {
  RESET,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE
};

const char colors[8][10] = {"\x1B[0m", "\x1B[31m", "\x1B[32m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m", "\x1B[37m"};

char* last_buffer = NULL;
char* next_buffer = NULL;
char* last_color_buffer = NULL;
char* color_buffer = NULL;

int   width = 0;
int   height = 0;

void draw_multichar(char *buffer, int w, int h, char *draw, int x, int y) {
  int draw_x = x;
  int draw_y = y;
  while (*draw != '\0') {
    // newline -> go down and reset x
    if (*draw == '\n') {
      draw_x = x;
      draw_y++;

      // bottom of screen -> can't draw any more
      if (draw_y >= h) return;

    } else if (
      // draw anything that isn't a space
      *draw != ' '
    ) {
      
      // bounds checking
      if (
           draw_x >= 0
        && draw_y >= 0
        && draw_x < w
        && draw_y < h
      ) {
        buffer[draw_y * w + draw_x] = *draw;
      }

      draw_x++;
    } else {
      draw_x++;
    }
    draw++;
  }
}

void set_color_rect(
  char *buffer, int buffer_w, int buffer_h,
  enum color color, 
  int left, int top, int w, int h
) {
  for (size_t y = 0; y < h; y++) {
    for (size_t x = 0; x < w; x++) {
      int draw_x = left + x;
      int draw_y = top + y;
      if (
           draw_x >= 0
        && draw_x < buffer_w
        && draw_y >= 0
        && draw_y < buffer_h
      ) {
        buffer[buffer_w * draw_y + draw_x] = color;
      }
    }
  }
}


void init_screen() {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  // Clear Screen
  printf("\x1B[1;1H\x1B[2J");
  fflush(stdout);
  width = w.ws_col;
  height = w.ws_row;
  last_buffer = malloc(w.ws_row * w.ws_col);
  memset(last_buffer, ' ', w.ws_row * w.ws_col);
  next_buffer = malloc(w.ws_row * w.ws_col);
  memset(next_buffer, ' ', w.ws_row * w.ws_col);

  color_buffer = malloc(w.ws_row * w.ws_col);
  memset(color_buffer, RESET, w.ws_row * w.ws_col);
  last_color_buffer = malloc(w.ws_row * w.ws_col);
  memset(last_color_buffer, RESET, w.ws_row * w.ws_col);
}
void close_screen() {
  free(last_buffer);
  free(next_buffer);
  free(color_buffer);
  free(last_color_buffer);
  printf("\x1B[1;1H\x1B[2J");
  fflush(stdout);
}

void draw_size_error(char* buffer) {
  // Draw top and bottom rows
  for(int i = 0; i < width; i++) {
    buffer[i] = '#';
    buffer[(height-1) * width + i] = '#';
  }

  // Draw left and right columns
  for(int i = 1; i < height-1; i++) {
    buffer[i * width] = '#';
    buffer[i * width + width - 1] = '#';
  }

  char msg[1024];
  sprintf(msg, "Term (%d x %d) Too Small", width, height);

  strcpy(buffer + (height/2-1)*width + width/2 -1 - (strlen(msg)/2), msg);
  buffer[(height/2-1)*width + width/2 + strlen(msg)/2] = ' ';
}

void print_screen(int min_width, int min_height, game *game) {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  
  // If screen size has changed, make new buffers, with last_buffer full of
  // 0's so the screen redraws completely
  if(width != w.ws_col || height != w.ws_row) {
    free(next_buffer);
    next_buffer = malloc(w.ws_row * w.ws_col);
    memset(next_buffer, ' ', w.ws_row * w.ws_col);
    free(last_buffer);
    last_buffer = malloc(w.ws_row * w.ws_col);
    memset(last_buffer, 0, w.ws_row * w.ws_col);

    free(color_buffer);
    color_buffer = malloc(w.ws_row * w.ws_col);
    memset(color_buffer, RESET, w.ws_row * w.ws_col);
    free(last_color_buffer);
    last_color_buffer = malloc(w.ws_row * w.ws_col);
    memset(last_color_buffer, RESET, w.ws_row * w.ws_col);

    width = w.ws_col;
    height = w.ws_row;
  }

  if(width < min_width || height < min_height) {
    draw_size_error(next_buffer);
  }
  else{
    memset(next_buffer, ' ', w.ws_row * w.ws_col);
    memset(color_buffer, RESET, w.ws_row * w.ws_col);

    // DRAW STUFF HERE
    // draw player
    if (
         game->player.pos.y >= 0
      && game->player.pos.y < w.ws_row
      && game->player.pos.x >= 0
      && game->player.pos.x < w.ws_col
    ) {
      next_buffer[w.ws_col * game->player.pos.y + game->player.pos.x] = 'A';
      color_buffer[w.ws_col * game->player.pos.y + game->player.pos.x] = GREEN;
    }

    // draw player projectiles
    da_iterate(
      game->player_projectiles, player_projectile, pp
    ) {
      if (
           pp->pos.y >= 0
        && pp->pos.y < w.ws_row
        && pp->pos.x >= 0
        && pp->pos.x < w.ws_col
      ) {
        next_buffer[w.ws_col * pp->pos.y + pp->pos.x] = '|';
      }
    }

    // draw enemy projectiles
    da_iterate(
      game->enemy_projectiles, enemy_projectile, ep
    ) {
      draw_multichar(
        next_buffer, width, height,
        "*", ep->pos.x, ep->pos.y
      );
    }

    // draw enemies
    da_iterate(game->enemies, enemy, e) {
      draw_multichar(
        next_buffer, width, height,
        "  |  \n--F--\n  |  ", e->pos.x - 2, e->pos.y - 1
      );

      if (e->damage_animation_frames_remaining > 0) {
        set_color_rect(
          color_buffer, width, height,
          RED, e->pos.x - 2, e->pos.y - 1, 5, 3
        );
      }
    }

    // draw hud
    draw_multichar(
      next_buffer, width, height,
      "HP: ", 0, 0
    );
    for (int i = 0; i < 20; i++) {
      draw_multichar(
        next_buffer, width, height,
        i >= game->player.hitpoints ? "_" : "#",
        4 + i, 0
      );
    }
    
  }

  // Copy buffer changes to screen & fflush it
  for(int i = 0; i < w.ws_row*w.ws_col; i++) {
    if(last_buffer[i] != next_buffer[i] || last_color_buffer[i] != color_buffer[i]) {
      printf("\x1B[%d;%dH%s%c", (i / w.ws_col) + 1, (i % w.ws_col) + 1, colors[color_buffer[i]], next_buffer[i]);
      last_buffer[i] = next_buffer[i];
      last_color_buffer[i] = color_buffer[i];
    }
  }
  //move cursor to bottom right, reset colors
  printf("\x1B[%d;%dH%s", w.ws_row, w.ws_col, colors[RESET]);
  fflush(stdout);
}