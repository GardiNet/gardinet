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
 * @author  Hannah Baccouch <hannah.baccouch@inria.fr>
 *
 * @brief   Sets of linear combinations of packets: management and decoding
 */


#include <stdint.h>
#include <string.h>

#include "general.h"
#include "bitmap.h"
#include "packet-set.h"

/*---------------------------------------------------------------------------*/

void packet_set_init(packet_set_t* set, uint8_t log2_nb_bit_coef,
		     notify_packet_decoded_func_t notify_packet_decoded_func,
		     notify_set_full_func_t notify_set_full_func,
		     get_decoded_packet_func_t get_decoded_packet_func,
		     void* notif_data)
{
  uint16_t i;
  for (i=0; i<MAX_CODED_PACKET; i++) {
    set->id_to_pos[i] = COEF_POS_NONE;
    set->pos_to_id[i] = PACKET_ID_NONE;
  }
  set->coef_pos_min = COEF_POS_NONE;
  set->coef_pos_max = COEF_POS_NONE;
  set->log2_nb_bit_coef = log2_nb_bit_coef;

  set->notify_packet_decoded_func = notify_packet_decoded_func;
  set->notify_set_full_func = notify_set_full_func;
  set->get_decoded_packet_func = get_decoded_packet_func;
  set->notif_data = notif_data;

  set->nb_decoded_packet = 0;
  bitmap_init(set->decoded_bitmap, DECODED_BITMAP_SIZE);
}

/* XXX: duplicate with packet_set_get_id_of_pos ? */
uint16_t packet_set_get_id_of_coef_pos(packet_set_t* set, uint16_t coef_pos)
{
  if (set->coef_pos_min == COEF_POS_NONE) {
    ASSERT( set->coef_pos_max == COEF_POS_NONE );
    return PACKET_ID_NONE;
  }
  
  if (coef_pos > set->coef_pos_max)
    return PACKET_ID_NONE;
  if (coef_pos < set->coef_pos_min)
    return PACKET_ID_NONE;
  return set->pos_to_id[coef_pos % MAX_CODED_PACKET];
}

static uint16_t packet_set_reduce
(packet_set_t* set, coded_packet_t* pkt, reduction_stat_t* stat)
{
  REQUIRE( set->log2_nb_bit_coef == pkt->log2_nb_bit_coef );
  uint8_t l = set->log2_nb_bit_coef;

  bool is_empty = !coded_packet_adjust_min_max_coef(pkt);
  if (is_empty)
    return COEF_POS_NONE;

  uint16_t coef_pos; 
  /* note that pkt->coef_pos_min|_max may change during loop */
  for (coef_pos = pkt->coef_pos_min; coef_pos <= pkt->coef_pos_max;
       coef_pos ++) {

    if (is_empty) {
      /* note: it is possible that stat->failure_count > 0 and still a empty
	 packet is obtained (e.g. second arrival of the same packet) */
      return COEF_POS_NONE;
    }
    uint8_t coef = coded_packet_get_coef(pkt, coef_pos);
    if (coef == 0)
      continue;
    uint16_t packet_id = packet_set_get_id_of_coef_pos(set, coef_pos);
    coded_packet_t* base_pkt = NULL;
    coded_packet_t tmp_base_pkt;

    if (packet_id != PACKET_ID_NONE) {
      base_pkt = &set->coded_packet[packet_id];
    } else {
      if (set->get_decoded_packet_func != NULL 
	  && (bitmap_get_bit(set->decoded_bitmap, 
			     DECODED_BITMAP_SIZE, coef_pos) != 0)) {
	bool_t ok = set->get_decoded_packet_func(set, coef_pos, &tmp_base_pkt);
	if (ok)
	  base_pkt = &tmp_base_pkt;
      }
      if (base_pkt == NULL) {
	stat->non_reduction ++;
	continue;
      }
    }

    ASSERT( coded_packet_get_coef(base_pkt, coef_pos) == 1 );

    ASSERT( base_pkt->coef_pos_min != COEF_POS_NONE );
    ASSERT( base_pkt->coef_pos_max != COEF_POS_NONE );
    ASSERT( pkt->coef_pos_min != COEF_POS_NONE );
    ASSERT( pkt->coef_pos_max != COEF_POS_NONE );
    uint16_t coef_pos_min = MIN(pkt->coef_pos_min, base_pkt->coef_pos_min);
    uint16_t coef_pos_max = MAX(pkt->coef_pos_max, base_pkt->coef_pos_max);
    if (coef_pos_max-coef_pos_min >= (1<<coded_packet_log2_window(base_pkt))
	|| (coef_pos_max-coef_pos_min >= MAX_CODED_PACKET)) { /* XXX:check */
      stat->reduction_failure ++;
      continue;
    }

    /* reduce by coded_packet */
    stat->reduction_success ++;
    uint8_t factor = lc_neg(coef, l);
    coded_packet_add_mult(pkt, factor, base_pkt);
    is_empty = !coded_packet_adjust_min_max_coef(pkt);
  }

  uint16_t i;
  for (i = pkt->coef_pos_min; i <= pkt->coef_pos_max; i ++) {
    coef_pos = pkt->coef_pos_max - i + pkt->coef_pos_min; /* start from high */
    if (coded_packet_get_coef(pkt, coef_pos) != 0
	&& packet_set_get_id_of_coef_pos(set, coef_pos) == PACKET_ID_NONE)
      return coef_pos;
  }
  return COEF_POS_NONE;
}

