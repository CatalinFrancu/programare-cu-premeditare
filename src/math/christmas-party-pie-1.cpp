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

int count_derangements(int n) {
  long long fact = 1;
  for (int i = 1; i <= n; i++) {
    fact = fact * i % MOD;
  }

  long long inv_fact = inverse(fact);
  int sign = (n % 2) ? -1 : 1;
  long long sum = 0;

  for (int i = n; i >= 0; i--) {
    // Invarianți:
    //   sign = (-1)^i
    //   inv_fact = 1/i!
    sum = (sum + fact * sign * inv_fact) % MOD;
    sign = -sign;
    inv_fact = inv_fact * i % MOD;
  }

  return sum;
}

int main() {
  int n;
  scanf("%d", &n);
  printf("%d\n", count_derangements(n));

  return 0;
}
