/*
Matrix.c
program files to control the matrix
Written by Samuel Hooker && Ben Lilburne
*/

#include "system.h"


// the main gameboard used for the application
char* matrix[] ={"eeeeeeeee",
					"eaaaaaaae",
					"ebbbbbbbe",
					"eccc1ccce",
					"eddddddde",
					"efffffffe",
					//screen two starts here
					"eggggggge",
					"ehhhhhhhe",
					"e0001000e",
					"eiiiiiiie",
					"ejjjjjjje",
					"eeeeeeeee"};




/*
Duuude function totally resets the game matrix when a new game is started
*/
void resetMatrix(void){
	matrix[0] = "eeeeeeeee";
	matrix[1] = "eaaaaaaae";
	matrix[2] = "ebbbbbbbe";
	matrix[3] = "eccc1ccce";
	matrix[4] = "eddddddde";
	matrix[5] = "efffffffe";
	//screen two starts here
	matrix[6] = "eggggggge";
	matrix[7] = "ehhhhhhhe";
	matrix[8] = "e0001000e";
	matrix[9] = "eiiiiiiie";
	matrix[10] = "ejjjjjjje";
	matrix[11] = "eeeeeeeee";
	}


/*
draws frames for an explosion animation
x: the x location of where to start the explosion
y: the y location of where to start the explosion
frame: the frame (1-3) of the animation to be applied
*/
void showExplosion(int x, int y, int frame){
	if(frame == 1){
	matrix[x+1][y+1] = 'w'; //frame 1
	matrix[x-1][y-1] = 'w';
	matrix[x+1][y-1] = 'w';
	matrix[x-1][y+1] = 'w';
}else if (frame ==2){
	matrix[x+2][y+0] = 'w'; //frame 2
	matrix[x-2][y-0] = 'w';
	matrix[x+0][y-2] = 'w';
	matrix[x-0][y+2] = 'w';
}else{
	matrix[x+3][y+3] = 'w'; //frame 3
	matrix[x-3][y-3] = 'w';
	matrix[x+3][y-3] = 'w';
	matrix[x-3][y+3] = 'w';
}
}
