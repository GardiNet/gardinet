/*---------------------------------------------------------------------------
 * Copyright 2013-2017 Inria
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *---------------------------------------------------------------------------*/

/**
 * @ingroup     liblc
 * @{
 *
 * @file
 *
 * @author  Cedric Adjih <cedric.adjih@inria.fr>
 *
 * @brief   General definitions and macros
 */

#include "general.h"

/*---------------------------------------------------------------------------*/

#ifdef CONF_WITH_FPRINTF

/* write a data block as a python string with hex escape, 
   e.g. {1,0x40,0xff} -> '\x01\x40\xff' */
void data_string_pywrite(FILE* out, uint8_t* data, int data_size)
{
  int i;
  fprintf(out, "'");
  for (i=0; i<data_size; i++)
    fprintf(out, "\\x%02x", data[i]);
  fprintf(out, "'");
}

#endif /* CONF_WITH_FPRINTF */

/*---------------------------------------------------------------------------*/
/** @} */
