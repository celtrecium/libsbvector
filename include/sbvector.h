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

/* Internal function to get a void pointer to the element at index to assign a
 * value. If the index exceeds the size of the vector, it will expand.
 */
SBVECT_API void *__sbv_set_f (sbvector_t *sbv, size_t index);

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

/* Internal function for filling vector value specify num times.
 * Macro for this function is available only in safe macros.
 */
SBVECT_API bool __sbv_fill_f (sbvector_t *sbv, void *data, size_t num);

/* -------------------------- Slice interfaces ----------------------------- */

/* Slice vector. */
SBVECT_API sbslice_t sbslice (sbvector_t *sbv, size_t begin, size_t end);

/* Internal function for getting a void pointer to an element by index. */
SBVECT_API void *__sbslice_get_f (sbslice_t *sbsl, size_t index);

/* Create a new vector and copy a slice of another vector into it */
SBVECT_API sbvector_t sbv_copy_slice (sbslice_t *sbsl);

/* --------------------------------- Macros -------------------------------- */

/* These are unsafe, generic macros. */
#define sbv_get(sbv, type, index) (*((type *)__sbv_get_f (sbv, index)))
#define sbv_set(sbv, type, index, data)                                       \
  (*((type *)__sbv_set_f (sbv, index)) = data)
#define sbv_push(sbv, type, data) sbv_set (sbv, type, (sbv)->length + 1, data)
#define sbslice_get(sbsl, type, index)                                        \
  (*((type *)__sbslice_get_f (sbsl, index)))
#define sbslice_set(sbsl, type, index, data)                                  \
  (sbslice_get (sbsl, type, index) = data)

/* This macro creates functions for working with a vector and a certain data
 * type. It is safe to use. */
#define sbv_define_type(type, postfix)                                        \
  static type *sbv_push_##postfix (sbvector_t *sbv, type data)                \
  {                                                                           \
    type *retdat = __sbv_set_f (sbv, sbv->length + 1);                        \
                                                                              \
    if (!retdat)                                                              \
      return NULL;                                                            \
                                                                              \
    *retdat = data;                                                           \
                                                                              \
    return retdat;                                                            \
  }                                                                           \
  static type *sbv_get_##postfix (sbvector_t *sbv, size_t index)              \
  {                                                                           \
    type *retdat = __sbv_get_f (sbv, index);                                  \
                                                                              \
    if (!retdat)                                                              \
      return NULL;                                                            \
                                                                              \
    return retdat;                                                            \
  }                                                                           \
  static type *sbv_set_##postfix (sbvector_t *sbv, size_t index, type data)   \
  {                                                                           \
    type *retdat = __sbv_set_f (sbv, index);                                  \
                                                                              \
    if (!retdat)                                                              \
      return NULL;                                                            \
                                                                              \
    *retdat = data;                                                           \
                                                                              \
    return retdat;                                                            \
  }                                                                           \
  static type *sbslice_get_##postfix (sbslice_t *sbsl, size_t index)          \
  {                                                                           \
    type *retdat = __sbslice_get_f (sbsl, index);                             \
                                                                              \
    if (!retdat)                                                              \
      return NULL;                                                            \
                                                                              \
    return retdat;                                                            \
  }                                                                           \
  static type *sbslice_set_##postfix (sbslice_t *sbsl, size_t index,          \
                                      type data)                              \
  {                                                                           \
    type *retdat = __sbslice_get_f (sbsl, index);                             \
                                                                              \
    if (!retdat)                                                              \
      return NULL;                                                            \
                                                                              \
    *retdat = data;                                                           \
                                                                              \
    return retdat;                                                            \
  }                                                                           \
  static bool sbv_fill_##postfix (sbvector_t *sbv, type data, size_t num)     \
  {                                                                           \
    return __sbv_fill_f (sbv, &data, num);                                    \
  }

#endif /* SBVECTOR_H */
