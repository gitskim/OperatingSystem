#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	// getuid = returns the real user id of the calling process. 
	// getgid = returns the real group id of the calling process.
	printf("uid = %d, gid = %d\n", getuid(), getgid());

	// result:
	// uid = 1842102106, gid = 42967409
}
