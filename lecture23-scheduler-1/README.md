## Kernel Premption: 

The process that was running goes ot sleep and taken out of the CPU (it may be still in the run queue). It is the act of interrupting a process without its involvement. It's different from context switch, because context switch is to move from one process to another process. Kernel preemption is one process is replaced by something else which doesn't have to be a process. Context switch is a consequence of kernel preemption.

## NOTE: jumping to hardware interrupt is not kernel preemption. Jumping to another process is an example of kernel preemption.
 
Difference between disabling kernel preemption and disabling interrupt are different.

## Relationship between spin lock and kernel preemption:

<code>preempt_count</code> is a global variable which is located in each task and lets the task pre-emptivble if the value is 0. The count is incremented every time a lock is held and decremented when the lock is released. The value of 0 represents that there are no locks currently being held, which enables preemption for the process. 

In **uni-processors**, all a spin lock does is **disable kernel preemption**.

Spin locks do not have to disable interrupts as long as interrupt handlers do not touch the data structure that it protects. 

## Difference between preemptive kernel and non-preemptive kernel: 

preemptive kernels have an intterupt which cna fire in between any two assembly instructions. In a non-premptive kernel, the running process must call a <code>yield()</code> to allow the other threads to run. <code>yield()</code> stores the state of the processor.

Symmetric Multiple Processing (SMP) = Multiple CPUs running at the same time

## <code>schedule()</code> is appropriate in the following cases to let preemption, if <code>TIF_NEED_RESCHED</code> is set:

(scheduler decided that a process of a higer priority needs to run, then ti will set TIF_NEED_RESCHED. Once it's set, if there was a timer interrupt, instead of returning back to the existing process, it would make the higher priority process execute. 

1. Returning to user space from a system call
2. Returning to user space from an interrupt handler. 

e.g. there was a timer interrupt and the scheduler checks the TIF_NEED_SCHED and if ti's set it can move onto another process with schedule().

3. Returning to kernel from an interrupt handler only if <code>preempt_count</code> is zero

e.g. there was a need to shcedule another process inside the interrupt handler and the preempt_count was zero, another process would run with schedule(). If the preempt_count is not zero, even if the flag is set, preemption will not happen.

4. <code>preempt_count</code> just became zero - right after spin_unlock()

5. Thread running in kernel mode calls schedule() itself - blocking syscall, for example


## Dispatcher:
* Low-level mechanism
* Responsibility: context switch

## Scheduler
* High-level policy
* Responsibility: deciding which process to run

## Coudl have an allocator for CPU as well
* Parallel and distributed systems. 

## timeslice:
The time a process runs before it is preempted is predetermined. 

## Policy:
* "It's the behavior of the scheduler that determines what runs when. A scheduler's policy often determines the overall feel of a system and is responsible for optimally utilizing processor time."

## SJF
* Shortest job first 
* It can be either premptive or non-preemptive. If it's not nonpreemptive - very optimal with no preemption
* Not practical: it's only possible by predicting future from the past.
* It includes SRTF

### SRTF 
* Shortest reaminig time first
* SJF with preemption

## RR
* If two jobs have the same prioriity and arrived together, FCFS
* Low average waiting time when job lengths vary widely but poor average waiting time if job lengths are similar

## Scheduler decisions
1. Switching from running to waiting state: 
* e.g. Waiting for I/O, called a system call
* This is **NON-PREMPTIVE, voluntary** because preemption is putting the runinng task to the end of run queue like HW6's "requeue" function.
2. Switching from running to ready state: 
* e.g. timer interrupt: while it was running it got interrupted. 
* **Preemptive, non-voluntary**
3. Switching from waiting to ready
* e.g. The process was waiting for I/O
* **Preemptive, non-voluntary** 
* After waiting I/O is now ready.
4. Process terminates
* e.g. exit() got called 
* This is **NON-PREMPTIVE, voluntary**
* question: definition of preemption: is it voluntary or non-voluntary or is it that a process was either running or in the ready state and now it's put in the back of the run queue?

* O(1) scheduler favored I/O bound processes. O(1) had a loophole that one can write a program that does fake I/O from all the time, even if you do CPU intensive program all the time. 
