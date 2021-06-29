#ifndef IO_H
#define IO_H

#include "common.h"
#include <stddef.h>

TIE_API size_t 
tie_fmap(const char *path, 
	 char      **pfile);

TIE_API void 
tie_funmap(char *file);

TIE_API size_t 
tie_flink(const char *path, 
	  char      **plink);

#endif