static uint16_t packet_set_alloc_packet_id(packet_set_t* set)
{
  /* find available packet set */
  uint16_t i;
  for (i=0; i<MAX_CODED_PACKET; i++) {
    if (set->id_to_pos[i] == COEF_POS_NONE) 
      return i;
  }
  return PACKET_ID_NONE;
}

uint8_t packet_set_free_first(packet_set_t* set)
{
  ASSERT (!packet_set_is_empty(set));
  uint16_t pos = set->coef_pos_min;
  uint16_t packet_id = set->pos_to_id[pos % MAX_CODED_PACKET];

  /* don't throw undecoded packets */
  if (packet_id == PACKET_ID_NONE)
    return false;
  if (!coded_packet_was_decoded(&set->coded_packet[packet_id]))
    return false;
  
  packet_id = PACKET_ID_NONE;
  if (packet_id == PACKET_ID_NONE) {
    uint16_t i;
    for (i=0; i<MAX_CODED_PACKET; i++)
      if (set->id_to_pos[i] != COEF_POS_NONE) {
	coded_packet_t* pkt = &set->coded_packet[i];
	if (pkt->coef_pos_min == set->coef_pos_min) {
	  packet_id = i;
	  pos = set->id_to_pos[packet_id];
	  break;
	}
      }
  }

  ASSERT (packet_id != PACKET_ID_NONE);
  set->id_to_pos[packet_id] = COEF_POS_NONE;
  set->pos_to_id[pos % MAX_CODED_PACKET] = PACKET_ID_NONE;

  packet_set_adjust_min_max_coef(set); /* XXX: not optimized */

  return true;
}

bool packet_set_is_empty(packet_set_t* set)
{
  if (set->coef_pos_min == COEF_POS_NONE) {
    ASSERT( set->coef_pos_max == COEF_POS_NONE );
    return true;
  }
  return false;
}

uint16_t packet_set_count(packet_set_t* set, bool_t count_decoded)
{
  if (packet_set_is_empty(set))
    return 0;
  uint16_t result = 0;
  uint16_t coef_pos;
  for (coef_pos = set->coef_pos_min; coef_pos <= set->coef_pos_max; coef_pos++){
    uint16_t packet_id = set->pos_to_id[coef_pos % MAX_CODED_PACKET];
    if (packet_id != PACKET_ID_NONE) {
      if (count_decoded 
	  || !coded_packet_was_decoded(&set->coded_packet[packet_id]))
	result++;
    }
  }
  return result;
}

#if 0
/* XXX: remove unused code */
uint16_t packet_set_get_id_of_pos(packet_set_t* set, uint16_t coef_pos)
{
  REQUIRE( coef_pos != COEF_POS_NONE );
  if (packet_set_is_empty(set))
    return PACKET_ID_NONE;
  
  if ( !(set->coef_pos_min <= coef_pos && coef_pos <= set->coef_pos_max) )
    return PACKET_ID_NONE;

  return set->pos_to_id[coef_pos % MAX_CODED_PACKET];
}
#endif

