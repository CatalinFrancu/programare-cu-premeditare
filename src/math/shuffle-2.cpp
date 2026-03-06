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

void extend_to_k_ones(int& r, int& ones) {
  while ((r < n - 1) && (ones < k)) {
    r++;
    ones += (s[r] == 1);
  }

  // Consumă și zerourile.
  while ((r < n - 1) && (s[r + 1] == 0)) {
    r++;
  }
}

void contract(int& l, int& ones) {
  ones -= (s[l] == 1);
}

int count_shuffles(int l, int r, int ones) {
  int zeroes = (r - l + 1) - ones;
  if (!ones || !zeroes) {
    // Fereastra are toate valorile egale. Nu putem schimba capătul stîng.
    return 0;
  }

  ones -= (s[l] == 0);
  zeroes -= (s[l] == 1);
  return comb(zeroes + ones, ones);
}

int try_all_left_ends() {
  long long result = 1; // neschimbat

  int r = -1, ones = 0;
  for (int l = 0; l < n; l++) {
    extend_to_k_ones(r, ones);
    result += count_shuffles(l, r, ones);
    contract(l, ones);
  }

  return result % MOD;
}

void write_answer(int answer) {
  printf("%d\n", answer);
}

int main() {
  read_data();
  precompute_factorials();
  int answer = ((k == 0) || (total_ones < k))
    ? 1
    : try_all_left_ends();
  write_answer(answer);

  return 0;
}
