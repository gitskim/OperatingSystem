## 9.4 Process Groups

Each process belongs to a process group. 


Each process group can have a process group leader. - the leader is its process group id being equal to its pid.

## 10.8 Reliable-Signal Terminology and Semantics

	E.g. of signal generating events
1. a hardware exception (e.g. division by 0)
	2. an alarm timer expiring
	3. a terminal generated signal
	4. a call to the kill function

## 10.9 kill and raise functions

	kill() - sends a signal to a process or a group of processes.

	a signal is delievered to the process before kill returns. 

	if pid < 0, the signal is set to all processes whose process group ID equals the absolute value of pid and for which the sender has persmission to send the signal. -> so pretty much a leader can send it to all its followers. 

	```c
#include <signal.h>
	int kill(pid_t pid, int signo);

	// kill(getpid(), signo);
	```

	raise() - allows a process to send a signal to itself. 

	```c
#include <signal.h>
	int kill(pid_t pid, int signo);

	// raise(signo);
	```

## 10.10 alarm and pause functions

	alarm - set a timer. when the timer goes off, the SIGALRM is generated. If we ignore or don't catch this signal, its default action is to terminate the process. 

	alarm(0) cancels the previous alarm if one was set.

	```c
#include <unistd.h>

	unsigned int alarm(unsigned int seconds);
	```

	pause() suspends the calling process until a signal is caught. 
	```c
#include <unistd.h>
	int pause(void);
	```
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

	static void sig_alarm(int signo) {
		// do nothing
	}

unsigned int sleep(unsigned int seconds) {
	if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
		return(seconds);
	}
	alarm(seconds);
	pause();
	return(alarm(0));
}

int main() {
	sleep(2);
}

3 problems:

1. If the caller already has an alarm set, it wipes it out. 
2. Others can't use this function. -> solution: we should save the state when this function is called and restore it when it's done. 
3. race condition: question? I am not sure if I understood it correctly. originally it's supposed to be 1. alarm 2. pause (until handler is caught) 3. handler but it could be 1. alarm 2. handler 3. pause. If ther is no other signals being sent, the program pauses forever. 

```c
static void sig_alrm(int);

	int
main(void)
{
	int     n;
	char    line[MAXLINE];

	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	alarm(10);
	if ((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
		err_sys("read error");
	alarm(0);

	write(STDOUT_FILENO, line, n);
	exit(0);
}

	static void
sig_alrm(int signo)
{
	/* nothing to do, just return to interrupt the read */
}
```
1. race condition: the alarm can be missed between alarm(10) and read()

2. This doesn’t work if slow system calls are automatically restarted

question? I don't see how this alarm actually does something useful. I understand there could be a race condition,but it doesn't matter, because alarm is not doing anything useful?
