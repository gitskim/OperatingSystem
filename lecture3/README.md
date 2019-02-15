Tue Jan. 23rd, 2018

## Boot Process

Under BIOS-based systems:

0. ROM (READ-only memory) which is permanent. You bake the first program that needs to run when the computer is turned on in ROM. That program is typically called BIOS (basic input adn ouput syste). 

1. BIOS does Power-on self-test (POST), which does a basic check if something exists or not, peripheral initializations

2. BIOS jumps to the boot code in the first 440 bytes of Master Boot Record (MBR).

3. MBR boot code locates and launches boot loader such as GRUB (this is what we use) (MBR is a little bigger than BIOS, so it can do a little more that BIOS). 

4. Boot loader reads its configuration and possibly presents menu

5. Boot loader loads the kernel and launches it

* Once you choose one of the things from the menu, boot loader will load that chosen kernel

6. Kernel unpacks the initramfs (initial RAM filesystem), which has common devices drivers to allow the kernel to boot the system. (no need to know the detail sof initramfs)

### NOTE: modern os is modular. There isn't a one chunk of code that wakes up all the hardware it needs. A small portion of code loads something which loads something else, and so on. 

* initial temporary root filesystem
* contains device drivers needed to mount real root filesystem

7. Kernel switches to real root filesystem

8. Kernel launches INIT, which is the first user-level process <code>/sbin/init</code> (pid == 1)

* /sbin/init is the mother of all processes 

### Except the very first process, every other process is a clone

* traditionally SysV-style init reads <code>/etc/inittab</code>
* in Arch, <code>/sbin/init</code> is a symlink to <code>systemd</code> 

### NOTE systemd is a newer one to replace init to shorten the booting time. 

### symlink = a symbolic link that contains a reference to another file. Therefore, in Arch Linux, mixture of init and systemd exist for backward-compatibility. 


## Under Session

9. init calls getty and which calls terminal emulators and checks user name and password against <code>/etc/passwd</code>.

NOTE: init forks 6 times to have 6 instances of getty.

10. getty calls <code>login</code> program by exec-ing into it and login then runs the user's shell such as bash. 

* <code>login</code> sets the user's environment variables.
* the user's shell is specified in <code>/etc/passwd</code>

## Process Control

### Displaying process hierarchy

f - forest view
x - deamons (including my background processes)
a - all processes

```c
# ps axf to display process tree 

PID TTY      STAT   TIME COMMAND
  2 ?        S      0:00 [kthreadd]
  3 ?        S      0:00  \_ [ksoftirqd/0
  4 ?        S      0:00  \_ [kworker/0:0]
  5 ?        S<     0:00  \_ [kworker/0:0H]

	...
```

#### Swapper has the pid of 0, which is a kernel process, not a user process. Technically init is the child of this process. NOTE it's not the first user process. First user process is init. 

```c
# ps axfj to see more info

// NOTE: these processes in the brackets are children swapper and they are kernel processes. In this class, we don't focus on those. 

PPID   PID  PGID   SID TTY      TPGID STAT   UID   TIME COMMAND
   0     2     0     0 ?           -1 S        0   0:00 [kthreadd]
   2     3     0     0 ?           -1 S        0   0:00  \_ [ksofti
   2     4     0     0 ?           -1 S        0   0:00  \_ [kworker
   2     5     0     0 ?           -1 S<       0   0:00  \_ [kworker
   2     6     0     0 ?           -1 S        0   0:00  \_ [kworker

```

#### every process underneath init needs to be indented under init to indicate that it's a children of init, but direct children of init are not indented. 

```c
# ps axfjww to format the view to see all the contents even if the views wrap around. 
PPID   PID  PGID   SID TTY      TPGID STAT   UID   TIME COMMAND
   0     1     1     1 ?           -1 Ss       0   0:00 /sbin/init
   1   155   155   155 ?           -1 Ss       0   0:00 /usr/lib/systemd/systemd-journald
   1   164   164   164 ?           -1 Ss       0   0:00 /usr/bin/lvmetad -f
   1   173   173   173 ?           -1 Ss       0   0:00 /usr/lib/systemd/systemd-udevd
   1   222   222   222 ?           -1 Ss       0   0:00 /usr/lib/systemd/systemd-logind
```

