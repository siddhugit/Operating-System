/**
 **     This file implements memory management functions. This module
 **     has following public methods.
 **     myMalloc() allocates memory
 **	myFree() deallocated memory
 **     memoryMap() prints current state of memory
 ** 	init_memory() allocates and initializes allocator's memory
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "error.h"

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

static unsigned int MEMORY_SIZE = 0;

//Double word size
static const uint32_t DOUBLE_WORD_SIZE = 8;

//data structure to represent a block of memory header
struct header_block
{
	//determines free bit
	uint32_t isAllocated : 1;
	//process id
	uint32_t pid : 7;
	//allocated size	
	uint32_t size : 24;
	//dummy member variable to double-word align the data structure	
	uint32_t dummy;
	//location of data part of memory block 
	uint8_t data[0];
};

//Head of memory linked list
static void *Head = NULL;

/*
*   turns free bit off of memory block
*   Parameters:
*       o struct header_block *block - pointer to memory block header
*/
static void unset_free_bit(struct header_block *block)
{
	block->isAllocated = true;
}
/*
*   turns free bit on of memory block
*   Parameters:
*       o struct header_block *block - pointer to memory block header
*/
static void set_free_bit(struct header_block *block)
{
	block->isAllocated = false;
}
/*
*   checks free bit of memory block
*   Parameters:
*       o struct header_block *block - pointer to memory block header
*   Return : 0 if free, otherwise 1
*/
static bool check_free_bit(const struct header_block *block)
{
	bool isAllocated = (block->isAllocated == true);
	return (!isAllocated);
}
/*
*   Accessor function for pid of memory block
*   Parameters:
*       o struct header_block *block - pointer to memory block header
*   Return : id of process that allocated memory
*/
static uint32_t get_pid(const struct header_block *block)
{
	return block->pid;
}
/*
*   sets pid of memory block
*   Parameters:
*       o struct header_block *block - pointer to memory block header
*	o uint32_t pid - id of process that allocated memory
*/
static void set_pid(struct header_block *block,uint32_t pid)
{
	block->pid = pid;
}
/*
*   Accessor function for size of allocated memory
*   Parameters:
*       o struct header_block *block - pointer to memory block header
*   Return : pid of memory block header
*/
static uint32_t get_size(const struct header_block *block)
{
	return block->size;
}
/*
*   sets size of allocated memory block
*   Parameters:
*       o struct header_block *block - pointer to memory block header
*	o uint32_t size - size of allocated memory
*/
static void set_size(struct header_block *block,uint32_t size)
{
	block->size = size;
}
/*
*   Dummy process to return pid of current process
*   Return : 0 for now
*/
static uint32_t getCurrentPID()
{
	return 0;
}
/*
*   Return : size of header block data structure
*/
static unsigned int header_size()
{
	return sizeof(struct header_block);
}
/*
*   Return : Maximum size that can be allocated by allocator
*/
static unsigned int max_size()
{
	return (MEMORY_SIZE - header_size());
}
/*
*   Checks if a memory is int the range of available memory to the allocator
*   Parameters:
*	o struct header_block *block - memory to be checked
*   Return : true if memory is valid, otherwise false
*/
static bool is_memory_in_range(const struct header_block *block)
{
	bool result = ( (block >= (struct header_block *)Head) && 
			((uint8_t*)block < ((uint8_t*)Head) + MEMORY_SIZE) );
	return result;
}
/*
*   Allocates a free memory to current process and resets remaining memory
*	as free.
*   Parameters: 
*   	o struct header_block *block - free memory
*	o uint32_t size - size of memory to allocate
*/
static void allocate_memory(struct header_block *block,unsigned int size)
{
	if(get_size(block) != size)//if memory requested doesn't exactly fit in first fit found memory
	{
		//new unallocated header
		unsigned int newSize = get_size(block) - size - header_size();
		struct header_block *newHeader = (struct header_block *)(block->data + size);
		set_free_bit(newHeader); 
		set_pid(newHeader,getCurrentPID());
		set_size(newHeader,newSize);
	}
	//initialize book keeping header and allocate memory
	unset_free_bit(block); 
	set_pid(block,getCurrentPID());
	set_size(block,size);
}
/*
*   This function implements first fit memory allocation algorithm. Traverses 
*	through the available memory and find fist block which if free and can 
*	fit th requested memory. It the allocates that memory to the callee 
*	process.
*   Parameters: 
*	o uint32_t size - requested memory size
*   Return: Pointer to the memory if allocated, otherwise null
*   In case memory is not available my_errno is to OUT_OF_MEMROY
*/
static void* first_fit(unsigned int size)
{
	struct header_block *next = (struct header_block *)Head;
	bool found = false;
	while(is_memory_in_range(next))//linked list traversal
	{
		if(check_free_bit(next))//not allocated
		{
			if(size <= get_size(next))//first fit found
			{
				found = true;
				break;
			}
			else//does not fit
			{
				//next block
				next = (struct header_block *)(next->data + get_size(next));
			}
		}
		else//allocated
		{
			next = (struct header_block *)(next->data + get_size(next));
		}
	}
	if(found)//allocate memory
	{
		allocate_memory(next,size);
		return (void*)(next->data);
	}
	my_errno = OUT_OF_MEMROY;
	return NULL;
}
/*
*   This is a public interface of memory module. This function is called by
*	processes to allocate memory. This function calls first_fit() 
*	function which allocates memory by implementing first fit algorithm. 
*   Parameters: 
*	o uint32_t size - requested memory size
*   Return: Pointer to the memory if allocated, otherwise null
*   In case memory is not available my_errno is to OUT_OF_MEMROY
*/
void *myMalloc(unsigned int size)
{
	if(size == 0)//zero size memory allocation
		return NULL;
	if(size > max_size())//more than maximum available memory 
	{
		my_errno = OUT_OF_MEMROY;
		return NULL;
	}
	//double word alignment
	size = ((size + (DOUBLE_WORD_SIZE - 1))/DOUBLE_WORD_SIZE)*DOUBLE_WORD_SIZE;
	return first_fit(size);
}
/*
*   This frees a given memory and merges neighboring memories into a bigger
*	chunk of free memory depending upon if neighboring memories are free
*	or allocated.
*   Parameters: 
*	o struct header_block *block - memory to be freed
*	o struct header_block *prev - Requires left neighbor of memory to be
*		freed as first fit uses singly linked list.
*/

