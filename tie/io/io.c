#include "io.h"
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFLEN 1024

TIE_API size_t 
tie_fmap(const char *path, 
	 char      **pfile)
{
	int    fd;
	char  *buf;
	size_t total = 0;
	ssize_t read_bytes;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		return total;

	buf = calloc(BUFLEN, sizeof(char));

	*pfile = (char *)NULL;

	while ((read_bytes = read(fd, buf, BUFLEN * sizeof(char))) > 0) {
		char *old_pfile = *pfile;

		read_bytes /= sizeof(char);
		*pfile = calloc(total + (size_t)read_bytes + 1, sizeof(char));

		if (old_pfile) {
			if (*pfile) {
				strncpy(*pfile, old_pfile, total);
			}

			free(old_pfile);
		}

		if (!*pfile) {
			total = 0;
			break;
		}

		strncpy(&(*pfile)[total], buf, (size_t)read_bytes);
		(*pfile)[total + (size_t)read_bytes] = '\x00';

		total += (size_t)read_bytes;
	}

	close(fd);
	free(buf);
	return total;
}

TIE_API void 
tie_funmap(char *file)
{
	if (file)
		free(file);
}

TIE_API size_t 
tie_flink(const char *path, 
	  char      **plink)
{
	char   *buf;
	ssize_t size;
	size_t  ret = 0;

	if (!path || !plink)
		return ret;
	
	buf = calloc(PATH_MAX, sizeof(char));

	if (!buf)
		return ret;

	size = readlink(path, buf, PATH_MAX);

	if (size == -1)
		goto _FREE_RET;

	*plink = calloc((size_t)size + 1, sizeof(char));

	if (!(*plink))
		goto _FREE_RET;

	ret = (size_t)size;
	strncpy((*plink), buf, ret);
	(*plink)[size] = '\x00';
_FREE_RET:
	free(buf);
	return ret;
}
