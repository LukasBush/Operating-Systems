/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    	static int initialized = 0;
    	static int tick = 1;
    	static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    	int proctmp;
    	int pagetmp;
	int pc; 
	int page;

    	if(!initialized){
		for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    		for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
				timestamps[proctmp][pagetmp] = 0; 
	   		 }
		}
		initialized = 1;
    	}

    	int minNum = 0;
	int posX = 0;
	int gDifference = 0;
	for(int proc = 0; proc < MAXPROCESSES; proc++)
	{		
		if(q[proc].active)
		{		
			pc = q[proc].pc;
			page = pc/PAGESIZE;
			if(!q[proc].pages[page])
			{
				if(!pagein(proc,page))
				{
					minNum = -1;	
					for(int i = 0; i < q[proc].npages; i++)
					{
						gDifference = tick - timestamps[proc][i];
						if((minNum < gDifference) && (q[proc].pages[i]))
						{
							minNum = gDifference;
							posX = i;
						}
					}
					pageout(proc, posX);
					timestamps[proc][page] = tick;
					tick++;
				}
			}			
		}
	}
}