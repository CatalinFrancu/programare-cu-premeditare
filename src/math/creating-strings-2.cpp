#include <ctype.h>
#include <stdio.h>

const int MAX_N = 1'000'000;
const int MOD = 1'000'000'007;
const int SIGMA = 26;

int fact[MAX_N + 1], inv_fact[MAX_N + 1];
int freq[SIGMA];
int n;

void read_string() {
  int c;
  while (islower(c = getchar())) {
    freq[c - 'a']++;
    n++;
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

void precompute_factorials() {
  fact[0] = 1;
  for (int i = 1; i <= n; i++) {
    fact[i] = (long long)fact[i - 1] * i % MOD;
  }

  inv_fact[n] = inverse(fact[n]);
  for (int i = n - 1; i >= 0; i--) {
    inv_fact[i] = (long long)inv_fact[i + 1] * (i + 1) % MOD;
  }
}

void count_permutations() {
  long long answer = fact[n];
  for (int c = 0; c < SIGMA; c++) {
    answer = answer * inv_fact[freq[c]] % MOD;
  }
  printf("%lld\n", answer);
}

int main() {
  read_string();
  precompute_factorials();
  count_permutations();

  return 0;
}
