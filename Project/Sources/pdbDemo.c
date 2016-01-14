/*
 * pdbDemo.c
 *
 *  Created on: Nov 15, 2015
 *      Author: URM
 */

#include "led.h"
#include "PDB.h"
#include "userproc.h"



void pdbDemo(int seconds)
{
	PDB0Init(seconds*9375, PDBTimerOneShot);
	PDB0Start();
}

void toggleOrangeLed()
{
	static char ledState = 0;
	if((ledState ^= 1) == 1)
	{
			ledOrangeOn();
	}
	else
	{
		ledOrangeOff();
	}
	setLedState(ledState);
}


