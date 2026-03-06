#include <stdio.h>

const int MAX_N = 14;
const int T_RANGE_SUM = 1;
const long long FACT[MAX_N] = {
  1, 1, 2, 6, 24, 120, 720, 5'040, 40'320, 362'880, 3'628'800, 39'916'800,
  479'001'600, 6'227'020'800,
};

// O permutare normală a mulțimii [0...n-1].
struct permutation {
  int v[MAX_N], psum[MAX_N];
  int n;

  void init(int n) {
    this->n = n;
  }

  int get_kth_free(int mask, int k) {
    int pos = 0;
    while (k || (mask & (1 << pos))) {
      k -= !(mask & (1 << pos));
      pos++;
    }
    return pos;
  }

  void unrank(long long r) {
    int occupancy = 0;
    for (int i = 0; i < n; i++) {
      int d = r / FACT[n - 1 - i];
      d = get_kth_free(occupancy, d);
      v[i] = d;
      psum[i] = v[i] + (i ? psum[i - 1] : 0);
      occupancy |= 1 << d;
      r %= FACT[n - 1 - i];
    }
  }
};

// O permutare care este sortată pe prefix și permutată doar pe sufix.
struct permutation_with_cherry_on_top {
  permutation p;
  int n, head, tail;
  long long rank;

  void init(int n) {
    this->n = n;
    rank = 0;
    tail = (n < MAX_N) ? n : MAX_N;
    head = n - tail;
    p.init(tail);
    p.unrank(0);
  }

  void advance(int x) {
    rank += x;
    p.unrank(rank);
  }

  long long prefix_sum(int k) {
    if (k <= head) {
      // Sumă pe interval în progresia sortată.
      return (long long)k * (k + 1) / 2;
    } else {
      // Toată partea sortată...
      return (long long)head * (head + 1) / 2
        // ...plus suma cozii permutate...
        + p.psum[k - head - 1]
        // ...plus faptul că elementele cozii încep de fapt la head+1
        + (long long)(k - head) * (head + 1);
    }
  }

  long long range_sum(int l, int r) {
    return prefix_sum(r) - prefix_sum(l - 1);
  }
};

permutation_with_cherry_on_top pc;

int main() {
  int n, q, type, l, r, x;
  scanf("%d %d", &n, &q);
  pc.init(n);

  while (q--) {
    scanf("%d", &type);
    if (type == T_RANGE_SUM) {
      scanf("%d %d", &l, &r);
      printf("%lld\n", pc.range_sum(l, r));
    } else {
      scanf("%d", &x);
      pc.advance(x);
    }
  }

  return 0;
}
