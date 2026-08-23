#include <stdio.h>

const int MAX_VAL = 100'000;

typedef unsigned long long u64;

int freq_gte[MAX_VAL + 2];
u64 triplets_having_f[MAX_VAL + 1];

void reset() {
  for (int i = 0; i <= MAX_VAL; i++) {
    freq_gte[i] = triplets_having_f[i] = 0;
  }
}

void read_frequencies() {
  int n, val;
  scanf("%d", &n);
  while (n--) {
    scanf("%d", &val);
    freq_gte[val]++;
  }
}

void partial_sum_frequencies() {
  for (int i = MAX_VAL - 1; i >= 0; i--) {
    freq_gte[i] += freq_gte[i + 1];
  }
}

void compute_triplets_having_f() {
  for (int x = MAX_VAL; x; x--) {
    u64& cnt = triplets_having_f[x];
    int left = 0;
    for (int b = x; b <= MAX_VAL; b += x) { // acesta este b din enunț
      int fb = freq_gte[b] - freq_gte[b + 1];
      int right = freq_gte[b + 1];
      cnt -= triplets_having_f[b];
      cnt += (u64)fb * (fb - 1) / 2 * (left + right);
      cnt += (u64)left * right * fb;
      cnt += (u64)fb * (fb - 1) * (fb - 2) / 6;
      left += fb;
    }
  }
}

u64 compute_sum_f() {
  u64 sum = 0;
  for (int i = 1; i <= MAX_VAL; i++) {
    sum += i * triplets_having_f[i];
  }
  return sum;
}

void solve_test() {
  reset();
  read_frequencies();
  partial_sum_frequencies();
  compute_triplets_having_f();
  u64 res = compute_sum_f();
  printf("%llu\n", res);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }
  return 0;
}
