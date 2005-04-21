#include "utils.h"
#include <stdlib.h>
#include <string.h>

/*=========================================================================*
 *				env_prefix				   *
 *=========================================================================*/
PUBLIC int env_prefix(env, prefix)
char *env;		/* environment variable to inspect */
char *prefix;		/* prefix to test for */
{
/* An environment setting may be prefixed by a word, usually "pci".  
 * Return TRUE if a given prefix is used.
 */
  char value[EP_BUF_SIZE];
  char punct[] = ":,;.";
  int s;
  size_t n;

  if ((s = sys_getkenv(env, strlen(env), value, sizeof(value))) != 0) {
  	if (s != ESRCH)		/* only error allowed */
  	printf("WARNING: sys_getkenv() failed in env_prefix(): %d\n", s);	
  }
  n = strlen(prefix);
  return(value != NULL
	&& strncmp(value, prefix, n) == 0
	&& strchr(punct, value[n]) != NULL);
}

