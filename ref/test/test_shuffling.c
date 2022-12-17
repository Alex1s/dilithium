#include <stdio.h>
#include "../polyvec.h"

#define TARGET_ITER (2)
#define COEFFS_PER_ITER (4)
#define ITERS_SHUFFLE_TEST (1000)
#define NUM_NON_ZERO_COEFFS (COEFFS_PER_ITER * TARGET_ITER)
#define NUM_ZERO_COEFFS (N - NUM_NON_ZERO_COEFFS)
#define EXPECTED_ZEROS_PER_ITER_DOUBLE (ITERS_SHUFFLE_TEST, NUM_ZERO_COEFFS / (double) L)


static void print_polyvecl(polyvecl *v) {
  uint16_t i, j;

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

static uint8_t is_in_polyvecl(polyvecl *v, int32_t number) {
  size_t i;
  size_t j;

  for(i = 0; i < L; ++i)
    for(j = 0; j < N; ++j)
      if(v->vec[i].coeffs[j] == number)
        return 1;

  return 0;
}

int main(void)
{
  uint16_t i, j;
  polyvecl y = {0}; // all zero
  uint8_t seed[CRHBYTES] = {0}; // all zero
  unsigned int total_zero_counts_per_poly[L] = {0}; // all zero

#ifndef SHUFFLE
  fprint(stderr, "Shuffling is not activated! This test can only be run if shuffling is activated. Activate shuffling by defining SHUFFLE.\n");
  return 1;
#endif

  for(i = 0; i < L; ++i)
    for(j = 0; j < N; ++j)
      y.vec[i].coeffs[j] = i * 1000 + j;

  printf("y before fault and before shuffle\n");
  print_polyvecl(&y);

  for(j = TARGET_ITER * COEFFS_PER_ITER; j < N; ++j)
    y.vec[0].coeffs[j] = 0;

  printf("y after fault and before shuffle\n");
  print_polyvecl(&y);

  printf("y after fault and after shuffle\n");
  polyvecl_shuffle(&y, seed, 0);
  print_polyvecl(&y);

  for(i = 0; i < L; ++i) {
    uint16_t upper = i == 0 ? NUM_NON_ZERO_COEFFS : N;
    for(j = 0; j < upper; ++j) {
      if(!is_in_polyvecl(&y, i * 1000 + j)) {
        fprintf(stderr, "The number %d got shuffled out of the polyvecl! This should never happen!\n", i * 1000 + j);
        return 1;
      }
    }
  }

  printf("Counting the zeros for each polynomial in the shuffled version of the faulted y vector:\n");
  for(int iter = 0; iter < ITERS_SHUFFLE_TEST; ++iter) {
    for(i = 0; i < L; ++i)
      for(j = 0; j < N; ++j)
        if(i == 0 && j > 2)
          y.vec[i].coeffs[j] = 0;
        else
          y.vec[i].coeffs[j] = i * 1000 + j;

    polyvecl_shuffle(&y, seed, iter);

    for(i = 0; i < L; ++i)
      for(j = 0; j < N; ++j)
        if(y.vec[i].coeffs[j] == 0)
          total_zero_counts_per_poly[i]++;
  }

  printf("Average zero count per polynomial over %d runs (we expect %f for every polynomial):\n", ITERS_SHUFFLE_TEST, EXPECTED_ZEROS_PER_ITER_DOUBLE);
  printf("PS: actually a bit more because of naturally occurring zeros ...\n");
  for(i = 0; i < L; ++i)
    printf("Poly %d: %f\n", i, total_zero_counts_per_poly[i] / (double) (ITERS_SHUFFLE_TEST));

  for(i = 0; i < L; ++i) {
    if(!(EXPECTED_ZEROS_PER_ITER_DOUBLE - 5 < total_zero_counts_per_poly[i] / (double) (ITERS_SHUFFLE_TEST) && total_zero_counts_per_poly[i] / (double) (ITERS_SHUFFLE_TEST) < EXPECTED_ZEROS_PER_ITER_DOUBLE + 5)) {
      fprintf(stderr, "Poly %d deviates too much from the expected amount of zero coeffs!\n", i);
      return 1;
    }
  }
}
