#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct level {
    int width;
    int height;
    char *statics_bitmap;
};


char* parseFile(char *filename) {
    struct level *lvl = malloc(sizeof(struct level));
    FILE *f = fopen(filename, "r");
    int lines = 0;
    if (f == NULL) {
        printf("Error opening file %s\n", filename);
        return NULL;
    }
    // Count number of lines
    while(getline(NULL, NULL, f) != -1) {
        lines++;
    }
    // Reset file pointer
    f = fopen(filename, "r");
    size_t i = 0;
    char *buffer = malloc(1000);
    getline(&buffer, &i, f);

    char* cmd = strtok(buffer, " ");
    int width = 0;
    while (cmd != NULL) {
        if(strcmp(cmd, "width") == 0) {
            width = atoi(strtok(NULL, " "));
            lines--;
        } else if(strcmp(cmd, "level") == 0) {
            lines--;
            break;
        } // Add other supported parameters here
        getline(&buffer, &i, f);
        cmd = strtok(buffer, " ");
    }

    free(buffer);
    buffer = malloc(width);
    lvl->statics_bitmap = malloc(lines * width);
    // Level parsing:
    memset(buffer, ' ', width);
    for(int i = 0; i < lines; i++) {
        size_t j = 0;
        getline(&buffer, &j, f);
        for(int k = 0; k < j; k++) {
            lvl->statics_bitmap[i * width + k] = 0;
            switch (buffer[k]) {
                case '#':
                    lvl->statics_bitmap[i * width + k] = 1;
                    break;
                case '=':
                    lvl->statics_bitmap[i * width + k] = 2;
                    break;
                case '[':
                    lvl->statics_bitmap[i * width + k] = 3;
                    break;
                case '-':
                    lvl->statics_bitmap[i * width + k] = 4;
                    break;
                case ']':
                    lvl->statics_bitmap[i * width + k] = 5;
                    break;
                case '|':
                    lvl->statics_bitmap[i * width + k] = 6;
                    break;
                case '>':
                    lvl->statics_bitmap[i * width + k] = 7;
                    break;
                case '%':
                    lvl->statics_bitmap[i * width + k] = 8;
                    break;
                case '<':
                    lvl->statics_bitmap[i * width + k] = 9;
                    break;
                case '0':
                    lvl->statics_bitmap[i * width + k] = 10;
                    break;
                default:
                    break;
            }
        }
        buffer[j-1] = '\0';
    }
    fclose(f);
}