static void deallocate_memory(struct header_block *block,struct header_block *prev)
{
	struct header_block *next = (struct header_block *)(block->data + get_size(block));
	//case 1: both sides allocated
	if((prev == block || !check_free_bit(prev)) && (!is_memory_in_range(next) || !check_free_bit(next)))
	{
		set_free_bit(block);
	}
	//case 2: left side is allocated and right side is free
	else if((prev == block || !check_free_bit(prev)) && (is_memory_in_range(next) && check_free_bit(next)))
	{
		set_size(block,get_size(next) + get_size(block) + header_size());
		set_free_bit(block);
	}
	//case 3: left side is free and right side is allocated
	else if((prev != block && check_free_bit(prev)) && (!is_memory_in_range(next) || !check_free_bit(next)))
	{
		set_size(prev,get_size(prev) + get_size(block) + header_size());
		set_free_bit(prev);
	}
	//case 4: both sides free
	else if((prev != block && check_free_bit(prev)) && (is_memory_in_range(next) && check_free_bit(next)))
	{
		set_size(prev,get_size(prev) + get_size(next) + get_size(block) + 2*header_size());
		set_free_bit(prev);
	}
}
/*
*   This frees a given memory and merges neighboring memories into a bigger
*	chunk of free memory depending upon if neighboring memories are free
*	or allocated.
*   Parameters: 
*	o void *ptr - memory to be freed
*   Sets my_errno to DELETE_NULL if null pointer is freed, to 
*	MEMORY_NOT_ALLOCATED if memory being freed is not a previously
*	allocated memory by the allocator or not owned by current process.
*/
void myFree(void *ptr)
{
	if(ptr == NULL)//free null pointer is no-op
	{
		my_errno = DELETE_NULL;
		return;
	}
	struct header_block *next = (struct header_block *)Head;
	struct header_block *prev = (struct header_block *)Head;
	bool found = false;
	while(is_memory_in_range(next))//list traversal
	{
		if((void*)(next->data) == ptr)//found
		{
			if(!check_free_bit(next) && get_pid(next) == getCurrentPID())//allocated
			{
				found = true;
				deallocate_memory(next,prev);//deallocate
			}
			else//free
			{
				my_errno = MEMORY_NOT_ALLOCATED;//freeing already freed memory
			}
			break;
		}
		prev = next;
		next = (struct header_block *)(next->data + get_size(next));//next block
	}
	if(!found)
	{
		my_errno = MEMORY_NOT_ALLOCATED;//not a previously allocated memory
	}
}
/*
*   Public interface to allocate memory for allocator. It calls malloc system 
*	call to allocate memory for the allocator. This function should be 
*	called only once during lifetime of allocator. The memory allocated is
*	never freed.
*   Parameters: 
*	o unsigned int memorySize - size of memory pool given to the
*		allocator
*/
void init_memory(unsigned int memorySize)
{
	if(Head == NULL)//if not already allocated
	{
		//double word alignment
		memorySize = ((memorySize + (DOUBLE_WORD_SIZE - 1))/DOUBLE_WORD_SIZE)*DOUBLE_WORD_SIZE;
		Head = malloc(memorySize);//allocator's memory
		if(Head == NULL)//if malloc fails
		{
			fprintf(stderr,"Malloc failed: %s\n",strerror(errno));
			exit(EXIT_FAILURE);
		}
		if(MEMORY_SIZE == 0)
		{
			MEMORY_SIZE = memorySize;
		}
		//initialize allocator's memory
		struct header_block *header = (struct header_block *)Head;
		set_free_bit(header); 
		set_pid(header,getCurrentPID());
		set_size(header,max_size());
	}
}
/*
*   Prints current state of allocator's memory on standard output showing 
*   which memory blocks are free of allocated.
*/
void memoryMap()
{
	fprintf(stdout,"location\tpid\t free/allocated\t     size(in bytes)\n");
	fprintf(stdout,"--------\t---\t --------------\t     --------------\n");
	//Head pointer for linked list traversal	
	const struct header_block *next = (struct header_block *)Head;
	unsigned int allocated = 0,unallocated = 0;
	while(is_memory_in_range(next))//linked list traversal
	{

		if(!check_free_bit(next))//allocated
		{
			fprintf(stdout,"%p\t%d\t %s\t     %d\n"
				,next->data,get_pid(next),"allocated",get_size(next));
			allocated += get_size(next) + header_size();
		}
		else if(get_size(next) != 0)//free memory with non-zero size (excluding header size)
		{
			fprintf(stdout,"%p\t%s\t %s\t\t     %d\n"
				,next->data,"NA","free",get_size(next));
			unallocated += get_size(next) + header_size();
		}
		next = (struct header_block *)(next->data + get_size(next));//next block of memory
	}
	fprintf(stdout,"\nallocated: %d bytes\n",allocated);//total of allocated memory
	fprintf(stdout,"free: %d bytes\n",unallocated);//total of free memory
}
