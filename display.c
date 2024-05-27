#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

char* last_buffer = NULL;
char* next_buffer = NULL;
int   width = 0;
int   height = 0;

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

void print_screen(int min_width, int min_height) {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  
  if(width != w.ws_col || height != w.ws_row) {
    free(next_buffer);
    next_buffer = malloc(w.ws_row * w.ws_col);
    memset(next_buffer, ' ', w.ws_row * w.ws_col);
    free(last_buffer);
    last_buffer = malloc(w.ws_row * w.ws_col);
    memset(last_buffer, 0, w.ws_row * w.ws_col);
    width = w.ws_col;
    height = w.ws_row;
  }

  if(width < min_width || height < min_height) {
    draw_size_error(next_buffer);
  }
  else{
    memset(next_buffer, ' ', w.ws_row * w.ws_col);
  }

  // DRAW STUFF HERE

  for(int i = 0; i < w.ws_row*w.ws_col; i++) {
    if(last_buffer[i] != next_buffer[i]) {
      printf("\x1B[%d;%dH%c", (i / w.ws_col) + 1, (i % w.ws_col) + 1, next_buffer[i]);
      last_buffer[i] = next_buffer[i];
    }
  }
  fflush(stdout);
}