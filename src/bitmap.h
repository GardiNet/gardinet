/*---------------------------------------------------------------------------
 * Copyright 2014-2017 Inria
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
 * @brief   Bitmap macros/inline functions
 *
 * @author  Cedric Adjih <cedric.adjih@inria.fr>
 * @author  Hannah Baccouch <hannah.baccouch@inria.fr>
 */

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

static inline void bitmap_set_bit(uint8_t* bitmap, uint16_t byte_size,
				  uint16_t pos)
{
  //int byte_index = BYTE_OF_BIT(pos);
  ASSERT( BYTE_OF_BIT(pos) < byte_size );
  bitmap[BYTE_OF_BIT(pos)] |= MASK_OF_INDEX(INDEX_OF_BIT(pos));
}

static inline void bitmap_clear_bit(uint8_t* bitmap, uint16_t byte_size,
				    uint16_t pos)
{
  //int byte_index = BYTE_OF_BIT(pos);
  ASSERT( BYTE_OF_BIT(pos) < byte_size );
  bitmap[BYTE_OF_BIT(pos)] &= ~(uint8_t)MASK_OF_INDEX(INDEX_OF_BIT(pos));
}

static inline bool_t bitmap_get_bit(uint8_t* bitmap, uint16_t byte_size,
				    uint16_t pos)
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
/*---------------------------------------------------------------------------*/
/** @} */
