#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "pio.h"
#include "../fonts/font5x7_1.h"


#define PACER_RATE 500
#define MESSAGE_RATE 10

typedef struct {
    int x; 
    int y;
} Character;


int stringToInt(char* str){
	str++;
    int result = 0;
    int i;
    for (i = 0; i < 7; i++) 
    {
		result = result << 1;
		result += ((*str != '0') && (*str != 'a') && (*str != 'b')&& (*str != 'c')&& (*str != 'd')&& (*str != 'f')&& (*str != 'g')&& (*str != 'h')&& (*str != 'i')&& (*str != 'j'));
		str++;
	}
	 
	return result;
	
}


/*int stringToBinary(char *s) {
  return (int) strtol(s, NULL, 2);
}*/

char* matrix[] ={"eeeeeeeee",
				  "eaaaaaaae",
				  "ebbbbbbbe",
				  "eccccccce",
				  "eddddddde",
				  "efffffffe",
				  //screen two starts here
				  "eggggggge",
				  "ehhhhhhhe",
				  "e0001000e",
				  "eiiiiiiie",
				  "ejjjjjjje",
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


int main (void)
{
	Character player1; 
	player1.x = 8; 
	player1.y = 4;
	
	uint8_t current_column = 0;
	int screen_index = 6;
	
    system_init ();
    //tinygl_init (PACER_RATE);
    //tinygl_font_set (&font5x7_1);
   // tinygl_text_speed_set (MESSAGE_RATE);
    navswitch_init ();

    /* TODO: Initialise IR driver.  */
	ir_uart_init();

    pacer_init (PACER_RATE);
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
    
    while (1)
    {
		
		display_column (stringToInt(matrix[screen_index]), current_column);
    
        current_column++;
        screen_index++;
    
        if (current_column > (LEDMAT_COLS_NUM - 1))
        {
            current_column = 0;
        }  
        if (screen_index > (10))
        {
            screen_index = 6;
        }  
		
		
		
        pacer_wait ();
        //tinygl_update ();
        navswitch_update ();
        
        
        if (navswitch_push_event_p (NAVSWITCH_NORTH)){
			
			/*pio_output_high(LEDMAT_COL1_PIO);
			pio_output_high(LEDMAT_COL5_PIO);
			pio_output_high(LEDMAT_COL2_PIO);
			pio_output_high(LEDMAT_COL3_PIO);
			pio_output_high(LEDMAT_COL4_PIO);

    
			pio_output_high(LEDMAT_ROW1_PIO);
			pio_output_high(LEDMAT_ROW2_PIO);
			pio_output_high(LEDMAT_ROW3_PIO);
			pio_output_high(LEDMAT_ROW4_PIO);
			pio_output_high(LEDMAT_ROW5_PIO);
			pio_output_high(LEDMAT_ROW6_PIO);
			pio_output_high(LEDMAT_ROW7_PIO);*/
			
			if (matrix[player1.x][player1.y + 1] != 'e'){
				matrix[player1.x][player1.y] = '0';
				player1.y++;
				matrix[player1.x][player1.y] = '1';
			}
			
		}

        if (navswitch_push_event_p (NAVSWITCH_SOUTH)){
			if (matrix[player1.x][player1.y - 1] != 'e'){
				matrix[player1.x][player1.y] = '0';
				player1.y--;
				matrix[player1.x][player1.y] = '1';
			}
		}
		
		if (navswitch_push_event_p (NAVSWITCH_EAST)){	
			
			if (matrix[player1.x+1][player1.y] != 'e'){
				matrix[player1.x][player1.y] = '0';
				player1.x++;
				matrix[player1.x][player1.y] = '1';
			}
		}

        if (navswitch_push_event_p (NAVSWITCH_WEST)){
			if (matrix[player1.x-1][player1.y] != 'e'){
				matrix[player1.x][player1.y] = '0';
				player1.x--;
				matrix[player1.x][player1.y] = '1';
			}
		}

       
       
        /* TODO: Transmit the character over IR on a NAVSWITCH_PUSH
           event.  */
        if (navswitch_push_event_p (NAVSWITCH_PUSH)){
			//ir_uart_putc (character);
		}
           
        //revieve???
        if(ir_uart_read_ready_p ()){
			
			//character = ir_uart_getc ();
				
		}
           
           
        
        //display_character (character);
        
    }
    

    return 0;
}
