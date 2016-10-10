#ifndef JOBS_H
#define JOBS_H
#include <unistd.h>
#include <sys/types.h>
struct Job {
	struct Job *prev;
	pid_t pid;
	char *name;
	struct Job *next;
};

typedef struct jobs {
	struct Job *first;
	struct Job *last;
} Jobs;


void addJob(pid_t pid, char *name);
void removePid(pid_t pid);
void myJobs();
void showErr(char *who);
#endif
