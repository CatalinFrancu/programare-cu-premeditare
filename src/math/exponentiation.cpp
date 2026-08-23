#include <stdio.h>

const int MOD = 1'000'000'007;

int bin_exp(int b, int e) {
  int result = 1;

  while (e) {
    if (e & 1) {
      result = (long long)result * b % MOD;
    }
    b = (long long)b * b % MOD;
    e >>= 1;
  }

  return result;
}

int main() {
  int n, a, b;
  scanf("%d", &n);
  while (n--) {
    scanf("%d %d", &a, &b);
    printf("%d\n", bin_exp(a, b));
  }
}
