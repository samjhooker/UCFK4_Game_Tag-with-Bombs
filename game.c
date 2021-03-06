/*
'Tag, with bombs'
ENCE260 Assignment;
Written by Samuel Hooker and Ben Lilburne
6/10/15
*/




#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "pio.h"
#include "../fonts/font5x7_1.h"
#include "matrix.h"
#include "utils.h"



#define PACER_RATE 500
#define MESSAGE_RATE 10
#define BOMB_TIME 6 //seconds
#define BOMB_FLASH_TIME 0.2 //seconds
#define LED_PIO PIO_DEFINE (PORT_C, 2)

static int win = 0;
static int lose = 0;


/*
Struct defining the stored properties for a single character instance
*/
typedef struct {
		int x;
		int y;
		int hasBomb;
		int prevX;
		int prevY;
} Character;




// list of all pins driving rows in the LED matrix
static const pio_t rows[] =
{
		LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
		LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
		LEDMAT_ROW7_PIO
};


// list of all pins driving columns in the LED matrix
static const pio_t cols[] =
{
		LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
		LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};



/*
displays a given integer, through binary representation, on a given column of the LED matrix
row_pattern: the 8 bit integer to be displayer
current_column: the column on ehich to display this integer
*/
static void display_column (uint8_t row_pattern, uint8_t current_column)

{
	int inCol;
	int current_row;

	pio_output_high(LEDMAT_ROW1_PIO);
	pio_output_high(LEDMAT_ROW2_PIO);
	pio_output_high(LEDMAT_ROW3_PIO);
	pio_output_high(LEDMAT_ROW4_PIO);
	pio_output_high(LEDMAT_ROW5_PIO);
	pio_output_high(LEDMAT_ROW6_PIO);
	pio_output_high(LEDMAT_ROW7_PIO);

	for (inCol = 0; inCol < LEDMAT_COLS_NUM; inCol++) {
		if (inCol == current_column) {
			pio_output_low(cols[inCol]);
		}
		else
		{
			pio_output_high(cols[inCol]);
		}
	}


	for (current_row = 0; current_row < LEDMAT_ROWS_NUM; current_row++)
	{
		if ((row_pattern >> current_row) & 1)
		{
			pio_output_low(rows[current_row]);
		}
		else
		{
			pio_output_high(rows[current_row]);
		}
	}

}



/*
checks for a win or a lose within the game for given points. checks for 'x', bombs, and for '1', other players
if found, the win/lose animation is triggered and the other player is notified through IR
x: the x position to check for a win/lose
y: the y position to check for a win/lose
*/
void checkForWinOrLose(int x, int y){
	if ((win == 0) && (lose ==0)){
	if (matrix[x][y] == '1') { //hit other player
		win = 1;
		ir_uart_putc ('L');
	}else if (matrix[x][y] == 'x'){ //hit a bomb
		lose = 1;
		ir_uart_putc ('W');
	}
}

}



/*
Moves a player in the matrix north by one position. transmits the location to the other MCU if required
player: pointer to the Character to move
transmit: boolean indicating whether the move will be transmitted to the other MCU via IR.
*/
void goNorth(Character *player, int transmit){

	if (matrix[player->x][player->y + 1] != 'e'){



		player->prevX = player->x;
		player->prevY = player->y;

		matrix[player->x][player->y] = '0';
		player->y++;
		checkForWinOrLose(player->x, player->y);
		matrix[player->x][player->y] = '1';


		if (transmit == 1){
			ir_uart_putc ('n');
		}

	}

}


/*
Moves a player in the matrix south by one position. transmits the location to the other MCU if required
player: pointer to the Character to move
transmit: boolean indicating whether the move will be transmitted to the other MCU via IR.
*/
void goSouth(Character *player, int transmit){

	if (matrix[player->x][player->y - 1] != 'e'){

		player->prevX = player->x;
		player->prevY = player->y;

		matrix[player->x][player->y] = '0';
		player->y--;
		checkForWinOrLose(player->x, player->y);
		matrix[player->x][player->y] = '1';


		if (transmit == 1){
			ir_uart_putc ('s');
		}

	}

}


