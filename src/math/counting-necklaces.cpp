#include <stdio.h>

const int MAX_LEN = 1'000'000;
const int MOD = 1'000'000'007;

int p[MAX_LEN + 1];

void precompute_powers(int base, int up_to) {
  p[0] = 1;
  for (int i = 1; i <= up_to; i++) {
    p[i] = (long long)p[i - 1] * base % MOD;
  }
}

void extended_euclid_iterative(int a, int b, int& d, int& x, int& y) {
  x = 1;
  y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = x - q * xp; x = tmp;
    tmp = yp; yp = y - q * yp; y = tmp;
  }
  d = a;
}

int inverse(int x) {
  int y, k, d;
  extended_euclid_iterative(x, MOD, d, y, k);
  return (y >= 0) ? y : (y + MOD);
}

int count_necklaces(int len, int colors) {
  long long sum = 0;
  for (int i = 0; i < len; i++) {
    int d, x, y;
    extended_euclid_iterative(i, len, d, x, y);
    sum += p[d];
  }

  return sum % MOD * inverse(len) % MOD;
}

int main() {
  int len, colors;
  scanf("%d %d", &len, &colors);
  precompute_powers(colors, len);
  printf("%d\n", count_necklaces(len, colors));

  return 0;
}
