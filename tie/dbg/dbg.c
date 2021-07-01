#include "dbg.h"
#include <stdlib.h>
#include <memory.h>
#include <wait.h>

TIE_API long 
tie_ptrace(enum __ptrace_request req, 
	   pid_t pid, 
	   void *addr, 
	   void *data)
{
	return ptrace(req, pid, addr, data);
}

static int
tie_iptrace(enum __ptrace_request req, 
	   pid_t pid, 
	   void *addr, 
	   void *data)
{
	return tie_ptrace(req, pid, addr, data) != -1 ? TIE_OK : TIE_PTRACE;
}

TIE_API int 
tie_attach(pid_t pid)
{
	int ret = tie_iptrace(PTRACE_ATTACH, pid, 
			      TIE_NULLPTR, TIE_NULLPTR);
	
	if (ret == TIE_OK) {
		int status;
		wait(&status);
	}

	return ret;
}

TIE_API int 
tie_detach(pid_t pid)
{
	return tie_iptrace(PTRACE_DETACH, pid, 
			   TIE_NULLPTR, TIE_NULLPTR);
}

TIE_API int 
tie_peek(pid_t  pid, 
	 void  *src, 
	 void  *dst, 
	 size_t size)
{
	size_t i;

	for (i = 0; i < size; ++i) {
		long data = tie_ptrace(PTRACE_PEEKDATA, 
				       pid, 
				       (void *)(&((char *)src)[i]), 
				       TIE_NULLPTR);
		((char *)dst)[i] = (char)data;
	}

	return i == size ? TIE_OK : TIE_PTRACE;
}

TIE_API int 
tie_poke(pid_t  pid, 
	 void  *dst, 
	 void  *src, 
	 size_t size)
{
	char *data;
	size_t i;
	size_t orig_size = size;

	size += size % sizeof(void *);
	size = (size >= sizeof(void *) ? size : sizeof(void *));

	data = malloc(size);

	if (!data)
		return TIE_ALLOC;

	memcpy((void *)data, src, orig_size);
	tie_peek(pid, (void *)(&((char *)src)[orig_size]), (void *)&data[orig_size], size - orig_size);

	for (i = 0; i < size; i += sizeof(void *)) {
		tie_ptrace(PTRACE_POKEDATA, pid, 
			   &((char *)dst)[i], (void *)&data[i]);
	}
	
	free(data);

	return i == size ? TIE_OK : TIE_PTRACE;
}

TIE_API int 
tie_getregs(pid_t pid,
	    struct user_regs_struct *regs)
{
	return tie_iptrace(PTRACE_GETREGS, pid, TIE_NULLPTR, regs);
}

TIE_API int 
tie_setregs(pid_t pid,
	    struct user_regs_struct *regs)
{
	return tie_iptrace(PTRACE_SETREGS, pid, regs, TIE_NULLPTR);
}
