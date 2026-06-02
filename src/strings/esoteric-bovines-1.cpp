#include <stdio.h>

const int MAX_N = 200'000;
const int NUM_BITS = 60;

struct trie {
  int child[MAX_N * NUM_BITS + 1][2];
  int cnt[MAX_N * NUM_BITS + 1];
  int size;

  void reset_node(int u) {
    child[u][0] = child[u][1] = cnt[u] = 0;
  }

  void init() {
    size = 1;
    reset_node(1);
  }

  void insert(long long x) {
    int u = 1;
    for (int bit = NUM_BITS - 1; bit >= 0; bit--) {
      int val = (x >> bit) & 1;
      if (!child[u][val]) {
        child[u][val] = ++size;
        reset_node(size);
      }
      u = child[u][val];
      cnt[u]++;
    }
  }

  // Numără valorile a_i care respectă a_i ^ b < x.
  int count_pairs(long long b, long long x) {
    int sum = 0;
    int u = 1, bit = NUM_BITS - 1;
    while (u && (bit >= 0)) {
      int bval = (b >> bit) & 1;
      int xval = (x >> bit) & 1;
      if (xval) {
        sum += cnt[child[u][bval]];
        u = child[u][!bval];
      } else {
        u = child[u][bval];
      }
      bit--;
    }
    return sum;
  }
};

trie t;
long long b[MAX_N];
int n;
long long k;

void read_data_and_build_trie() {
  t.init();
  scanf("%d %lld", &n, &k);
  for (int i = 0; i < n; i++) {
    long long x;
    scanf("%lld", &x);
    t.insert(x);
  }
  for (int i = 0; i < n; i++) {
    scanf("%lld", &b[i]);
  }
}

// Numără perechile (a_i, b_j) care respectă a_i ^ b_j < x.
long long count_pairs(long long x) {
  long long sum = 0;
  for (int i = 0; i < n; i++) {
    sum += t.count_pairs(b[i], x);
  }
  return sum;
}

long long bin_search() {
  long long lo = 0, hi = 1ll << NUM_BITS;
  while (hi - lo > 1) {
    long long mid = (lo + hi) / 2;
    if (count_pairs(mid) >= k) {
      hi = mid;
    } else {
      lo = mid;
    }
  }
  return lo;
}

void solve_test_case() {
  read_data_and_build_trie();
  long long ans = bin_search();
  printf("%lld\n", ans);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);

  while (num_tests--) {
    solve_test_case();
  }

  return 0;
}
