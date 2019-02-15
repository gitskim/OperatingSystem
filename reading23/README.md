#6.1 Basic Concepts
* CPU bursts - I/O system bursts when it services requests to fetch information. 
* I/O bursts - read or writes data until the requested data is read/or written or the space to sotre it cache runs out
* Dispatcher - switching context, switching to user mode, jumping to the proper location in the user program to restart that program
* dispatcher latency - the time it takes for the dispatcher to stop one process and start another running

#6.3 Scheduling Algorithms

1. First-Come, First-Served Scheduling (FCFS)

* simple but wiating time is long

2. Shortest-Job-First-Scheduling (SJF) - shortest one first. 

* optimal but knowing the length of the next CPU request is difficult, so approximation is made. 
* Used frequently in long-term scheduling

## Priority scheduling

* SJF is a special case of priority-scheudling. The larger the CPU burst, the lower the priority. 
* Indefinite blocking or starvation could occur with low-priority processes. 
* Solution is aging, which increaes the priority of processes that wait for a long time. 

## Round-Robin Scheduling

// question?
* If a process's CPU burst exceeds 1 time quantum, that process is preempted and is put back in the ready queue. 
* preemptive
// question?
* if there are n processes in the reqdy queue and the time quantum is q, then each process gets 1/n of the CPU time in chunks of at most q time units. Each process must wait no longer than (n-1) * q

## Multilevel feedback Queue scheduling
* If a process uses too much CPU time, it will be moved to a lower-priority queue. 