/*
Moves a player in the matrix north by east position. transmits the location to the other MCU if required
player: pointer to the Character to move
transmit: boolean indicating whether the move will be transmitted to the other MCU via IR.
*/
void goEast(Character *player, int transmit){

	if (matrix[player->x+1][player->y] != 'e'){

		player->prevX = player->x;
		player->prevY = player->y;

		matrix[player->x][player->y] = '0';
		player->x++;
		checkForWinOrLose(player->x, player->y);
		matrix[player->x][player->y] = '1';


		if (transmit == 1){
			ir_uart_putc ('e');
		}

	}

}


/*
Moves a player in the matrix west by one position. transmits the location to the other MCU if required
player: pointer to the Character to move
transmit: boolean indicating whether the move will be transmitted to the other MCU via IR.
*/
void goWest(Character *player, int transmit){

	if (matrix[player->x-1][player->y] != 'e'){

		player->prevX = player->x;
		player->prevY = player->y;

		matrix[player->x][player->y] = '0';
		player->x--;
		checkForWinOrLose(player->x, player->y);
		matrix[player->x][player->y] = '1';


		if (transmit == 1){
			ir_uart_putc ('w');
		}

	}

}

/*
Places a bomb at the last know position of a character. transmits the location to the other MCU if required
player: pointer to the Character to place bomb
transmit: boolean indicating whether the bomb location will be transmitted to the other MCU via IR.
*/
void placeBomb(Character *player, int transmit){

	matrix[player->prevX][player->prevY] = 'x';
	if (transmit == 1){
		ir_uart_putc ('b');
	}

}


/*
shows the letter 'W' on the LED matrix. used for the winner
*/
void showWinScreen(void){


	char* win[] = {"e0000000e", "e0101010e", "e0101010e", "e0010100e", "e0000000e"};
	int screenIndex = 0;

	while(1){
		display_column (stringToInt(win[screenIndex], 1), screenIndex);
		pacer_wait();
		if (screenIndex > 4){
			screenIndex = 0;
		}else{
			screenIndex ++;
		}
	}

}

/*
shows the letter 'L' on the LED matrix, used for the loser
*/
void showLoseScreen(void){

	char* lose[] = {"e0010000e", "e0010000e", "e0010000e", "e0011100e", "e0000000e"};
	int screenIndex = 0;

	while(1){
		display_column (stringToInt(lose[screenIndex], 1), screenIndex);
		pacer_wait();
		if (screenIndex > 4){
			screenIndex = 0;
		}else{
			screenIndex ++;
		}
	}

}

/*
shows a start screen and waits for user input or IR signal to be recieved
*/
void showStartScreen(void){

	char* skull[] = {"e0011100e", "e0111110e", "e0101010e", "e0011100e", "e0011100e"};
	int screenIndex = 0;

	while(1){
		pacer_wait();

		if(ir_uart_read_ready_p ()){
			break;
		}
		navswitch_update();
		if (navswitch_push_event_p (NAVSWITCH_PUSH)){
			ir_uart_putc ('q');
			break;
		}

		display_column (stringToInt(skull[screenIndex], 1), screenIndex);

		if (screenIndex > 4){
			screenIndex = 0;
		}else{
			screenIndex ++;
		}


	}

}




static int counter = 0;
static int flashCounter = 0;
static int flash = 1;
static int explosionCounter = 0;

