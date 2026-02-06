/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: 
 *		to control led11 and led 10 through EINT buttons (similarly to project 03_)
 *		to control leds9 to led4 by the timer handler (1 second - circular cycling)
 * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/

                  
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "led.h"
#include "button.h"
#include "timer.h"
#include "systick.h"
#include "RIT.h"
#include "../GLCD/GLCD.h"
#include "../joystick/joystick.h"
#include "../tetris.c"
#include "adc/adc.h"

extern int matrice[20][10];
extern int generateNewBlock;
//int joystick_down = 0; 
int hard_drop = 0;
int gameState = 3;
int update_matrix = 1;
int currentScore;
int topScore = 0;
int clearedLines = 0;
int updateScore = 0;

int slowDownCounter = 0;
int slowDownActive = 0;

void drawBlock(int x, int y, int number) {
	uint16_t color;
	if (number == 0) {
		color = Black;
	}
	else if (number == 1) {
		color = Cyan;
	}
	else if (number == 3) {
		color = Yellow;
	}
	else if (number == 4) {
		color = Magenta;
	}
	else if (number == 5) {
		color = Blue;
	}
	else if (number == 6) {
		color = Orange;
	}
	else if (number == 7) {
		color = Green;
	}
	else if (number == 8) {
		color = Red;
	}
	else if (number == 99) {
		color = White;
	}
	
	x = x*14;
	y = y*14;
	int i, j;
	for (i=0; i<14; i++) {
		for (j=0; j<14; j++) {
			LCD_SetPoint(x+i, y+j, color);
		}
	}
}

void generatePowerUp(int matrice[20][10]) {
	int row = (rand() % 20);
	int col = (rand() % 10);
	
	int count = 0;
	int i,j;
	
	for (i=0; i<20; i++) {
		for (j=0; j<10; j++) {
			if (matrice[i][j] > 10) {
				count++;
			}
		}
	}
	
	int randomNum = (rand() % count);
	count = 0;
	for (i=0; i<20; i++) {
		for (j=0; j<10; j++) {
			if (matrice[i][j] > 10) {
				if (count == randomNum) {
					matrice[i][j] = 99;
					count++;
				}else {
					count++;
				}
			}
		}
	}
	update_matrix = 1;
}

int countLines(int matrice[20][10]) {
	int x, y;
	int count = 0;
	for (x=11; x<20; x++) {
		int block = 0;
		for (y=0; y<10; y++) {
			if (matrice[x][y] > 10) {
				block = 1;
			}
		}
		if (block == 1) {
			count++;
		}
	}
	return count;
}

void generateMalus(int matrice[20][10]) {
	int i,j;
	for (j=0; j<10; j++) {
		if (matrice[0][j] != 0) {
			gameState = 2;
			return;
		}
	}
	for (i=0; i<20; i++) {
		for (j=0; j<10; j++) {
			if (i != 19) {
				matrice[i][j] = matrice[i+1][j];
			} else {
				matrice[i][j] = 0;
			}
		}
	}
	
	int randomNum = (rand() % 10);
	int count = 0;
	while (count < 7) {
		while (matrice[19][randomNum] != 0) {
			randomNum = (rand() % 10);
		}
		matrice[19][randomNum] = 11;
		count++;
	}
	
	
}

void activate_slow_down(void) {
	if (LPC_TIM0->MR0 < 0xda120) {
		slowDownActive = 1;
		slowDownCounter = 45;
		
		LPC_TIM0->MR0 = 0xDA120;
		LPC_TIM0->TC = 0;
	}
}

