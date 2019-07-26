

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


// DONE & TESTED
// Returns the TLB entry that corresponds to a virtual page
// Returns -1 if the page's translation is not in the TLB
// Written by : Jan Iglesias
int TLB_lookup(unsigned int TLB[][5], int size, unsigned int vpn)
{
	// Iterating through the whole TBL
	for(int i = 0; i < size; i++)
	{
		// If the current entry in TLB has a matching vpn return index
		if( TLB[i][3] == vpn && TLB[i][0] == 1)
			return i;
	}

	return -1;

}
// DONE & TESTED
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

// DONE  & TESTED
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

// DONE & Tested
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

// DONE & TESTED
// Copies a translation from the Page Table to the TLB
// The first available TLB entry is used; otherwise, a TLB shootdown is performed
// It copies the D,R bits and the frame number from the page table
// Parameter: virtual page number
// Returns: (+1: shootdown performed)
// Written by : Jan Iglesias
int cache_translation_in_TLB (unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4],
							 int page_table_size, unsigned int vpn	)
{
	// printf("cache_translation_in_TLB with vpn = %d\n", vpn);
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
		// printf("In currentEntry = %d (Looking for available entry in TLB)\n", currentEntry);
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

	// printf("About to perform a shootdown\n");
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


// DONE & TESTED
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
// DONE & TESTED
// Search the PageTable for VDR values passed as parameters
// Return -1 if not found; otherwise, return the index of one such
// randomized entry (using rand function)
// Pre-condition: VDR are 0 or 1
// Written by : Jan Iglesias
int search_PageTable_by_VDR(unsigned int PageTable[][4], int size, int V, int D, int R)
{
	// Creating counter for results found an an array that will hold matches
	int resultsFound = 0;
	int validEntries[size];
	int result;

	// Initiazliing validEntries to zero
	for(int i = 0; i < size; i++)
		validEntries[i] = 0;
	
	// Iterating through PageTable
	for (int i = 0; i < size; i++)
	{
		// Checking if current entry in PageTable has matching VDR
		if(PageTable[i][0] == V && PageTable[i][1] == D && PageTable[i][2] == R)
		{
			// If current entry has matching VDR save entry and increment resultsFound
			validEntries[resultsFound] = i;
			resultsFound++;
		}
	}

	// If atleast one result is found (Return a random one that qualifies)
	if(resultsFound != 0)
	{
		result =  rand() % resultsFound ;
		return validEntries[result];
	}

	// If no results are found
	else
		return -1;

}


// DONE & TESTED
// Selects the virtual page that will be replaced
// Pre-condition: All the frames are allocated
// Criteria: Valid must be 1; choose in order as below
//     VDR.bits: 100   110   101   111
// Between pages with the same category, randomize (using rand)
// Written by : Jan Iglesias
unsigned int select_page_eviction_candidate(unsigned int PageTable[][4], int size)
{
	int possibleCandidate;

	// Check if virtual page w..ith VDR = 100 exists
	possibleCandidate = search_PageTable_by_VDR(PageTable, size, 1, 0, 0);
	
	// Checking if a virtual page with VDR = 100 exists ( -1  -> DNE )
	if(possibleCandidate == -1)
		possibleCandidate = search_PageTable_by_VDR(PageTable, size, 1, 1, 0); // Checking following criteria ( VDR = 110)

	// Checking if a virtual page with VDR = 110 exists ( -1  -> DNE )
	if(possibleCandidate == -1)
		possibleCandidate = search_PageTable_by_VDR(PageTable, size, 1, 0, 1); // Checking following criteria ( VDR = 101)

	// Checking if a virtual page with VDR = 101 exists ( -1  -> DNE )
	if(possibleCandidate == -1)
		possibleCandidate = search_PageTable_by_VDR(PageTable, size, 1, 1, 1); // Checking following criteria ( VDR = 111)

	return possibleCandidate;

}

// DONE & TESTED
// Evict a page from RAM to the hard disk
// If its translation is in the TLB, perform a TLB shootdown
// If the page is dirty, write it to hard disk
// Update the Frame Table and the page table
// Pre-condition: the page is currently allocated in the RAM
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed)
// Written by : Jan Iglesias
int page_evict (	unsigned int PageTable[][4], int page_table_size, unsigned int TLB[][5],
				 int tlb_size, int FrameTable[], int frame_table_size, int vpn	)
{
	int evictTlbEntry, evictedFrame, result = 0, dirty;

	// Getting frame from page
	evictedFrame = PageTable[vpn][3];

	// Getting dirty bit
	dirty = PageTable[vpn][1];

	// Setting valid bit to 0 in the PageTable
	PageTable[vpn][0] = 0;

	// Write to hard drive is necessary
	if (dirty == 1)
		result+= 10;
	
	

	// Getting tlb entry of page to be evicted
	evictTlbEntry = TLB_lookup(TLB, tlb_size, vpn);

	

	// TLB shootdown is necessary
	if( evictTlbEntry != -1)
	{
		TLB_shootdown (TLB, tlb_size, PageTable, page_table_size, evictTlbEntry);
		PageTable[vpn][0] = 0;
		result += 1;
	}



	// Setting evictedFrame to unallocated in memory
	FrameTable[evictedFrame] = 0;
		
	return result;

}



