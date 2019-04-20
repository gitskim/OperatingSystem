Loop device is a fake device that acts as block-based device so that one can mount a file like .iso as entire filesystem. 

* dd if=/dev/zero of=./ext2.img bs=1024 count=100
* Reading 1024 bytes of /dev/zero 100 times to create 100 of ./ext2.img

* losetup --find --show ext2.img
* find the first unused loop device for ext2.img 

* mkfs -t ext2 /dev/loop0 
* Build a linux file system

* mount /dev/loop0 ./mnt
* // if the file contains an entire file system, the file may then be mounted as if it were a disk device.
* /mnt doesn't count the nubmer of files, but it only counts the number of folders. Its subdirectory counts the number of files as well. 

```c
# mkdir mnt
# mount /dev/loop0 ./mnt
# cd mnt
# ll
total 17 // total number of file system blocks, including indirect blocks, used by the listed files.
// it's actualy 17k. if you do ls -larth you will see how many kilobytes each file is taking up
drwxr-xr-x 3 root root   1024 Apr 20 11:19 ./
drwxr-xr-x 5 jae  users  4096 Apr 20 11:20 ../
drwx------ 2 root root  12288 Apr 20 11:19 lost+found/
// On Linux, the fsck command—short for “file system check”—examines your file systems for errors. fsck may find bits of “orphaned” or corrupted files in the file system. If it does, fsck removes those corrupted bits of data from the file system and places them in the lost+found folder.

vm04 /home/jae/tmp/mnt # mkdir sub2
vm04 /home/jae/tmp/mnt # ll
total 18
drwxr-xr-x 4 root root   1024 Apr 20 11:26 ./
drwxr-xr-x 5 jae  users  4096 Apr 20 11:20 ../
drwx------ 2 root root  12288 Apr 20 11:19 lost+found/
drwxr-xr-x 2 root root   1024 Apr 20 11:26 sub2/
vm04 /home/jae/tmp/mnt # cd sub2
vm04 /home/jae/tmp/mnt/sub2 # ll
total 2
drwxr-xr-x 2 root root 1024 Apr 20 11:26 ./
drwxr-xr-x 4 root root 1024 Apr 20 11:26 ../
vm04 /home/jae/tmp/mnt/sub2 # mkdir sub2.1
vm04 /home/jae/tmp/mnt/sub2 # ll
total 3
drwxr-xr-x 3 root root 1024 Apr 20 11:27 ./
drwxr-xr-x 4 root root 1024 Apr 20 11:26 ../
drwxr-xr-x 2 root root 1024 Apr 20 11:27 sub2.1/
vm04 /home/jae/tmp/mnt/sub2 # touch file2.1
vm04 /home/jae/tmp/mnt/sub2 # ll
total 3
drwxr-xr-x 3 root root 1024 Apr 20 11:28 ./
drwxr-xr-x 4 root root 1024 Apr 20 11:26 ../
-rw-r--r-- 1 root root    0 Apr 20 11:28 file2.1
drwxr-xr-x 2 root root 1024 Apr 20 11:27 sub2.1/
# cd ../../
# umount mnt
# losetup --find
/dev/loop1
# losetup --detach /dev/loop0
# losetup --find
/dev/loop0
# ll mnt/
total 8
drwxr-xr-x 2 root root  4096 Apr 20 11:20 ./
drwxr-xr-x 5 jae  users 4096 Apr 20 11:20 ../
```
