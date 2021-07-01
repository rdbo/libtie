#ifndef PROC_H
#define PROC_H

#define PROCFS_DIR "/proc"

#include "common.h"
#include <pwd.h>
#include <sys/types.h>

typedef struct proc_t {
	pid_t id;
	pid_t parent;
	pid_t tracer;
	uid_t owner;
	int   threads;
	char  state;
	char *name;
	char *path;
	char *cwd;
	char *cmdline;
} proc_t;

TIE_API int 
tie_enumprocs(int( *callback)(pid_t   pid, 
			      void   *arg),
	      void *arg);

TIE_API int 
tie_openproc(pid_t   pid, 
	     proc_t *proc);

TIE_API void 
tie_closeproc(proc_t *proc);

TIE_API pid_t 
tie_getpid(void);

TIE_API pid_t 
tie_findpid(const char *name);

#endif
