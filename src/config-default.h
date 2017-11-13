/*---------------------------------------------------------------------------
 * Copyright 2017 Inria
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
 * @defgroup    LibLC    Linear Coding Library
 * @ingroup     liblc
 * @brief       linear coding and decoding of packets.
 * @{
 *
 * @file
 * @brief   Configuration through `#define`
 *
 * @author  Cedric Adjih <cedric.adjih@inria.fr>
 */

#ifndef __CONFIG_DEFAULT_H__
#define __CONFIG_DEFAULT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

/* ----- Configuration of general.h */

/* Defines whether `printf` is available */
#define CONF_WITH_FPRINTF

/* ----- Configuration of linear-coding.h */

/* GF(16) operations (multiplication, inversion) are available iff `WITH_GF_16`
  is defined. 
  In that case, internally, a multication table 16x256 (4 KiB) is included
  and used. */
#define WITH_GF16

/* GF(256) operations (multication, inversion) are available iff WITH_GF_256
   is defined. 
   Depending on whether WITH_GF256_MUL_TABLE two strategies are used:
   - if WITH_GF256_MUL_TABLE is defined, a multiplication table 256x256
    (64 KiB) is included and used. 
   - otherwise, a 'log' table and 'exp' table based on a generator element 
     `g` are used (256+256 bytes). We have g ** log[x] = x (for x != 0). 
     Then x*y = exp[log[x]+log[y]], for x !=0 and y != 0. */

#define WITH_GF256
#undef WITH_GF256_MUL_TABLE

/* ----- Configuration of coded-vector.h */

#undef CONF_CODED_PACKET_SIZE
#undef CONF_LOG2_COEF_HEADER_SIZE

/* ----- Configuration of packet-set.h */

/* Maximum number of packets that should be hold in the packet set:
   Assumptions: XXX */
#undef CONF_MAX_CODED_PACKET

/* Higher index ever possible to be reached 
  (this is used to keep a bitmap of packets that have already been decoded) */
#undef CONF_MAX_COEF_POS

/*---------------------------------------------------------------------------*/
  
#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_DEFAULT_H__ */

/** @} */
