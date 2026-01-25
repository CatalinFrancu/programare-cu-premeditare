#include <stdio.h>

const int MOD = 1'000'000'007;
const int QUARTER = 250'000'002;

long long pow2(long long e) {
  long long result = 1;
  long long b = 2;

  while (e) {
    if (e & 1) {
      result = result * b % MOD;
    }
    b = b * b % MOD;
    e >>= 1;
  }

  return result;
}

int count_grids(long long n) {
  long long sum;
  if (n % 2 == 0) {
    sum =
      pow2(n * n) +          // nicio rotație
      2 * pow2(n * n / 4) +  // rotație cu 90/270°
      pow2(n * n / 2);       // rotație cu 180°
  } else {
    sum =
      pow2(n * n) +
      2 * pow2((n / 2) * (n / 2) + (n / 2) + 1) +
      pow2((n /2) * n + (n / 2) + 1);
  }

  return sum * QUARTER % MOD;
}

int main() {
  int n;
  scanf("%d", &n);
  printf("%d\n", count_grids(n));

  return 0;
}
