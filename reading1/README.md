APUE: chapter 1, 2.3, 2.4
OSCE2/OSC9: 1.2, 1.4, 1.5, 1.8.3, 2.7, 2.10
LKD: chapter 1

What is register? It's a special, high-speed storage area within the CPU. A process register (CPU register) is a place to hold data such as instruction, a storage address. A register is 64 bits in lenght in 64-bit computer.

Cache = RAM (Random Access Memory)

* GNU GRUB: GNU Grand Unified Bootloader == is a boot loader package from the GNU project. 
* BIOS: Basic Input Output System = is software stored on a small memory chip on the motherboard. It instructs the computer on how to perform a number of basic functions such as booting and keybaord control. BIOS is responsibile for the POST and therefore it is teh very first software to run when a computer is started.  
* Motherboard: It connects all parts of a computer together including CPU, memory (RAM), hard drives. 
* POST (Power On Self Test) is the intial set of diagnostic tests performed by the computer right after it's powered on. 
* A computer peripheral is a device that is connected to a computer but is not part of the computer such as monitor, printer and 
scanner. 

disk drive is a magnetic plate. and bios can locate the first sector of the disk.  MBR = is the first portion of the disk. It is a little bigger than bio so itcan do more. 
when you pick a kernel, it loads the kernel and luacnehs it
Kernel doesn't have everything you need to have to run on a hardware, which would make kernel way too big. so modern kernels are modular
Details don't matter. Once the kernel runs, it creates the first level process. init() is as user level process.


mklable msdos = mklabel creates a new disk label, of type msdos. mac disk label is only for MacOS whcih only undersands the mac disk label. question? why msdos level? is MSDOS for any BIOS system?

parted /dev/sda -> calling a program named "parted" to partition the /dev/sda 
mklabel msdos -> a new disk level
mkpart primary ext4 0% 100% -> this is wehre a real partition happens. We created a "primary" partition which will fill up 100% of the disk and let it know later we will format it as an "ext4" file system type. 
set 1 boot on -> this partition is now bootable. 
print 
quit

question? if fork failed, can the log file still exist?
question? what happens when both child and parent try to write to the file at the same time?

APUE

1.2 UNIX Architecture
Kernel: software that controls the hardware resources of the computer and provides an environment under which programs can run. 
OS: It is a combination of kernel, system utilties, applications, shells, libraries of common functions and so on. 

![kernel]("../img/kernel.png")


![kernel2]("/../img/kernel2.png")

e.g. Linux is the kernel used by the GNU OS. 

Shells: A shell is a command-line interpreter that reads user input and executes commands. The user input to a shell is normally from the terminal (an interactive shell) or from a file (a shell script). The shell was standardized in the POSIX 1003.2 standard. 

1.4 Files and Directories

File system: Everything starts in teh directory called **root**, whose name is the single character "/"

* stat and fstat return a struct of info containing all attributes of a file. Attributes of a file are info such as the type of file, the size of the file, the owner of the file, permissions for the file, etc. It can be used like this

```c
struct stat sb;
if (stat(argv[1], &sb) == -1) {
	perror("stat");
}
```

1.5 Input and Output

File Descriptors: Small non-negative integers that the kernel uses to identify the files accessed by a process. Whenever it opens an existing file or creates a new file, it returns a file descriptor. 

stdin, stdout, stderr: By convention, all shells open three descriptors whenever a new program is run: stdin, stdout, stderr. Note that in c, everything is a file including keyboard, monitor, etc. All three file descriptors are connected to the terminal. 

Unbufferred I/O: It is provided by the functions open, read, write, lseek, and close. 

Standard I/O: With Unbeffered I/O, one needs to choose buffer sizes by themselves. For example, read() reads a specified number of bytes. However, with standard I/O, the buffer size is chosen. For example, fgets reads line by line as wella s printf. 

## 1.6 Programs and Processes

### Program and a process: 
A program is an executable file residing on disk in a directory. A program is read into memory and is execeuted by the kernel as a result of one of the seven exec functions. An executing instance of a program is called a process. Every process has a unique numeric identifier called the process ID, which is a non-negative integer. 

### Process Control: 
There are three primary functions for process control: fork, exec and waitpid. Note: exec has seven variants. 

### Threads and Thread IDs: 
A process can have multiple threads. Multiple threads of control can exploit the parallelism possible on multiprocessor systems. All threads within a process share the same address space, file descriptors, stacks, and process-related attributes. Each thread executes on its own stack, although any thread can access the stacks of other treads in the same process. The threads need to synchronize access to shared data among themselves to avoid inconsistencies. Threads are identified by IDs like processes. 

## 1.7 Error Handling
UNIX system functions return a negative value on error, and the integer <bold>errno<bold> is set to a value that tells why. 

e.g. the open function returns 15 possible errno values, such as file doesn't exist, permission problem and so on. 

Exceptions to this would be returning a null pointer to indicate error when it comes to functions returning a pointer. (READ up to the first page. Skipping the rest)

## 1.8 User Identification

### User ID: 
The user ID is a numeric value that is assigned by the system administrator. It's normally assigned to be unique for every user and cannot be changed. The User ID of 0 is either root or the super user. We refer to the special privileges of this user as superuser privileges. (NOTE: client versions of mac os x ship with the superuser account disabled while server versions ship with the account already enabled)

### Group ID: 
Group ID is also assigned by the system administrator. Grouping allows the sharing of resources such as files. There is a group file that maps group names into numeric group IDs.

## 1.9 System Calls and library functions

All OS provide API which programs request services from the kernel. 

UNIX based systems provide a well-defined limited number of kernel calls, which are called system calls. 

Difference between system calls and library functions: Library functions aren't entry points intot he kernel, but may invoke one or more of the kernel's system calls. To the user's perspective, there isn't much difference between system calls and library functions. They are both c functions. 
