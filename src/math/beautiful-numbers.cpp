#include <stdio.h>

const int MAX_N = 1'000'000;
const int MOD = 1'000'000'007;

int fact[MAX_N + 1];
bool is_favorite[10];

void extended_euclid(int a, int b, int& d, int& x, int& y) {
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
  // Rezolvă x * y + k * MOD = 1.
  int y, k, d;
  extended_euclid(x, MOD, d, y, k);
  return (y >= 0) ? y : (y + MOD);
}

void precompute(int n) {
  fact[0] = 1;
  for (int i = 1; i <= n; i++) {
    fact[i] = (long long)fact[i - 1] * i % MOD;
  }
}

bool is_beautiful(int x) {
  while (x) {
    if (!is_favorite[x % 10]) {
      return false;
    }
    x /= 10;
  }
  return true;
}

__int128 comb(int n, int k) {
  // n! / [ k! * (n-k)! ]
  int denom = (long long)fact[k] * fact[n - k] % MOD;
  return (__int128)fact[n] * inverse(denom);
}

int iterate_a_and_b(int n, int a, int b) {
  is_favorite[a] = is_favorite[b] = true;
  __int128 result = 0;

  for (int num_a = n, num_b = 0; num_a >= 0; num_a--, num_b++) {
    int sum = a * num_a + b * num_b;
    if (is_beautiful(sum)) {
      // Dacă suma este frumoasă, atunci orice combinație de num_a a-uri și
      // num_b b-uri va fi frumoasă.
      result += comb(n, num_a);
    }
  }

  return result % MOD;
}

int main() {
  int a, b, n;

  scanf("%d %d %d", &a, &b, &n);
  precompute(n);
  int result = iterate_a_and_b(n, a, b);
  printf("%d\n", result);

  return 0;
}
