#include "UART.h"
#include "tm4c123gh6pm.h"
#include <stdlib.h> 

//Initialize UART0 
void UART_Init(void){
			unsigned char delay;
			SYSCTL_RCGCUART_R |= 0x01;
			SYSCTL_RCGCGPIO_R |= 0x01;
			delay = SYSCTL_RCGCGPIO_R;  //dummy var to make sure that clock enabled successfully
			UART0_CTL_R &= ~0x01;
			//Baud rate = 9600 bits/sec .. 8-bits word .. no parity .. one stop bit .. FIFO enabled .. 80MHz sysclck
			UART0_IBRD_R = 520;   //int (80*1000000/(16*9600)) = int (520.833)
			UART0_FBRD_R = 53;    // (0.833*64 + 0.5)
			UART0_LCRH_R = 0x0070;
			UART0_CTL_R |= 0x301;   //enable uart,Tx,Rx
			GPIO_PORTA_AFSEL_R |= 0x03; //turn on alternate function mode to portA
			GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00) | (0x10 | 0x01); //assign PA0 to be Rx, PA1 to be Tx
			GPIO_PORTA_DEN_R |= 0x03; // enable digital mode to PA0, PA1
			GPIO_PORTA_AMSEL_R &= ~0x03; //disbale analog mode to PA0, PA1
}


//Recieve char from UART0
unsigned char UART_ReadChar(void){
		while((UART0_FR_R&0x10) != 0){};
		 return (unsigned char)(UART0_DR_R&0xFF);
}



//Recieve string via UART0
unsigned char* UART_ReadString(void)
{
	unsigned char index = 1;
	unsigned char* Str;
	Str = (unsigned char*)malloc(index*sizeof(unsigned char));
	while(UART_ReadChar()!='\0')
	{
		Str[index-1] = UART_ReadChar();
		index++;
	  Str = (unsigned char*)realloc(Str,index*sizeof(unsigned char));
	}
	return Str;
}
