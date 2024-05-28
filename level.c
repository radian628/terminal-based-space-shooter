#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "level.h"
#include "game.h"
#include "dynarray.h"


void parseFile(char *filename, game *g) {
    g->level = malloc(sizeof(struct level));
    FILE *f = fopen(filename, "r");
    g->level->height = 0;
    if (f == NULL) {
        printf("Error opening file %s\n", filename);
        g->level = NULL;
        return;
    }
    // Count number of lines
    int ch=0;
    while((ch = fgetc(f)) != EOF) {
        if(ch == '\n')
            g->level->height++;
    }
    fclose(f);
    // Reset file pointer
    f = fopen(filename, "r");
    size_t buffer_size = 1000;
    size_t line_len = 0;
    char *buffer = malloc(buffer_size);
    memset(buffer, ' ', buffer_size);
    line_len = getline(&buffer, &buffer_size, f);
    buffer[line_len-1] = '\0';
    if(buffer[line_len-2] == '\r') {
        buffer[line_len-2] = '\0';
    }

    char* cmd = strtok(buffer, " ");
    g->level->width = 0;
    while (cmd != NULL) {
        if(strcmp(cmd, "width") == 0) {
            g->level->width = atoi(strtok(NULL, " "));
            g->level_progress = -(double)g->level->height;
        } else if(strcmp(cmd, "level") == 0) {
            break;
        } // Add other supported parameters here
        else if(strcmp(cmd, "\n") == 0 || strcmp(cmd, "\r\n") == 0) {
            g->level->height--;
        }
        else {
            printf("Unknown command: %s\n", cmd);
            g->level = NULL;
            return;
        }
        memset(buffer, ' ', buffer_size);
        line_len = getline(&buffer, &buffer_size, f);
        buffer[line_len-1] = '\0';
        if(buffer[line_len-2] == '\r') {
            buffer[line_len-2] = '\0';
        }
        cmd = strtok(buffer, " ");
    }
    //Trailing Thingamabob
    g->level->height--;

    // It breaks when I take out this debug statement. I'm going to sleep.
    printf("Width: %d, Height: %d\n", g->level->width, g->level->height);
    char* temp = realloc(buffer, g->level->width);
    if(temp == NULL) {
        printf("Failed to realloc\n");
        g->level = NULL;
        return;
    } else {
        buffer = temp;
    }
    g->level->statics_map = malloc(g->level->height * g->level->width);
    // Level parsing:
    for(int i = 0; i < g->level->height; i++) {
        size_t j = 0;
        memset(buffer, ' ', g->level->width);
        memset(g->level->statics_map + i * g->level->width, 0, g->level->width);
        getline(&buffer, &j, f);
        for(int k = 0; k < j; k++) {
            switch (buffer[k]) {
                case '#':
                    g->level->statics_map[i * g->level->width + k] = 1;
                    break;
                case '=':
                    g->level->statics_map[i * g->level->width + k] = 2;
                    break;
                case '[':
                    g->level->statics_map[i * g->level->width + k] = 3;
                    break;
                case '-':
                    g->level->statics_map[i * g->level->width + k] = 4;
                    break;
                case ']':
                    g->level->statics_map[i * g->level->width + k] = 5;
                    break;
                case '|':
                    g->level->statics_map[i * g->level->width + k] = 6;
                    break;
                case '>':
                    g->level->statics_map[i * g->level->width + k] = 7;
                    break;
                case '%':
                    g->level->statics_map[i * g->level->width + k] = 8;
                    break;
                case '<':
                    g->level->statics_map[i * g->level->width + k] = 9;
                    break;
                case '0':
                    g->level->statics_map[i * g->level->width + k] = 10;
                    break;
                // Non static entities:
                case 'F': {
                    enemy e;
                    e.pos.x = k;
                    e.pos.y = i;
                    e.type = FOUR_DIRECTIONS;
                    e.time_until_fire = 0.3;
                    e.hitpoints = 3;
                    e.damage_animation_frames_remaining = 0;
                    da_append(g->enemies, &e);
                    break;
                }
                case '~': {
                    enemy e;
                    e.pos.x = k;
                    e.pos.y = i;
                    e.type = FOLLOWER;
                    e.time_until_fire = 0;
                    e.hitpoints = 1;
                    e.damage_animation_frames_remaining = 0;
                    da_append(g->enemies, &e);
                    break;
                }
                case '?': {
                    enemy e;
                    e.pos.x = k;
                    e.pos.y = i;
                    e.type = DOWN_SHOOTER;
                    e.time_until_fire = 0.3;
                    e.hitpoints = 3;
                    e.damage_animation_frames_remaining = 0;
                    e.dir = LEFT;
                    da_append(g->enemies, &e);
                }
                default:
                    g->level->statics_map[i * g->level->width + k] = 0;
                    break;
            }
        }
    }
    for(int i = 0; i < g->level->height; i++) {
        for(int j = 0; j < g->level->width; j++) {
            printf("%d ", g->level->statics_map[i * g->level->width + j]);
        }
        printf("\n");
    }
    fclose(f);
}