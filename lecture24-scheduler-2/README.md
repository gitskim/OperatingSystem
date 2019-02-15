Mar. 29t, 2018
## Normal processes
* It goes through all processes and bump up the priority based on its age, so that older processes have a chance to run. 

## Inversion
* Process 1 has a priority 1 and it holds a lock. And p3 comes in, which has a priority 3, but it needs to wait for Porc1, although it has a higher priority, because Proc1 holds a lock and p3 needs to hold a lock as well. Then Proc2 comes in, which has a priority of 2 and doesn't need to hold a lock. It has a higher prioirty than p1 and doesn't need to hold the lock, so it preempts p1. Then p3 ends up waiting for both p1 and p2, both of which have a lower priority than p3. In order to solve this, p1's priority is boosted up temporarily as soon as a higher-prority process comes in and tries to hold the lock that p1 already holds, so that p1 can finish its job and p3 can run.

## Typical scheduling algorithms have multiple queues. 
* Except BFS which only has a single linked list.
* Other scheduling algorithms use multiple queues. 


Tue Apr 3, 2018

## Real-time scheduling
* Hard real-time systmes: It needs to finish a task within a guaranteed amount of time. (Linux does not do that or any other os)
* Soft real-time computing: critical processes are prioritized. (Linux does that)

## Linux Scheduling Priorities:
* 100 static priority levels from 1 to 99, whicch makes 100 linked lists from 1 to 99 for tasks
* you can have FIFO and RR together. For FIFO, it won't be preempted, but for RR it would be preempted. 
* Even for high priority processes, if it needs to do I/O, it would sleep because it voluntarily does so.
* 40 nice values are dynamic priority because they change with aging. negative nice values require root user. 
* **SCHED_BATCH**: CPU bound processes
* **SCHED_IDLE**: lower priorty

## Completely Fair Scheduler
* **schedule latency**: It is not productive because it spends time scheduling
* Every process gets a fair chance by being run once per schedule latency period. 
* e.g. if schedule latency is 10 ms, it would try to run all the processes within that scheduling latency. 
* **Time slice for process**: Scheduling latency * weight / (Sum of all weights)
* If schedule latency is too small, it's not going to work, because each process doesn't get enough time to overcome switching overhead time
* So, **schedule latency = lower bound * number of proc**
* **vruntime** is executed time adjusted by its weight. [executed time / weight]
* lower vruntime task is preferred. 
* 
### Picking the next process
* Find proecss with weighted minimum runtime (vruntime) so far, which is on the left of the tree
*  