int newMatrix(int matrice[20][10], int random) {
	// 0 vuoto e 2 bloccato
	if (random == 0) { // I block -- 1
		if (matrice[0][0] == 0 && matrice[0][1] == 0 && matrice[0][2] == 0 && matrice[0][3] == 0) {
			matrice[0][0] = 1;
			drawBlock(0,0,1);
			matrice[0][1] = 1;
			drawBlock(1,0,1);
			matrice[0][2] = 1;
			drawBlock(2,0,1);
			matrice[0][3] = 1;
			drawBlock(3,0,1);
			return 1;
		}else {
			gameState = 2;
			return 0;
		}
	} else if (random == 1) { // O block -- 3
		if (matrice[0][0] == 0 && matrice[0][1] == 0 && matrice[1][0] == 0 && matrice[1][1] == 0) {
			matrice[0][0] = 3;
			drawBlock(0,0,3);
			matrice[0][1] = 3;
			drawBlock(1,0,3);
			matrice[1][0] = 3;
			drawBlock(0,1,3);
			matrice[1][1] = 3;
			drawBlock(1,1,3);
			return 1;
		} else {
			gameState = 2;
			return 0;
		}
	} else if (random == 2) { // T block -- 4
		if (matrice[0][0] == 0 && matrice[0][1] == 0 && matrice[0][2] == 0 && matrice[1][1] == 0) {
			matrice[0][0] = 4;
			drawBlock(0,0,4);
			matrice[0][1] = 4;
			drawBlock(1,0,4);
			matrice[0][2] = 4;
			drawBlock(2,0,4);
			matrice[1][1] = 4;
			drawBlock(1,1,4);
			return 1;
		} else {
			gameState = 2;
			return 0;
		}
	} else if (random == 3) { // J block -- 5
		if (matrice[0][1] == 0 && matrice[1][1] == 0 && matrice[2][1] == 0 && matrice[2][0] == 0) {
			matrice[0][1] = 5;
			drawBlock(1,0,5);
			matrice[1][1] = 5;
			drawBlock(1,1,5);
			matrice[2][1] = 5;
			drawBlock(1,2,5);
			matrice[2][0] = 5;
			drawBlock(0,2,5);
			return 1;
		} else {
			gameState = 2;
			return 0;
		}
	} else if (random == 4) { // L block -- 6
		if (matrice[0][0] == 0 && matrice[1][0] == 0 && matrice[2][0] == 0 && matrice[2][1] == 0) {
			matrice[0][0] = 6;
			drawBlock(0,0,6);
			matrice[1][0] = 6;
			drawBlock(0,1,6);
			matrice[2][0] = 6;
			drawBlock(0,2,6);
			matrice[2][1] = 6;
			drawBlock(1,2,6);
			return 1;
		} else {
			gameState = 2;
			return 0;
		}
	} else if (random == 5) { // S block -- 7
		if (matrice[0][1] == 0 && matrice[0][2] == 0 && matrice[1][0] == 0 && matrice[1][1] == 0) {
			matrice[0][1] = 7;
			drawBlock(1,0,7);
			matrice[0][2] = 7;
			drawBlock(2,0,7);
			matrice[1][0] = 7;
			drawBlock(0,1,7);
			matrice[1][1] = 7;
			drawBlock(1,1,7);
		return 1;
		} else {
			gameState = 2;
			return 0;
		}
	} else { // Z block -- 8
		if (matrice[0][0] == 0 && matrice[0][1] == 0 && matrice[1][1] == 0 && matrice[1][2] == 0) {
			matrice[0][0] = 8;
			drawBlock(0,0,8);
			matrice[0][1] = 8;
			drawBlock(1,0,8);
			matrice[1][1] = 8;
			drawBlock(1,1,8);
			matrice[1][2] = 8;
			drawBlock(2,1,8);
		return 1;
		} else {
			gameState = 2;
			return 0;
		}	
	}
	//return 0;
}


void drawMatrix(int matrice[20][10]) {
	int i,j;
	for (i=0; i<=19; i++) {
		for (j=0; j<=9; j++) {
			if (matrice[i][j] != 0 && matrice[i][j] < 10) {
				drawBlock(j,i,matrice[i][j]);
			}
			else if (matrice[i][j] > 10 && matrice[i][j] != 99) {
				drawBlock(j,i,matrice[i][j]-10);
			}
			else if (matrice[i][j] == 99) {
				drawBlock(j, i, 99);
			}
			else{
				drawBlock(j,i,0);
			}
		}
	}
}