```c
 145 ?        Ss     0:00 login -- jae     
 406 tty1     Ss     0:00  \_ -bash
 421 tty1     S+     0:00      \_ xinit /etc/xdg/xfce4/xinitrc -- /etc/X11/xinit/xserverrc
 422 ?        Ss     0:03          \_ /usr/bin/X -nolisten tcp :0 vt1
 425 tty1     S      0:00          \_ sh /etc/xdg/xfce4/xinitrc
 430 tty1     Sl     0:00              \_ xfce4-session
 448 tty1     S      0:00                  \_ xfwm4 --display :0.0 --sm-client-id 27c950fe4-6
 450 tty1     Sl     0:00                  \_ Thunar --sm-client-id 2ed5a75a5-2f67-42c8-9765-
 452 tty1     Sl     0:01                  \_ xfce4-panel --display :0.0 --sm-client-id 24a77
 494 tty1     S      0:00                  |   \_ /usr/lib/xfce4/panel/wrapper /usr/lib/xfce4
 498 tty1     S      0:00                  |   \_ /usr/lib/xfce4/panel/wrapper /usr/lib/xfce4
 454 tty1     Sl     0:00                  \_ xfdesktop --display :0.0 --sm-client-id 29a760d
 456 tty1     Sl     0:01                  \_ xfce4-terminal --geometry=100x36 --display :0.0
 505 tty1     S      0:00                      \_ gnome-pty-helper
 507 pts/0    Ss+    0:00                      \_ bash
 518 pts/1    Ss     0:00                      \_ bash
2330 pts/1    R+     0:00                      |   \_ ps afx
1218 pts/2    Ss+    0:00                      \_ bash
```
### graphical windowing system is called X window system, which was developed in MIT
* What we see above is XORG. 
* XORG is an implementation of the x window system. It is a server, which listenes on a few ports. It receives what to draw on your screen. a.k.a. the network client-server architecture of X window system (MAC and PC don't use this architecture)

<NOT IMPORTANT>

NOTE: ssh -X whatever -> (let's say you are using mobile xterm, which has xterm embedded) if you're running x server on your mac, the machine that you ssh'ed into will forward the stff to your mac and your mac also acts as a server.

### Creating processes

Simple shell program from APUE3, 1.6:

```c

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// handler function to register this function when SIGINT is called
static void sig_int(int signo) {
	printf("stop pressing ctr-c!\n");
}

int main() {

	// SIGINT is generated by pressing ctr+c
	if (signal(SIGINT, &sig_int) == SIG_ERR) {
		perror("signal() failed");
		exit(1);
	}

	int i = 0;

	for (;;) {
		printf("%d\n", i++);
		sleep(1);
	}
}
```

Here is the output of the program. This program can be killed by kill [pid] instead of ctr+c

```c
0
1
2
3
4
5
6
^Cstop pressing ctr-c!
7
8
9
10
^Cstop pressing ctr-c!
11
Terminated: 15
```

You can try to not be able to kill the program like the following. but there is an ultimate shortcut that kills any process and cannot be modified, kill -9. The only way to kill the following process is kill -9

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void sig_int(int signo) {
	printf("stop pressing ctr-c!\n");
}
static void sig_term(int signo) {
	printf("stop trying to kill me!\n");
}
int main() {

	// SIGINT is generated by pressing ctr+c
	if (signal(SIGINT, &sig_int) == SIG_ERR) {
		perror("signal() failed");
		exit(1);
	}

	if (signal(SIGTERM, &sig_term) == SIG_ERR) {
		perror("signal() failed");
		exit(1);
	}

	int i = 0;

	for (;;) {
		printf("%d\n", i++);
		sleep(1);
	}
}
```

#### Signal function

```c
void (*signal(int sig, void (*func)(int)))(int);
```

Note that signal's return value is a function, which you can tell if you take the <code>*signal</code> out.  

```c
void (  )(int);

```

signal has two params:

1. <code>int</code> sig
2. <code>function</code> func

```c
void (*func)(int)
```

func - it's a function that returns void and accepts int as a param
