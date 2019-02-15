* Lazy swapping
* invalid page - the page is either not in the logical address pace of the process or is valid but currently on the disk. 

Once invalid bit is set

1. Free frame is found. 
2. disk operation to read the desired page into the newly allocated frame is schedueld
3. internal table is modified to indicate that the page is now in memory.
4. The instruction is restarted. 

* Problem: instruction gets modified when the page fault occurs.
* Solution: access both versions of before modification and after modification and before anything gets modified page fault needs to occur. 
* Solution2: if there is a page fault, all the old values are written back into memory before the trap occurs so the instruction can be restrated. 

## FIFO page replacement
* oldest page is chosen to be replaced.

## Optimal page replacement: 
replace the page that will not be used for the longest period of time. 

## LRU Page replacement:
replace the pate that was not used for the longest period of time. 

## Belady's anamoly - only for FIFO:
Increasing the cache size can increase fault rate.

## Stack algorithm - LRU
* Keep a stack of page numbers.
* Whenever a page is referenced, removed from the stack and put on the top.
* changing the six pointers at worst
