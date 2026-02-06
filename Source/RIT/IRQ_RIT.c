/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../GLCD/GLCD.h"
#include "../adc/adc.h"
#include "../music/music.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern int collision(int matrice[20][10], int move);
extern int matrice[20][10];
extern int update_matrix;
extern int hard_drop;
extern int gameState;
extern int currentScore;
extern int topScore;
extern int updateScore;
extern int clearedLines;
extern unsigned short AD_current;

extern int slowDownCounter;
extern int slowDownActive;
extern void activate_slow_down(void);

uint32_t speed;

#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1

NOTE song[] =
{
    /* Jingle Bells – verse */
    {e4, time_croma}, {e4, time_croma}, {e4, time_semicroma*2},
    {e4, time_croma}, {e4, time_croma}, {e4, time_semicroma*2},
    {e4, time_croma}, {g4, time_croma}, {c4, time_croma},
    {d4, time_croma}, {e4, time_semicroma*2},
    {f4, time_croma}, {f4, time_croma}, {f4, time_croma},
    {f4, time_croma}, {e4, time_croma},
    {e4, time_croma}, {e4, time_croma},
    {e4, time_croma}, {d4, time_croma}, {d4, time_croma},
    {e4, time_croma}, {d4, time_semicroma*2},
    {g4, time_semicroma*2},
    {pause, time_croma},

    /* chorus */
    {e4, time_croma}, {e4, time_croma}, {e4, time_semicroma*2},
    {e4, time_croma}, {e4, time_croma}, {e4, time_semicroma*2},
    {e4, time_croma}, {g4, time_croma}, {c4, time_croma},
    {d4, time_croma}, {e4, time_semicroma*2},
    {f4, time_croma}, {f4, time_croma}, {f4, time_croma},
    {f4, time_croma}, {e4, time_croma},
    {e4, time_croma}, {e4, time_croma},
    {g4, time_croma}, {g4, time_croma},
    {f4, time_croma}, {d4, time_croma},
    {c4, time_semicroma*4},
    {pause, time_semicroma*2}
};


