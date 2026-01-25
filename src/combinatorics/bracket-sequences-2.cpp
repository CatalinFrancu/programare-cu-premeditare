// (k + 1) / (n + 1) * comb(2n - k, n)
#include <ctype.h>
#include <stdio.h>

const int MOD = 1'000'000'007;
int n, k;
bool feasible;

void read_string() {
  scanf("%d ", &n);
  int excess = 0;
  int c;

  while ((excess >= 0) && ispunct(c = getchar())) {
    k++;
    excess += (c == '(') ? +1 : -1;
  }

  feasible = (n % 2 == 0) && (excess >= 0);

  // Redu problema la lungime n, începînd cu k paranteze deschise.
  n = (n - k + excess) / 2;
  k = excess;
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

int extended_catalan() {
  if (!feasible) {
    return 0;
  }

  long long fact = 1;
  for (int i = 1; i <= n + 1; i++) {
    fact = fact * i % MOD;
  }
  long long result = (long long)(k + 1) * inverse(fact) % MOD;

  for (int i = n - k + 1; i <= 2 * n - k; i++) {
    result = result * i % MOD;
  }

  return result;
}

int main() {
  read_string();
  printf("%d\n", extended_catalan());

  return 0;
}
