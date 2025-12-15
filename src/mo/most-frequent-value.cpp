// Complexitate: O((n + q) sqrt n).
#include <algorithm>
#include <stdio.h>

const int MAX_N = 100'000;
const int BLOCK_SIZE = 316;
const int MAX_QUERIES = 100'000;
const int MAX_VAL = 100'000;

struct query {
  int l, r;
  int orig_index;
};

int v[MAX_N];
int f[MAX_VAL + 1];
int num_having_f[MAX_N + 1], max_f;
query q[MAX_QUERIES];
int answer[MAX_QUERIES];
int n, num_queries;

void read_data() {
  scanf("%d %d", &n, &num_queries);
  for (int i = 0; i < n; i++) {
    scanf("%d", &v[i]);
  }
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d", &q[i].l, &q[i].r);
    q[i].orig_index = i;
  }
}

void sort_queries_by_left_block() {
  std::sort(q, q + num_queries, [](query a, query b) {
    int x = a.l / BLOCK_SIZE, y = b.l / BLOCK_SIZE;
    if (x != y) {
      return (x < y);
    } else if (x % 2) {
      return a.r > b.r;
    } else {
      return a.r < b.r;
    }
  });
}

void add(int pos) {
  int x = ++f[v[pos]];
  num_having_f[x - 1]--;
  num_having_f[x]++;
  if (x > max_f) {
    max_f = f[v[pos]];
  }
}

void remove(int pos) {
  int x = --f[v[pos]];
  num_having_f[x + 1]--;
  num_having_f[x]++;
  if (num_having_f[max_f] == 0) {
    max_f--;
  }
}

void mo() {
  num_having_f[0] = n;
  int l = 0, r = -1; // vid
  for (int i = 0; i < num_queries; i++) {
    while (l > q[i].l) {
      add(--l);
    }
    while (r < q[i].r) {
      add(++r);
    }
    while (l < q[i].l) {
      remove(l++);
    }
    while (r > q[i].r) {
      remove(r--);
    }
    answer[q[i].orig_index] = max_f;
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%d\n", answer[i]);
  }
}

int main() {
  read_data();
  sort_queries_by_left_block();
  mo();

  write_answers();

  return 0;
}
