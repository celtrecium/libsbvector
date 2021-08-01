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

typedef enum SBVERR
{
  SBV_OK,
  SBV_MALLOC_ERR,
  SBV_REALLOC_ERR,
  SBV_ZERO_PARAMS,
  SBV_CAPACITY_OVERFLOW
} SBVERR_T;

typedef struct sbvector
{
  void *vector;
  size_t _single_block_size;
  size_t _typesize;
  size_t _capacity;
  size_t length;
  bool _fixed_capacity;
  SBVERR_T err;
} sbvector_t;

SBVECT_API sbvector_t sbvector (size_t datasz, size_t tsize, size_t blocksz,
                      bool fixcapacity);

SBVECT_API int sbv_resize (sbvector_t *sbv, size_t newsize);
SBVECT_API int sbv_free (sbvector_t *sbv);
SBVECT_API inline void *__sbv_push_f (sbvector_t *sbv);
SBVECT_API int sbv_pop (sbvector_t *sbv);
SBVECT_API int sbv_clear (sbvector_t *sbv);
SBVECT_API void *__sbv_get_f (sbvector_t *sbv, size_t index);
SBVECT_API void *__sbv_set_f (sbvector_t *sbv, size_t index);

/* These are unsafe, generic macros. */
#define sbv_get(sbv, type, index) *((type *)__sbv_get_f (sbv, index))
#define sbv_set(sbv, type, index, data)                                       \
  (*((type *)__sbv_set_f (sbv, index)) = data)
#define sbv_push(sbv, type, data) sbv_set (sbv, type, sbv->length + 1, data)

/* This macro is responsible for creating safe push and get functions. */
/* It can not work with structures */
#define sbv_define_type(type)                                                 \
  type sbv_push_##type (sbvector_t *sbv, type data)                           \
  {                                                                           \
    type *retdat = __sbv_set_f (sbv, sbv->length + 1);                        \
                                                                              \
    if (sbv->err != SBV_OK)                                                   \
      return 0;                                                               \
                                                                              \
    return *retdat = data;                                                    \
  }                                                                           \
  type sbv_get_##type (sbvector_t *sbv, size_t index)                         \
  {                                                                           \
    type *retdat = __sbv_get_f (sbv, index);                                  \
                                                                              \
    if (retdat == NULL)                                                       \
      return 0;                                                               \
                                                                              \
    return *retdat;                                                           \
  }                                                                           \
  type sbv_set_##type (sbvector_t *sbv, size_t index, type data)              \
  {                                                                           \
    type *retdat = __sbv_set_f (sbv, index);                                  \
                                                                              \
    if (sbv->err != SBV_OK)                                                   \
      return 0;                                                               \
                                                                              \
    return *retdat = data;                                                    \
  }

#endif /* SBVECTOR_H */
