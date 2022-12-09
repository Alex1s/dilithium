#include <stdio.h>
#include "../randombytes.h"
#include "../polyvec.h"

static void print_polyvecl(polyvecl *v) {
  size_t i;
  size_t j;

  printf("[\n");
  for(i = 0; i < L; i++) {

    printf("[");
    for(j = 0; j < N; ++j) {
      if(j < N - 1)
        printf("%d, ", v->vec[i].coeffs[j]);
      else
        printf("%d]", v->vec[i].coeffs[j]);
    }
    if(i < L - 1)
      printf(",\n");
    else
      printf("\n");
  }
  printf("]\n");
}

static uint8_t is_in_polyvecl(polyvecl *v, int32_t needle) {
  size_t i;
  size_t j;

  for(i = 0; i < L; ++i)
    for(j = 0; j < N; ++j)
      if(v->vec[i].coeffs[j] == needle)
        return 1;

  return 0;
}

int main(void)
{
  uint16_t i, j;
  polyvecl y;
  uint8_t seed[CRHBYTES] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};
  for(i = 0; i < L; ++i)
    for(j = 0; j < N; ++j)
      y.vec[i].coeffs[j] = i * 1000 + j;

  print_polyvecl(&y);
  polyvecl_shuffle(&y, seed, 5);
  print_polyvecl(&y);

  for(i = 0; i < L; ++i)
    for(j = 0; j < N; ++j)
      if(!is_in_polyvecl(&y, i * 1000 + j))
        fprintf(stderr, "Needle %dl which was at (%hu, %hu) was not found in the shuffled polyvecl\n", i * 1000 + j, i, j);

  printf("Shuffling seems to be okay.");
  return 0;
}
