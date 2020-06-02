#include "UART.h"
#include "KeyPad.h"
#include "tm4c123gh6pm.h"
#include "stdint.h"

#define Number_Of_Rooms 3  // I assumed that the hotel consists of 3 rooms and represented them with the 3 LEDS in PORTF
// Room 1 --> Red LED
// Room 2 --> Blue LED
// Room 3 --> Green LED
void SystemInit(){}
typedef enum Status {
	Free = 0,
	Occupied = 1,
	Room_Cleaning = 2
}Status;

struct Room{
	unsigned char* Password;
	unsigned char Room_Status;
};
void LockTheDoor(unsigned char room_num){
	switch(room_num){
		case 1:
			GPIO_PORTF_DATA_R |= 0x02; // Turn on Red LED indecate to that the door of room 1 is open
		case 2:
			GPIO_PORTF_DATA_R |= 0x04; // Turn on Blue LED indecate to that the door of room 2 is open
		case 3:
			GPIO_PORTF_DATA_R |= 0x08; // Turn on Green LED to indicate that the door of room 3 is open
	}
}
void UnLockTheDoor(unsigned char room_num){
		switch(room_num){
		case 1:
			GPIO_PORTF_DATA_R &= ~0x02; //Turn off Red LED to indecate that the door of room 1 is closed
		case 2:
			GPIO_PORTF_DATA_R &= ~0x04; //Turn off Blue LED to indecate that the door of room 2 is closed
		case 3:
			GPIO_PORTF_DATA_R &= ~0x08; //Turn off Green LED to indicate that the door of room 3 is closed
	}
}
unsigned char IsCorrectPassword(unsigned char* room_password, unsigned char* client_password){
	unsigned char i;
	for(i=0;i<4;i++){
		if(client_password[i] != room_password[i]) return 0;
	}
	return 1;
}

void PORTF_Init(){
	unsigned char delay;
	SYSCTL_RCGCGPIO_R |=0x20; //enable clock for port F
	delay = SYSCTL_RCGCGPIO_R;  // dummy variable to make sure that the clock is enabled successfully 
	//**********unlock LOCK_R must done first to manage to initialize the rest registers*******//
	GPIO_PORTF_LOCK_R =0x4C4F434B;   //to unclock lock reg write the magic number from datasheet in it
	GPIO_PORTF_CR_R =0x1F;           // to allow changes it all pins in port F
	GPIO_PORTF_AFSEL_R =0;		//turn the alternative function mode to 0 as we deal with GPIO
	GPIO_PORTF_PCTL_R =0; 		//used with AFSEL if you deal with GPIO turn it off
	GPIO_PORTF_AMSEL_R =0;    // turn off Analog Mode selection cause we deal with digital signals
	GPIO_PORTF_DEN_R =0x1F;   //enable digital mode for port F
	GPIO_PORTF_DIR_R =0x0E;		//assign PF3-1 to be output (RGB LEDS)	
	GPIO_PORTF_DATA_R = 0x00;
}

	
int main(){
	unsigned char request,room_num,client_password[4];
	UART_Init();
	KeyPad_Init();
	PORTF_Init();
	while(1){
		struct Room Hotel_Rooms[Number_Of_Rooms];
		unsigned char i;
		//Set up process
		for(i=0; i<Number_Of_Rooms;i++){
				Hotel_Rooms[i].Password = (unsigned char*)"***";
				Hotel_Rooms[i].Room_Status = Free;
		}
  SetUp:
		room_num = UART_ReadChar();
		request = UART_ReadChar();
		switch(request){
			case 0:   //Check-Out (change status to be Free '0' and Lock the door)
				Hotel_Rooms[room_num].Room_Status = Free; //change status to be Free '0'
				LockTheDoor(room_num); //Lock the door it can`t be open even with password
				
			case 1:  // Check-In (set a password, chage status to be occupied)
				Hotel_Rooms[room_num].Room_Status = Occupied;   //change status to be Occupied '1'
			  Hotel_Rooms[room_num].Password = UART_ReadString(); //set a password via UART0
			
			case 2: //Room-Cleaning (change status to be room_cleaning '2' and unlock the door without any password)
				Hotel_Rooms[room_num].Room_Status = Room_Cleaning;
				UnLockTheDoor(room_num);
		}
		while(1){
			unsigned char i,n;
		EnterPassword:
			 n =(unsigned char)Keypad_GetPressedKey(); //the client enters his room number
			for(i=0;i<4;i++){
			client_password[i] = (unsigned char)Keypad_GetPressedKey;  //the client entes his room password
			}
			if(IsCorrectPassword(Hotel_Rooms[n].Password, client_password) == 0)
				goto EnterPassword;   //ask the client to enter the password again in case it`s invalid
			UnLockTheDoor(n); // in case the password is correct, open the room door
			if(UART_ReadChar() == '#')goto SetUp; //If the receptionist wants to do new operation
		}
	}
}
