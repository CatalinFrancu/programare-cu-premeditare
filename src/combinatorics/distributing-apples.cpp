#include <stdio.h>

const int MAX_VAL = 2'000'000;
const int MOD = 1'000'000'007;

int fact[MAX_VAL + 1], inv_fact[MAX_VAL + 1];

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

void precompute_factorials() {
  fact[0] = 1;
  for (int i = 1; i <= MAX_VAL; i++) {
    fact[i] = (long long)fact[i - 1] * i % MOD;
  }

  inv_fact[MAX_VAL] = inverse(fact[MAX_VAL]);
  for (int i = MAX_VAL - 1; i >= 0; i--) {
    inv_fact[i] = (long long)inv_fact[i + 1] * (i + 1) % MOD;
  }
}

int comb(int n, int k) {
  return (long long)fact[n] * inv_fact[k] % MOD * inv_fact[n - k] % MOD;
}

void process_query() {
  int n, k;
  scanf("%d %d", &n, &k);
  printf("%d\n", comb(n + k - 1, k));
}

int main() {
  precompute_factorials();
  process_query();

  return 0;
}
