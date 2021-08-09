#include <sbvector.h>
#include <stdio.h>

sbv_define_type (int, int)

int
main (void)
{
  const int array[] = { 1, 2, 3, 4, 5, 6, 7 };
  sbvector_t vect = sbvector_from_array (array, 7, sizeof (int));

  sbslice_t slice = sbslice (&vect, 2, 5);
  size_t i;

  /* Print numbers from vector */
  fputs ("Vector:\n", stdout);
  
  for (i = 0; i < vect.length; ++i)
    printf ("%d ", *sbv_get_int (&vect, i));

  /* Print numbers from slice of vector */
  fputs ("\nVector[2:5]:\n", stdout);

  for (i = 0; i < slice.length; ++i)
    printf ("%d ", *sbslice_get_int (&slice, i));

  putchar ('\n');

  sbv_free (&vect);

  return EXIT_SUCCESS;
}
