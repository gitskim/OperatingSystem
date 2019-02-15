## Segmentation: 
It permits the physical address space of a process to be non contiguous. 

## Paging: 
To offer segmentation.

It's a memeory management scheme that eliminates the need for contiguous allocation of physical memory. 

Paging | Segmentation
--- | ---
To get a large linear address space without having to buy more physical memory | Segmentation allows programs and data to be broken up into logically independent address spaces and to aid sharing and protection. 
Paging does not distinguish and protect procedures and data separately. | Segmentation distingushes and separately protects procedures and data. 
Transparent to programmers | requires programmer to be aware of memory limits.
the physical memory is divided in blocks of equal size and a process is allocated a number of pages that is just larger than what it physically requires. So a little space is wasted at the end of the last page | segmented memory is allocated in non-uniform chunks it has the problem of “fragmentation”. 

The page size is defined by the hardware. 

# MMU: Memory Management Unit

* The mapping from virtual to physical address is done by the memory management unit which is a hardware device. 
* The Physical Address Space is divided into a number of fixed-size blocks, called frames. 
* The Logical Address Space is divded into a number of fixed-size blocks called pages. 
* Page Size = Frame Size


* Page number: Number of bits required to represent the pages in Logical Address Space or Page number - an index into a page table
* Page offset: Number of bits required to represent particular words in a page or page size of Logical Address Space or word number of a page or page offset. - physical address 
* Frame number(f): Number of bits required to represent the frame of Physical Address Space or Frame number. - an index into a page table
* Frame offset(d): Number of bits required to represent particular word in a frame or frame size of Physical Address Space or word number of a frame or frame offset. - physical address

Internal Fragmentation | External Fragmentation
--- | ---
When a process is allocated more memory than required, few space is left unused. | If many small non contiguous blocks of unused spaces, which can serve a new reqest, were attempted to be requested, there would be external fragmentation.
It occurs when memory is divded into fized-sized partitions. | It occurs when memory is divded into variable-sized partitions based on size of process. 
It can be cured by allocating memory dynamically or having paritions of different sizes. | It can be cured by compaction, paging and segmentation

# Paging disadvantages:

* It can still have internal fragmentation - process may not use memory in exact multiples of pages. 
* Memory reference overhead: 2 references are used for address lookup including page table and memory.  - Solution: Use a hardware cahce to absorbe page table lookups (Translation Lookaside Buffer TLB)
* Memory required to hold page tables can be large. OS has separate page tables per process. If there are 25 processes, then 100MB of page tables. - Solution: page the page tables.
