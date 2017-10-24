/*---------------------------------------------------------------------------
 * Definitions specific for compiling on Linux
 *---------------------------------------------------------------------------
 * Author: Cedric Adjih
 * Copyright 2013 Inria
 * All rights reserved. Distributed only with permission.
 *---------------------------------------------------------------------------*/

#ifndef __PLATFORM_EMBEDDED_H__
#define __PLATFORM_EMBEDDED_H__

/*---------------------------------------------------------------------------*/

#include "config-embedded.h"

/*---------------------------------------------------------------------------*/

#if 0
/* XXX: ^^^ */
#define CONF_WITH_FPRINTF
#define fprintf(out, ...) printf(__VA_ARGS__)
#define FILE int
#define stdout 0
#endif

/*---------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

/* see also: http://en.wikipedia.org/wiki/C_data_types (c99 data types) */
/** boolean type */
//typedef unsigned char bool_t;

/*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

#define ASSERT( x ) do { if (!(x)) { FATAL("ASSERTION FAILED: "#x); } } while(0)

#define FATAL(...)				\
  BEGIN_MACRO					\
  printf("\nFATAL (" __FILE__ ":%d):\n  ", __LINE__);	\
     printf(__VA_ARGS__);			\
     printf("\n");			\
     dint(); \
     while (1) ; \
  END_MACRO

#define WARN(...)				\
  BEGIN_MACRO					\
     printf("WARNING: " __VA_ARGS__);	\
     printf("\n");			\
  END_MACRO

/*---------------------------------------------------------------------------*/

/* only for MSP430 */
#define RAND_CLOCK (TBR^TAR)

/*---------------------------------------------------------------------------*/

#endif /* __PLATFORM_EMBEDDED_H__ */