uint16_t packet_set_add(packet_set_t* set, coded_packet_t* pkt,
			reduction_stat_t* stat, 
			bool_t can_remove)
{
  (void)can_remove;
  ASSERT (pkt->coef_pos_max - pkt->coef_pos_min < MAX_CODED_PACKET);

  uint8_t l = set->log2_nb_bit_coef;
  REQUIRE( l == pkt->log2_nb_bit_coef );
  reduction_stat_t local_stat;
  if (stat == NULL)
    stat = &local_stat;
  reduction_stat_init(stat);

  /* reduce the packet */
  uint16_t coef_pos = packet_set_reduce(set, pkt, stat);
  if (coef_pos == COEF_POS_NONE)
    return PACKET_ID_NONE;

  /*check if it can be inserted as new reference for base packet at coef_pos */
  if (packet_set_is_empty(set)) {
    set->coef_pos_min = pkt->coef_pos_min;
    set->coef_pos_max = pkt->coef_pos_max;
  }

  /*..................................................*/
#if 0
  if (pkt->coef_pos_max > set->coef_pos_max) {
    if (can_remove && pkt->coef_pos_max-set->coef_pos_min >= MAX_CODED_PACKET) {
      /* attempt to call to make room */
      if (set->notify_set_full_func != NULL) {
	set->notify_set_full_func(set, 	pkt->coef_pos_max - MAX_CODED_PACKET-1);
	return packet_set_add(set, pkt, stat, false);
      } else return PACKET_ID_NONE;

#if 0 // XXX
      /* second test, if notify_set_full_func has made enough room */
      if (pkt->coef_pos_max - set->coef_pos_min >= MAX_CODED_PACKET) {
	stat->coef_pos_too_high ++;
	return PACKET_ID_NONE;
      }
#endif
    }
    set->coef_pos_max = pkt->coef_pos_max;
  }
#endif
  /*..................................................*/

  if (pkt->coef_pos_max > set->coef_pos_max) {
    if (pkt->coef_pos_max - set->coef_pos_min >= MAX_CODED_PACKET) {
      /* attempt to call to make room */
      if (set->notify_set_full_func != NULL)
        set->notify_set_full_func(set, pkt->coef_pos_max - MAX_CODED_PACKET-1);

      /*check if, *now*, it can be inserted as new reference */
      if (packet_set_is_empty(set)) { /* XXX:clean-up */
	set->coef_pos_min = pkt->coef_pos_min;
	set->coef_pos_max = pkt->coef_pos_max;
	ASSERT (pkt->coef_pos_max - set->coef_pos_min < MAX_CODED_PACKET);
      }

      /* second test, if notify_set_full_func has made enough room */
      if (pkt->coef_pos_max - set->coef_pos_min >= MAX_CODED_PACKET) {
        stat->coef_pos_too_high ++;
        return PACKET_ID_NONE;
      }
    }
    set->coef_pos_max = pkt->coef_pos_max;
  }

  if (pkt->coef_pos_min < set->coef_pos_min) {
    if (set->coef_pos_max - pkt->coef_pos_min >= MAX_CODED_PACKET) {
      stat->coef_pos_too_low ++;
      return PACKET_ID_NONE;
    } else set->coef_pos_min = pkt->coef_pos_min;
  }

  /* get a packet_id */
  uint16_t packet_id = packet_set_alloc_packet_id(set);
  if (packet_id == PACKET_ID_NONE && set->notify_set_full_func != NULL) {
    set->notify_set_full_func(set, COEF_POS_NONE);
    packet_id = packet_set_alloc_packet_id(set);
  }

  if (packet_id == PACKET_ID_NONE) {
    WARN("no room in packet_set_add");
    return PACKET_ID_NONE;
  }

  /* store packet in packet_id */
  ASSERT( packet_id < MAX_CODED_PACKET );
  ASSERT( set->pos_to_id[coef_pos % MAX_CODED_PACKET] == PACKET_ID_NONE );
  ASSERT( set->id_to_pos[packet_id] == COEF_POS_NONE );

  coded_packet_t* stored_pkt = &set->coded_packet[packet_id];
  coded_packet_copy_from(stored_pkt, pkt);
  set->pos_to_id[coef_pos % MAX_CODED_PACKET] = packet_id;
  set->id_to_pos[packet_id] = coef_pos;
  
  uint8_t coef = coded_packet_get_coef(stored_pkt, coef_pos);
  ASSERT( coef != 0 );
  coded_packet_to_mul(stored_pkt, lc_inv(coef, l) );

  if (coded_packet_was_decoded(stored_pkt)) {
    bitmap_set_bit(set->decoded_bitmap, DECODED_BITMAP_SIZE, 
		   stored_pkt->coef_pos_min);
    stat->decoded ++;
    if (set->notify_packet_decoded_func != NULL)
      set->notify_packet_decoded_func(set, packet_id);
  }

  /* eliminate */
  uint16_t i;
  for (i=set->coef_pos_min; i<=set->coef_pos_max; i++)
    if (i != coef_pos && set->pos_to_id[i%MAX_CODED_PACKET] != PACKET_ID_NONE) {
      uint16_t other_packet_id = set->pos_to_id[i%MAX_CODED_PACKET];
      ASSERT( other_packet_id < MAX_CODED_PACKET );
      coded_packet_t* other_pkt = &set->coded_packet[other_packet_id];
      if (coded_packet_was_decoded(other_pkt))
	continue;
      uint8_t other_coef = coded_packet_get_coef(other_pkt, coef_pos);
      if (other_coef == 0)
	continue;
      stat->elimination++;
      uint8_t factor  = lc_neg(other_coef, l);
      coded_packet_add_mult(other_pkt, factor, stored_pkt);
      coded_packet_adjust_min_max_coef(other_pkt);
      if (coded_packet_was_decoded(other_pkt)) {
	bitmap_set_bit(set->decoded_bitmap, DECODED_BITMAP_SIZE, 
		       other_pkt->coef_pos_min);
	stat->decoded ++;
	if (set->notify_packet_decoded_func != NULL)
	  set->notify_packet_decoded_func(set, other_packet_id);
      }
      /* XXX: check this cannot occur */
      ASSERT( !coded_packet_was_empty(other_pkt) );
    }
  return packet_id;
}

