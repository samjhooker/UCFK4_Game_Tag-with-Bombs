#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "pio.h"
#include "../fonts/font5x7_1.h"


#define PACER_RATE 500
#define MESSAGE_RATE 10
#define BOMB_TIME 12 //seconds
#define BOMB_FLASH_TIME 0.2 //seconds

#define LED_PIO PIO_DEFINE (PORT_C, 2)

typedef struct {
    int x; 
    int y;
    int hasBomb;
    int prevX;
    int prevY;
} Character;




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


// define some basic game boards
char* gameBoard[] ={"eeeeeeeee",
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

char* matrix[] ={"eeeeeeeee",
				  "e0000000e",
				  "e0001000e",
				  "e0011100e",
				  "e0111110e",
				  "e0000000e",
				  //screen two starts here
				  "e0000000e",
				  "e0001000e",
				  "e0011100e",
				  "e0111110e",
				  "e0000000e"
				  "eeeeeeeee"};

/** Define PIO pins driving LED matrix rows.  */

static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO, 
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};


/** Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};


static const uint8_t bitmap[] =
{
    //stringToInt(matrix[5]), stringToInt(matrix[6]), stringToInt(matrix[7]), stringToInt(matrix[8]), stringToInt(matrix[9])
};



void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}


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

void placeBomb(Character player, int transmit){
	if (player.hasBomb == 1){
		matrix[player.prevX][player.prevY] = 'x';
		player.hasBomb = 0;
		if (transmit == 1){
			ir_uart_putc('b');
		}
		
	}
	
}

void goNorth(Character player, int transmit){
	if (matrix[player.x][player.y + 1] != 'e'){
				
		player.prevX = player1.x;
		player.prevY = player1.y;
				
		matrix[player.x][player.y] = '0';
		player.y++;
		matrix[player.x][player.y] = '1';
		if (transmit == 1){
			ir_uart_putc('n');
		}
	}
}
void goSouth(Character player, int transmit){
	if (matrix[player.x][player.y - 1] != 'e'){
				
		player.prevX = player1.x;
		player.prevY = player1.y;
				
		matrix[player.x][player.y] = '0';
		player.y--;
		matrix[player.x][player.y] = '1';
		if (transmit == 1){
			ir_uart_putc('s');
		}
	}
}
void goEast(Character player, int transmit){
	if (matrix[player.x++][player.y] != 'e'){
				
		player.prevX = player1.x;
		player.prevY = player1.y;
				
		matrix[player.x][player.y] = '0';
		player.x++;
		matrix[player.x][player.y] = '1';
		if (transmit == 1){
			ir_uart_putc('e');
		}
	}
}
void goWest(Character player, int transmit){
	if (matrix[player.x--][player.y] != 'e'){
				
		player.prevX = player1.x;
		player.prevY = player1.y;
				
		matrix[player.x][player.y] = '0';
		player.x--;
		matrix[player.x][player.y] = '1';
		if (transmit == 1){
			ir_uart_putc('w');
		}
	}
}

void playGame (Character player, character otherPlayer)
{
	
	int counter = 0;
	int flashCounter = 0;
	int flash = 1;
	
	Character player1; 
	player1.x = 8; 
	player1.y = 4;
	player1.hasBomb = 0;

	Character player2; 
	player2.x = 3; 
	player2.y = 4;
	player2.hasBomb = 0;
	
	int prevX;
	int prevY;
	
	uint8_t current_column = 0;



	int screenIndex;
	int minScreenIndex;
	int maxScreenIndex;

	if (player.x == 8){
		screenIndex = 6;
		minScreenIndex = 6;
		maxScreenIndex = 10;
	}else{
		screenIndex = 0;
		minScreenIndex = 0;
		maxScreenIndex = 5;
	}
	
    
    
    while (1)
    {
		counter ++;
		if (counter > (PACER_RATE * BOMB_TIME)){
			player.hasBomb = 1;
			counter = 0;
		}
		flashCounter++;
		if (flashCounter > (PACER_RATE * BOMB_FLASH_TIME)){
			flash = 1-flash;
			flashCounter = 0;
		}
		
		display_column (stringToInt(matrix[screenIndex], flash), current_column);
    
        current_column++;
        screenIndex++;
    
        if (current_column > (LEDMAT_COLS_NUM - 1))
        {
            current_column = 0;
        }  
        if (screenIndex > maxScreenIndex)
        {
            screenIndex = minScreenIndex;
        }  
		
		//pio_output_high (LED_PIO);
		
		
        pacer_wait ();
        //tinygl_update ();
        navswitch_update ();
        
        
        if (navswitch_push_event_p (NAVSWITCH_NORTH))
        	goNorth(player, 1);

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
			goSouth(player, 1);
		
		if (navswitch_push_event_p (NAVSWITCH_EAST))
			goEast(player, 1);

        if (navswitch_push_event_p (NAVSWITCH_WEST))
        	goWest(player, 1);
		
		
		if (player.hasBomb == 1){
			pio_output_high (LED_PIO);
		}else{
			pio_output_low (LED_PIO);
		}
		
      
        if (navswitch_push_event_p (NAVSWITCH_PUSH))
			placeBomb(player, 1);
           
        //revieve???
        if(ir_uart_read_ready_p ()){
			
			char character = ir_uart_getc ();
			switch(character){
    			case 'n':
       				goNorth(otherPlayer, 0);
       				break; /* optional */
    			case 's'  :
      				goSouth(otherPlayer, 0);
       				break; /* optional */
    			case 'e'  :
      				goEast(otherPlayer, 0);
       				break; /* optional */
       			case 'w'  :
      				goWest(otherPlayer, 0);
       				break; /* optional */
       			case 'b'  :
      				placeBomb(otherPlayer, 0);
       				break; /* optional */
       			default:
       				break;
			}	
		}
           
           
        
        //display_character (character);
        
    }
}








int main(void){


	system_init ();
    //tinygl_init (PACER_RATE);
    //tinygl_font_set (&font5x7_1);
   // tinygl_text_speed_set (MESSAGE_RATE);
    navswitch_init ();

    /* TODO: Initialise IR driver.  */
	ir_uart_init();

    pacer_init (PACER_RATE);
    pio_config_set (LED_PIO, PIO_OUTPUT_LOW);
    
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

    int playerNumber;

    uint8_t current_column = 0;



    while(1){


    	pacer_wait();



    	display_column (stringToInt(matrix[current_column], 1), current_column);

    	if (current_column > (LEDMAT_COLS_NUM - 1))//resets column counter if necessary
    	{
            current_column = 0;
        }  



    	if (navswitch_push_event_p (NAVSWITCH_PUSH)){ //if buttom pressed, transmit to start.
			ir_uart_putc (a);
			playerNumber = 1;
			break;
		}
           
        //revieve???
        if(ir_uart_read_ready_p ()){ //if there is something to retrieve
			playerNumber = 2;
			break;
		}



    }


    matrix = gameBoard; //assigns the gameboard to be the main one

    if(playerNumber ==1 ){
    	player1Run();
    }else{
    	player2Run();
    }


}
