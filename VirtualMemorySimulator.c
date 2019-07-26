// Virtual Memory Simulator
// This program simulates a virtual memory by creating a virtual TLB,
// virtual page table, and virtual hard disk. This program will be
// able to simulate hits and misses and perform the actions accordingly.
// ---------------------------------------------------------------------
// Due date : 7/26/2019
// Written by Jan Iglesias & Moses Quiliche

// Included libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "project_headers.h"

// Definitions
#define VPAGES		1024		// Number of virtual pages
#define TLB_SIZE	8			// TLB Size
#define FRAMES		256			// Number of frames


// Global variables
unsigned int PageTable[VPAGES][4]; 
unsigned int TLB [TLB_SIZE][5]; 
unsigned int FrameTable[FRAMES];


// int statistics [8] will contain the relevant stats data
// statistics [0] = TLB hit
// statistics [1] = TLB miss
// statistics [2] = Page Table Hit
// statistics [3] = Page Table Fault
// statistics [4] = TLB shootdown
// statistics [5] = Page Eviction
// statistics [6] = HD read
// statistics [7] = HD write
int statistics[8];



// Prints out TLB
void printTLB(void)
{
	printf("=======================TLB=====================\n");
	printf("|Entry|  V  |  D  |  R  |  VPN  |  Physical   |\n");
	printf("===============================================\n");

	for(int i = 0; i < TLB_SIZE; i++)
		printf("|  %d  |  %d  |  %d  |  %d  |  %d  |      %d     |\n", i, TLB[i][0], TLB[i][1], TLB[i][2], TLB[i][3], TLB[i][4]);

	printf("===============================================\n");
}

// Prints out valid entries of the PageTable
void printValidPageTable(void)
{
	printf("=======================PageTable======\n");
	printf("|  vpn  |  V  |  D  |  R  |  Physical |\n");
	printf("======================================\n");

	for(int i = 0; i < VPAGES; i++)
	{
		if(PageTable[i][0] == 1)
			printf("|  %d  |  %d  |  %d  |  %d  |     %d     |\n", i, PageTable[i][0], PageTable[i][1], PageTable[i][2], PageTable[i][3]);
	}

	printf("======================================\n");
}

// Prints out allocated entires in FrameTable
void printAllocatedFrames(void)
{
	printf("ALLOCATED FRAMES\n");
	for(int i = 0; i < FRAMES; i++)
		if(FrameTable[i] == 1)
			printf("%d\n", i);
	return;
}

// Prints out unallocated entires in FrameTable
void printUnAllocatedFrames(void)
{
	printf("UNALLOCATED FRAMES\n");
	for(int i = 0; i < FRAMES; i++)
		if(FrameTable[i] == 0)
			printf("%d\n", i);
	return;
}


// Main function
int main(void)
{
	// Seeding RNG needed for functions
	srand(time(NULL));
	int result = 0;

	printTLB();
	printValidPageTable();
	memory_access (	TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, address, read_write, statistics);

	return 0;
}

