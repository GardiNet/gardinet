/*---------------------------------------------------------------------------
 * Bitmap functions
 *---------------------------------------------------------------------------
 * Author: Cedric Adjih, Hana Baccouch
 * Copyright 2014 Inria
 * All rights reserved. Distributed only with permission.
 *---------------------------------------------------------------------------*/


#ifndef __BITMAP_H__
#define __BITMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#define BITS_PER_BYTE 8
#define BYTE_OF_BIT(bit_index) ((bit_index) / BITS_PER_BYTE)
#define INDEX_OF_BIT(bit_index) ((bit_index) % BITS_PER_BYTE)
#define BYTES_PER_BITMAP(nb_bit) (((nb_bit) + BITS_PER_BYTE-1)/ BITS_PER_BYTE)
#define MASK_OF_INDEX(index) (1<<(index))

static inline void bitmap_init(uint8_t* bitmap, uint16_t byte_size)
{ memset(bitmap, 0, byte_size); }

static inline void bitmap_set_bit(uint8_t* bitmap, uint16_t byte_size, uint16_t pos)
{
  //int byte_index = BYTE_OF_BIT(pos);
  ASSERT( BYTE_OF_BIT(pos) < byte_size );
  bitmap[BYTE_OF_BIT(pos)] |= MASK_OF_INDEX(INDEX_OF_BIT(pos));
}

static inline void bitmap_clear_bit(uint8_t* bitmap, uint16_t byte_size, uint16_t pos)
{
  //int byte_index = BYTE_OF_BIT(pos);
  ASSERT( BYTE_OF_BIT(pos) < byte_size );
  bitmap[BYTE_OF_BIT(pos)] &= ~(uint8_t)MASK_OF_INDEX(INDEX_OF_BIT(pos));
}

static inline bool_t bitmap_get_bit(uint8_t* bitmap, uint16_t byte_size, uint16_t pos)
{
  //int byte_index = BYTE_OF_BIT(pos);
  ASSERT( BYTE_OF_BIT(pos) < byte_size );
  return (bitmap[BYTE_OF_BIT(pos)] & MASK_OF_INDEX(INDEX_OF_BIT(pos))) != 0;
}

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __BITMAP_H__ */
