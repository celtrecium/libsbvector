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

#define _get_size(size, block) (((size / block) + !!(size % block)) * block)

sbvector_t
sbvector (size_t datasz, size_t tsize, size_t blocksz, bool fixcapacity)
{
  sbvector_t vec = {NULL, 0, 0, 0, 0, false, SBV_ZERO_PARAMS};

  if (tsize == 0 || blocksz == 0)
    return vec;
  
  vec._single_block_size = blocksz;
  vec.length = 0;
  vec._capacity = _get_size (datasz, blocksz);
  vec._typesize = tsize;
  vec.err = SBV_OK;
  
  if ((vec.vector = calloc (vec._capacity, tsize)) == NULL)
    vec.err = SBV_MALLOC_ERR;
  
  vec._fixed_capacity = fixcapacity;
  
  return vec;
}

int
sbv_resize (sbvector_t *sbv, size_t newsize)
{
  void *tmp = NULL;
  
  if (sbv == NULL)
    return EXIT_FAILURE;

  sbv->err = SBV_OK;
  tmp = sbv->vector;
  
  if (sbv->_capacity / 2 < newsize || sbv->_capacity > newsize)
    {
      sbv->_capacity = newsize == 0
                           ? sbv->_single_block_size
                           : _get_size (newsize, sbv->_single_block_size);
      
      if ((sbv->vector = realloc (sbv->vector, sbv->_capacity)) == NULL)
        {
          sbv->vector = tmp;
          sbv->err = SBV_REALLOC_ERR;

          return EXIT_FAILURE;
        }
    }
  
  return EXIT_SUCCESS;
}

int
sbv_free (sbvector_t *sbv)
{
  if (sbv == NULL)
    return EXIT_FAILURE;

  sbv_clear (sbv);
  if (sbv->vector != NULL)
    free (sbv->vector);
  
  return EXIT_SUCCESS;
}

void *
__sbv_set_f (sbvector_t *sbv, size_t index)
{
  if (sbv == NULL)
    return NULL;

  sbv->err = SBV_OK;

  if (sbv->_fixed_capacity == true)
    {
      if (index > sbv->_capacity)
        {
          sbv->err = SBV_CAPACITY_OVERFLOW;
          
          return NULL;
        }
    }
  else if (index > sbv->_capacity)
    {
      if (sbv_resize (sbv, index) == EXIT_FAILURE)
        return NULL;
    }

  sbv->length = sbv->length < index ? index : sbv->length;

  return (char *)sbv->vector + (sbv->length - 1) * sbv->_typesize;
}

int
sbv_pop (sbvector_t *sbv)
{
  if (sbv == NULL || sbv->length == 0)
    return EXIT_FAILURE;

  sbv->err = SBV_OK;
  
  if (sbv->_fixed_capacity != true)
    {
      if (sbv_resize (sbv, sbv->length - 1) == EXIT_FAILURE)
        return EXIT_FAILURE;
    }

  --sbv->length;

  return EXIT_SUCCESS;
}

void *
__sbv_get_f (sbvector_t *sbv, size_t index)
{
  if (sbv == NULL || sbv->length <= index)
    return NULL;

  sbv->err = SBV_OK;

  return (char *)sbv->vector + index * sbv->_typesize;
}

int
sbv_clear (sbvector_t *sbv)
{
  if (sbv == NULL)
    return EXIT_FAILURE;

  sbv->err = SBV_OK;
  
  if (sbv->_fixed_capacity == true)
    sbv->length = 0;
  else
    {
      if (sbv_resize (sbv, 0) == EXIT_FAILURE)
        return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
