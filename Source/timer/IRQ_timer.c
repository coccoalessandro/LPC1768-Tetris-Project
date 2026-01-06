/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "led.h"
#include "../GLCD/GLCD.h"

extern void drawBlock(int x, int y, int number);
extern void drawMatrix(int matrice[20][10]);
extern int newMatrix(int matrice[20][10], int random);
extern int collision(int matrice[20][10], int move);
extern int gameOver(int matrice[20][10]);
extern int update_matrix;
extern int hard_drop;
extern int currentScore;
extern int updateScore;
extern int clearedLines;

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int matrice[20][10] = {0};
/*	
	{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0}	
};
*/
int i;
int generateNewBlock = 0;
int moveOn;
int stop;

void TIMER0_IRQHandler (void)
{
	if (generateNewBlock == 0) {
		int num = (rand() % 7);
		stop = newMatrix(matrice, num);
		}
		moveOn = collision(matrice, 0);
		if (moveOn == 1) {
			generateNewBlock = 1;
			int x;
			int y;
			for (x=19; x>=0; x--) {
				for (y=9; y>=0; y--) {
					if (matrice[x][y] != 0 && matrice[x][y] < 10) {
						matrice[x+1][y] = matrice[x][y];
						matrice[x][y] = 0;
						drawBlock(y, x+1, matrice[x+1][y]);
						drawBlock(y, x, matrice[x][y]);
					}
				}
			}
		} else {
			generateNewBlock = 0;
			currentScore += 10;
			updateScore = 1;
			int x;
			int y;
			for (x=19; x>=0; x--) {
				for (y=9; y>=0; y--) {
					if (matrice[x][y] != 0 && matrice[x][y] < 10) {
						matrice[x][y] = matrice[x][y] + 10;
					}
				}
			}
		}
		
		int numLines = 0;
		// resetta la linea se completa
			int i,j;
			for (i=19; i>=0; i--) {
				int count = 0;
				for (j=9; j>=0; j--) {
					if (matrice[i][j] > 10) {
						count++;
					}
				}
				if (count == 10) {
					numLines += 1;
					clearedLines += 1;
					currentScore += 100;
					updateScore = 1;
					int x1;
					int y1;
					for (x1=i; x1>=0; x1--) {
						for (y1=0; y1<=9; y1++) {
							if (x1!=0) {
								matrice[x1][y1] = matrice[x1-1][y1];
							}else {
								matrice[0][y1] = 0;
							}
						}
					}
					//drawMatrix(matrice);
					update_matrix = 1;
					i++;
				}
			}
			if (numLines == 4) {
				currentScore += 200;
				updateScore = 1;
			}
	
	/* alternatively to LED_On and LED_off try to use LED_Out */
	//LED_Out((1<<position)|(led_value & 0x3));							
	/* LED_Out is CRITICAL due to the shared led_value variable */
	/* LED_Out MUST NOT BE INTERRUPTED */
  LPC_TIM0->IR = 0x3F;		/* clear interrupt flag (clear all bits in IR) */
	/* an alternative instruction clearing only the bit set to 1 is LPC_TIM0->IR = LPC_TIM0->IR; */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 0x3F;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)
{
  LPC_TIM2->IR = 0x3F;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler (void)
{
  LPC_TIM3->IR = 0x3F;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
