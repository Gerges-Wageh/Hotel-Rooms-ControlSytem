#include "KeyPad.h"
#include "tm4c123gh6pm.h"

static unsigned char KeyPad_4x3_Map(unsigned char row,unsigned char col){
   unsigned char keyboard[4][3]={
                                {'0','1','2'},
                                {'3','4','5'},
                                {'6','7','8'},
                                {'9','*','='}};
   return keyboard[row][col];
}

//Initializing portD that will be connected to KeyPad
 void KeyPad_Init(void)
 {
volatile unsigned long delay;
  SYSCTL_RCGCGPIO_R |= 0x0000008;      //enable clock for Port D
  delay = SYSCTL_RCGCGPIO_R;           //delay to make sure that the sysclock enabled successfully for Port D
  GPIO_PORTD_LOCK_R = 0x4C4F434B;   //unlock GPIO Port D
  GPIO_PORTD_CR_R = 0xFF;           //allow changes to PD7-0
  GPIO_PORTD_AMSEL_R = 0x00;        //disable analog mode for Port D
	GPIO_PORTD_DEN_R = 0xFF;          //enable digital mode for Port D
  GPIO_PORTD_PCTL_R = 0x00000000;   //use Port D as GPIO
  GPIO_PORTD_DIR_R = 0x00;          //PD7-0 input
  GPIO_PORTD_AFSEL_R = 0x00;        //disable alternate function for Port D
	GPIO_PORTD_PUR_R = 0xFF;					//enable Pull-Up resistor for PD7-0
 }
 
 
 // Get the pressed key on the KeyPad
 unsigned char Keypad_GetPressedKey(void)
{
	unsigned char row,col;
	while(1)
	{
		for(col=0;col<3;col++)
		{
			//each time only one of the column pins will be output and the rest will be input pins include the row pins 
			GPIO_PORTD_DIR_R = 0x10<<col; 
			
			//clear the output pin column in this trace and enable the internal  pull up resistors for the rows pins
			GPIO_PORTD_DATA_R = (~(0x10<<col));

			for(row=0;row<4;row++) 
			{
				if(!(GPIO_PORTD_DATA_R & (1<<row))) // if the key is press in this row 
			
						return KeyPad_4x3_Map(row,col); //return the corresponding char
			}
		}
	}	
}

