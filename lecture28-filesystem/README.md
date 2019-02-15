System | pros | cons
--- | --- | ---
Contiguous allocation | easy | - you can't grow if things are right next to each other - external fragmentation
Extent-based allocation | When the file size needs to be changed, a certain size of extent is added. | - extent size is fixed, so there is still fragmentation, although you can have multiple extents. 
Linked allocation | All blocks are made up of linked lists, so there is no external fragmentation and it can be easily grown | Random access is too slow, It changes the default size of the frame (linked list overhead wastes the space)
FAT table (e.g. of linked allocation) | Blocks are NOT made up of linked list, but acts like a linked list because there is a table to keep track of the next pointer for easier access of random access. And the cache is saved for faster access. The blocks don't know about each other's pointers, but only the FAT table keeps track of how blocks are connected. **No external fragmentation** | fast random access not interely possible 
Indexed allocation | There is a dedicated block to save all the associated blocks inside an array, **FAST random access** by finding the middle of the array to go to the middle of the file, **no external fragmentation** | Since array is fixed, growing the size of the file creates internal fragmentation but no external fragmentation, sequential access may be slow because blockss are not contigous anymore
Multi-level indexed allocation | **Fast random access**, No external fragmentation| slow sequential access, large overhead

* Partition: it is to limmit the growth of the file system back in the days
* cylinder group: minimize the disk head pointer movement
* i-nodes - a data structure that contains all the meta data about your files: what time was the file accessed. It has all the data about your files, but not the file name you see
* i-node: it stores the link count that contains the number of directory entires that point to it. Only when the link cont goes to 0 can the file be deleted. This is why for the file you need to "unlink" and "delete" and for the directory you need to "unlink" to remove the directory. 
* NOTE: the real identifier for a file is a number, which is an index into the array. file names are 0 - 99 if there are 100 files. It's not the string file name. 
* NOTE: directory is a file and every directory has "." and ".." files by default. 

## Hard link vs Soft link
* Each file name is nothing but a **hard link** to a file. For examples, two different file names under the same directory can point to the same file.

* e.g. ln /home/archie/practice/dirs/file1 /home/archie/practice/dirs/hard   file1 content is now pointed to by another name which is hard

* **Symbolink link** stores the name of the file that the link points to as the contents of the file. 

```c
ln -s file1 soft
```

```c
total 16
drwxr-xr-x 2 archie users 4096 Apr 12 04:45 .
drwxr-xr-x 3 archie users 4096 Apr 12 02:56 ..
-rw-r--r-- 2 archie users    6 Apr 12 03:02 file1
-rw-r--r-- 2 archie users    6 Apr 12 03:02 hard
lrwxrwxrwx 1 archie users    5 Apr 12 04:45 soft -> file1
```

NOTE: soft link's file count is the same as the file **name** count that it points to.

The i-node type of symbolic links is S_IFLINK and that's how the system knows that this is a symbolic link and it opens the file that its content has the name of. 

QUESTION: does a symbolic link file also have a data block to store the file name content/

```c
// result of ls -lai

total 16
682762 drwxr-xr-x 2 archie users 4096 Apr 12 06:13 .
658839 drwxr-xr-x 3 archie users 4096 Apr 12 02:56 ..
683219 -rw-r--r-- 2 archie users    7 Apr 12 04:47 hard
683219 -rw-r--r-- 2 archie users    7 Apr 12 04:47 longlong
683251 lrwxrwxrwx 1 archie users    6 Apr 12 05:25 randomsoft -> random
683250 lrwxrwxrwx 1 archie users    8 Apr 12 04:48 soft -> longlong
```
* NOTE: after creating the hard link to longlong, the count for both hard and longlong went upto 2
* NOTE: symbolic link doesn't add to the count of the links

```c
unlink filename
```

it removes the link of the file and if the link count == 0, it removes the file. 


## Directory
* It only stores the filename and the i-node number.
* Its link depends only on parent (1), itself (1), and children(n), so it at least has 2 link counts 
* Direcotry entry's inode number **ino_t** cannot point to an inode in a different file system.
* QUESTION: "Because the i-node number in the directory entry points to an i-node in the same file system, a directory entry can’t refer to an i-node in a different file system. This is why the ln(1) command (make a new directory entry that points to an existing file) can’t cross file systems. We describe the link function in the next section."
* One can create a symbolink link for a directory, but one can't create a file (hard link) to link a directory
```c
ln: dir: hard link not allowed for directory
```



