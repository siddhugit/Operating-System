/*
 * systick.c
 *
 *  Created on: Dec 1, 2015
 *      Author: URM
 */

#include "derivative.h"
#include "scheduler.h"
//SYST_CSR

#define QUANTUM_VAL  0x124F80
#define SYSTICK_MaxPriority 14
#define PENDSV_MaxPriority 14
#define SYSTICK_PriorityShift 4

static unsigned char SYSTICKERSTARTED = 0;

void setSysTickPriority(unsigned char priority){
	if(priority > SYSTICK_MaxPriority)
		return;

	SCB_SHPR3 = (SCB_SHPR3 & ~SCB_SHPR3_PRI_15_MASK) | SCB_SHPR3_PRI_15(priority << SYSTICK_PriorityShift);
}

void setPendSVPriority(unsigned char priority){
	if(priority > PENDSV_MaxPriority)
		return;

	SCB_SHPR3 = (SCB_SHPR3 & ~SCB_SHPR3_PRI_14_MASK) | SCB_SHPR3_PRI_14(priority << SYSTICK_PriorityShift);
}

void sysTickInit(void) {
	SYST_CSR |= SysTick_CSR_CLKSOURCE_MASK;
	SYST_RVR = QUANTUM_VAL;
}

void sysTickStart(void) {
	SYST_CSR |= (SysTick_CSR_ENABLE_MASK | SysTick_CSR_TICKINT_MASK);
}

void sysTickStop(void) {
	SYST_CSR &= ~(SysTick_CSR_ENABLE_MASK | SysTick_CSR_TICKINT_MASK);
}

void sysTickIsr(void) {
	uint32_t copyOfSP;
	copyOfSP = 0;
	
	__asm("push {r4,r5,r6,r7,r8,r9,r10,r11}");
	
	__asm("ldr  r0, [%[shcsr]]"     "\n"
	"mov  r1, %[active]"      "\n"
	"orr  r1, r1, %[pended]"  "\n"
	"and  r0, r0, r1"         "\n"
	"push {r0}"
	:
	: [shcsr] "r" (&SCB_SHCSR),
	  [active] "I" (SCB_SHCSR_SVCALLACT_MASK),
	  [pended] "I" (SCB_SHCSR_SVCALLPENDED_MASK)
	: "r0", "r1", "memory", "sp");
	
	__asm("mrs %[mspDest],msp" : [mspDest]"=r"(copyOfSP));
	
	if(SYST_CSR & SysTick_CSR_COUNTFLAG_MASK)//
	{
		if(SYSTICKERSTARTED > 0)
		{
		  setCurrProcSP(copyOfSP,(SYST_RVR-SYST_CVR)+SYST_RVR);
		}
		else
		{
		  ++SYSTICKERSTARTED;
		}
	}
	else//yield happened
	{
		if(SYSTICKERSTARTED > 0)
		{
		  setCurrProcSP(copyOfSP,(SYST_RVR-SYST_CVR));
		}
		else
		{
		  ++SYSTICKERSTARTED;
		}
		SYST_CVR = 0;
	}
	
	
	copyOfSP = getNextProcSP();
	
	__asm("msr msp,%[mspSource]" : : [mspSource]"r"(copyOfSP) : "sp");
	
	__asm("pop {r0}"               "\n"
	"ldr r1, [%[shcsr]]"     "\n"
	"bic r1, r1, %[active]"  "\n"
	"bic r1, r1, %[pended]"  "\n"
	"orr r0, r0, r1"         "\n"
	"str r0, [%[shcsr]]"
	:
	: [shcsr] "r" (&SCB_SHCSR),
	  [active] "I" (SCB_SHCSR_SVCALLACT_MASK),
	  [pended] "I" (SCB_SHCSR_SVCALLPENDED_MASK)
	: "r0", "r1", "sp", "memory");
	
	__asm("pop {r4,r5,r6,r7,r8,r9,r10,r11}");
}
