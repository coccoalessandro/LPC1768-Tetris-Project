#include "button.h"
#include "LPC17xx.h"


#include "../led/led.h"
#include "../GLCD/GLCD.h"

extern int matrice[20][10];
extern int collision(int matrice[20][10], int move);
extern int update_matrix;
extern int hard_drop;
extern int gameState;
extern int currentScore;
extern int topScore;
extern int updateScore;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	enable_RIT();										/* enable RIT to count 50ms				 */
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts	--> the button does not generate interrupts anymore */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection 	 */
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt				 */
	/*
  if (gameState == 0) {
		enable_timer(0);
		enable_RIT();
		gameState = 1;
	}
	else if (gameState == 1) {
		disable_timer(0);
		disable_RIT();
		gameState = 0;
	}
	else if (gameState == 2) {
		
		if (currentScore > topScore) {
			topScore = currentScore;
			updateScore = 1;
		}
		disable_timer(0);
		disable_RIT();
		gameState = 3;
		int i;
		int j;
		for (i=0;i<20; i++) {
			for (j=0; j<10; j++) {
				matrice[i][j] = 0;
			}
		}
		update_matrix=1;
	}
	else if (gameState == 3) {
		enable_RIT();
		enable_timer(0);
		currentScore = 0;
		updateScore = 1;
		gameState = 1;
	}
	*/
	//LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	// tetromino falls instantly
	
	//hard_drop = 1;
	
  enable_RIT();										/* enable RIT to count 50ms				 */
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts	--> the button does not generate interrupts anymore */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection 	 */
	
	LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt				 */
}


