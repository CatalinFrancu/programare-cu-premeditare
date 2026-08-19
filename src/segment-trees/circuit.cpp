#include <algorithm>
#include <stdio.h>

const int MAX_N = 300'000;
const int MAX_SEGTREE_NODES = 1 << 20;
const int MOD = 1'000'000'007;
const int HALF = (MOD + 1) / 2; // Pentru că (HALF * 2) % MOD = 1.

struct elem {
  int val;
  int pos;
};

elem v[MAX_N];
int pow2[MAX_N + 1], inv_pow2[MAX_N + 1];
int n;

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

struct segtree_node {
  int sum;    // suma din intervalul subîntins
  int lazy;   // numărul de dublări de aplicat
  // Invariant: pentru nodul curent, sum include lazy.
};

// Arbore de segmente cu dublare pe prefix și sumă pe prefix.
struct segment_tree {
  segtree_node v[MAX_SEGTREE_NODES];
  int n, bits;

  void init(int size) {
    n = next_power_of_2(size);
    bits = __builtin_popcount(n - 1);

    for (int pos = n; pos < n + size; pos++) {
      v[pos].sum = 1;
      v[pos].lazy = 0;
    }

    for (int pos = n - 1; pos; pos--) {
      v[pos].sum = v[2 * pos].sum + v[2 * pos + 1].sum;
      v[pos].lazy = 0;
    }
  }

  void push(int pos) {
    int l = v[pos].lazy;
    if (l) {
      v[2 * pos].sum = (long long)v[2 * pos].sum * pow2[l] % MOD;
      v[2 * pos].lazy += l;
      v[2 * pos + 1].sum = (long long)v[2 * pos + 1].sum * pow2[l] % MOD;
      v[2 * pos + 1].lazy += l;
      v[pos].lazy = 0;
    }
  }

  void push_path(int pos) {
    for (int b = bits; b; b--) {
      push(pos >> b);
    }
  }

  void pull_path(int pos) {
    for (pos /= 2; pos; pos /= 2) {
      if (!v[pos].lazy) {
        v[pos].sum = (v[2 * pos].sum + v[2 * pos + 1].sum) % MOD;
      }
    }
  }

  void double_prefix(int r) {
    r += n;
    int orig_r = r;

    push_path(r);

    while (r)  {
      if (!(r & 1)) {
        v[r].sum = 2 * v[r].sum % MOD;
        v[r--].lazy++;
      }
      r >>= 1;
    }

    pull_path(orig_r);
  }

  int prefix_sum(int r) {
    long long result = 0;

    r += n;
    push_path(r);

    int quot = v[r].lazy;

    while (r)  {
      if (!(r & 1)) {
        result += v[r--].sum;
      }
      r >>= 1;
    }

    return result % MOD * inv_pow2[quot] % MOD;
  }
};

segment_tree pref_st, suf_st;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &v[i].val);
    v[i].pos = i;
  }
}

void sort_by_value() {
  std::sort(v, v + n, [](elem a, elem b) {
    return (a.val < b.val);
  });
}

void compute_powers() {
  pow2[0] = inv_pow2[0] = 1;
  for (int i = 1; i <= n; i++) {
    pow2[i] = 2 * pow2[i - 1] % MOD;
    inv_pow2[i] = (long long)HALF * inv_pow2[i - 1] % MOD;
  }
}

int count_contributions() {
  __int128 sum = 0;

  pref_st.init(n);
  suf_st.init(n);

  for (int i = 0; i < n; i++) {
    int p = v[i].pos;
    int pref_sum = pref_st.prefix_sum(p);
    int suf_sum = suf_st.prefix_sum(n - 1 - p);
    sum += (__int128)pref_sum * suf_sum * v[i].val;
    pref_st.double_prefix(p);
    suf_st.double_prefix(n - 1 - p);
  }
  return sum % MOD;
}

int main() {
  read_data();
  sort_by_value();
  compute_powers();
  int answer = count_contributions();
  printf("%d\n", answer);

  return 0;
}
