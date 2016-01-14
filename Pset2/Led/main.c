#include <stdbool.h>
#include "led.h"
#include "pushbutton.h"
#include "delay.h"

/*
* Turns on one light and turns off remaing LEDs depending upon counter value
*/
void changestate(int counter)
{
	switch(counter)
	{
		case 0 ://orange on
			ledOrangeOn();
			ledYellowOff();
			ledGreenOff();
			ledBlueOff();
			break;
		case 1 ://yellow on
			ledOrangeOff();
			ledYellowOn();
			ledGreenOff();
			ledBlueOff();
			break;
		case 2 ://green on
			ledOrangeOff();
			ledYellowOff();
			ledGreenOn();
			ledBlueOff();
			break;
		case 3 ://blue on
			ledOrangeOff();
			ledYellowOff();
			ledGreenOff();
			ledBlueOn();
			break;
		default:
			break;
						
	}
}
/*
* increment counter value in circular manner, call changestate()
* which turns on one LED and turns off others depending upon
* counter value.
*/
void rotate()
{
	const unsigned long int delayCount = 0x7ffff;
	int counter = 0;
	changestate(counter);
	while(1) 
	{
		if(sw1In()) 
		{
			counter = ((counter + 1) % 4);//move to next led
			changestate(counter);
			delay(delayCount);//wait for switch to be depressed
		}
	}
}

int main(void) {
	/* Initialize all of the LEDs */
		ledInitAll();
		/* Initialize both of the pushbuttons */
		pushbuttonInitAll();
	rotate();
	return 0;
}
