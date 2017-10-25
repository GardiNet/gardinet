/*---------------------------------------------------------------------------
 * Linear operations (GF(2), GF(4), GF(16) and GF(256))
 *---------------------------------------------------------------------------
 * Author: Cedric Adjih
 *---------------------------------------------------------------------------
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

#ifndef __LINEAR_CODE_H__
#define __LINEAR_CODE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#include <stdint.h>

/*---------------------------------------------------------------------------*/

#define MAX_LOG2_NB_BIT_COEF 3 /* included */

uint8_t lc_inv(uint8_t x, uint8_t log2_nb_bit_coef);

uint8_t lc_mul(uint8_t x, uint8_t y, uint8_t log2_nb_bit_coef);

static inline uint8_t lc_neg(uint8_t x, uint8_t log2_nb_bit_coef) 
{ (void)log2_nb_bit_coef; return x; /* -x == x in GF(2^n) */ } 

/*---------------------------------------------------------------------------*/

#define DIV_LOG2(value, log2_divisor) ((value)>>(log2_divisor))
#define MUL_LOG2(value, log2_mult) ((value)<<(log2_mult))
#define MASK(nb_bit) ((1 << (nb_bit))-1)
#define MOD_LOG2(value, log2_divisor) ((value)&MASK((log2_divisor)))

#define LOG2_BITS_PER_BYTE 3

void lc_vector_add(uint8_t* data1, uint16_t size1,
		   uint8_t* data2, uint16_t size2,
		   uint8_t* result, uint16_t* result_size);

void lc_vector_mul(uint8_t coef, uint8_t* data, uint16_t size,
		   uint8_t log2_nb_bit_coef, uint8_t* result);

void lc_vector_set(uint8_t* data, uint16_t size, uint8_t log2_nb_bit_coef,
		   uint16_t coef_pos, uint8_t coef_value);

uint8_t lc_vector_get(uint8_t* data, uint16_t size, uint8_t log2_nb_bit_coef,
		      uint16_t coef_pos);

uint16_t lc_vector_get_byte_pos_of_coef(uint8_t log2_nb_bit_coef,
		   uint16_t coef_pos);

/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* __LINEAR_CODE_H__ */
