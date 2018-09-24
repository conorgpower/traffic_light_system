/*
 * main.c
 *
 *  Created on: 2018 Apr 10 10:24:28
 *  Author: Conor
 */




#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */
 
enum state { RED, GREEN, AMBER, REDPED, REDPEDFLASH };
enum state current = RED;

bool hasWrapped;
bool buttonPressed;
uint32_t cylesSinceLastPedSeq = 1;
 
void ButtonHandler(void)
{
	buttonPressed = true;
}

void ThirtyTimerHandler(void)
{
	TIMER_Stop(&TMRThirty);
	TIMER_Clear(&TMRThirty);
	TIMER_ClearEvent(&TMRThirty);
	current = GREEN;
	TIMER_Start(&TMRForty);
}

void TenTimerHandler(void)
{
	TIMER_Stop(&TMRTen);
	TIMER_Clear(&TMRTen);
	TIMER_ClearEvent(&TMRTen);
	if(current == AMBER)
	{
		if(cylesSinceLastPedSeq > 3 && buttonPressed == true)
		{
			cylesSinceLastPedSeq = 0;
			current = REDPED;
			TIMER_Start(&TMRForty);
		}
		else
		{
			current = RED;
			cylesSinceLastPedSeq++;
			TIMER_Start(&TMRThirty);
		}
	}
	
	if(current == REDPEDFLASH)
	{
		current = GREEN;
		TIMER_Start(&TMRForty);
	}
}

void FortyTimerHandler(void)
{
	TIMER_Stop(&TMRForty);
	TIMER_Clear(&TMRForty);
	TIMER_ClearEvent(&TMRForty);
	if(current == GREEN) 
	{
		current = AMBER;
		TIMER_Start(&TMRTen);
	}	
	
	if(current == REDPED)
	{
		if(hasWrapped == true)
		{
			current = REDPEDFLASH;
			hasWrapped = false;
			TIMER_Start(&TMRTen);
		}
		else
		{
			hasWrapped=true;
			TIMER_Start(&TMRForty);
		}
	}
}

int main(void)
{
  DAVE_STATUS_t status;

  status = DAVE_Init();           /* Initialization of DAVE APPs  */

  if(status != DAVE_STATUS_SUCCESS)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");

    while(1U)
    {

    }
  }

  /* Placeholder for user application code. The while loop below can be replaced with user application code. */
  while(1U)
  {
		switch(current) 
		{
			case RED:
				/* Turning Red LED on and previous LED's off */
				DIGITAL_IO_SetOutputLow(&LEDAmber);
				DIGITAL_IO_SetOutputHigh(&LEDRed);
				break;
			
			case GREEN:
				/* Turning Green LED on and previous LED's off */
				DIGITAL_IO_SetOutputLow(&LEDRed);
				DIGITAL_IO_SetOutputHigh(&LEDGreen);
				break;
			
			case AMBER:
				/* Turning Amber LED on and previous LED's off */
				DIGITAL_IO_SetOutputLow(&LEDGreen);
				DIGITAL_IO_SetOutputHigh(&LEDAmber);
				break;
			
			case REDPED:
				/* Turing Red LED on, Pedestrain LED on and previous LED's off */
				DIGITAL_IO_SetOutputLow(&LEDAmber);
				DIGITAL_IO_SetOutputHigh(&LEDRed);
				PWM_CCU4_SetDutyCycle(&PWMPedestrian, 10000);
				PWM_CCU4_SetFreq(&PWMPedestrian, 100);
				break;
			
			case REDPEDFLASH:
				/* Keeping Red LED on, Pedestrian LED to flashing */
				PWM_CCU4_SetDutyCycle(&PWMPedestrian, 5000);
				PWM_CCU4_SetFreq(&PWMPedestrian, 100);
		}
  }
}
