#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct level {
    int width;
    int height;
    char *statics_bitmap;
};


struct level* parseFile(char *filename) {
    struct level *lvl = malloc(sizeof(struct level));
    FILE *f = fopen(filename, "r");
    lvl->height = 0;
    if (f == NULL) {
        printf("Error opening file %s\n", filename);
        return NULL;
    }
    // Count number of lines
    int ch=0;
    while((ch = fgetc(f)) != EOF) {
        if(ch == '\n')
            lvl->height++;
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
    lvl->width = 0;
    while (cmd != NULL) {
        if(strcmp(cmd, "width") == 0) {
            lvl->width = atoi(strtok(NULL, " "));
        } else if(strcmp(cmd, "level") == 0) {
            break;
        } // Add other supported parameters here
        else if(strcmp(cmd, "\n") == 0 || strcmp(cmd, "\r\n") == 0) {
            lvl->height--;
        }
        else {
            printf("Unknown command: %s\n", cmd);
            return NULL;
        }
        memset(buffer, ' ', buffer_size);
        line_len = getline(&buffer, &buffer_size, f);
        buffer[line_len-1] = '\0';
        if(buffer[line_len-2] == '\r') {
            buffer[line_len-2] = '\0';
        }
        cmd = strtok(buffer, " ");
    }

    // It breaks when I take out this debug statement. I'm going to sleep.
    printf("Width: %d, Height: %d\n", lvl->width, lvl->height);
    char* temp = realloc(buffer, lvl->width);
    if(temp == NULL) {
        printf("Failed to realloc\n");
        return NULL;
    } else {
        buffer = temp;
    }
    lvl->statics_bitmap = malloc(lvl->height * lvl->width);
    // Level parsing:
    for(int i = 0; i < lvl->height; i++) {
        size_t j = 0;
        memset(buffer, ' ', lvl->width);
        memset(lvl->statics_bitmap + i * lvl->width, 0, lvl->width);
        getline(&buffer, &j, f);
        for(int k = 0; k < j; k++) {
            switch (buffer[k]) {
                case '#':
                    lvl->statics_bitmap[i * lvl->width + k] = 1;
                    break;
                case '=':
                    lvl->statics_bitmap[i * lvl->width + k] = 2;
                    break;
                case '[':
                    lvl->statics_bitmap[i * lvl->width + k] = 3;
                    break;
                case '-':
                    lvl->statics_bitmap[i * lvl->width + k] = 4;
                    break;
                case ']':
                    lvl->statics_bitmap[i * lvl->width + k] = 5;
                    break;
                case '|':
                    lvl->statics_bitmap[i * lvl->width + k] = 6;
                    break;
                case '>':
                    lvl->statics_bitmap[i * lvl->width + k] = 7;
                    break;
                case '%':
                    lvl->statics_bitmap[i * lvl->width + k] = 8;
                    break;
                case '<':
                    lvl->statics_bitmap[i * lvl->width + k] = 9;
                    break;
                case '0':
                    lvl->statics_bitmap[i * lvl->width + k] = 10;
                    break;
                default:
                    lvl->statics_bitmap[i * lvl->width + k] = 0;
                    break;
            }
        }
    }
    fclose(f);
    return lvl;
}