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

#ifndef SBVECTOR_H
#define SBVECTOR_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if defined(_WIN32) || defined(__CYGWIN__)
# if defined(_MSC_VER)
#  define SBVECT_API __declspec(dllexport)
# elif (defined(__GNUC__) && (__GNUC__ >= 4))
#  define SBVECT_API __attribute__((dllexport))
# else
#  define SBVECT_API
# endif /* _MSC_VER */
#elif defined(__unix__)
# if (defined(__GNUC__) && (__GNUC__ >= 4))
#  define SBVECT_API __attribute__((visibility("default")))
# else
#  define SBVECT_API
# endif
#else
# define SBVECT_API
#endif /* _WIN32 || __CYGWIN__ */

#define SBV_DEFAULT_BLOCKSZ 20

typedef struct sbvector
{
  void *vector;
  size_t _block_size;
  size_t _type_size;
  size_t _capacity;
  size_t length;
} sbvector_t;

typedef struct sbslice
{
  void *slice;
  size_t length;
  sbvector_t *vector;
} sbslice_t;

/* ------------------------- Vector interfaces ----------------------------- */

/* Create vector. */
SBVECT_API sbvector_t sbvector (size_t typesize);

/* Create vector from array. */
SBVECT_API sbvector_t sbvector_from_array (const void *array, size_t array_size,
                                           size_t type_size);

/* Free vector.
 * Attention: if the vector contains pointers to the allocated memory, you need
 * to free them yourself.
 */
SBVECT_API bool sbv_free (sbvector_t *sbv);

/* Pop one element from back of vector.
 * Attention: if the element contains a pointer to the allocated memory, you
 * need to free it yourself.
 */
SBVECT_API bool sbv_pop (sbvector_t *sbv);

/* Clear vector.
 * Attention: if the vector contains pointers to the allocated memory, you need
 * to free them yourself.
 */
SBVECT_API bool sbv_clear (sbvector_t *sbv);

/* Internal function for getting a void pointer to an element by index. */
SBVECT_API void *__sbv_get_f (sbvector_t *sbv, size_t index);

/* Set new vector size */
SBVECT_API bool sbv_resize (sbvector_t *sbv, size_t new_size);

/* Copy src vector to dest vector. If the sizes of the types of vectors do not
 * match, then copying is impossible.
 */
SBVECT_API bool sbv_copy (sbvector_t *dest, sbvector_t *src);

/* Crop the vector's capacity if it is too large. */
SBVECT_API bool sbv_crop_capacity (sbvector_t *sbv);

/* Set the number of elements by which the capacity will be expanded when it
 * overflows.
 * By default it equals 20 (SBV_DEFAILT_BLOCKSZ macro).
 */
SBVECT_API bool sbv_set_blocksize (sbvector_t *sbv, size_t new_block_size);

/* Function for filling vector value specify num times.
 */
SBVECT_API bool sbv_fill (sbvector_t *sbv, void *data, size_t num);

/* -------------------------- Slice interfaces ----------------------------- */

/* Slice vector. */
SBVECT_API sbslice_t sbslice (sbvector_t *sbv, size_t begin, size_t end);

/* Internal function for getting a void pointer to an element by index. */
SBVECT_API void *__sbslice_get_f (sbslice_t *sbsl, size_t index);

/* Create a new vector and copy a slice of another vector into it */
SBVECT_API sbvector_t sbv_copy_slice (sbslice_t *sbsl);

/* --------------------------------- Macros -------------------------------- */

/* These are generic macros. */
#define sbv_get(sbv, type, index) ((type *)__sbv_get_f (sbv, index))
#define sbv_set(sbv, type, index, data)                                       \
  (index < (sbv)->length ? (*((type *)__sbv_get_f (sbv, index)) = (data))     \
                         : (data))
#define sbv_push(sbv, type, data)                                             \
  (sbv_resize (sbv, (sbv)->length + 1)                                        \
       ? sbv_set (sbv, type, (sbv)->length - 1, (data))                           \
       : (data))
#define sbslice_get(sbsl, type, index)                                        \
  ((type *)__sbslice_get_f (sbsl, index))
#define sbslice_set(sbsl, type, index, data)                                  \
  (sbsl->length > index ? sbslice_get ((sbsl, type, index) = (data)) : 0)

#endif /* SBVECTOR_H */
