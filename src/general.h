/*---------------------------------------------------------------------------
 * General definitions and macros
 *---------------------------------------------------------------------------
 * Author: Cedric Adjih
 * Copyright 2013 Inria
 * All rights reserved. Distributed only with permission.
 *---------------------------------------------------------------------------*/

#ifndef __GENERAL_H__
#define __GENERAL_H__

/*---------------------------------------------------------------------------*/

#include <stdint.h>

/*---------------------------------------------------------------------------*/

#define BEGIN_MACRO do {
#define END_MACRO } while(0)

/*---------------------------------------------------------------------------*/

#define BITS_PER_BYTE 8
#define LOG2_BITS_PER_BYTE 3

#define BOOL_FALSE 0
#define BOOL_TRUE  1

#define REQUIRE(...) ASSERT(__VA_ARGS__)

#define STATIC_ENSURE_EQUAL(name,a,b) \
  struct name { char l1[(a)-(b)]; char l2[(b)-(a)]; };

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

static inline uint16_t min_except(uint16_t v1, uint16_t v2, uint16_t ignored)
{
  if (v1 == ignored) 
    return v2;
  if (v2 == ignored)
    return v1;
  return MIN(v1, v2);
}

static inline uint16_t max_except(uint16_t v1, uint16_t v2, uint16_t ignored)
{
  if (v1 == ignored) 
    return v2;
  if (v2 == ignored)
    return v1;
  return MAX(v1, v2);
}

/*---------------------------------------------------------------------------*/

typedef uint8_t bool_t;

/*---------------------------------------------------------------------------*/

#ifndef EMBEDDED
#include "platform-linux.h"
#else /* EMBEDDED */
#include "platform-embedded.h"
#endif /* EMBEDDED */

/*---------------------------------------------------------------------------*/

#ifdef CONF_WITH_FPRINTF 

void data_string_pywrite(FILE* out, uint8_t* data, int data_size);

#endif /* CONF_WITH_FPRINTF */

/*---------------------------------------------------------------------------*/


#endif /* __GENERAL_H__ */
