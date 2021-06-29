#ifndef COMMON_H
#define COMMON_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define TIE_NULL  (0)
#define TIE_FALSE (TIE_NULL)
#define TIE_TRUE  (!TIE_FALSE)
#define TIE_API   extern

#define TIE_ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))

enum {
	TIE_OK = 0,   /* No Errors */
	TIE_ARGS,     /* Invalid Arguments */
	TIE_ALLOC,    /* Unable to Allocate Memory */
	TIE_PROC_DIR, /* Missing 'PROCFS_DIR' */
	TIE_PROC_FS   /* The 'PROCFS_DIR' does not follow the 'procfs' structure */
};

#endif
