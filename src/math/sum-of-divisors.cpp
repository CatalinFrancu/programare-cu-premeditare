#include <math.h>
#include <stdio.h>

const int MOD = 1'000'000'007;
const int HALF = (1 + MOD) / 2;

int main() {
  long long n;
  scanf("%lld", &n);
  int root = sqrt(n);

  // Pînă la sqrt(n)
  long long result = 0;
  for (int d = 1; n / d > root; d++) {
    // printf("Ia divizorul %d de %lld ori\n", d, n / d);
    result = (result + n / d * d) % MOD;
  }

  // Dincolo de sqrt(n)
  for (int k = root; k >= 1; k--) {
    long long min_d = n / (k + 1) + 1;
    long long max_d = n / k;

    long long gauss = (min_d + max_d) % MOD *
      ((max_d - min_d + 1) % MOD) % MOD *
      HALF % MOD;

    result = (result + gauss * k) % MOD;
    // printf("Ia divizorii de la %lld la %lld de cîte %d ori\n", min_d, max_d, k);
  }

  printf("%lld\n", result);
  return 0;
}
