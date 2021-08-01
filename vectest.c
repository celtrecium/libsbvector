#include <sbvector.h>
#include <stdlib.h>
#include <stdio.h>

sbv_define_type (int)

int
main (void)
{
  sbvector_t vect = sbvector (3, sizeof (int), 10, true);
  
  sbv_push_int (&vect, 94);
  sbv_push_int (&vect, 115);
  sbv_push_int (&vect, 927);
  
  printf ("_typesize = %lu\n"
          "_capacity = %lu\n"
          "length = %lu\n"
          "---------------\n",
          vect._typesize,
          vect._capacity,
          vect.length);
  
  printf ("%d %d %d\n",
          sbv_get_int (&vect, 0),
          sbv_get_int (&vect, 1),
          sbv_get_int (&vect, 2));

  sbv_pop (&vect);
  sbv_pop (&vect);  

  sbv_push_int (&vect, 359);
  sbv_push_int (&vect, 1488);

  printf ("%d %d %d\n",
          sbv_get_int (&vect, 0),
          sbv_get_int (&vect, 1),
          sbv_get_int (&vect, 2));
  
  sbv_free (&vect);
  
  return EXIT_SUCCESS;
}
