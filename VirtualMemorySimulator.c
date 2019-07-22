// Virtual Memory Simulator
// This program simulates a virtual memory by creating a virtual TLB,
// virtual page table, and virtual hard disk. This program will be
// able to simulate hits and misses and perform the actions accordingly.
// ---------------------------------------------------------------------
// Due date : 7/26/2019
// Written by Jan Iglesias & Moses Quiliche

// Included libraries
#include <stdio.h>
#include "project_headers.h"

// Definitions
#define VPAGES		1024		// Number of virtual pages
#define TLB_SIZE	8			// TLB Size
#define FRAMES		256			// Number of frames


// Global variables
unsigned int PageTable[VPAGES][4]; 
unsigned int TLB [TLB_SIZE][5]; 
unsigned int FrameTable[FRAMES];

// Main function
int main(void)
{
	
	return 0;
}