/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "variante.h"
#include "readcmd.h"
#include "jobs.h"
#include "err.h"

#ifndef VARIANTE
#error "Variante non défini !!"
#endif

/* Guile (1.8 and 2.0) is auto-detected by cmake */
/* To disable Scheme interpreter (Guile support), comment the
 * following lines.  You may also have to comment related pkg-config
 * lines in CMakeLists.txt.
 */

#if USE_GUILE == 1
#include <libguile.h>

int question6_executer(char *line)
{
	/* Question 6: Insert your code to execute the command line
	 * identically to the standard execution scheme:
	 * parsecmd, then fork+execvp, for a single command.
	 * pipe and i/o redirection are not required.
	 */
	printf("Not implemented yet: can not execute %s\n", line);

	/* Remove this line when using parsecmd as it will free it */
	free(line);

	return 0;
}

SCM executer_wrapper(SCM x)
{
	return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}
#endif


void terminate(char *line) {
#if USE_GNU_READLINE == 1
	/* rl_clear_history() does not exist yet in centOS 6 */
	clear_history();
#endif
	if (line) free(line);
	printf("exit\n");
	exit(0);
}

void execFils(char *prog, char **arg) {
	if(strcmp(prog, "jobs") == 0) { 
		myJobs();
		exit(0);
	} 
	if(execvp(prog, arg) == -1) showErrno("exec");
	puts("cette ligne ne doit JAMAIS être affichée");
}

void executer(char *line) {
	struct cmdline *cmds = NULL;
	pid_t pid = 0;
	int pipe_fd[2];

	if(!(cmds=parsecmd(&line))) { // = NULL
		perror("parsecmd"), terminate(0);
	}

	if(cmds->err) {
		fprintf(stderr, "err : %s\n", cmds->err);
		terminate(0);
	}

	for(int i=0; cmds->seq[i] != NULL; ++i) {
		if(cmds->seq[i+1]) { 
			if (pipe(pipe_fd) == - 1) showErrno("pipe error");
		}
		switch((pid=fork())) {
			case -1: showErrno("fork");
			case 0: // fils 
				 if(cmds->seq[i+1]) { // There is a next command
//					 printf("%d write-end here %s\n", pid, cmds->seq[i][0]);
					 // close pipe output
					 if(close(pipe_fd[0]) == -1) showErrno("close");
					 // our standard output go in the pipe
					 if(dup2(pipe_fd[1], STDOUT_FILENO) == -1) showErrno("dup2");
				 }

				 if(i > 0) { // There was a previous command
//					 printf("%d read-end here %s\n", pid, cmds->seq[i][0]);
					 // close pipe input
					 if(close(pipe_fd[1]) == -1) showErrno("close");
					 // our input comes from the pipe
					 if(dup2(pipe_fd[0], STDIN_FILENO) == -1) showErrno("dup2");
				 }


				 execFils(cmds->seq[i][0], cmds->seq[i]);
				 break;
			default: // père
				 // si on a eu une série de pipe et qu'on est au dernier proc
				 printf("pid %d = %s\n", pid, cmds->seq[i][0]);
				 if(!cmds->seq[i+1]) {
					 if(i > 0) {
						 // fermeture des pipe
						 if(close(pipe_fd[0]) == -1) showErrno("close");
						 if(close(pipe_fd[1]) == -1) showErrno("close");
					 }

					 if(!cmds->bg) { // task not launched in background ("&")
						 printf("I wait for pid : %d\n", pid);
						 if(waitpid(pid, NULL, WUNTRACED | WCONTINUED) == -1) showErrno("waitpid");
						 printf("I no longer wait for pid : %d\n", pid);
					 } else { 
						 addJob(pid, cmds->seq[i][0]);
					 }
				 }
		}
	}
}

int main() {
	printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
	scm_init_guile();
	/* register "executer" function in scheme */
	scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif

	while (1) {
		//		struct cmdline *l;
		char *line=0;
		//		int i, j;
		char *prompt = "ensishell>";

		/* Readline use some internal memory structure that
		   can not be cleaned at the end of the program. Thus
		   one memory leak per command seems unavoidable yet */
		line = readline(prompt);
		if (line == 0 || ! strncmp(line,"exit", 4)) {
			terminate(line);
		}

#if USE_GNU_READLINE == 1
		add_history(line);
#endif


#if USE_GUILE == 1
		/* The line is a scheme command */
		if (line[0] == '(') {
			char catchligne[strlen(line) + 256];
			sprintf(catchligne, "(catch #t (lambda () %s) (lambda (key . parameters) (display \"mauvaise expression/bug en scheme\n\")))", line);
			scm_eval_string(scm_from_locale_string(catchligne));
			free(line);
			continue;
		}
#endif
		executer(line);
#ifdef EXEC_PRINT
		/* parsecmd free line and set it up to 0 */
		l = parsecmd( & line);

		/* If input stream closed, normal termination */
		if (!l) {
			terminate(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);
		if (l->bg) printf("background (&)\n");

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
			for (j=0; cmd[j]!=0; j++) {
				printf("'%s' ", cmd[j]);
			}
			printf("\n");
		}
#endif

	}

}
