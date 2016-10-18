#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void showErr(char *who) {
	fprintf(stderr, "%s\n", who);
	exit(errno);
}

void showErrno(char *who) {
	perror(who), exit(errno);
}