// DONE TESTED
// Copies a page from the hard disk to the RAM 
// Pre-conditions: Page currently not in RAM; page's translation is not in the TLB
// Find a frame for the page; if all the frames are used, performa a page eviction
// Find a TLB entry for the page; if the TLB is full, perform a TLB shootdown
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed) (+100: page eviction performed)
// Parameter read_write: indicates read access or write access
// Written by : Jan Iglesias
int cache_page_in_RAM (	unsigned int PageTable[][4], int page_table_size, unsigned int TLB[][5],
						int tlb_size, unsigned int FrameTable[], int frame_table_size, unsigned int vpn,
						int read_write	)
{
	int evictionPageCandidate, result = 0, frameLocation, availableTLBEntry, pageCandidate;

	// Finding available frame if any
	frameLocation = get_available_frame(FrameTable, frame_table_size);

	// If frameLocation remains as -1 then no spot is found and we must make space.
	if(frameLocation == -1)
	{
		//printf("No frames found when looking for vpn = %d\n", vpn);
		// Getting eviction page candidate, storing frameLocation and evicting page
		evictionPageCandidate = select_page_eviction_candidate(PageTable, page_table_size);
		frameLocation = PageTable[evictionPageCandidate][3];
		result += page_evict (PageTable, page_table_size, TLB, tlb_size, FrameTable, frame_table_size, evictionPageCandidate);

		// Filling newly evicted frame
		FrameTable[frameLocation] = 1;

		// Filling newly evicted page
		PageTable[vpn][0] = 1;
		PageTable[vpn][1] = read_write;
		PageTable[vpn][2] = 1;
		PageTable[vpn][3] = frameLocation;


		result += cache_translation_in_TLB (TLB, tlb_size, PageTable, page_table_size, vpn);

		return result;
	}

	// Filling frame
	FrameTable[frameLocation] = 1;


	// Filling page
	PageTable[vpn][0] = 1;
	PageTable[vpn][1] = read_write;
	PageTable[vpn][2] = 1;
	PageTable[vpn][3] = frameLocation;

	// Filling TLB entry
	result += cache_translation_in_TLB (TLB, tlb_size, PageTable, page_table_size, vpn);

	return result;
}
//******************************************************************************
// DONE
// Clears the reference bits of the TLB and the Page Table
// Written by : Moses Quiliche
void reset_reference_bits (unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4], int page_table_size)
{
	// Clearing reference bit in the TLB (setting R = 0)
	for(int i = 0; i < tlb_size; i++)
		TLB[i][2] = 0;

	// Clearing reference bit in the PageTable (setting R = 0)
	for(int i = 0; i < page_table_size; i++)
		PageTable[i][2] = 0;

	return;
}