void RIT_IRQHandler (void)
{
	/*
	static int currentNote = 0;
	static int ticks = 0;
	if(!isNotePlaying())
	{
		++ticks;
		if(ticks == UPTICKS)
		{
			ticks = 0;
			playNote(song[currentNote++]);
		}
	}
	
	if(currentNote == (sizeof(song) / sizeof(song[0])))
	{
		// choose one of the following
		//disable_RIT();		// stop the music
		currentNote = 0;	// restart the music
	}
	*/
	if (slowDownActive == 1) {
		if (slowDownCounter > 0) {
			slowDownCounter--;
			
			speed = 0xda120;
		} else {
			slowDownActive = 0;
		}
	}else {
		ADC_start_conversion();
		speed = 0xda120 / (1 + (AD_current / 820));
	}
	
	static int down1 = 0;
	static int down2 = 0;
	
	//down1++;
	//down2++;
	
	if ((LPC_GPIO2->FIOPIN & (1<<11)) == 0) {
		down1++;
		//LED_On(0);
		if (down1 == 1) {
			if (gameState == 0) {
		enable_timer(0);
		//enable_RIT();
		gameState = 1;
	}
	else if (gameState == 1) {
		disable_timer(0);
		//disable_RIT();
		gameState = 0;
	}
	else if (gameState == 2) {
		
		if (currentScore > topScore) {
			topScore = currentScore;
			updateScore = 1;
		}
		disable_timer(0);
		//disable_RIT();
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
		//enable_RIT();
		enable_timer(0);
		currentScore = 0;
		clearedLines = 0;
		updateScore = 1;
		update_matrix = 1;
		gameState = 1;
			}
		}
	}
	else {
		if (down1 > 0) {
			down1 = 0;
			LPC_SC->EXTINT = (1<<1);
			NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
			
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	//if (gameState == 1) {
		if ((LPC_GPIO2->FIOPIN & (1<<12)) == 0) {
		down2++;
		if (down2 == 1 && gameState == 1) {
			hard_drop = 1;
		}
	}
	else {
		down2 = 0;
		LPC_SC->EXTINT = (1<<2);
		NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
		
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
	}
	if (gameState == 1) {
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0) {
		// joystick left pressed
		int moveLeft = collision(matrice, 1);
		if (moveLeft == 1) {
			int x,y;
			for (x=0; x<=19; x++) {
				for (y=0; y<=9; y++) {
					if (matrice[x][y] != 0 && matrice[x][y] < 10) {
						matrice[x][y-1] = matrice[x][y];
						matrice[x][y] = 0;
						drawBlock(y-1, x, matrice[x][y-1]);
						drawBlock(y, x, 0);
					}
				}
			}
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<28)) == 0) {
		// joystick right pressed
		int moveRight = collision(matrice, 2);
		if (moveRight == 1) {
			int x,y;
			for (x=0; x<=19; x++) {
				for (y=9; y>=0; y--) {
					if (matrice[x][y] != 0 && matrice[x][y] < 10) {
						matrice[x][y+1] = matrice[x][y];
						matrice[x][y] = 0;
						drawBlock(y+1, x, matrice[x][y+1]);
						drawBlock(y, x, 0);
					}
				}
			}
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<29)) == 0) {
		// joystick up pressed
		int x, y, i, j;
		int first = 0;
		int startX=0;
		int startY=0;
		int minX = 20;
		int minY = 10;
		for (x=0; x<20; x++) {
			for (y=0; y<10; y++) {
				if (matrice[x][y] < 10 && matrice[x][y] != 0) {
					if (x < minX) {
						minX = x;
					}
					if (y < minY) {
						minY = y;
					}
					first = 1;
					
			}
		}
	}
		if (first == 1) {
			startX = minX;
			startY = minY;
			if (startY > 6) {
						startY =6;
					}
					if (startX > 16) {
						startX = 16;
					}
					if (startX < 0) {
						startX = 0;
					}
					if (startY < 0) {
						startY = 0;
					}
		}
		int temp[4][4] = {0};
		i = 0;
		
		for (x=startX; x<startX+4; x++) {
			int j = 0;
			for (y=startY; y<startY+4; y++) {
				if (matrice[x][y] < 10 && matrice[x][y] != 0) {
					temp[i][j] = matrice[x][y];
				}
				else {
					temp[i][j] = 0;
				}
				j++;
			}
			i++;
		}
		// rotation
		
		for (i=0; i<4; i++) {
			for (j=i+1; j<4; j++) {
				int tmp = temp[i][j];
				temp[i][j] = temp[j][i];
				temp[j][i] = tmp;
			}
		}
		for (i=0; i<4; i++) {
			for (j=0; j<2; j++) {
				int tmp = temp[i][j];
				temp[i][j] = temp[i][4-1-j];
				temp[i][4-1-j] = tmp;
			}
		}
		
		i = 0;
		int coll = 0;
		for (x=startX; x<startX+4; x++) {
			j =0;
			for (y=startY; y<startY+4; y++) {
				if (temp[i][j] != 0) {
						if (x >= 20 || y < 0 || y >= 10) {
							coll = 1;
						}
						else if (matrice[x][y] >= 10) {
							coll = 1;
						}
					}
				j++;
			}
			i++;
		}
		
		
		if (coll == 0) {
			for (x=startX; x<startX+4; x++) {
				for (y=startY; y<startY+4; y++) {
					if (x < 20 && y < 10 && matrice[x][y] < 10) {
						matrice[x][y] = 0;
						drawBlock(y, x, 0);
					}
				}
			}
			i=0;
			for (x=startX; x<startX+4; x++) {
				j=0;
				for (y=startY; y<startY+4; y++) {
					if (temp[i][j] != 0) {
						matrice[x][y] = temp[i][j];
						drawBlock(y, x, temp[i][j]);
					}
					j++;
				}
				i++;
			}
		}
	}	
	
	else {	
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0) {
		// joystick down pressed
			uint32_t soft_drop_speed = speed / 2;
			if (LPC_TIM0->MR0 != soft_drop_speed) {
				LPC_TIM0->MR0 = soft_drop_speed;
				LPC_TIM0->TC = 0;
			}
			
		} else {
			if (LPC_TIM0->MR0 != speed) {
				LPC_TIM0->MR0 = speed;
				LPC_TIM0->TC = 0;
			}
		}
	}
	}
	
	
	LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
