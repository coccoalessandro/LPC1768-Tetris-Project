
#include "button.h"
#include "LPC17xx.h"

/**
 * @brief  Function that initializes Buttons
 */
void BUTTON_init(void) {
	
  LPC_PINCON->PINSEL4    |= (1 << 20);		 /* External interrupt 0 pin selection */
  LPC_PINCON->PINSEL4    &= ~(1 << 21);

  LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
  LPC_PINCON->PINSEL4    &= ~(1 << 23);
  
  LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
  LPC_PINCON->PINSEL4    &= ~(1 << 25);

  //LPC_SC->EXTMODE = 0x7;                   /* Level-sensitivity is selected for external interrupts */
  //LPC_SC->EXTPOLAR = 0;                    /* External interrupts are low-active or falling-edge sensitive */

  NVIC_EnableIRQ(EINT2_IRQn);              /* enable irq in nvic                 */
	//NVIC_SetPriority(EINT2_IRQn, 1);				 /* priority, the lower the better     */
  NVIC_EnableIRQ(EINT1_IRQn);              /* enable irq in nvic                 */
	//NVIC_SetPriority(EINT1_IRQn, 2);				 
  NVIC_EnableIRQ(EINT0_IRQn);              /* enable irq in nvic                 */
	//NVIC_SetPriority(EINT0_IRQn, 3);				 /* decreasing priority	from EINT2->0	 */
}
