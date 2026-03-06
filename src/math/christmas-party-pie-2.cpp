#include <stdio.h>

const int MOD = 1'000'000'007;

int count_derangements(int n) {
  long long n_fact_over_i_fact = 1;
  int sign = (n % 2) ? -1 : 1;
  long long sum = 0;

  for (int i = n; i >= 0; i--) {
    sum = (sum + sign * n_fact_over_i_fact) % MOD;
    sign = -sign;
    n_fact_over_i_fact = n_fact_over_i_fact * i % MOD;
  }

  return (sum + MOD) % MOD;
}

int main() {
  int n;
  scanf("%d", &n);
  printf("%d\n", count_derangements(n));

  return 0;
}