static uint16_t packet_set_get_highest_decoded(packet_set_t* set)
{
  uint16_t i;
  uint16_t result = COEF_POS_NONE;
  for (i=0; i<MAX_CODED_PACKET; i++)
    if (set->id_to_pos[i] != COEF_POS_NONE) {
      coded_packet_t* pkt = &set->coded_packet[i];
      if (coded_packet_was_decoded(pkt)) {
	if (result == COEF_POS_NONE || pkt->coef_pos_max > result)
	  result = pkt->coef_pos_max;
      }
    }
  return result;
}

uint16_t packet_set_get_low_index(packet_set_t* set)
{
  uint16_t i;
  uint16_t result = COEF_POS_NONE;

  for (i=0; i<MAX_CODED_PACKET; i++)
    if (set->id_to_pos[i] != COEF_POS_NONE) {
      coded_packet_t* pkt = &set->coded_packet[i];
      if (!coded_packet_was_decoded(pkt)) {
#if 1
	if (result == COEF_POS_NONE || pkt->coef_pos_min < result)
	  result = pkt->coef_pos_min;
#else
	uint16_t current_coef = pkt->coef_pos_min;
	if (result != COEF_POS_NONE && current_coef < result)
	  current_coef = result;
	while (current_coef <= pkt->coef_pos_max) {
	  if (packet_set_get_id_of_coef_pos(set, current_coef)!=PACKET_ID_NONE)
	    current_coef ++; /* was seen */
	  else break;
	}
	if (current_coef <= pkt->coef_pos_max 
	    && (result == COEF_POS_NONE || current_coef < result))
	  result = current_coef;
#endif
      }
    }
  if (result == COEF_POS_NONE)
    /*: XXX: note that in this case it should be = packet_set->coef_pos_max */
    result = packet_set_get_highest_decoded(set);

  if (result != COEF_POS_NONE) {
    /* XXX: temporary fix */
    for (i=0; i<result ;i++)
      if (bitmap_get_bit(set->decoded_bitmap, DECODED_BITMAP_SIZE, i) == 0)
	return i;
  }

  return result;
}

/*---------------------------------------------------------------------------*/

#ifdef CONF_WITH_FPRINTF


void bitmap_pywrite(FILE* out, uint8_t* bitmap, uint16_t size)
{
  uint16_t i;
  bool_t first = true;
  fprintf(out, "[");

  for (i=0; i<size*BITS_PER_BYTE; i++)
    if (bitmap_get_bit(bitmap, size, i)) {
      if (first) first = false;
      else fprintf(out, ",");
      fprintf(out, "%d",i);
    }
  
  fprintf(out, "]");
}

void coef_pos_pywrite(FILE* out, uint16_t coef_pos)
{
  if (coef_pos == COEF_POS_NONE)
    fprintf(out, "None");
  else fprintf(out, "%u", coef_pos);
}

