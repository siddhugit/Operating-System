#include <stdio.h>
#include <stdint.h>
#include "memory.h"
#include "error.h"

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

static const uint32_t ONE_MEGA_BYTE  = (1<<20);

int main()
{
	unsigned int size;;
	init_memory(ONE_MEGA_BYTE);
	size = 4231;
	fprintf(stdout,"\nTest Case# 1: Interleaved memory allocacations/deallocations and print memory map");
	fprintf(stdout,"\n----------------------------------------------------------------------------------\n\n");
	void* ptr1 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr1);
	size = 10487;
	void* ptr2 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr2);
	size = 75934;
	void* ptr3 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr3);
	size = 532432;
	void* ptr4 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr4);
	size = 343253;
	void* ptr5 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr5);	
	myFree(ptr2);
	fprintf(stdout,"Deallocated memory %p\n",ptr2);
	myFree(ptr4);
	fprintf(stdout,"Deallocated memory %p\n",ptr4);
	size = 65762;
	void* ptr6 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr6);
	size = 43234;
	void* ptr7 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr7);
	myFree(ptr6);
	fprintf(stdout,"Deallocated memory %p\n",ptr6);
	size = 8364;
	void* ptr8 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr8);
	fprintf(stdout,"\nPrinting memory map after test case# 1\n\n");
	memoryMap();	
	fprintf(stdout,"\nTest Case# 2: Allocating memory with size in hexadecimal (0xFFFF)");
	fprintf(stdout,"\n------------------------------------------------------------------\n\n");
	size = 0xFFFF;
	void* ptr9 = myMalloc(size);
	fprintf(stdout,"Allocated memory of size %d at %p\n",size,ptr9);
	fprintf(stdout,"\nPrinting memory map after test case# 2\n\n");
	memoryMap();
	fprintf(stdout,"\nTest Case# 3: Freeing not a previously allocated memory(0xFFFFFFFF)");
	fprintf(stdout,"\n--------------------------------------------------------\n\n");
	myFree((void*)0xFFFFFFFF);
	fprintf(stderr,"%s\n",my_strerr(my_errno));
	fprintf(stdout,"\nTest Case# 4: Freeing a null pointer");
	fprintf(stdout,"\n-------------------------------------------\n\n");
	myFree(0);
	fprintf(stderr,"%s\n",my_strerr(my_errno));	
	fprintf(stdout,"\nTest Case# 5: Allocating zero sized memory");
	fprintf(stdout,"\n-------------------------------------------\n\n");
	void* ptr10 = myMalloc(0);
	fprintf(stderr,"%p\n",ptr10);	
	fprintf(stdout,"\nTest Case# 6: Allocating memory of a very big size(%d bytes)",ONE_MEGA_BYTE);
	fprintf(stdout,"\n-------------------------------------------\n\n");
	myMalloc(ONE_MEGA_BYTE);	
	fprintf(stderr,"%s\n",my_strerr(my_errno));		
	fprintf(stdout,"\nPrinting memory map after all test cases\n\n");
	memoryMap();
	return 0;
}