//******************************************************************************
//***********************  Overall  Function  **********************************
//******************************************************************************

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
// statistics [10] = Total accesses
// Simulates a memory access; updates all the data and statistics
// Written by : Jan Iglesias
void memory_access (	unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4],
						int page_table_size, unsigned int FrameTable[], int frame_table_size,
						unsigned int address, int read_write, int statistics [11])
{
	int tlbLookUpResult, validCopy, dirtyCopy, refCopy, frameCopy;
	int result = 0;
	int vpn = address / 1024;
	statistics[10]++;


	// Every 25 accesses we will reset the reference bits
	if(statistics[10] % 25 == 0)
	{
		reset_reference_bits (TLB, tlb_size,PageTable, page_table_size);
		// printf("****************PREFERENCE BITS HAVE BEEN RESET****************\n");
	}
		


	// Check if TLB contains address
	tlbLookUpResult = TLB_lookup(TLB, tlb_size, vpn);


	// printf("tlbLookUpResult = %d\n", tlbLookUpResult);
	// If tlbLookUp returns a -1 then it is a miss
	if(tlbLookUpResult == -1)
	{
		//printf("TLB Miss\n");
		//Incrementing TLB Miss and TLB Write
		statistics[1]++;
		statistics[8]++;

		// Checking if data is in Page Table
		// If it is in page table then we transfer it to TLB (Hit)
		if(PageTable[vpn][0] == 1)
		{

			//Incrementing PageTable hit
			statistics[2]++;



			// We must get the info from the pageTable and put it in the TLB
			//printf("About to go into cache_translation_in_TLB from TLB Hit\n");
			result = cache_translation_in_TLB (TLB, tlb_size, PageTable, page_table_size, vpn);



			// Shootdown was performed
			if(result == 1)
			{
				//Incrementing TLB shootdown
				// printf("Shootdown was performed on vpn = %d and page table hit\n", vpn);
				statistics[4]++;
			}

			// Getting location of TLB entry
			tlbLookUpResult = TLB_lookup(TLB, tlb_size, vpn);

			// Read
			if(read_write == 0)
			{
				TLB[tlbLookUpResult][2] = 1;	// Ref bit is 1
			}
			// Write
			else
			{
				TLB[tlbLookUpResult][1] = 1;	// Dirty bit is 1
				TLB[tlbLookUpResult][2] = 1;	// Ref bit is 1
				

			}

			return;

		}
		// It is not in page table. (Miss)
		else
		{
			// printf("Not in table\n");
			//Incrementing PageTable miss and write
			statistics[3]++;
			statistics[9]++;

			// printf("About to go into cache_page_in_RAM from Page Miss\n");
			result = cache_page_in_RAM (PageTable, page_table_size, TLB, tlb_size, FrameTable, frame_table_size, vpn, read_write);

			// Shootdown performed
			if(result & 1 == 1)
			{
				//Incrementing TLB shootdown
				// printf("Shootdown was performed on vpn = %d cuz of cache_page_in_RAM and page table miss\n", vpn);
				statistics[4]++;
			}

			// Hard drive write
			if( (result >> 1) & 1 == 1)
			{
				//Incrementing HD Write
				statistics[7]++;
			}

			// Page Eviction
			if( (result >> 2) & 1 == 1)
			{
				//Incrementing Page Eviction
				statistics[5]++;
			}

			// Getting location of TLB entry
			tlbLookUpResult = TLB_lookup(TLB, tlb_size, vpn);

			// Read
			if(read_write == 0)
			{
				TLB[tlbLookUpResult][2] = 1;	// Ref bit is 1
			}
			// Write
			else
			{
				TLB[tlbLookUpResult][1] = 1;	// Dirty bit is 1
				TLB[tlbLookUpResult][2] = 1;	// Ref bit is 1
				

			}

			//Incrementing HD Read
			statistics[6]++;

			return;
		}

	}
	// else it is a hit
	else
	{
		// Incrementing TLB Hit
		statistics[0]++;

		// Enabling valid bit
		TLB[tlbLookUpResult][0] = 1;

		// Read
		if(read_write == 0)
		{
			TLB[tlbLookUpResult][2] = 1;	// Ref bit is 1
		}
		// Write
		else
		{
			TLB[tlbLookUpResult][1] = 1;	// Dirty bit is 1
			TLB[tlbLookUpResult][2] = 1;	// Ref bit is 1
			
		}



		return;
	}
}











