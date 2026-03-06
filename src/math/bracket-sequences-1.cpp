//   comb(2n, n) / (n + 1) =
//   (2n)! / (n! * n! * (n + 1)) =
//   (n + 2) * (n + 3) * ... * (2n) / n!
#include <stdio.h>

const int MOD = 1'000'000'007;

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

int catalan(int n) {
  long long fact = 1;
  for (int i = 1; i <= n; i++) {
    fact = fact * i % MOD;
  }

  long long result = inverse(fact);
  for (int i = n + 2; i <= 2 * n; i++) {
    result = result * i % MOD;
  }

  return result;
}

int num_strings_of_length(int n) {
  return (n % 2)
    ? 0
    : catalan(n / 2);
}

int main() {
  int n;
  scanf("%d", &n);
  printf("%d\n", num_strings_of_length(n));

  return 0;
}
