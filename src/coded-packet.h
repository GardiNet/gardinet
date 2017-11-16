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
 * @defgroup    LibLC    Linear Coding Library
 * @ingroup     LibLC
 * @brief       linear coding and decoding of packets.
 * @{
 *
 * @file
 * @brief   Functions and operations on linear combination of packets
 *          with sliding window header.
 *
 * @author  Cedric Adjih <cedric.adjih@inria.fr>
 */

#ifndef __CODED_PACKET_H__
#define __CODED_PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#include <stdint.h>
#include <string.h>

#include "general.h"
#include "linear-code.h"

/*---------------------------------------------------------------------------*/

#ifdef CONF_CODED_PACKET_SIZE
#define CODED_PACKET_SIZE CONF_CODED_PACKET_SIZE
#else  /* CONF_CODED_PACKET_SIZE */
#define CODED_PACKET_SIZE 128
#endif /* CONF_CODED_PACKET_SIZE */

#ifdef CONF_LOG2_COEF_HEADER_SIZE
#define LOG2_COEF_HEADER_SIZE CONF_LOG2_COEF_HEADER_SIZE
#else  /* CONF_LOG2_COEF_HEADER_SIZE */
#define LOG2_COEF_HEADER_SIZE 4
#endif /* CONF_LOG2_COEF_HEADER_SIZE */

#define COEF_HEADER_SIZE_SWIG 16 /* work-around swig bug */
#ifndef SWIG
#define COEF_HEADER_SIZE (1<<LOG2_COEF_HEADER_SIZE)
#else /* SWIG */
#define COEF_HEADER_SIZE COEF_HEADER_SIZE_SWIG
#endif /* SWIG */

#define COEF_POS_NONE 0xffffu

/**
 * @brief The coded packet coded_packet_t contains the coded payload with
 *        an encoding vector header.
 *        The coded packet also targets sliding encoding window coding, so one
 *        additional feature is that the array of coefficients of encoding 
 *        vector corresponds to the source packet indices modulo its size.
 * @details 
 *        The encoding vector header and the coded payload are hold in the
 *        same array in `content` as follows:
 *
 *        <--- encoding vector ---><--- coded payload --->
 */  
typedef struct {
  uint8_t  log2_nb_bit_coef; /* 0,1,2,3 [-> 1,2,4,8 bits for coefficients] */

  
  uint16_t coef_pos_min; /**< minimum source packet index */
  uint16_t coef_pos_max; /**< maximum source packet index */
  uint16_t data_size;    /**< size of the coded payload */

  union {
    uint32_t u32[(COEF_HEADER_SIZE+CODED_PACKET_SIZE+3)/4];
    uint8_t  u8[COEF_HEADER_SIZE+CODED_PACKET_SIZE];
  } content; /**< encoding vector (COEF_HEADER_SIZE bytes) followed by coded payload (CODED_PACKET_SIZE bytes) */
} coded_packet_t;

/*---------------------------------------------------------------------------*/

static inline uint16_t log2_window_size(uint8_t l)
{ return LOG2_COEF_HEADER_SIZE+LOG2_BITS_PER_BYTE-l; }

static inline uint16_t coded_packet_log2_window(coded_packet_t* pkt)
{ return log2_window_size(pkt->log2_nb_bit_coef); }

/**
 * @brief Initialize a coded packet, as an empty packet (no coefficients)
 *
 * @param[in,out] pkt is the coded packet
 * @param[in]     log2_nb_bit_coef is the "log2 of the number of bits" of the
 *                finite field order for indicating which of GF(2), GF(4), 
 *                GF(16), GF(256) is used (it would be `0,1,2,3` respectively).
 */
void coded_packet_init(coded_packet_t* pkt, uint8_t log2_nb_bit_coef);

/**
 * @brief Initialize a coded packet as exactly one source packet.
 *
 * @param[in,out] pkt is the coded packet
 * @param[in]     log2_nb_bit_coef same as in `coef_packet_init`
 * @param[in]     base_index is the source packet index
 * @param[in]     data is the source packet (source payload)
 * @param[in]     data_size is the size of the source packet
 */
void coded_packet_init_from_base_packet
(coded_packet_t* pkt, uint8_t log2_nb_bit_coef, uint16_t base_index,
 uint8_t* data, uint8_t data_size);
  
/**
 * @brief Copy a given coded packet
 * @param[out] dst is the place the where coded packet is copied
 * @param[in]  src is the coded packet that is being copied
 */
void coded_packet_copy_from(coded_packet_t* dst, coded_packet_t* src);

/**
 * @brief Set the value of one coefficient in the encoding header. 
 *          (XXX:constraints)
 *
 * @param[in,out] pkt is the coded packet
 * @param[in]     coef_pos is the source packet index.
 * @param[in]     coef_value is the value of the coefficient (in the proper
 *                finite field).
 */
void coded_packet_set_coef(coded_packet_t* pkt, uint16_t coef_pos,
			   uint8_t coef_value);

/**
 * @brief Get the value of in the encoding header.
 *
 * @param[in,out] pkt is the coded packet
 * @param[in]     coef_pos is the source packet index.
 * @return        the value of the coefficient.
 */  
uint8_t coded_packet_get_coef(coded_packet_t* pkt, uint16_t coef_pos);

/**
 * @brief Indicates the position in the `content` array of the byte that
 *        contains coefficient corresponding to the given source packet index.
 * @param[in,out] pkt is the coded packet
 * @param[in]     coef_pos is the source packet index.
 * @return        the value of the coefficient.
 */  
uint16_t coded_packet_get_byte_pos_of_coef(coded_packet_t* pkt,
		   uint16_t coef_pos);

/**
 * @brief Indicates the position in the encoding vector of the coefficient
 *        corresponding to the given source packet index.
 *
 * @param[in,out] pkt is the coded packet
 * @param[in]     coef_pos is the source packet index.
 * @return        the value of the coefficient.
 */  
static inline uint16_t coded_packet_get_actual_coef_pos
(coded_packet_t* pkt, uint16_t coef_pos)
{ return MOD_LOG2(coef_pos, coded_packet_log2_window(pkt)); }

/**
 * @brief (Internal) Adjust coef_min and coef_max
 *
 * @param[in,out] pkt is the coded packet
 */  
bool coded_packet_adjust_min_max_coef(coded_packet_t* pkt);

/**
 * @brief Indicates whether the packet is empty (zero), i.e. all coefficients
 *        are zero.
 * @param[in] pkt is the coded packet
 * @return    true if the packet is empty, false otherwise
 */  
static inline bool coded_packet_is_empty(coded_packet_t* pkt)
{ return !coded_packet_adjust_min_max_coef(pkt); }

/**
 * @brief Indicates whether the packet is decoded, i.e. corresponds one source 
 *        to only one source packet (one and only one encoding vector 
 *        coefficient is different from zero).
 * @param[in] pkt is the coded packet
 * @return    true if the packet is decoded, false otherwise
 */
static inline bool coded_packet_was_decoded(coded_packet_t* pkt)
{ return pkt->coef_pos_min == pkt->coef_pos_max; /* ok if empty */ } 

static inline bool coded_packet_was_empty(coded_packet_t* pkt)
{ return pkt->coef_pos_min == COEF_POS_NONE; } 

static inline void coded_packet_to_mul(coded_packet_t* pkt, uint8_t coef)
{ lc_vector_mul(coef, pkt->content.u8, COEF_HEADER_SIZE+pkt->data_size,
		pkt->log2_nb_bit_coef, pkt->content.u8); }

/**
 * @brief Compute the sum of two coded packets
 * @param[out] result      Where the result of the sum p1+p2 is put
 * @param[in]  p1          First coded packet
 * @param[in]  p2          Second coded packet
 */
/*  XXX: check of details of lc_vector_add are verified here:
 * details `p1`, `p2` and `result` could be equal as pointers (two of
 *          them or all three), and the `result` would still be correct.
 *          Other than such strict equality, `result` should point to memory
 *          areas disjoint from `data1` or `data2`,
 *          e.g. `lc_vector_add(p,l-1,p,l-1,p+1,&l2)` gives improper result. 
 */
void coded_packet_to_add(coded_packet_t* result,
			 coded_packet_t* p1,
			 coded_packet_t* p2);

/**
 * @brief Take a coded packet and add another coded packet multiplied by a 
 *        coefficient, e.g. performs the equivalent of: p1 += coef * p2
 * @param[in,out] p1     First coded packet (to which coef*p2 will be added)
 * @param[in]     coef2  Coefficient by which the second packet is multiplied
 * @param[in]     p2     Second coded packet
 */
void coded_packet_add_mult
(coded_packet_t* p1, uint8_t coef2, coded_packet_t* p2);

bool coded_packet_is_empty_safe(coded_packet_t* pkt);

#ifdef CONF_WITH_FPRINTF
void coded_packet_pywrite(FILE* out, coded_packet_t* p);
#endif /* CONF_WITH_FPRINTF */

static inline uint8_t* coded_packet_data(coded_packet_t* p)
{ return p->content.u8 + COEF_HEADER_SIZE; }

/* warning: slow function, also may change p1, p2 internals by calling
   coded_packet_adjust_min_max_coef(...) */
bool coded_packet_is_similar(coded_packet_t* p1, coded_packet_t* p2);

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __CODED_PACKET_H__ */
/*---------------------------------------------------------------------------*/
/** @} */
