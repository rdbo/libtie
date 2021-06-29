#include <stdio.h>
#include <tie.h>
#include <unistd.h>

int
proc_callback(pid_t pid,
	      void *arg)
{
	proc_t proc;
	pid_t  curpid = *(pid_t *)arg;

	if (pid == curpid) {
		tie_openproc(pid, &proc);
		printf("PID: %d\n", proc.id);
		printf("PPID: %d\n", proc.parent);
		printf("Tracer: %d\n", proc.tracer);
		printf("Owner: %d\n", proc.owner);
		printf("Threads: %d\n", proc.threads);
		printf("State: %c\n", proc.state);
		printf("Name: %s\n", proc.name);
		printf("Path: %s\n", proc.path);
		printf("CWD: %s\n", proc.cwd);
		printf("CmdLine: %s\n", proc.cmdline);
		tie_closeproc(&proc);

		return TIE_FALSE;
	}

	return TIE_TRUE;
}

int
main()
{
	pid_t curpid;

	curpid = getpid();
	tie_enumprocs(proc_callback, (void *)&curpid);
	return 0;
}
