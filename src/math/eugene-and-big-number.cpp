#include <stdio.h>

// get_decimal_mask(123) = 1000
long long get_decimal_mask(int a) {
  long long d = 1;
  while (a) {
    d *= 10;
    a /= 10;
  }
  return d;
}

int prog_sum(long long a, long long n, int mod) {
  int num_bits = 64 - __builtin_clzll(n);
  long long sum = 0;
  long long ak = 1;

  for (int i = 0; i < num_bits; i++) {
    sum = (1 + ak) * sum % mod;
    ak = ak * ak % mod;

    int bit = (n >> (num_bits - i - 1)) & 1;
    if (bit) {
      sum = (1 + a * sum) % mod;
      ak = ak * a % mod;
    }
  }

  return sum;
}

int process_test(int a, long long n, int mod) {
  long long d = get_decimal_mask(a);
  int psum = prog_sum(d, n, mod);
  return (long long)a * psum % mod;
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    int a, mod;
    long long n;
    scanf("%d %lld %d", &a, &n, &mod);
    printf("%d\n", process_test(a, n, mod));
  }

  return 0;
}
