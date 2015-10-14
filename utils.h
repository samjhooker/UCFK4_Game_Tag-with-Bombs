/*
Header file defining the actions within utils.c
*/

#include "system.h"
//#ifndef UTILS_H
//#define UTILS_H



/*
converts a given string of characters into an binary integer with 0 being zero and anything else being one
str: the string to be converted into an integer
bombsActive: toggles the flash, ie, the char 'x' to be counted as a bit or not
returns: integer representation of the given string
*/
int stringToInt(char* str, int bombsActive);
