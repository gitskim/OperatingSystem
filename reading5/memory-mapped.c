#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define COPYINCR (1024*1024*1024) // 1GB

size_t getFilesize(const char* filename) {
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}
int main(int argc, char** argv) {
	int fdin, fdout;
	void *src, *dst;
	size_t copysz;
	struct stat sbuf;
	off_t fsz = 0;

	if (argc != 3) perror("error");

	if ((fdin = open(argv[1], O_RDONLY)) < 0) 
		perror("can't open");

	if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, FILESEC_MODE)) < 0)
		perror("can't create");

	// need size of the input file
	if (fstat(fdin, &sbuf) < 0) perror("fstat error");

	// set output file size. truncates the file indicated by the file descriptor to the indicated length. 
	if (ftruncate(fdout, sbuf.st_size) < 0) perror("ftruncate error");

	while (fsz < sbuf.st_size) {
		if ((sbuf.st_size - fsz) > COPYINCR)
			copysz = COPYINCR;
		else 
			copysz = sbuf.st_size - fsz;

		if ((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fdin, fsz)) == MAP_FAILED) perror("mmap failed");

		if ((dst = mmap(0, copysz, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, fsz)) == MAP_FAILED) perror("mmap error");

		memcpy(dst, src, copysz);

		// un map the memory region
		munmap(src, copysz);
		munmap(dst, copysz);

		fsz += copysz;
	}
}