int collision(int matrice[20][10], int move) {
	// move --> 0:down; 1:left; 2:right; 3:up, 4:falls instantly
	if (move == 0) {
		// move down
		int i,j;
		for (i=19; i>=0;i--) {
			for (j=9; j>=0; j--) {
				if (matrice[i][j] != 0 && matrice[i][j] < 10) {
					if (i != 19) {
						if (matrice[i+1][j] > 10) {
							return 0;
						}
					} else {
							return 0;
					}
				}
			}
		}
		return 1;
		}
	else if (move == 1) {
		// move left
		int i,j;
		for (i=0; i<=19; i++) {
			for (j=0; j<=9; j++) {
				if (matrice[i][j] != 0 && matrice[i][j] < 10) {
					if (j != 0) {
						if (matrice[i][j-1] > 10) {
							return 0;
						}
					} else {
						return 0;
					}
				}
			}
		}
		return 1;
	}
	else if (move == 2) {
		// move right
		int i,j;
		for (i=0; i<=19; i++) {
			for (j=9; j>=0; j--) {
				if (matrice[i][j] != 0 && matrice[i][j] < 10) {
					if (j!=9){
						if (matrice[i][j+1] > 10){
							return 0;
						}
					}else {
						return 0;
					}
				}
			}
		}
		return 1;
	}
	else {
		return 0;
	}
}


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();
	LCD_Initialization();
	LCD_Clear(Black);
	
	// 10x20 blocks
	// tetris --> 140x320
	// score --> 100x320
	
	//LCD_DrawLine(140,0,140,MAX_Y, White);
	//LCD_DrawLine(0,0,0,MAX_Y, White);
	//LCD_DrawLine(0,0,140,0, White);
	//LCD_DrawLine(MAX_X,0,MAX_X,MAX_Y, White);
	
	/* choose one of the following timers.
	They have the same behaviour: leds are turned off and on at regular intervals */
	
	/* 1) System Tick Timer */
	// systick_init(0xE4E1C0);							/* T = 0.15 sec. */
																					/* F = 100 MHz; K is expressed on 24 bits */
	
	/* 2) Standard Timers 	*/
	// init_timer(0,0x017D7840);					/* TIMER0 initialization, interrupts at 1 sec - 1 sec - 2 sec */
																				/* K = T * Fr = [s]*[Hz] = [s]*[1/s]	  */
																				/* T = K / Fr = 0x017D7840 / 25MHz = 25000000 / 25 * 10^6 Hz  = 1 sec	(one second) */
	GUI_Text(150, 50, (uint8_t *) "TOP", White, Black);
	GUI_Text(150, 100, (uint8_t *) "SCORE", White, Black);
	GUI_Text(150, 150, (uint8_t *) "LINES", White, Black);
	
	LCD_DrawLine(141, 0, 141, 281, White);
	LCD_DrawLine(0, 281, 141, 281, White);
	/*
	char buffer[10];
	sprintf(buffer, "%d", currentScore);
	GUI_Text(150, 120, (uint8_t *) buffer, White, Black);
	*/
	
	char bufferTop[10];
	sprintf(bufferTop, "%d", topScore);
	GUI_Text(150, 70, (uint8_t *) bufferTop, White, Black);
	/* Other notable values: */
	init_timer(0,0xda120);					// 5 min 43sec
	// init_timer(0,0x00B41780);					// 1 min 0x2CB41780
	// init_timer(0,0x0EE6B280);					// 10 sec
	//enable_timer(0);
	
	/* 3) Repetitive Interrupt Timer (RIT) */
	//init_RIT(0x350000);									/* RIT initialization 1 sec --> as in the systick timer, here the clock frequency is set to 100 MHz */
	init_RIT(0x1312D0);
	//enable_RIT();
	
	//LED_On(7);
	
	ADC_init();
	
	while(1) {
		if (hard_drop == 1) {
			currentScore += 10;
			updateScore = 1;
			while(collision(matrice, 0) == 1) {
		int x;
		int y;
			for (x=19; x>=0; x--) {
				for (y=9; y>=0; y--) {
					if (matrice[x][y] != 0 && matrice[x][y] < 10) {
						matrice[x+1][y] = matrice[x][y];
						matrice[x][y] = 0;
						//drawBlock(y,x,Black);
						}
					}
				}
	}
		
		int x;
		int y;
			for (x=19; x>=0; x--) {
				for (y=9; y>=0; y--) {
					if (matrice[x][y] != 0 && matrice[x][y] < 10) {
						matrice[x][y] = matrice[x][y] + 10;
						drawBlock(y, x, matrice[x][y]);
				}
			}
	}
			hard_drop = 0;
			update_matrix=1;
		}
		if (update_matrix == 1) {
			generateNewBlock = 1;
			drawMatrix(matrice);
			generateNewBlock = 0;
			if (gameState == 3) {
				GUI_Text(20, 50, (uint8_t *) "PRESS KEY1", White, Black);
				GUI_Text(20, 70, (uint8_t *) "TO START", White, Black);
				GUI_Text(20, 90, (uint8_t *) "A NEW GAME", White, Black);
			}
			update_matrix = 0;
		}
		if (updateScore == 1) {
			char buffer[10];
			sprintf(buffer, "%d", currentScore);
			GUI_Text(150, 120, (uint8_t *) "      ", Black, Black);
			GUI_Text(150, 120, (uint8_t *) buffer, White, Black);
			
			char bufferTop[10];
			sprintf(bufferTop, "%d", topScore);
			GUI_Text(150, 70, (uint8_t *) bufferTop, White, Black);
			
			char bufferLines[10];
			sprintf(bufferLines, "%d", clearedLines);
			GUI_Text(150, 170, (uint8_t *) "      ", Black, Black);
			GUI_Text(150, 170, (uint8_t *) bufferLines, White, Black);
			
			updateScore = 0;
		}
	}

	LPC_SC->PCON |= 0x1;		/* power-down mode */								
	LPC_SC->PCON &= ~(0x2); 
	SCB->SCR |= 0x2;				/* set SLEEPONEXIT */
	
	__ASM("wfi");
}
