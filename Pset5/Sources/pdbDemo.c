/*
 * pdbDemo.c
 *
 *  Created on: Nov 15, 2015
 *      Author: URM
 */

#include "led.h"
#include "PDB.h"

void toggleOrangeLed()
{
	static char ledState = 0;
	if((ledState ^= 1) == 1)
			ledOrangeOn();
	else
		ledOrangeOff();
}

void pdbDemo(int seconds)
{
	PDB0Init(seconds*9375, PDBTimerOneShot);
	PDB0Start();
}


