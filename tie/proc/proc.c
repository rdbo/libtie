#include "proc.h"
#include "io/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define _RETCHK(fn) \
	retchk = fn(proc);\
	if (retchk != TIE_OK) {\
		tie_closeproc(proc);\
		return retchk;\
	}
#define RETCHK(fn) retchk |= fn(proc);
#define PROC_FREE(ptr, type) if (ptr) { free(ptr); ptr = (type)TIE_NULL; }
#define PROC_FFREE(fmap) if (fmap) { tie_funmap(fmap); fmap = (char *)TIE_NULL; }

struct _findpid_args {
	pid_t pid;
	const char *name;
};

static int 
read_exe(proc_t *proc);

static int 
read_comm(proc_t *proc);

static int 
read_cwd(proc_t *proc);

static int 
read_status(proc_t *proc);

static int 
read_cmdline(proc_t *proc);

static int 
findpid_callback(pid_t pid,
		 void *arg);

TIE_API int 
tie_enumprocs(int( *callback)(pid_t   pid, 
			      void   *arg),
	      void *arg)
{
	DIR *dir;
	struct dirent *pdirent;

	if (!callback)
		return TIE_ARGS;

	dir = opendir(PROCFS_DIR);

	if (!dir)
		return TIE_PROC_DIR;

	while ((pdirent = readdir(dir))) {
		pid_t pid = (pid_t)atoi(pdirent->d_name);

		if (pid || !strcmp(pdirent->d_name, "0")) {
			if (callback(pid, arg) != TIE_TRUE)
				break;
		}
	}

	closedir(dir);

	return TIE_OK;
}

TIE_API int 
tie_openproc(pid_t   pid, 
	     proc_t *proc)
{
	int retchk = 0;

	if (!proc)
		return TIE_ARGS;
	
	memset((void *)proc, 0x0, sizeof(*proc));
	proc->id = pid;

	RETCHK(read_exe);
	RETCHK(read_comm);
	RETCHK(read_cwd);
	RETCHK(read_status);
	RETCHK(read_cmdline);

	return retchk != TIE_OK ? TIE_PROC_FS : TIE_OK;
}

TIE_API void 
tie_closeproc(proc_t *proc)
{
	if (!proc)
		return;

	proc->id = proc->parent = proc->tracer = 0;
	proc->owner = 0;
	proc->threads = 0;
	proc->state = '\x00';
	PROC_FREE(proc->path,    char *);
	PROC_FREE(proc->cwd,     char *);
	PROC_FREE(proc->name,    char *);
	PROC_FFREE(proc->name);
	PROC_FFREE(proc->cmdline);
}

TIE_API pid_t 
tie_getpid(void)
{
	return getpid();
}

TIE_API pid_t 
tie_findpid(const char *name)
{
	struct _findpid_args args;
	args.pid = -1;
	args.name = name;

	tie_enumprocs(findpid_callback, (void *)&args);
	return args.pid;
}

static int 
read_exe(proc_t *proc)
{
	char exe_path[64] = { 0 };

	if (!proc)
		return TIE_ARGS;

	snprintf(exe_path, TIE_ARRLEN(exe_path) - 1,
		 "%s/%d/exe", PROCFS_DIR, proc->id);
	
	if (!tie_flink(exe_path, &proc->path))
		proc->path = (char *)TIE_NULL;

	return proc->path ? TIE_OK : TIE_PROC_FS;
}

static int 
read_comm(proc_t *proc)
{
	char comm_path[64] = { 0 };
	size_t size;

	if (!proc)
		return TIE_ARGS;

	snprintf(comm_path, TIE_ARRLEN(comm_path) - 1, 
		 "%s/%d/comm", PROCFS_DIR, proc->id);
	
	if (!(size = tie_fmap(comm_path, &proc->name)))
		proc->name = (char *)TIE_NULL;

	proc->name[size - 1] = '\x00';

	return proc->name ? TIE_OK : TIE_PROC_FS;
}

static int 
read_cwd(proc_t *proc)
{
	char cwd_path[64] = { 0 };

	if (!proc)
		return TIE_ARGS;

	snprintf(cwd_path, TIE_ARRLEN(cwd_path) - 1,
		 "%s/%d/cwd", PROCFS_DIR, proc->id);
	
	if (!tie_flink(cwd_path, &proc->cwd))
		proc->cwd = (char *)TIE_NULL;

	return proc->cwd ? TIE_OK : TIE_PROC_FS;
}

static int 
read_status(proc_t *proc)
{
	char status_path[64] = { 0 };
	char *status_file;
	char *ptr;

	if (!proc)
		return TIE_ARGS;

	snprintf(status_path, TIE_ARRLEN(status_path) - 1, 
		 "%s/%d/status", PROCFS_DIR, proc->id);

	if (!tie_fmap(status_path, &status_file))
		return TIE_PROC_FS;
	

	ptr = strstr(status_file, "State:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->state = *ptr;

	ptr = strstr(ptr, "PPid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->parent = (pid_t)atoi(ptr);

	ptr = strstr(ptr, "TracerPid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->tracer = (pid_t)atoi(ptr);

	ptr = strstr(ptr, "Uid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->owner = (uid_t)atoi(ptr);

	ptr = strstr(ptr, "Threads:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->threads = (int)atoi(ptr);

	tie_funmap(status_file);

	return TIE_OK;
}

static int 
read_cmdline(proc_t *proc)
{
	char cmdline_path[64] = { 0 };

	if (!proc)
		return TIE_ARGS;

	snprintf(cmdline_path, TIE_ARRLEN(cmdline_path) - 1, 
		 "%s/%d/cmdline", PROCFS_DIR, proc->id);

	if (!tie_fmap(cmdline_path, &proc->cmdline))
		proc->cmdline = (char *)NULL;
	
	return proc->cmdline ? TIE_OK : TIE_PROC_FS;
}

static int 
findpid_callback(pid_t pid,
		 void *arg)
{
	proc_t proc;
	struct _findpid_args *parg = (struct _findpid_args *)arg;

	if (tie_openproc(pid, &proc) == TIE_OK && 
	    !strcmp(parg->name, proc.name)) {
		parg->pid = pid;
		return TIE_FALSE;	
	}

	return TIE_TRUE;
}
