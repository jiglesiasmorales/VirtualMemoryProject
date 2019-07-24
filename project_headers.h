

#include <stdlib.h>
#include <time.h>
#include <stdio.h>


//******************************************************************************
//***********************  TLB  Functions  *************************************
//******************************************************************************
//
//
// TLB FORMAT
//
//						0				1				  2							 3									4
//|  TLB index	|	Valid Bit 	|	Dirty bit 	|	Reference bit 	| 	Tag (Full virtual Page Number)	|	Frame # (Physical page #)	|
//=======================================================================================================================================
//|      0      |	   V		|       D  		|		  R 		|				vpn					|	mustBeCalculatedInFunct		|
//=======================================================================================================================================
//|      1      |	   V		|       D  		|		  R 		|				vpn					|	mustBeCalculatedInFunct		|
//=======================================================================================================================================
//|      2      |	   V		|       D  		|		  R 		|				vpn					|	mustBeCalculatedInFunct		|
//=======================================================================================================================================
//|      3      |	   V		|       D  		|		  R 		|				vpn					|	mustBeCalculatedInFunct		|
//=======================================================================================================================================
//|      4      |	   V		|       D  		|		  R 		|				vpn					|	mustBeCalculatedInFunct		|
//=======================================================================================================================================
//|      5      |	   V		|       D  		|		  R 		|				vpn					|	mustBeCalculatedInFunct		|
//=======================================================================================================================================
//|      6      |	   V		|       D  		|		  R 		|				vpn					|	mustBeCalculatedInFunct		|
//=======================================================================================================================================
//|      7      |	   V		|       D  		|		  R 		|				vpn					|	mustBeCalculatedInFunct		|
//=======================================================================================================================================
//


// DONE
// Returns the TLB entry that corresponds to a virtual page
// Returns -1 if the page's translation is not in the TLB
// Written by : Jan Iglesias
int TLB_lookup(unsigned int TLB[][5], int size, unsigned int vpn)
{
	// Iterating through the whole TBL
	for(int i = 0; i < size; i++)
	{
		// If the current entry in TLB has a matching vpn return index
		if( TLB[i][3] == vpn)
			return i;
	}

	return -1;

}
// DONE
// Returns the index of the first available TLB entry
// Returns -1 if all TLB entries are used
// Written by : Jan Iglesias
int get_available_TLB_entry (unsigned int TLB[][5], int size)
{
	// Iterating through the whole TBL
	for(int i = 0; i < size; i++)
	{
		// If the current entry in TLB has a valid field of 0 then it is empty and return index
		if( TLB[i][0] == 0)
			return i;
	}

	return -1;

}

// DONE  
// Selects the TLB entry that will be evicted
// Pre-condition: All TLB entries are full
// Criteria: Select a random entry with ref.bit=0; if all ref.bit=1, select a random entry
// Written by: Jan Iglesias
unsigned int select_TLB_shootdown_candidate(unsigned int TLB[][5], int size)
{
	// The following array will contain a 0 in TLB entries with ref = 1 and 1 in TLB entries with ref = 0
	int indicesWithRefZero[size];
	// Will hold the number of entries found
	int indexCount = 0;
	int shootDownCandidate;

	// Seeding rng
	srand(time(0));
	// Initializing array to -1
	for(int i = 0; i < size; i++)
		indicesWithRefZero[i] = -1;

	// Marking which indices have a ref of 0 in the aux array
	for(int i = 0; i < size; i++)
	{
		if(TLB[i][2] == 0)
		{
			indicesWithRefZero[indexCount] = i;
			indexCount++;
		}
	}

	// If atleast one entry is found then pick a random one
	if(indexCount != 0)
	{
		shootDownCandidate =  rand() % indexCount ;
		return indicesWithRefZero[shootDownCandidate];
	}
	// Else pick a random one
	else
	{
		shootDownCandidate = rand() % size; 
		return shootDownCandidate;
	}

}

// DONE
// Perform a TLB shootdown (set V=0, copy D,R bits to the page table)
// Pre-condition: shootdown entry is currently valid
// Parameter index is the TLB entry to shoot down
// Written by : Jan Iglesias
void TLB_shootdown (	unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4],
						int page_table_size, int index	)
{
	// Creating variables to hold D and R (V is always 1 in this function) and pageNumber
	int shootDownD, shootDownR, pageNumber, frameNumber;

	// Getting info from TLB entry
	shootDownD = TLB[index][1];		// Getting D
	shootDownR = TLB[index][2];		// Getting R
	pageNumber = TLB[index][3];		// Getting page number
	frameNumber = TLB[index][4];	// Getting frame number

	TLB[index][0] = 0;				// Setting this entry to empty.

	// Filling PageTable entry with values of shootdown entry
	PageTable[pageNumber][0] = 1;
	PageTable[pageNumber][1] = shootDownD;
	PageTable[pageNumber][2] = shootDownR;
	PageTable[pageNumber][3] = frameNumber;

	return;
}

