#include <stdio.h>

const int MOD = 1'000'000'007;

int count_derangements(int n) {
  if (n == 0) {
    return 1;
  } else if (n == 1) {
    return 0;
  }

  int a, b = 0, c = 1; // termenii 1 și 2
  for (int i = 3; i <= n; i++) {
    a = b;
    b = c;
    c = (long long)(i - 1) * (a + b) % MOD;
  }
  return c;
}

int main() {
  int n;
  scanf("%d", &n);
  printf("%d\n", count_derangements(n));

  return 0;
}
