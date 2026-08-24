#include <stdio.h>

const int MAX_N = 100'000;
const int MAX_MODULI = 60; // 2 * log_2{MAX_VAL}

int w[MAX_N + 1];
int mod[MAX_MODULI];
int n, last_mod;

void read_array() {
  scanf("%d %d", &n, &mod[0]);
  for (int i = 1; i <= n; i++) {
    scanf("%d", &w[i]);
  }
}

int euler_totient(int n) {
  int phi = n;
  for (int d = 2; d * d <= n; d++) {
    if (n % d == 0) {
      phi = phi / d * (d - 1);
      while (n % d == 0) {
        n /= d;
      }
    }
  }
  if (n > 1) {
    phi = phi / n * (n - 1);
  }
  return phi;
}

void compute_moduli() {
  last_mod = 0;
  while (mod[last_mod] != 1) {
    ++last_mod;
    mod[last_mod] = euler_totient(mod[last_mod - 1]);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int dirty_mod(long long x, int m) {
  return (x > m) ? (m + x % m) : x;
}

int bin_exp(int b, int e, int mod) {
  long long result = 1;

  while (e) {
    if (e & 1) {
      result = dirty_mod(result * b, mod);
    }
    b = dirty_mod((long long)b * b, mod);
    e >>= 1;
  }

  return result;
}

int query(int l, int r) {
  int end = min(l + last_mod - 1, r - 1);
  int res = w[end + 1];

  for (int i = end; i >= l; i--) {
    int reduced = dirty_mod(res, mod[i - l + 1]);
    res = bin_exp(w[i], reduced, mod[i - l]);
  }
  return res % mod[0];
}

void process_queries() {
  int num_queries, l, r;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &l, &r);
    printf("%d\n", query(l, r));
  }
}

int main() {
  read_array();
  compute_moduli();
  process_queries();

  return 0;
}