// DONE
// Copies a translation from the Page Table to the TLB
// The first available TLB entry is used; otherwise, a TLB shootdown is performed
// It copies the D,R bits and the frame number from the page table
// Parameter: virtual page number
// Returns: (+1: shootdown performed)
// Written by : Jan Iglesias
int cache_translation_in_TLB (unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4],
							 int page_table_size, unsigned int vpn	)
{
	int currentEntry = 0;
	int copiedV, copiedD, copiedR, copiedPageNumber, copiedFrameNumber;
	int shootDownCandiate;

	// Getting fields to be copied
	copiedV = PageTable[vpn][0];
	copiedD = PageTable[vpn][1];
	copiedR = PageTable[vpn][2];
	copiedFrameNumber = PageTable[vpn][3];

	// Finding an available entry
	for(currentEntry = 0; currentEntry < tlb_size; currentEntry++)
	{
		// If an available entry is found then fill it and return 0.
		if(TLB[currentEntry][0] == 0)
		{
			TLB[currentEntry][0] = 1;
			TLB[currentEntry][1] = copiedD;
			TLB[currentEntry][2] = copiedR;
			TLB[currentEntry][3] = vpn;
			TLB[currentEntry][4] = copiedFrameNumber;
			return 0;
		}
	}

	// If no available entry is found then we must make one.
	// Finding shootdown candiate
	shootDownCandiate = select_TLB_shootdown_candidate(TLB, tlb_size);

	// Performing shootdown
	TLB_shootdown (TLB, tlb_size, PageTable, page_table_size, shootDownCandiate	);

	// Filling in shotdown entry
	TLB[shootDownCandiate][0] = 1;
	TLB[shootDownCandiate][1] = copiedD;
	TLB[shootDownCandiate][2] = copiedR;
	TLB[shootDownCandiate][3] = vpn;
	TLB[shootDownCandiate][4] = copiedFrameNumber;

	// Return 1 to mark a shootdown was performed
	return 1;
}	


//******************************************************************************
//***********************  Page Table Functions  *******************************
//******************************************************************************
//
//
//
//
// PAGE TABLE FORMAT
//
//						0				1				  2							 3	
//|    Page # 	|	Valid Bit 	|	Dirty bit 	|	Reference bit 	| 	Frame # (Physical page #)	|
//===================================================================================================
//|      0      |	   V		|       D  		|		  R 		|	mustBeCalculatedInFunct		|
//===================================================================================================
//|      1      |	   V		|       D  		|		  R 		|	mustBeCalculatedInFunct		|
//===================================================================================================
//|      2      |	   V		|       D  		|		  R 		|	mustBeCalculatedInFunct		|
//===================================================================================================
//|      3      |	   V		|       D  		|		  R 		|	mustBeCalculatedInFunct		|
//===================================================================================================
//|      4      |	   V		|       D  		|		  R 		|	mustBeCalculatedInFunct		|
//===================================================================================================
//
//
//
//
// Frame table contains all spaces in memory.
// If an index constains a 0 it is not allocated if it contains a 1 then it is allocated.


// DONE
// Returns the index of the first available frame
// Returns -1 if all frames are allocated
// Written by : Jan Iglesias
int get_available_frame (unsigned int FrameTable[], int size)
{
	// Going through entire FramtTable. (0 = unallocated / 1 = allocated)
	for(int i = 0; i < size; i++)
	{
		if(FrameTable[i] == 0)
			return i;
	}

	return -1;

}

// Search the PageTable for VDR values passed as parameters
// Return -1 if not found; otherwise, return the index of one such
// randomized entry (using rand function)
// Pre-condition: VDR are 0 or 1
int search_PageTable_by_VDR(unsigned int PageTable[][4], int size, int V, int D, int R)
{

}

// Selects the virtual page that will be replaced
// Pre-condition: All the frames are allocated
// Criteria: Valid must be 1; choose in order as below
//     VDR.bits: 100   110   101   111
// Between pages with the same category, randomize (using rand)
unsigned int select_page_eviction_candidate(unsigned int PageTable[][4], int size)
{

}

// Evict a page from RAM to the hard disk
// If its translation is in the TLB, perform a TLB shootdown
// If the page is dirty, write it to hard disk
// Update the Frame Table and the page table
// Pre-condition: the page is currently allocated in the RAM
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed)
int page_evict (	unsigned int PageTable[][4], int page_table_size, unsigned int TLB[][5],
				 int tlb_size, int FrameTable[], int frame_table_size, int vpn	)
{

}

// Copies a page from the hard disk to the RAM 
// Pre-conditions: Page currently not in RAM; page's translation is not in the TLB
// Find a frame for the page; if all the frames are used, performa a page eviction
// Find a TLB entry for the page; if the TLB is full, perform a TLB shootdown
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed) (+100: page eviction performed)
// Parameter read_write: indicates read access or write access
int cache_page_in_RAM (	unsigned int PageTable[][4], int page_table_size, unsigned int TLB[][4],
						int tlb_size, unsigned int FrameTable[], int frame_table_size, unsigned int vpn,
						int read_write	)
{

}




//******************************************************************************
// Clears the reference bits of the TLB and the Page Table
void reset_reference_bits (unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4], int page_table_size)
{

}




//******************************************************************************
//***********************  Overall  Function  **********************************
//******************************************************************************

// Simulates a memory access; updates all the data and statistics
void memory_access (	unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4],
						int page_table_size, unsigned int FrameTable[], int frame_table_size,
						unsigned int address, int read_write	)
{

}











