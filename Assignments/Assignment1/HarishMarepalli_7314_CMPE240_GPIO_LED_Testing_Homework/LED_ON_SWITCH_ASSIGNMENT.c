/*
===============================================================================
 Name        : LED_ON_SWITCH_ASSIGNMENT.c
 Author      : Harish Marepalli (016707314)
 Version     :
 Copyright   : $(copyright)
 Description : This program is used to turn the LED ON and OFF using a
 	 	 	   push button. If the push button is pressed, the LED
 	 	 	   turns ON and if it is released, the LED turns OFF. The
 	 	 	   input pin used is p2.13 (j2-27) and the output pin used
 	 	 	   is p0.21 (j2-23).
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

//Initialize the port and pin as outputs.
void GPIOinitOut(uint8_t portNum, uint32_t pinNum)
{
	if (portNum == 0)
	{
		LPC_GPIO0->FIODIR |= (1 << pinNum);
	}
	else if (portNum == 1)
	{
		LPC_GPIO1->FIODIR |= (1 << pinNum);
	}
	else if (portNum == 2)
	{
		LPC_GPIO2->FIODIR |= (1 << pinNum);
	}
	else
	{
		puts("Not a valid port!\n");
	}
}

void GPIOinitInput(uint8_t portNum, uint32_t pinNum)
{
	if (portNum == 0)
	{
		LPC_GPIO0->FIODIR |= (0 << pinNum);
	}
	else if (portNum == 1)
	{
		LPC_GPIO1->FIODIR |= (0 << pinNum);
	}
	else if (portNum == 2)
	{
		LPC_GPIO2->FIODIR |= (0 << pinNum);
	}
	else
	{
		puts("Not a valid port!\n");
	}
}

void setGPIO(uint8_t portNum, uint32_t pinNum)
{
	if (portNum == 0)
	{
		LPC_GPIO0->FIOSET = (1 << pinNum);
		printf("Pin 0.%d has been set.\n",pinNum);
	}
	//Can be used to set pins on other ports for future modification
	else
	{
		puts("Only port 0 is used, try again!\n");
	}
}

//Deactivate the pin
void clearGPIO(uint8_t portNum, uint32_t pinNum)
{
	if (portNum == 0)
	{
		LPC_GPIO0->FIOCLR = (1 << pinNum);
		printf("Pin 0.%d has been cleared.\n", pinNum);
	}
	//Can be used to clear pins on other ports for future modification
	else
	{
		puts("Only port 0 is used, try again!\n");
	}
}

int main(void)
{

    // Force the counter to be placed into memory
    //volatile static int i = 0 ;
	uint32_t switchStatus;
	uint32_t switchPinNumber = 13;

	//LPC_PINCON->PINMODE4 = 0x08000000;
	//Set pin 0.21 as output
	GPIOinitOut(0,21);
	//Set pin 2.13 as input
	GPIOinitInput(2,13);

    while(1)
    {
    	//Get the status of the switch
    	switchStatus = (LPC_GPIO2->FIOPIN >> switchPinNumber) & 0x01;

		if (switchStatus == 1)
		{
			//Activate pin 0.21
			setGPIO(0,21);
		}
		else
		{
			//Clear pin 0.21
			clearGPIO(0, 21);
		}
    }
    return 0 ;
}
