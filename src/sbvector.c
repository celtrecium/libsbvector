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

#include "../include/sbvector.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define _get_size(size, block)                                                \
  (((size / block) + (size % block ? 1 : 0)) * block)

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

sbvector_t
sbvector (size_t datasz, size_t tsize, size_t blocksz, bool fixcapacity)
{
  sbvector_t vec = { NULL, 0, 0, 0, 0, false };

  if (tsize == 0 || blocksz == 0)
    return vec;
  
  vec._single_block_size = blocksz;
  vec.length = 0;
  vec._capacity = _get_size (datasz, blocksz);
  vec._typesize = tsize;
  vec.vector = calloc (vec._capacity, tsize);
  vec._is_capacity_fixed = fixcapacity;
  
  return vec;
}

sbvector_t
sbvector_from_array (void *array, size_t arrsz, size_t tsize, size_t blocksz,
                     bool fixcapacity)
{
  sbvector_t vect = sbvector (arrsz, tsize, blocksz, fixcapacity);

  vect.length = arrsz;
  memcpy (vect.vector, array, tsize * arrsz);

  return vect;
}


bool
sbv_resize_capacity (sbvector_t *sbv, size_t newsize)
{
  void *tmp = NULL;
  
  if (!sbv)
    return false;

  tmp = sbv->vector;
  
  if (sbv->_capacity / 2 < newsize || sbv->_capacity > newsize)
    {
      sbv->_capacity = newsize == 0
                           ? sbv->_single_block_size
                           : _get_size (newsize, sbv->_single_block_size);

      tmp = sbv->vector;
      
      sbv->vector = realloc (sbv->vector, sbv->_capacity);

      if (!sbv->vector)
        {
          sbv->vector = tmp;

          return false;
        }
    }
  
  return true;
}

bool
sbv_resize (sbvector_t *sbv, size_t newsize)
{
  if (!sbv_resize_capacity (sbv, newsize))
    return false;

  sbv->length = newsize;

  return true;
}

bool
sbv_free (sbvector_t *sbv)
{
  if (sbv == NULL)
    return false;

  sbv_clear (sbv);
  if (sbv->vector != NULL)
    free (sbv->vector);
  
  return true;
}

void *
__sbv_set_f (sbvector_t *sbv, size_t index)
{
  if (!sbv)
    return NULL;

  if (!sbv->_is_capacity_fixed)
    {
      if (index > sbv->_capacity)
        return NULL;
    }
  else if (index > sbv->_capacity)
    {
      if (!sbv_resize_capacity (sbv, index))
        return NULL;
    }

  sbv->length = sbv->length < index ? index : sbv->length;

  return (char *)sbv->vector + (sbv->length - 1) * sbv->_typesize;
}

bool
sbv_pop (sbvector_t *sbv)
{
  if (!sbv || sbv->length == 0)
    return false;

  if (!sbv->_is_capacity_fixed)
    {
      if (!sbv_resize (sbv, sbv->length - 1))
        return false;
    }

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

  if (sbv->_is_capacity_fixed)
    sbv->length = 0;
  else
    {
      if (!sbv_resize (sbv, 0))
        return false;
    }

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

sbslice_t
sbslice (sbvector_t *sbv, size_t begin, size_t end)
{
  sbslice_t slice = { NULL, 0, 0 };

  if (!sbv)
    return slice;

  if (begin > end)
    _swap_size_t (&begin, &end);

  if (begin >= sbv->length)
    return slice;

  slice.vector = sbv;
  slice.slice = __sbv_get_f (sbv, begin);
  slice.length = end > sbv->length ? sbv->length - begin : end - begin;
  
  return slice;
}

void *
__sbslice_get_f (sbslice_t *sbsl, size_t index)
{
  if (!sbsl || sbsl->length <= index)
    return NULL;

  return _get_element(sbsl->slice, sbsl->vector->_typesize, index);
}

sbvector_t
sbv_copy_slice (sbslice_t *sbsl)
{
  return sbvector_from_array (
      sbsl->slice, sbsl->length, sbsl->vector->_typesize,
      sbsl->vector->_single_block_size, sbsl->vector->_is_capacity_fixed);
}
