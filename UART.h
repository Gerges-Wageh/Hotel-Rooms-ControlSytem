
#ifndef __UART_H__
#define	__UART_H__

//Initialize UART0 
void UART_Init(void);

//Recieve char from UART0
unsigned char UART_ReadChar(void);

//Recieve string via UART0
unsigned char* UART_ReadString(void);

#endif 
