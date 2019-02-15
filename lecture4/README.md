Thu, Jan. 25th, 2018
* Back in the days, once the kernel compiles, it couldn't change. In the old days, if you want to install a device driver such as a printer or mouse, you would have to recompile the kernel and reboot. However, these days, the kernel code is modular and you can load it during runtime and no need to reboot. 

## UNIX FILE I/O

### FILE I/O System Calls

* fread, fwrite, fopen (standard library) are c language library functions, which means they are available in Windows, Linux or any OS that supports C language. 
* <code>.a</code> is for a static library
* <code>.so</code> is for a shared library
* When the compiler links, it finds libstd.a and pull out the necessary .o files in there and link with them to produce the necessary code. 
* Note that fread() would eventually have to go into hardware to read from memory. The c code in the standard library doesn't go into hardware. fread() would eventually call some underlying operating calls, which are referred to as <bold>system calls</bold>. 

* system calls: it doesn't have buffers. Buffers come from either a library or from a device level. Therefore, read(), write() don't have buffers, but fread() and fwrite() do. 

* fread() may call a window function called read-file(), but in Linux, it would be read() since unix and the language C are developed together. 

#### open

```c
#include <fcntl.h>
mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 
// bitwise OR. By or-ing them together, you are turning on certain bits.
// mode is permission. Read for the user | write for the user | read for the group | read for others => typical mode.
char *pathname = "/tmp/file";
fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode); // equivalent to fopen with the "w" mode. 
```
* open returns a non-negative value which describes what that file is. Unix sees everything a file including hardware such as a keyboard, mouse, etc. 

```c
fd = open("/var/run/myprog.pid", O_WRONLY | O_CREAT | O_EXCL, 0644); //0644 is equivalent to the mode above. By putting a 0 infront of 644, the number became an octal number.

```
* The testing if a file exists and creating a file are combined to a single atomic operation, which means there can't be an interruption between those two behaviors. 
* // O_EXCL makes open fail if it's already open. It's useful when identical multiple processes communicate together and try to open a file, only one process gets to create and open that file and other processes won't be able to open it again. 
* O_EXCL can't be used alone. opening multiple files doesn't have to be atomic. 
* This flag is used commonly with O_CREAT. You can get rid of overlap between those processes = a form of locking. ?question? lambda paradigm?
question? is it that O_EXCL makes it atomic? If O_EXCL is used with O_CREAT, testing if the file is created and creating that file are an atomic operation? Which one is atomic out of those two?

#### creat
Redundant: <code> creat(path, mode)</code> is equivalent to <code>open(path, O_WRONLY | O_CREAT | O_TRUNC, mode)</code> 

#### close
```c
int close(int fildes);
```
* File descriptors can be used again. It is a <bold>deterministic behavior</bold> = given a particular input, you will always get the same output. Some programs take advantage of it by using a pipe to transfer the file descriptor to another program. 

#### lseek
```c
off_t lseek(int fildes, off_t offset, int whence);
```
* if whence is SEEK_SET, the file offset is set to offset bytes.
* if whence is SEEK_CUR, the file offset is set to its current location plus offset. 
* if whence is SEEK_END, the offset is set to the size of the file plus offset. 

#### read
```c
ssize_t read(int fildes, void *buf, size_t nbytes);
```

* returns number of bytes read, 0 if end of file, -1 on error.

* un buffered 

* number of bytes read may be less than the requested nbyte. 

* Note that everything to unix is a file including a keyboard and stuff. So reading can read() something that's not a file. So read() is considered to be a slow system because it may block forever on a "slow" read from pipes, FIFOs (aka named pipes), sockets, or keyboard 

* Reading from a disk is an exception to the slow system. (question? how is read() not a disk I/O? 

* question? where is read in fread source code? https://github.com/lattera/freebsd/blob/master/lib/libc/stdio/fread.c

#### write

```c
ssize_t write(int fildes, const void *buf, size_t nbyte);
```

* After each write is complete, the current file offset is moved by the number of bytes wirte() completed. If the current file offset exceeds the current file size after write, the file size will increase to the current file offset.

* lseek() and write() are two different operations. If there is an interruption between those function calls, the file that was written could get messed up. In order to solve it, the positioning to the current end of file and the write need to be an atomic operationg, which means those two set behaviors occur as one and there can't be an interruption between them. It's possible by <code>O_APPEND</code> flag. 

* If the file was opened with <code>O_APPEND</code> flag, the file offset gets set to the end of the file prior to each write.

* O_APPEND is atomic. It is a mode of fopen(). When two processes are appending to the same file, they never overwrite each other. Since appending operation is atomic, moving the current file position to the end and adding it and moving the file position further, this process cannot be distrupted. lseek() and write() are not atomic, so then there might be a bug. 

* Caveat to O_APPEND: these days the place you are writing to is usually a network file system (A network file system is a network abstraction over a file system that allows a remote client to access it over a network in a similar way to a local file system.) shared by multiple machines. Instead of writing to a disk on a machine, write() operating will be transferred to packets. O_APPEND breaks in this case, because there is no simple way of implementing it. O_APPEND works the way it's supposed to only if you're writing on the physical device. If not, there might be some bugs. question? is nfs ssh? There is a modern way of doing append over NFS, but it's costly since it makes the system slow. So it's like an option that can be turned on or off. 




#### IF you have a slow system call and a signal occurs, the call returns a failure.