/*
Main function, runs everything
return: 0 as per c convention
*/
int main (void)
{
	//Brah this resets the matrix brah
	resetMatrix();
	//initiilize counters for lights
	counter = 0;
	flashCounter = 0;
	flash = 1;
	explosionCounter = 0;
	win = 0;
	lose = 0;


	//intiilizes chatacters
	Character player1;
	player1.x = 8;
	player1.y = 4;
	player1.prevX = 8;
	player1.prevY = 5;
	player1.hasBomb = 0;

	Character player2;
	player2.x = 3;
	player2.y = 4;
	player2.prevX = 3;
	player2.prevY = 5;
	player2.hasBomb = 0;


	//initilize counters for the columns and the shown matrix columns
	uint8_t current_column = 0;
	int screen_index = 6;

	//initilize system, navswitch and Infared
	system_init ();
	navswitch_init ();
	ir_uart_init();

	//init the pacer and the LED light
	pacer_init (PACER_RATE);
	pio_config_set (LED_PIO, PIO_OUTPUT_LOW);

	//init all rows and columns of the LED matrix
	pio_config_set(LEDMAT_COL1_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_COL5_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_COL2_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_COL3_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_COL4_PIO, PIO_OUTPUT_HIGH);

	pio_config_set(LEDMAT_ROW1_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_ROW2_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_ROW3_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_ROW4_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_ROW5_PIO, PIO_OUTPUT_HIGH);
	pio_config_set(LEDMAT_ROW6_PIO, PIO_OUTPUT_HIGH);
  pio_config_set(LEDMAT_ROW7_PIO, PIO_OUTPUT_HIGH);

  //shows a start screen before game is initilized
  showStartScreen();

  //loop while game is in operation
	while (1)
	{


			//loop and counter to wait for the time for a bomb to spawn	int counter = 0;

			counter ++;
			if (counter > (PACER_RATE * BOMB_TIME)){
				player1.hasBomb = 1;
				counter = 0;
			}

			//loop and counter to wait for time for bombs to flash
			flashCounter++;
			if (flashCounter > (PACER_RATE * BOMB_FLASH_TIME)){
				flash = 1-flash;
				flashCounter = 0;
			}

			//displays a line in the game board matrix in a line in the LED matrix
			display_column (stringToInt(matrix[screen_index], flash), current_column);

				//incremnets these lines
				current_column++;
				screen_index++;

				//and resets there if necessary
				if (current_column > (LEDMAT_COLS_NUM - 1))
				{
						current_column = 0;
				}
				if (screen_index > (10))
				{
						screen_index = 6;
				}


				//if win, play win animation
				if (win > 0){
					if (explosionCounter > (PACER_RATE * 0.15)){
						if (win == 4){ //three frames of animation
							break; //breaks out of the game loop and ends the game
						}
						showExplosion(player2.x, player2.y, win);
						explosionCounter = 0;
						win++;
					}
					explosionCounter++;
				}

				//if lose, play lose animation
				if (lose > 0){
					if (explosionCounter > (PACER_RATE * 0.15)){
						if (lose == 4){
							break; //breaks out of the game loop and ends the game
						}
						showExplosion(player1.x, player1.y, lose);
						explosionCounter = 0;
						lose++;
					}
					explosionCounter++;
				}


				//waits for time to continue
				pacer_wait ();


			//updates navswitch
				navswitch_update ();



		//listeners for the navswitches.
		//when pressed, calls the directional functions with a transmission of IR to the other MCU
		if (navswitch_push_event_p (NAVSWITCH_NORTH))
			goNorth(&player1, 1);

		if (navswitch_push_event_p (NAVSWITCH_SOUTH))
			goSouth(&player1, 1);

		if (navswitch_push_event_p (NAVSWITCH_EAST))
			goEast(&player1, 1);

		if (navswitch_push_event_p (NAVSWITCH_WEST))
			goWest(&player1, 1);


		//triggers the LED to indicate whether a bomb availiable
		if (player1.hasBomb == 1){
			pio_output_high (LED_PIO);
		}else{
			pio_output_low (LED_PIO);
		}

		//Listener for the navswitch
		//when pressed, place a bomb is one is possible
		if (navswitch_push_event_p (NAVSWITCH_PUSH)){

			if (player1.hasBomb == 1){
					placeBomb(&player1, 1);
			}
			player1.hasBomb = 0; //resets bomb
		}


	//checks whether there is something to recieve
	if(ir_uart_read_ready_p () && win == 0 && lose == 0){
			//recieves character
			char character = ir_uart_getc ();

			//switch statement to assign action based upon char recieved.
			//calls the required directional functions and does not transmit to the other MCU.

			//These recieved characters are used to update the local version of player2's location based ...
			//upon the actions of the other MCU.
			switch(character){
					case 'n':
							goSouth(&player2, 0);
							break;
					case 's'  :
							goNorth(&player2, 0);
							break;
					case 'e'  :
							goWest(&player2, 0);
							break;
					case 'w'  :
							goEast(&player2, 0);
							break;
					case 'b'  :
							placeBomb(&player2, 0);
							break;
					case 'W' :
							win = 1; //win
							break;
					case 'L' :
							lose = 1; //lose
							break;
					default:
							break;
			}
		}




		}


		//after game is finished (loop breaks after explosion), a win or lose screen is displayed
		if (win > 0){
			showWinScreen();
		}else{
			showLoseScreen();
		}


		return 0;
}
