#ifndef EX_ERR_INCLUDED
#define EX_ERR_INCLUDED

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* If 'cond' is true then print message for given 
   error code 'errcode' and abort */

#define COND_MSG_ABORT(cond, errcode, msg)				\
  do {									\
    if ((cond))								\
      {									\
	fprintf (stderr, "%s at \"%s\":%d: %s\n",			\
		 (msg), __FILE__, __LINE__, strerror((errcode)));	\
	abort();							\
      }									\
  }while (0)

#endif
