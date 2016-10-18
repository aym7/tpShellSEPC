#include "jobs.h"
#include "err.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

Jobs *jobs = NULL;
void removeJob(struct Job *job);
void createList();

void createList() {
	jobs = malloc(sizeof(Jobs));
	if(jobs == NULL) showErr("malloc");
	jobs->first = jobs->last = NULL;
}

void addJob(pid_t pid, char *name) {
	printf("adding %d to job\n", pid);
	if (jobs == NULL) createList();

	// allocate memory
	struct Job *job = malloc(sizeof(struct Job));
	if(job == NULL) showErr("malloc");
	job->name = malloc(strlen(name)+1);
	if(job->name == NULL) showErr("malloc");

	// set job parameters
	job->pid = pid;
	strcpy(job->name, name);
	job->next = NULL;
	// add job to list
	if (jobs->first == NULL) {
	   	jobs->first = jobs->last = job;
	} else {
	   jobs->last->next = job;
	   jobs->last = job;
	}
}

void removePid(pid_t pid) {
	struct Job *parc = jobs->first;

	while (parc != NULL) {
		if(parc->pid == pid) {
			removeJob(parc);
		}
		parc = parc->next;
	}
}

void removeJob(struct Job *job) {
	if (job == jobs->first) {
		jobs->first = job->next;
	} else if (job == jobs->last) {
		// besoin ancienne valeur (job->prev ?)
		printf("hi");
	} else {
		// prev->next = job->next;
		jobs->last = job->next;
	}

	free(job->name);
	free(job);
}

void myJobs() {
	struct Job *parc = jobs->first;
//	int status;

	while (parc != NULL) {
		// TODO remove and print when terminated
#ifdef WAITPID
		if(waitpid(parc->pid, &status, WNOHANG | WUNTRACED) == -1) {
			perror("waitpid"), exit(errno);
//			showErr("waitpid");
		}
#endif
		printf("[%d] %s", parc->pid, parc->name);
#ifdef WAITPID
		if ((WIFEXITED(status) || WIFSTOPPED(status)) && WSTOPSIG(status)) { // process terminated normally
			printf("TERMINATED");
			removeJob(parc);
		}
#endif
		printf("\n");
		parc = parc->next;
	}
}
