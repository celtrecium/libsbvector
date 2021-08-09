/*
 * This file is part of Simple Block Vector.
 *
 * Simple Block Vector is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Simple Block Vector is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Simple Block Vector. If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include "sbvector.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
  
static inline size_t
_get_size (size_t size, size_t block)
{
  return (size / block + (size % block ? 1 : 0)) * block;
}

static inline void
_swap_size_t (size_t *x, size_t *y)
{
  *x ^= *y;
  *y ^= *x;
  *x ^= *y;
}

static inline void *
_get_element (void *vdptr, size_t tpsz, size_t ind)
{
  return (char *)vdptr + ind * tpsz;
}

static bool
_realloc_s (void **ptr, size_t size)
{
  void *tmp = *ptr;

  *ptr = realloc (*ptr, size);

  if (!*ptr)
    {
      *ptr = tmp;
      
      return false;
    }

  return true;
}

sbvector_t
sbvector (size_t tsize)
{
  sbvector_t vec = {
    NULL,                /* vector */
    SBV_DEFAULT_BLOCKSZ, /* _single_block_size */
    tsize,               /* _typesize */
    SBV_DEFAULT_BLOCKSZ, /* _capacity */
    0                    /* length */
  };

  if (tsize)
    vec.vector = calloc (vec._capacity, tsize * vec._capacity);
  
  return vec;
}

sbvector_t
sbvector_from_array (const void *array, size_t arrsz, size_t tsize)
{
  sbvector_t vect = sbvector (tsize);

  if (array)
    {
      sbv_resize (&vect, arrsz);
      memcpy (vect.vector, array, tsize * arrsz);
    }

  return vect;
}

bool
sbv_resize (sbvector_t *sbv, size_t newsize)
{
  if (!sbv)
    return false;

  if (sbv->_capacity < newsize)
    {
      sbv->_capacity = !newsize ? sbv->_block_size
                                : _get_size (newsize, sbv->_block_size);
      
      if (!_realloc_s (&sbv->vector, sbv->_capacity * sbv->_typesize))
        return false;
    }
  
  sbv->length = newsize;

  return true;
}

bool
sbv_free (sbvector_t *sbv)
{
  if (!sbv)
    return false;

  sbv_clear (sbv);
  
  if (sbv->vector)
    free (sbv->vector);
  
  return true;
}

void *
__sbv_set_f (sbvector_t *sbv, size_t index)
{
  if (!sbv)
    return NULL;

  if (index > sbv->length)
    sbv_resize (sbv, index);

  return _get_element (sbv->vector, sbv->_typesize, index);
}

bool
sbv_pop (sbvector_t *sbv)
{
  if (!sbv || !sbv->length)
    return false;

  if (!sbv_resize (sbv, sbv->length - 1))
    return false;

  return true;
}

void *
__sbv_get_f (sbvector_t *sbv, size_t index)
{
  if (!sbv || sbv->length <= index)
    return NULL;

  return _get_element (sbv->vector, sbv->_typesize, index);
}

bool
sbv_clear (sbvector_t *sbv)
{
  if (!sbv)
    return false;

  sbv->length = 0;

  return true;
}

bool
sbv_copy (sbvector_t *dest, sbvector_t *src)
{
  if (!dest || !src || dest->_typesize != src->_typesize)
    return false;

  sbv_resize (dest, src->length);
  memcpy (dest->vector, src->vector, src->length * src->_typesize);
  
  return true;
}

bool
sbv_crop_capacity (sbvector_t *sbv)
{
  size_t tmpsz = 0;

  if (!sbv)
    return false;

  tmpsz = _get_size (sbv->length, sbv->_block_size);

  if (tmpsz != sbv->_capacity)
    {
      if (!_realloc_s(&sbv->vector, tmpsz * sbv->_typesize))
        return false;

      sbv->_capacity = tmpsz;
    }
  
  return true;
}

bool
sbv_set_blocksize (sbvector_t *sbv, size_t newblksz)
{
  size_t tmpsz = 0;
  
  if (!sbv || !newblksz)
    return false;

  sbv->_block_size = newblksz;
  tmpsz = _get_size (sbv->length, newblksz);
  
  if (tmpsz > sbv->_capacity)
    {
      if (!_realloc_s (&sbv->vector, tmpsz * sbv->_typesize))
        return false;

      sbv->_capacity = tmpsz;
    }
  
  return true;
}

sbslice_t
sbslice (sbvector_t *sbv, size_t begin, size_t end)
{
  sbslice_t slice = { sbv, 0, 0 };

  if (!sbv)
    return slice;

  if (begin > end)
    _swap_size_t (&begin, &end);

  if (begin >= sbv->length)
    return slice;

  slice.slice = __sbv_get_f (sbv, begin);
  slice.length = end > sbv->length ? sbv->length - begin : end - begin;
  slice.vector = sbv;
  
  return slice;
}

void *
__sbslice_get_f (sbslice_t *sbsl, size_t index)
{
  if (!sbsl || sbsl->length <= index)
    return NULL;

  return _get_element (sbsl->slice, sbsl->vector->_typesize, index);
}

sbvector_t
sbv_copy_slice (sbslice_t *sbsl)
{
  return sbvector_from_array (sbsl->slice, sbsl->length,
                              sbsl->vector->_typesize);
}
