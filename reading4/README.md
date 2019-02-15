Reading:
APUE 10.1 - 10.6 (skim signal descriptions in 10.2)
APUE 3.1 - 3.13
APUE 8.1 - 8.10:

## 10.3 signal function

```c
#include <signal.h>
void (*signal(int signo, void (*func)(int)))(int);

// Returns: previous disposition of signal (the signal function is defined vaguely by ISO C so it's useless for UNIX systems. The derivatives of UNIX System V support the signal function. ) if OK, SIG_ERR on error
```

* signo is the name of the signal from the list. The value func is a signal handler. The signal function returns a function pointer that returns nothing. 
//User defined signals means that these signals have no definite meaning unlike SIGSEGV, SIGCONT,  SIGSTOP, etc. The kernel will never send SIGUSR1 or SIGUSR2 to a process, so the meaning of the signal can be set by you as per the needs of your application. 

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

static void sig_user(int); //a handler function for both signals

int main() {
	if (signal(SIGUSR1, sig_user) == SIG_ERR)
		perror("can't catch SIGUSR1");
	if (signal(SIGUSR2, sig_user) == SIG_ERR)
		perror("can't catch SIGUSR2");
	for (;;)
		pause();
}

static void sig_user(int signo) {
	if (signo == SIGUSR1) {
		printf("received SIGUSR1\n");
	} else if (signo == SIGUSR2)
		printf("received SIGSUR2\n");
	else 
		perror("bad signal");
}

```
NOTE: kill function does not kill but instead it sends a signal.

```c
$ ./a.out & NOTE: "&"start process in background job and the shell prints job number and process ID send it SIGUSR1

-> result: [1] 7216

$ kill -USR1 7216 

-> result: received SIGUSR1 

$ kill -USR2 7216 

-> result: received SIGUSR2 

$ kill 7216

-> result: [1]+ Terminated
```

### Program Start-up
When a program starts, all signals are set to their default action, unless the process that calls exec is ignoring the signal. Note that child process inherits from its parent. Also note that when exec is called to launch a new program, the program replaces the entire process. Therefore, a signal that is being caught by a process that calls exec cannot be called by the same function in the new program.

## 10.4 Unreliable Signals
Earlier versions of the UNIX system had unreliable signals. There were bugs and the signals couldn't be turned off. They could only be ignored. 

## 10.5 Interrupted System Calls
A characteristic of earlier UNIX systems was that if a process caught a signal during a slow system call, the system call in the kernel was interrupted. The kernel system call returned an error because since the process caught the signal, there is a good chance that should happen to get the slow process going. So the system calls are divided into two categories: the "slow" and all the others. The slow system calls are made for the slow speed of slow processes, so it can block forever until the process is over. For example, opening on certain file types that block the caller until some condition occurs. An exception to these slow system calls is I/O stuff (not read or write of a disk file). For example, if waiting on a terminal input was a slow system call, and the user never gives an input to the terminal, it would block the entire process unless the system is taken down. That's why I/O is an exception. 

Note that sigaction() is always preferred over signal().

signal()|sigaction()
---|---
siganl() does not necessarily block othe rsignals from arriving while the current handler is executing|sigaction() can block other signals until the current handler returns.
signal() resets the signal action back to SIG_DFL(default) for almost all signals. signal() handler must reinstall itself after its first action.The time between the signal is detected and the handler is reinstalled will allow the default behavior for signals. | sigaction() does not default after the first action. 
the exact behavior of signal() varies between systems and the standards permit those variations | sigaction() behavior does not differ

## 10.6 Reentrant Functions
Reendtrant Functions: One needs to be careful when calling functions in signal handlers, since one doesn't know when a signal handler will be called. There are functions that are guaranteed to be safe to call from within a singal handler and they are called "reentrant functions"

## 3.2 File Descriptors
To the kernel, all open files are referred to by file descriptors, which is a non-negative integer. 
* 0: stdin by default
* 1: stdout by default
* 2: stderr by default

## 3.3 open and openat Functions

A file is opened or created by calling either the open function or the openat function.

```c
#include <fcntl.h>

int open(const char *path, int oflag, ... /* mode_t mode */ );
int openat(int fd, const char *path, int oflag, ... /* mode_t mode */ );
// both return: file descriptor if OK, -1 on error
```
The file descriptor returned by open and openat are guaranteed to be the lowest numbered unused descriptor. 

#### last argument is "...", because the number of arguments may vary, which means in this case the last argument mode is optional. 

* openat is added later and it makes multiple trheads in the same process work in different directories and open a file using a relative path in whatever directory it's in. 

* Also, it provides a way to avoid time-of-check-to-time-of-use(TOCTTOU) errors. TOCTTOU means two files are trying to change one thing at the same time when that thing being changed is not atomic. 
