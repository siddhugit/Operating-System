/*
 * systick.h
 *
 *  Created on: Dec 1, 2015
 *      Author: URM
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

void sysTickInit(void) ;
void sysTickStart(void);
void sysTickStop(void) ;
void sysTickIsr(void);
void setSysTickPriority(unsigned char priority);
void setPendSVPriority(unsigned char priority);

#endif /* SYSTICK_H_ */
