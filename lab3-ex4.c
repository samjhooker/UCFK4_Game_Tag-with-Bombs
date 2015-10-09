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


void goNorth(Character *player, int transmit){

  if (matrix[player->x][player->y + 1] != 'e'){

    player->prevX = player->x;
    player->prevY = player->y;

    matrix[player->x][player->y] = '0';
    player->y++;
    matrix[player->x][player->y] = '1';

    if (transmit == 1){
      ir_uart_putc ('n');
    }

  }

}
void goSouth(Character *player, int transmit){

  if (matrix[player->x][player->y - 1] != 'e'){

    player->prevX = player->x;
    player->prevY = player->y;

    matrix[player->x][player->y] = '0';
    player->y--;
    matrix[player->x][player->y] = '1';

    if (transmit == 1){
      ir_uart_putc ('s');
    }

  }

}
void goEast(Character *player, int transmit){

  if (matrix[player->x+1][player->y] != 'e'){

    player->prevX = player->x;
    player->prevY = player->y;

    matrix[player->x][player->y] = '0';
    player->x++;
    matrix[player->x][player->y] = '1';

    if (transmit == 1){
      ir_uart_putc ('e');
    }

  }

}
void goWest(Character *player, int transmit){

  if (matrix[player->x-1][player->y] != 'e'){

    player->prevX = player->x;
    player->prevY = player->y;

    matrix[player->x][player->y] = '0';
    player->x--;
    matrix[player->x][player->y] = '1';

    if (transmit == 1){
      ir_uart_putc ('w');
    }

  }

}

int main (void)
{

	int counter = 0;
	int flashCounter = 0;
	int flash = 1;

	Character player1;
	player1.x = 8;
	player1.y = 4;
  player1.prevX = 8;
	player1.prevY = 4;
	player1.hasBomb = 0;

  Character player2;
  player2.x = 3;
  player2.y = 4;
  player2.prevX = 3;
  player2.prevY = 4;
  player2.hasBomb = 0;

	//int prevX;
	//int prevY;

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

    while (1)
    {
		counter ++;
		if (counter > (PACER_RATE * BOMB_TIME)){
			player1.hasBomb = 1;
			counter = 0;
		}
		flashCounter++;
		if (flashCounter > (PACER_RATE * BOMB_FLASH_TIME)){
			flash = 1-flash;
			flashCounter = 0;
		}

		display_column (stringToInt(matrix[screen_index], flash), current_column);

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

			goNorth(&player1, 1);
      /*if (matrix[player1.x][player1.y + 1] != 'e'){

        player1.prevX = player1.x;
        player1.prevY = player1.y;

        matrix[player1.x][player1.y] = '0';
        player1.y++;
        matrix[player1.x][player1.y] = '1';
        ir_uart_putc ('n');
      }*/
		}

    if (navswitch_push_event_p (NAVSWITCH_SOUTH)){
      	goSouth(&player1, 1);
			/*if (matrix[player1.x][player1.y - 1] != 'e'){

				player1.prevX = player1.x;
				player1.prevY = player1.y;

				matrix[player1.x][player1.y] = '0';
				player1.y--;
				matrix[player1.x][player1.y] = '1';
        ir_uart_putc ('s');
			}*/
		}

		if (navswitch_push_event_p (NAVSWITCH_EAST)){
      goEast(&player1, 1);
		/*	if (matrix[player1.x+1][player1.y] != 'e'){

				player1.prevX = player1.x;
				player1.prevY = player1.y;

				matrix[player1.x][player1.y] = '0';
				player1.x++;
				matrix[player1.x][player1.y] = '1';
        ir_uart_putc ('e');
			}*/
		}

        if (navswitch_push_event_p (NAVSWITCH_WEST)){
          goWest(&player1, 1);
			/*if (matrix[player1.x-1][player1.y] != 'e'){

				player1.prevX = player1.x;
				player1.prevY = player1.y;

				matrix[player1.x][player1.y] = '0';
				player1.x--;
				matrix[player1.x][player1.y] = '1';
        ir_uart_putc ('w');
			}*/
		}


		if (player1.hasBomb == 1){
			//pio_output_high (LED_PIO);
		}else{
		//	pio_output_low (LED_PIO);
		}



        /* TODO: Transmit the character over IR on a NAVSWITCH_PUSH
           event.  */
      if (navswitch_push_event_p (NAVSWITCH_PUSH)){

		      if (player1.hasBomb == 1){
				        matrix[player1.prevX][player1.prevY] = 'x';
			        }
			    player1.hasBomb = 0;
		}

        //revieve???
   if(ir_uart_read_ready_p ()){

			char character = ir_uart_getc ();
      switch(character){
    			case 'n':
       				//goNorth(otherPlayer, 0);
              pio_output_high (LED_PIO);
              player2.prevX = player2.x;
              player2.prevY = player2.y;

              matrix[player2.x][player2.y] = '0';
              player2.y--;
              matrix[player2.x][player2.y] = '1';
              break;
    			case 's'  :
            pio_output_low (LED_PIO);
      				//goSouth(otherPlayer, 0);
              player2.prevX = player2.x;
              player2.prevY = player2.y;

              matrix[player2.x][player2.y] = '0';
              player2.y++;
              matrix[player2.x][player2.y] = '1';
       				break;
    			case 'e'  :
      				//goEast(otherPlayer, 0);
              player2.prevX = player2.x;
              player2.prevY = player2.y;

              matrix[player2.x][player2.y] = '0';
              player2.x--;
              matrix[player2.x][player2.y] = '1';
       				break;
       			case 'w'  :
      				//goWest(otherPlayer, 0);
              player2.prevX = player2.x;
              player2.prevY = player2.y;

              matrix[player2.x][player2.y] = '0';
              player2.x++;
              matrix[player2.x][player2.y] = '1';
       				break;
       		//	case 'b'  :
      				//placeBomb(otherPlayer, 0);
       			//	break;
       			default:
       				break;
			}
		}




    }


    return 0;
}
