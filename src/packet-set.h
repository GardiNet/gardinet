/*---------------------------------------------------------------------------
 * Sets of linear combinations of packets: management and decoding
 *---------------------------------------------------------------------------
 * Author: Cedric Adjih, Hannah Baccouch
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

#ifndef __PACKET_SET_H__
#define __PACKET_SET_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#include "bitmap.h"
#include "coded-packet.h"

/*---------------------------------------------------------------------------*/

typedef struct {
  unsigned int non_reduction;
  unsigned int reduction_success;
  unsigned int reduction_failure;
  unsigned int coef_pos_too_low;
  unsigned int coef_pos_too_high;
  unsigned int elimination;
  unsigned int decoded;
} reduction_stat_t;

static inline void reduction_stat_init(reduction_stat_t* stat)
{ memset(stat, 0, sizeof(reduction_stat_t)); }

/*---------------------------------------------------------------------------*/

#ifdef CONF_MAX_CODED_PACKET
#define MAX_CODED_PACKET CONF_MAX_CODED_PACKET
#else  /* CONF_MAX_CODED_PACKET */
#define MAX_CODED_PACKET 4
#endif /* CONF_MAX_CODED_PACKET */

#ifdef CONF_MAX_COEF_POS
#define MAX_COEF_POS CONF_MAX_COEF_POS
#else /* CONF_MAX_COEF_POS */
#define MAX_COEF_POS 1024*8
#endif

#define DECODED_BITMAP_SIZE (BYTES_PER_BITMAP(MAX_COEF_POS))

struct s_packet_set_t;

typedef void (*notify_packet_decoded_func_t) 
(struct s_packet_set_t* packet_set, uint16_t packet_index);

typedef void (*notify_set_full_func_t) 
(struct s_packet_set_t* packet_set, uint16_t required_min_coef_pos);

typedef bool_t (*get_decoded_packet_func_t) 
(struct s_packet_set_t* packet_set, uint16_t required_min_coef_pos,
 coded_packet_t* res_coded_packet);


#define PACKET_ID_NONE 0xfffeu

typedef struct s_packet_set_t {
  coded_packet_t coded_packet[MAX_CODED_PACKET];
  uint16_t id_to_pos[MAX_CODED_PACKET];
  uint16_t pos_to_id[MAX_CODED_PACKET];

  void* notif_data;
  notify_packet_decoded_func_t notify_packet_decoded_func;
  notify_set_full_func_t notify_set_full_func;
  get_decoded_packet_func_t get_decoded_packet_func;

  uint16_t coef_pos_min; 
  uint16_t coef_pos_max; 
  uint8_t log2_nb_bit_coef;
  uint16_t nb_decoded_packet;

  uint8_t decoded_bitmap[DECODED_BITMAP_SIZE];
} packet_set_t;


void packet_set_init(packet_set_t* set, uint8_t log2_nb_bit_coef,
		     notify_packet_decoded_func_t notify_packet_decoded_func,
		     notify_set_full_func_t notify_set_full_func,
		     get_decoded_packet_func_t get_decoded_packet_func,
		     void* notif_data);

/* 
   return the packet_id associated with the inserted packet,
   or PACKET_ID_NONE otherwise 
   pkt is (usually) modified by the function
*/
uint16_t packet_set_add(packet_set_t* set, coded_packet_t* pkt,
			reduction_stat_t* stat,
			bool_t can_remove);

uint16_t packet_set_get_id_of_pos(packet_set_t* set, uint16_t coef_pos);

bool packet_set_is_empty(packet_set_t* set);

uint16_t packet_set_count(packet_set_t* set, bool_t count_decoded);

//uint16_t packet_set_free_packet_id(packet_set_t* set, uint16_t packet_id);
uint8_t packet_set_free_first(packet_set_t* set);

uint16_t packet_set_get_low_index(packet_set_t* set);

void packet_set_adjust_min_max_coef(packet_set_t* set);

void packet_set_check(packet_set_t* set);

/*---------------------------------------------------------------------------*/

#ifdef CONF_WITH_FPRINTF

void coef_pos_pywrite(FILE* out, uint16_t coef_pos);

void packet_set_pywrite(FILE* out, packet_set_t* set);

void reduction_stat_pywrite(FILE* out, reduction_stat_t* stat);

#endif /* CONF_WITH_FPRINTF */

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __PACKET_SET_H__ */
