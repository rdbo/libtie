#include <stdio.h>
#include <tie.h>
#include <unistd.h>

#define MYINT_ADDR ((void *)0x4ac0f0)

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
	pid_t pid = 0;
	const int myint_val = 1337;

	curpid = tie_getpid();
	tie_enumprocs(proc_callback, (void *)&curpid);

	pid = tie_findpid("target");

	if (pid == (pid_t)-1) {
		printf("'target' is not running\n");
		return -1;
	}

	tie_attach(pid);

	tie_poke(pid, MYINT_ADDR, (void *)&myint_val, sizeof(myint_val));

	tie_detach(pid);

	return 0;
}
