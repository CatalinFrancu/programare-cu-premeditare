#include <stdio.h>

const int MOD = 1'000'000'007;

int bin_exp(int b, int e, int mod) {
  int result = 1;

  while (e) {
    if (e & 1) {
      result = (long long)result * b % mod;
    }
    b = (long long)b * b % mod;
    e >>= 1;
  }

  return result;
}

int main() {
  int n, a, b, c;
  scanf("%d", &n);
  while (n--) {
    scanf("%d %d %d", &a, &b, &c);
    int e = bin_exp(b, c, MOD - 1);
    int answer = bin_exp(a, e, MOD);
    printf("%d\n", answer);
  }
}
