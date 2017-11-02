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
 * @brief   Test linear coding of packets
 */

#include <stdio.h>
#include <stdlib.h>

#include "coded-packet.h"

/*---------------------------------------------------------------------------*/

#define L 3

#define COEF_PER_HEADER ((COEF_HEADER_SIZE*BITS_PER_BYTE)>>L)
coded_packet_t packet_table[COEF_PER_HEADER * 2];

int main(int argc, char** argv)
{
  int i;
  for (i=0; i<COEF_PER_HEADER*2; i++) {
    coded_packet_init(&packet_table[i], L);
    coded_packet_set_coef(&packet_table[i], i, 1);
  }

  coded_packet_t current;
  coded_packet_init(&current, L);
  coded_packet_set_coef(&current, 0, 1);
  for (i=0; i< COEF_PER_HEADER*2-1; i++) {
    coded_packet_t p;
    coded_packet_to_add(&p, &packet_table[i], &packet_table[i+1]);
    coded_packet_to_add(&current, &current, &p);
    coded_packet_adjust_min_max_coef(&current);
  }

  coded_packet_pywrite(stdout, &current);
  fprintf(stdout,"\n");

  exit(EXIT_SUCCESS);
}

/*---------------------------------------------------------------------------*/
/** @} */