void packet_set_adjust_min_max_coef(packet_set_t* set)
{
  uint16_t coef_pos_min = COEF_POS_NONE;
  uint16_t coef_pos_max = COEF_POS_NONE;
  uint16_t i;
  for (i=0; i<MAX_CODED_PACKET; i++)
    if (set->id_to_pos[i] != COEF_POS_NONE) {
      coded_packet_t* pkt = &set->coded_packet[i];
      coded_packet_adjust_min_max_coef(pkt);
      if (coef_pos_min == COEF_POS_NONE
	  || pkt->coef_pos_min < coef_pos_min)
	coef_pos_min = pkt->coef_pos_min;
      if (coef_pos_max == COEF_POS_NONE
	  || pkt->coef_pos_max > coef_pos_max)
	coef_pos_max = pkt->coef_pos_max;
    }
  
  set->coef_pos_min = coef_pos_min;
  set->coef_pos_max = coef_pos_max;
}


void packet_set_check(packet_set_t* set)
{
  ASSERT( (set->coef_pos_min != COEF_POS_NONE 
	   && set->coef_pos_max != COEF_POS_NONE)
	  || (set->coef_pos_min == COEF_POS_NONE 
	      && set->coef_pos_max == COEF_POS_NONE) );
  if (set->coef_pos_min != COEF_POS_NONE) {
    uint16_t i;
    for (i=0; i<MAX_CODED_PACKET; i++) {
      if (set->id_to_pos[i] != COEF_POS_NONE) {
	coded_packet_t* pkt = &set->coded_packet[i];
	ASSERT( pkt->coef_pos_min != COEF_POS_NONE );
	ASSERT( pkt->coef_pos_max != COEF_POS_NONE );
	ASSERT( pkt->coef_pos_min >= set->coef_pos_min );
	ASSERT( pkt->coef_pos_max <= set->coef_pos_max );
	ASSERT( set->pos_to_id[set->id_to_pos[i]%MAX_CODED_PACKET] != COEF_POS_NONE );
      }
    }
  }
}

void packet_set_pywrite(FILE* out, packet_set_t* set)
{
  fprintf(out, "{ 'type':'packet-set'");
  fprintf(out, ", 'l':%u", set->log2_nb_bit_coef);
  fprintf(out, ", 'coefPosMin':");
  coef_pos_pywrite(out, set->coef_pos_min);
  fprintf(out, ", 'coefPosMax':"); 
  coef_pos_pywrite(out, set->coef_pos_max);
  fprintf(out, ", 'packetTable': {");
  uint16_t i;
  bool is_first = true;
  for (i=0; i<MAX_CODED_PACKET; i++)
    if (set->id_to_pos[i] != COEF_POS_NONE) {
      if (is_first) is_first = false;
      else fprintf(out, ", ");
      fprintf(out, "%d:", i);
      coded_packet_pywrite(out, &set->coded_packet[i]);
    }
  fprintf(out, "}");
  
  fprintf(out, ", 'posToId':{");
  is_first = true;
  if (set->coef_pos_min != COEF_POS_NONE)
    for (i=set->coef_pos_min; i<=set->coef_pos_max; i++) {
      uint16_t packet_id = set->pos_to_id[i % MAX_CODED_PACKET];
      if (packet_id != PACKET_ID_NONE) {
	if (is_first) is_first = false;
	else fprintf(out, ", ");
	fprintf(out, "%d:%d", i, packet_id);
      }
    }
  fprintf(out, "}");

  fprintf(out, ", 'idToPos':{");
  is_first = true;
  for (i=0; i<MAX_CODED_PACKET; i++) {
    uint16_t coef_pos = set->id_to_pos[i];
    if (coef_pos != COEF_POS_NONE) {
      if (is_first) is_first = false;
      else fprintf(out, ", ");
      fprintf(out, "%d:%d", i, coef_pos);
    }
  }
  fprintf(out, "}");
  fprintf(out, ", 'decoded':");
  bitmap_pywrite(out, set->decoded_bitmap, DECODED_BITMAP_SIZE);

  fprintf(out, " }");
  packet_set_check(set); // XXX: remove
}

void reduction_stat_pywrite(FILE* out, reduction_stat_t* stat)
{
  fprintf(out, "{ 'type':'insert-stat'");
  fprintf(out, ", 'nonReduc':%u", stat->non_reduction );
  fprintf(out, ", 'reducSucces':%u", stat->reduction_success );
  fprintf(out, ", 'reducFail':%u", stat->reduction_failure );
  fprintf(out, ", 'coefTooLow':%u", stat->coef_pos_too_low );
  fprintf(out, ", 'coefTooHigh':%u", stat->coef_pos_too_high );
  fprintf(out, ", 'elim':%u", stat->elimination );
  fprintf(out, ", 'decoded':%u", stat->decoded );
  fprintf(out, " }");
}

#endif /* CONF_WITH_FPRINTF */

/*---------------------------------------------------------------------------*/
/** @} */
