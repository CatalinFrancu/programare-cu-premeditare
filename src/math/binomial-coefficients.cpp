#include <stdio.h>

const int MAX_N = 1'000'000;
const int MOD = 1'000'000'007;

int fact[MAX_N + 1], inv_fact[MAX_N + 1];

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

int comb(int n, int k) {
  return (long long)fact[n] * inv_fact[k] % MOD * inv_fact[n - k] % MOD;
}

void precompute_factorials() {
  fact[0] = 1;
  for (int i = 1; i <= MAX_N; i++) {
    fact[i] = (long long)fact[i - 1] * i % MOD;
  }

  inv_fact[MAX_N] = inverse(fact[MAX_N]);
  for (int i = MAX_N - 1; i >= 0; i--) {
    inv_fact[i] = (long long)inv_fact[i + 1] * (i + 1) % MOD;
  }
}

void process_queries() {
  int num_tests, n, k;
  scanf("%d", &num_tests);
  while (num_tests--) {
    scanf("%d %d", &n, &k);
    printf("%d\n", comb(n, k));
  }
}

int main() {
  precompute_factorials();
  process_queries();

  return 0;
}
