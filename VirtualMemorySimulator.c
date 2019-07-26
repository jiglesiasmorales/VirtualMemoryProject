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
#include <ctype.h>
#include "project_headers.h"

// Definitions
#define VPAGES		1024		// Number of virtual pages
#define TLB_SIZE	8			// TLB Size
#define FRAMES		256			// Number of frames


// Global variables
unsigned int PageTable[VPAGES][4]; 
unsigned int TLB [TLB_SIZE][5]; 
unsigned int FrameTable[FRAMES];
unsigned int simTime;
unsigned int refCounter;


// int statistics [8] will contain the relevant stats data
// statistics [0] = TLB hit
// statistics [1] = TLB miss
// statistics [2] = Page Table Hit
// statistics [3] = Page Table Fault
// statistics [4] = TLB shootdown
// statistics [5] = Page Eviction
// statistics [6] = HD read
// statistics [7] = HD write
// statistics [8] = TLB Write
// statistics [9] = Page Table Write
// statistics [10] =  Total accesses
int statistics[11];



// Prints out TLB
// Written by Moses Quiliche
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
// Written by Moses Quiliche
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
// Written by Jan Iglesias
void printAllocatedFrames(void)
{
	printf("ALLOCATED FRAMES\n");
	for(int i = 0; i < FRAMES; i++)
		if(FrameTable[i] == 1)
			printf("%d\n", i);
	return;
}

// Prints out unallocated entires in FrameTable
// Written by Moses Quiliche
void printUnAllocatedFrames(void)
{
	printf("UNALLOCATED FRAMES\n");
	for(int i = 0; i < FRAMES; i++)
		if(FrameTable[i] == 0)
			printf("%d\n", i);
	return;
}

// Prints Stats
// statistics [0] = TLB hit
// statistics [1] = TLB miss
// statistics [2] = Page Table Hit
// statistics [3] = Page Table Fault
// statistics [4] = TLB shootdown
// statistics [5] = Page Eviction
// statistics [6] = HD read
// statistics [7] = HD write
// statistics [8] = TLB Write
// statistics [9] = Page Table Write
// statistics [10] =  Total accesses
// Written by Jan Iglesias
void printStats(void)
{
	printf("\n-----------------------------------------------------------------------------\n");
	printf("                           SIMULATION RESULTS \n");
	printf("-----------------------------------------------------------------------------\n");
	printf("TLB hits: %d\t\tTLB misses: %d\t\tTLB hit rate: %.2f%%\n", statistics[0], statistics[1], ((double) statistics[0]) / (statistics[0] + statistics[1]) * 100);
	printf("TLB Shootdowns: %d\tTLB Writes: %d\n\n",  statistics[4],  statistics[8]);
	printf("Pg Table accesses: %d\n",  statistics[2] + statistics[3]);
	printf("Pg Table hits: %d\tPg faults: %d\t\tPg Table hit rate: %.2f%%\n",  statistics[2], statistics[3], ((double) statistics[2]) / (statistics[2] + statistics[3]) * 100);
	printf("Pg evictions: %d\t\tPg Table writes: %d\n\n", statistics[5], statistics[9]);
	printf("Hard disk reads: %d\tHard disk writes: %d\n", statistics[6], statistics[7]);
	printf("-----------------------------------------------------------------------------\n");


}
// The following function simulates one access
// Written by : Jan Iglesias
void doSim (unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4],
						int page_table_size, unsigned int FrameTable[], int frame_table_size,
						unsigned int address, int read_write, int statistics [10])
	{
		char readOrWrite;

		if(read_write == 1)
			readOrWrite = 'w';
		else
			readOrWrite = 'r';
		
		printf("T:\t%d\tAddress: %c\t%d\t\tVPN: %d\n", simTime++, readOrWrite, address, address / 1024);
		memory_access (	TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, address, read_write, statistics );
	}

