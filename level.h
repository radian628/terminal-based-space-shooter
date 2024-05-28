#pragma once

// statics_map key:
// 0 = space
// 1 = wall
// 2 = laser

/*   statics_map key:
* 0  = space
* 1  = wall
* 2  = laser wall
* 3  = open laser gate
* 4  = horizontal breakable wall
* 5  = close laser gate
* 6  = vertical breakable wall
* 7  = left shooting enemy
* 8  = floating mine
* 9  = right shooting enemy
* 10 = coin
*/

struct level {
    int width;
    int height;
    char *statics_map;
};
typedef struct level level;

level* parseFile(char *filename);