#include <stdio.h>

const int NUM_BITS = 30;

unsigned ask(unsigned a, unsigned b) {
  printf("? %u %u\n", a, b);
  fflush(stdout);

  int response;
  scanf("%d", &response);
  return response;
}

bool kth_bit_is_1(int k, int so_far) {
  int gcd = ask((1 << k) - so_far,
                (3 << k) - so_far);
  return gcd % (2 << k) == 0;
}

void solve_test_case() {
  unsigned answer = 0;

  for (int k = 0; k < NUM_BITS; k++) {
    bool bit = kth_bit_is_1(k, answer);
    answer += (bit << k);
  }

  printf("! %u\n", answer);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test_case();
  }

  return 0;
}
