#ifndef DBG_H
#define DBG_H

#include "common.h"
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/user.h>

TIE_API long 
tie_ptrace(enum __ptrace_request req, 
	   pid_t pid, 
	   void *addr, 
	   void *data);

TIE_API int 
tie_attach(pid_t pid);

TIE_API int 
tie_detach(pid_t pid);

TIE_API int 
tie_peek(pid_t  pid, 
	 void  *src, 
	 void  *dst, 
	 size_t size);

TIE_API int 
tie_poke(pid_t  pid, 
	 void  *dst, 
	 void  *src, 
	 size_t size);

TIE_API int 
tie_getregs(pid_t pid,
	    struct user_regs_struct *regs);

TIE_API int 
tie_setregs(pid_t pid,
	    struct user_regs_struct *regs);

#endif