// The following function will take the input from the user and simulate those memory accesses
// and display the results.
// Written by : Jan Iglesias
void getInput(void)
{

	int address;
	char readOrWrite;
	int inputs = 0;
	int addressArray[1024];
	char readOrWriteArray[1024];

	printf("Please enter r for read or w for write followed by the memory address you would like to access\n");
	printf("To finish inputting values please enter z -1.\n");
	printf("For example: To read 142, write 82, read 45, read 84. Please enter the following\n");
	printf("\tr 142 \n\tw 82 \n\tr 45 \n\tr 84 \n\tz -1 \n");
	printf("INPUT\n");
	
	while(address != -1)
	{
		scanf(" %c %d", &readOrWrite, &address);
		// printf("\n");
		// printf("I just got %d\n", address);

		if(address == -1)
			break;

		addressArray[inputs] = address;

		if(tolower(readOrWrite) == 'r')
			readOrWriteArray[inputs] = 0;
		else
			readOrWriteArray[inputs] = 1;
		
		inputs++;
	}

	printf("-----------------------------------------\n");
	printf("        VIRTUAL MEMORY SIMULATION\n");
	printf("-----------------------------------------\n");


	for(int i = 0; i < inputs; i++)
	{
		// printf("About to simulate: address = %d, readOrWrite = %d\n", addressArray[i], readOrWriteArray[i]);

		doSim (TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, addressArray[i], readOrWriteArray[i], statistics );
	}
		

	printStats();

}

// The following function will generate n random addresses with a 50/50 chance of being a read or write and run those memory accesses in the sim
// Written by : Jan Iglesias
void runSimWithNRandomRWFair (int n)
{
	int generatedAddress;
	int fairReadOrWrite;
	int addressArray[1024];
	char readOrWriteArray[1024];

	for(int i = 0; i < n; i++)
	{
		generatedAddress = (rand() % (1048575 - 0 + 1) + 0);
		fairReadOrWrite = (rand() % (1 - 0 + 1) + 0);
		addressArray[i] = generatedAddress;
		readOrWriteArray[i] = fairReadOrWrite;
	}

	printf("-----------------------------------------\n");
	printf("        VIRTUAL MEMORY SIMULATION\n");
	printf("-----------------------------------------\n");


	for(int i = 0; i < n; i++)
	{
		// printf("About to simulate: address = %d, readOrWrite = %d\n", addressArray[i], readOrWriteArray[i]);

		doSim (TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, addressArray[i], readOrWriteArray[i], statistics );
	}
		

	printStats();

}

// Written by : Moses Quiliche
// The following function has the same functionality as runSimWithNRandomRWFair, but will assign a write access to all addresses.
void runSimWithNRandomAllW (int n)
{
	int generatedAddress;
	int fairReadOrWrite;
	int addressArray[1024];
	char readOrWriteArray[1024];

	for(int i = 0; i < n; i++)
	{
		generatedAddress = (rand() % (1048575 - 0 + 1) + 0);
		addressArray[i] = generatedAddress;
		readOrWriteArray[i] = 1;
	}

	printf("-----------------------------------------\n");
	printf("        VIRTUAL MEMORY SIMULATION\n");
	printf("-----------------------------------------\n");


	for(int i = 0; i < n; i++)
	{
		// printf("About to simulate: address = %d, readOrWrite = %d\n", addressArray[i], readOrWriteArray[i]);

		doSim (TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, addressArray[i], readOrWriteArray[i], statistics );
	}
		

	printStats();

}

