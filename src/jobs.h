#ifndef JOBS_H
#define JOBS_H
#include <unistd.h>
#include <sys/types.h>
struct Job {
	pid_t pid;
	struct Job *next;
};

typedef struct {
	struct Job *first;
	struct Job *last;
} Jobs;


void addJob(pid_t pid);
void removePid(pid_t pid);
void myJobs();
void showErr(char *who);
#endif
