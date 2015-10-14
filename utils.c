/*
utils.c
program files to control independent algorithms within the application
Written by Samuel Hooker && Ben Lilburne
*/

#include "system.h"




/*
converts a given string of characters into an binary integer with 0 being zero and anything else being one
str: the string to be converted into an integer
bombsActive: toggles the flash, ie, the char 'x' to be counted as a bit or not
returns: integer representation of the given string
*/
int stringToInt(char* str, int bombsActive){
	str++;
		int result = 0;
		int i;
		for (i = 0; i < 7; i++)
		{
		result = result << 1;
		if (bombsActive == 0){
			result += ((*str != '0') && (*str != 'a') && (*str != 'b')&& (*str != 'c')&& (*str != 'd')&& (*str != 'f')&& (*str != 'g')&& (*str != 'h')&& (*str != 'i')&& (*str != 'j'));
		}else{
			result += ((*str != '0') && (*str != 'a') && (*str != 'b')&& (*str != 'c')&& (*str != 'd')&& (*str != 'f')&& (*str != 'g')&& (*str != 'h')&& (*str != 'i')&& (*str != 'j')&& (*str != 'x'));
		}
		str++;
	}

	return result;

}