// Written by : Moses Quiliche
// The following function will generate a 1000 total memory accesses. Starting at addresses assigned to page 0
// and incrementing the page every 40 memory_accesses
void consecutiveRandomAccessThousand (void)
{
	int count = 0, upper, lower; 
	int generatedAddress;
	int fairReadOrWrite;
	int addressArray[1024];
	char readOrWriteArray[1024];

	for(int i = 1; i <= 25; i++)
	{
		for(int j = 0; j < 40; j++)
		{
			upper = i * 1024;
			lower = (i - 1) * 1024;
			generatedAddress = (rand() % (upper - lower + 1)) + lower; 
			fairReadOrWrite = (rand() % (1 - 0 + 1) + 0);
			addressArray[count] = generatedAddress;
			readOrWriteArray[count] = fairReadOrWrite;
			count++;
		}
	}

	printf("-----------------------------------------\n");
	printf("        VIRTUAL MEMORY SIMULATION\n");
	printf("-----------------------------------------\n");


	for(int i = 0; i < 1000; i++)
	{
		// printf("About to simulate: address = %d, readOrWrite = %d\n", addressArray[i], readOrWriteArray[i]);

		doSim (TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, addressArray[i], readOrWriteArray[i], statistics );
	}
		

	printStats();

}

// Written by : Moses Quiliche
// The following function will generate a 3000 total memory accesses. Starting at addresses assigned to page 0
// and incrementing the page every 10 memory_accesses
void consecutiveRandomAccessThreeThousand (void)
{
	int count = 0, upper, lower; 
	int generatedAddress;
	int fairReadOrWrite;
	int addressArray[4096];
	char readOrWriteArray[4096];

	for(int i = 1; i <= 300; i++)
	{
		for(int j = 0; j < 10; j++)
		{
			upper = i * 1024;
			lower = (i - 1) * 1024;
			generatedAddress = (rand() % (upper - lower + 1)) + lower; 
			fairReadOrWrite = (rand() % (1 - 0 + 1) + 0);
			addressArray[count] = generatedAddress;
			readOrWriteArray[count] = fairReadOrWrite;
			count++;
		}
	}

	printf("-----------------------------------------\n");
	printf("        VIRTUAL MEMORY SIMULATION\n");
	printf("-----------------------------------------\n");


	for(int i = 0; i < 3000; i++)
	{
		// printf("About to simulate: address = %d, readOrWrite = %d\n", addressArray[i], readOrWriteArray[i]);

		doSim (TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, addressArray[i], readOrWriteArray[i], statistics );
	}
		

	printStats();

}

// Written by Jan Iglesias
// The following function will generate 20 random addresses in page 0, then 20 random addresses in page 19
// then 20 random addresses in page 1, then 20 random addresses in page 18, until 400 addresses are generated.
void randomAccessInvertShuffle (void)
{
	int count = 0, upper, lower; 
	int generatedAddress;
	int fairReadOrWrite;
	int addressArray[4096];
	char readOrWriteArray[4096];


	for(int i = 1; i <= 10; i++)
	{
		// Getting lower 20
		for(int j = 0; j < 20; j++)
		{
			upper = i * 1024;
			lower = (i - 1) * 1024;
			generatedAddress = (rand() % (upper - lower + 1)) + lower; 
			fairReadOrWrite = (rand() % (1 - 0 + 1) + 0);
			addressArray[count] = generatedAddress;
			readOrWriteArray[count] = fairReadOrWrite;
			count++;
		}
		// Getting upper 20
		for(int j = 0; j < 20; j++)
		{
			upper = (20 - i + 1) * 1024;
			lower = (20 - i ) * 1024;
			generatedAddress = (rand() % (upper - lower + 1)) + lower; 
			fairReadOrWrite = (rand() % (1 - 0 + 1) + 0);
			addressArray[count] = generatedAddress;
			readOrWriteArray[count] = fairReadOrWrite;
			count++;
		}
	}

	printf("-----------------------------------------\n");
	printf("        VIRTUAL MEMORY SIMULATION\n");
	printf("-----------------------------------------\n");


	for(int i = 0; i < 400; i++)
	{
		// printf("About to simulate: address = %d, readOrWrite = %d\n", addressArray[i], readOrWriteArray[i]);

		doSim (TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, addressArray[i], readOrWriteArray[i], statistics );
	}
		

	printStats();


}
// Main function
int main(void)
{
 refCounter=0;

	int address, read_write;

	// Seeding RNG needed for functions and starting timer
	simTime = 1;
	srand(time(NULL));

	getInput();
	
	
	

	return 0;
}

