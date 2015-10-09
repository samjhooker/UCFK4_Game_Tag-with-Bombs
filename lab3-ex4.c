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

int win = 0;
int lose = 0;


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




void showExplosion(int x, int y, int frame){
  if(frame == 1){
  matrix[x+1][y+1] = 'w';
  matrix[x-1][y-1] = 'w';
  matrix[x+1][y-1] = 'w';
  matrix[x-1][y+1] = 'w';
}else if (frame ==2){
  matrix[x+2][y+0] = 'w';
  matrix[x-2][y-0] = 'w';
  matrix[x+0][y-2] = 'w';
  matrix[x-0][y+2] = 'w';
}else{
  matrix[x+3][y+3] = 'w';
  matrix[x-3][y-3] = 'w';
  matrix[x+3][y-3] = 'w';
  matrix[x-3][y+3] = 'w';
}
}


void checkForWinOrLose(int x, int y){

  if (matrix[x][y] == '1') { //hit other player
    win = 1;
    ir_uart_putc ('L');
  }else if (matrix[x][y] == 'x'){ //hit a bomb
    lose = 1;
    ir_uart_putc ('W');
  }

}

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


void placeBomb(Character *player, int transmit){

  matrix[player->prevX][player->prevY] = 'x';
  if (transmit == 1){
    ir_uart_putc ('b');
  }

}



void showWinScreen(){
  while(1){
    display_column (stringToInt("e0000000e", 1), 0);
    display_column (stringToInt("e0101010e", 1), 1);
    display_column (stringToInt("e0101010e", 1), 2);
    display_column (stringToInt("e0010100e", 1), 3);
    display_column (stringToInt("e0000000e", 1), 4);
  }

}

void showLoseScreen(){
  while(1){
    display_column (stringToInt("e0010000e", 1), 0);
    display_column (stringToInt("e0010000e", 1), 1);
    display_column (stringToInt("e0010000e", 1), 2);
    display_column (stringToInt("e0011100e", 1), 3);
    display_column (stringToInt("e0000000e", 1), 4);
  }

}

int main (void)
{

	int counter = 0;
	int flashCounter = 0;
	int flash = 1;
  int explosionCounter = 0;

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


        if (win > 0){
          if (explosionCounter > (PACER_RATE * 0.15)){
            if (win == 4){
              break;
            }
            showExplosion(player2.x, player2.y, win);
            explosionCounter = 0;
            win++;
          }
          explosionCounter++;
        }

        if (lose > 0){
          if (explosionCounter > (PACER_RATE * 0.15)){
            if (lose == 4){
              break;
            }
            showExplosion(player1.x, player1.y, lose);
            explosionCounter = 0;
            lose++;
          }
          explosionCounter++;
        }



        pacer_wait ();
        //tinygl_update ();
        navswitch_update ();


    if (navswitch_push_event_p (NAVSWITCH_NORTH))
			goNorth(&player1, 1);

    if (navswitch_push_event_p (NAVSWITCH_SOUTH))
      goSouth(&player1, 1);

		if (navswitch_push_event_p (NAVSWITCH_EAST))
      goEast(&player1, 1);

    if (navswitch_push_event_p (NAVSWITCH_WEST))
      goWest(&player1, 1);


		if (player1.hasBomb == 1){
			pio_output_high (LED_PIO);
		}else{
			pio_output_low (LED_PIO);
		}

    if (navswitch_push_event_p (NAVSWITCH_PUSH)){

		  if (player1.hasBomb == 1){
          placeBomb(&player1, 1);
			}
			player1.hasBomb = 0;
		}

        //revieve???
   if(ir_uart_read_ready_p ()){

			char character = ir_uart_getc ();
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
              win = 1;
              break;
          case 'L' :
              lose = 1;
              break;
       		default:
       				break;
			}
		}




    }


    if (win > 0){
      showWinScreen();
    }else{
      showLoseScreen();
    }


    return 0;
}
