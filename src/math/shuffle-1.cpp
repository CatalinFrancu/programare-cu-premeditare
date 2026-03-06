#include <stdio.h>

const int MAX_N = 5'000;
const int MOD = 998'244'353;

int fact[MAX_N + 1], inv_fact[MAX_N + 1];
char s[MAX_N + 1];
int n, k, total_ones;

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
  for (int i = 1; i <= n; i++) {
    fact[i] = (long long)fact[i - 1] * i % MOD;
  }

  inv_fact[n] = inverse(fact[n]);
  for (int i = n - 1; i >= 0; i--) {
    inv_fact[i] = (long long)inv_fact[i + 1] * (i + 1) % MOD;
  }
}

void read_data() {
  scanf("%d %d ", &n, &k);
  for (int i = 0; i < n; i++) {
    s[i] = getchar() - '0';
    total_ones += (s[i] == 1);
  }
}

int count_shuffles(int l, int r, int num_ones) {
  if (num_ones > k) {
    return 0;
  }

  int num_zeroes = r - l + 1 - num_ones;

  int outer_zeroes = (s[l] == 0) + (s[r] == 0);
  int outer_ones = 2 - outer_zeroes;

  int inner_zeroes = num_zeroes - outer_ones;
  int inner_ones = num_ones - outer_zeroes;

  return ((inner_zeroes < 0) || (inner_ones < 0))
    ? 0
    : comb(inner_zeroes + inner_ones, inner_ones);
}

int try_all_pairs() {
  long long result = 0;

  if (total_ones >= k) {
    for (int l = 0; l < n; l++) {
      int num_ones = (s[l] == 1);
      for (int r = l + 1; r < n; r++) {
        num_ones += (s[r] == 1);
        result += count_shuffles(l, r, num_ones);
      }
    }
  }

  result++; // neschimbat

  return result % MOD;
}

void write_answer(int answer) {
  printf("%d\n", answer);
}

int main() {
  read_data();
  precompute_factorials();
  int answer = try_all_pairs();
  write_answer(answer);

  return 0;
}
