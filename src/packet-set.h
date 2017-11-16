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
 * @ingroup     liblc
 * @brief       linear coding and decoding of packets.
 * @{
 *
 * @file
 * @brief   Sets of linear combinations of packets: management and decoding.
 *
 * @author  Cedric Adjih <cedric.adjih@inria.fr>
 * @author  Hannah Baccouch <hannah.baccouch@inria.fr>
 */

#ifndef __PACKET_SET_H__
#define __PACKET_SET_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#include "bitmap.h"
#include "coded-packet.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief reduction_stat_t gives information (statistics) about the Gaussian 
 *        Elimination step that is performed every time a packet is added.
 */
typedef struct {
  unsigned int non_reduction;     /**< */
  unsigned int reduction_success; /**< */
  unsigned int reduction_failure; /**< */
  unsigned int coef_pos_too_low;  /**< */
  unsigned int coef_pos_too_high; /**< */
  unsigned int elimination;       /**< */
  unsigned int decoded;           /**< */
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

/**
 * @brief packet_set_t is the main decoding buffer, 
 *        it keeps packets added with packet_set_add(...)
 *        and that are being decodeding, it maintains
 *        the data structures related to Gaussian Elimination
 *        (and it also holds the function callbacks).
 *
 * @details 
 */
typedef struct s_packet_set_t {
  coded_packet_t coded_packet[MAX_CODED_PACKET]; /**< the actual set */
  uint16_t id_to_pos[MAX_CODED_PACKET]; /**< index in coded_packet array (packet_id) to source packet index (coef_pos) */
  uint16_t pos_to_id[MAX_CODED_PACKET]; /**< source packet (coef_pos) to index in coded_packet (packet_id) */

  void* notif_data;
  notify_packet_decoded_func_t notify_packet_decoded_func;
  notify_set_full_func_t notify_set_full_func;
  get_decoded_packet_func_t get_decoded_packet_func;

  uint16_t coef_pos_min;
  uint16_t coef_pos_max; 
  uint8_t log2_nb_bit_coef; /**< log2_nb_bit_coef is the "log2 of the number 
     of bits" of the finite
     field order to specify which of GF(2), GF(4), GF(16), GF(256) 
     is used (it would be `0,1,2,3` respectively). */
  uint16_t nb_decoded_packet;

  uint8_t decoded_bitmap[DECODED_BITMAP_SIZE];
} packet_set_t;


/**
 * @brief     Initializes one packet set.
 * @param[in] set is the packet set
 * @param[in] log2_nb_bit_coef is the "log2 of the number of bits" of the finite
 *            field order, to specify which of GF(2), GF(4), GF(16), GF(256) 
 *            is used (it would be `0,1,2,3` respectively).
 * @param[in] notify_packet_decoded_func (optional, can be NULL) is a callback 
 *            called every time a packet has been decoded if not NULL.
 * @param[in] notify_packet_decoded_func (optional, can be NULL) is a callback 
 *            called every time a packet has been decoded if not NULL.
 * @param[in] get_decoded_packet_func (optional, can be NULL) is a function
 *            used by the library, to get back decoded packet if it has been
 *            removed from the packet set. This is specific to some use cases.
 * @param[in] notif_data not used for now (XXX), intended to be passed as last
 *            argument of each callback function.
 */
void packet_set_init(packet_set_t* set, uint8_t log2_nb_bit_coef,
		     notify_packet_decoded_func_t notify_packet_decoded_func,
		     notify_set_full_func_t notify_set_full_func,
		     get_decoded_packet_func_t get_decoded_packet_func,
		     void* notif_data);

/**
 * @brief         Add one coded packet to a packet set, and perform one step of 
 *                Gaussian Elimination. Multiple packets can be decoded,
 *                appropriate callbacks are called (decoded, set full, ...).
 * @param[in]     set is the packet set
 * @param[in,out] pkt is the coded packet that is added to the set. It is 
 *                modified during the Gaussian Elimination process.
 * @param[out]    stat holds information (counters) of the events that
 *                happened in the Gaussian Elimination process step.
 * @param[in]     can_remove is currently not used.
 * @return        return the packet_id associated with the inserted packet,
 *                or PACKET_ID_NONE otherwise.
 */
uint16_t packet_set_add(packet_set_t* set, coded_packet_t* pkt,
			reduction_stat_t* stat,
			bool_t can_remove);

/**
 * @brief         Get the index of the internal set->coded_packet array
 *                of the coded_packet corresponding to the pivot for
 *                source index (in the
 *                Gaussian Elimination process)
 * @param[in]     set is the packet set
 * @param[in]     coef_pos is the index of the source packet for which we
 *                are trying to find the pivot.
 * @return        the packet_id if one pivot exists, otherwise PACKET_ID_NONE
 */
uint16_t packet_set_get_id_of_coef_pos(packet_set_t* set, uint16_t coef_pos);

/**
 * @brief         Indicates if the packet set is empty.
 * @param[in]     set is the packet set.
 * @return        true if the packet set is empty, false otherwise.
 */
bool packet_set_is_empty(packet_set_t* set);

/**
 * @brief         Counts the number of coded packets in the packet set.
 * @param[in]     set is the packet set
 * @param[in]     count_decoded indicates whether decoded packets should 
 *                be included in the count 
 *                (otherwise only non-decoded packets are counted).
 * @return        the number of packets
 */
uint16_t packet_set_count(packet_set_t* set, bool_t count_decoded);

/**
 * @brief         Remove the packet that correspond to the lowest source
 *                packet index in the packet set, if there is one, and
 *                it has been decoded.
 * @param[in]     set is the packet set
 * @return        the packet_id if one packet has been removed,
 *                otherwise PACKET_ID_NONE
 */
uint8_t packet_set_free_first(packet_set_t* set);

/**
 * @brief         Get the lowest source packet index of (undecoded?) packets
 * @param[in]     set is the packet set
 * @return        the lowest source packet index, or COEF_POS_NONE
 */
uint16_t packet_set_get_low_index(packet_set_t* set);

/**
 * @brief         (Internal) Adjust the coef_pos_min and coef_pos_max of the
 *                packet set to the current value.
 * @param[in]     set is the packet set
 */
void packet_set_adjust_min_max_coef(packet_set_t* set);

/**
 * @brief         (Internal) Performs some consistency/sanity checks on the
 *                packet set
 * @param[in]     set is the packet set
 * @details       ASSERT(...) is used to check for assertions.
 */
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
/*---------------------------------------------------------------------------*/
/** @} */
