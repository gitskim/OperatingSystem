### 17.2 UNIX Domain Sockets

* a cross between sockets and pipes - network-oriented socket nterfaces or socketpair function

```c
int socketpair(int domain, int type, int protocol, int sv[2]);

// returns 0 if ok, -1 on error
```

### 17.4 Passing File Descriptors

* Each process has its own file table entry
* After fork(), two processes share an open file table
* When descriptor is passed from one process to another - the process that passed the descriptor closes the descriptor. Closing the descriptor doesn't close the file, because if the descriptor is open by another process, it's considered open. 
