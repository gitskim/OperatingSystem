Thu Jan. 18th, 2018 
OS
- dictator = fair resource share among programs. An infinite while loop will be stopped by a cpu
- servant = providing easy abstract interface so that programs don't have to care about details = OS provides an astract interfacae to send something to print to printers - so the types of printers don't matter

- How does CPU - an oscillator chip next to CPU which keeps generating interrupt signals to interrpt the cPU, and it gets interrupted 1000 times a second. CPU 3ghz - cpu can execute 3 billion times in a second. interrupt service . scheudler will keep track of how long a program has been running. it's going to jump to a different location in the memory and run another program. 
- there should exist memory limitation for programs to have access to for security reasons. no two programs should know each others memory unless one wants to share its memory. Kernel can access all the programs whlie individual programs cannot. If a program is runningi on a kernel mode, it can do anything. only os runs on a kernel mode and the rest runs on user mode. User program can't have direct access to hardware stuff, it uses api that os provides such as reading from microphones and requests it. Those services are called system calls. 

<History>
- MS DOS ran at the same privlege level as the user programs. and only one program could run. 
- 1945 is when it starts - no monitor but punch cards to give a program 
- 1970 is the start of a modern computer for that reason, time() gives you seconds passed since 1970 jan 1. Multics OS is worked on by K&R, so they cerated UNIX. UNIX was the first OS written in high level language C. prior to that OS was written in machine code. DR&KT invented C so that they can write OS. The invention of pointers made it possible for c to do a lot of things assembly language can do. 
- 4.4BSD added TCP IP in unix
- BDS - you can do whatever you want. GPL- you have to publish your source code. 
- Bill Gates bought QDOS quick and dirty OS and customized for the IBM hardware. He maintained the license which made it


