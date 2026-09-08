#include <stdio.h>

typedef unsigned long long u64;

const int MAX_N = 100'000;
const int BASE = 61; // modulo 2^64

char s[MAX_N + 2];
int spf[MAX_N + 1]; // spf[x] = cel mai mic factor prim al lui x
u64 bpow[MAX_N + 1];
u64 h[MAX_N + 1];
int n;

void read_and_hash() {
  scanf("%d %s", &n, s + 1);

  bpow[0] = 1;
  for (int i = 1; i <= n; i++) {
    h[i] = h[i - 1] * BASE + (s[i] - 'a');
    bpow[i] = bpow[i - 1] * BASE;
  }
}

void sieve() {
  spf[1] = 1;
  for (int i = 2; i <= n; i++) {
    if (!spf[i])  {
      for (int j = i; j <= n; j += i) {
        if (!spf[j]) {
          spf[j] = i;
        }
      }
    }
  }
}

u64 substr_hash(int l, int r) {
  return h[r] - h[l - 1] * bpow[r - l + 1];
}

bool matches(int l, int r, int per) {
  return substr_hash(l, r - per) == substr_hash(l + per, r);
}

int query(int l, int r) {
  int len = r - l + 1;
  bool found = false;

  while (!found && (len > 1)) {
    int x = spf[len];
    found = matches(l, r, (r - l + 1) / x);
    do {
      len /= x;
    } while (len % x == 0);
  }

  return found;
}

void process_queries() {
  int q, l, r;

  scanf("%d", &q);
  while (q--) {
    scanf("%d %d", &l, &r);
    l++;
    r++;
    printf(query(l, r) ? "YES\n" : "NO\n");
  }
}

int main() {
  read_and_hash();
  sieve();
  process_queries();

  return 0;
}
