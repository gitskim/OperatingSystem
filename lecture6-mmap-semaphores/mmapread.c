#include "he.h"
#include <sys/stat.h>

int main() {
	int fd;
	// if file was opened read only mode, writing to mmaped region of that file would fail. 
	fd = open("/home/archie/practice/file", O_RDWR);
	struct stat fInfo = {0};
	fstat(fd, &fInfo);
	char *p = mmap(0, 1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	for (int i = 0; i < fInfo.st_size; i++) {
		fprintf(stderr, "%c", *p);
		*p = 'z'; // if mmap is only PROT_READ, this line will seg fault.
		p++; 
	}
}
