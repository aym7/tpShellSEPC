#include "jobs.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

Jobs *jobs;
void removeJob(Job *job);

void showErr(char *who) {
	fprintf(stderr, "%s", who);
	exit(errno);
}

void addJob(pid) {
	struct Job *job = malloc(sizeof(struct Job));
	job->pid=pid;
	job->next = NULL;
	if (Jobs->first == NULL) jobs->first = jobs->last = job;
	else {
	   jobs->last->next = job;
	   jobs->last = job;
	}
}

void removePid(pid_t pid) {
	struct Job *parc = jobs->first;

	while (*parc != NULL) {
		if(parc->pid == pid) {
			removeJob(parc);
		}
		parc = parc->next;
	}
}

void removeJob(Job *job) {
	if (job == jobs->first) {
		jobs->first = job->next;
	} else if (job == jobs->last) {
		// besoin ancienne valeur (job->prev ?)

	}
	else {
		// prev->next = job->next;
		jobs->last = job->next;
	}

	free(job)
}

void jobs() {
	struct Job *parc = jobs->first;
	int status;

	while (*parc != NULL) {
		waitpid(pid, &status, WNOHANG);
		if(WIFEXITED(status) || WIFSTOPPED(status)) { // process terminated normally
			removeJob(parc);
		}

		printf("%d \n", parc->pid);
		if(WIFEXITED(status) || WIFSTOPPED(status)) { // process terminated normally
			removeJob(parc);
		}
		parc = parc->next;
	}
}
