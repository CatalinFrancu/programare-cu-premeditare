#include <stdio.h>

const int MAX_N = 200'000;
const int NUM_BITS = 60;

struct pair {
  long long val;
  int u;
};

struct trie {
  int child[MAX_N * NUM_BITS + 1][2];
  int cnt[MAX_N * NUM_BITS + 1];
  int size;

  pair p[MAX_N];
  int num_pairs;

  void reset_node(int u) {
    child[u][0] = child[u][1] = cnt[u] = 0;
  }

  void init() {
    size = 1;
    reset_node(1);
    num_pairs = 0;
  }

  void add_val_to_root(long long x) {
    p[num_pairs++] = { x, 1 };
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

  long long count_zeroes(int bit) {
    long long sum = 0;
    for (int i = 0; i < num_pairs; i++) {
      int u = p[i].u;
      int val = (p[i].val >> bit) & 1;
      sum += cnt[child[u][val]];
    }
    return sum;
  }

  void push_once(int bit, int target_val) {
    int j = 0;
    for (int i = 0; i < num_pairs; i++) {
      int u = p[i].u;
      int val = (p[i].val >> bit) & 1;
      int c = child[u][val ^ target_val];
      if (c) {
        p[j++] = { p[i].val, c };
      }
    }
    num_pairs = j;
  }

  long long push_to_bottom(long long k) {
    long long answer = 0;

    for (int bit = NUM_BITS - 1; bit >= 0; bit--) {
      long long z = count_zeroes(bit);
      if (z < k) {
        k -= z;
        push_once(bit, 1);
        answer += 1ll << bit;
      } else {
        push_once(bit, 0);
      }
    }

    return answer;
  }
};

trie t;
int n;
long long k;

void read_data_and_build_trie() {
  long long x;

  t.init();
  scanf("%d %lld", &n, &k);
  for (int i = 0; i < n; i++) {
    scanf("%lld", &x);
    t.insert(x);
  }
  for (int i = 0; i < n; i++) {
    scanf("%lld", &x);
    t.add_val_to_root(x);
  }
}

void solve_test_case() {
  read_data_and_build_trie();
  long long ans = t.push_to_bottom(k);
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
