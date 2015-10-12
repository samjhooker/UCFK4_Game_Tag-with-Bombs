/*
Header file defining the actions within matrix.c
*/

#include "system.h"
#ifndef MATRIX_H
#define MATRIX_H



char* matrix[];

/*
Duuude function totally resets the game matrix when a new game is started
*/
void resetMatrix(void);


/*
draws frames for an explosion animation
x: the x location of where to start the explosion
y: the y location of where to start the explosion
frame: the frame (1-3) of the animation to be applied
*/
void showExplosion(int x, int y, int frame);


#endif
