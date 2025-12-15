#include <algorithm>
#include <stdio.h>

const int MAX_N = 200'000;
const int BLOCK_SIZE = 300;
const int MAX_QUERIES = 200'000;
const int MAX_VAL = 1'000'000;

typedef unsigned long long u64;

struct query {
  int l, r;
  int orig_index;
};

int v[MAX_N];
int f[MAX_VAL + 1];
u64 power;
query q[MAX_QUERIES];
u64 answer[MAX_QUERIES]; // separat de q[] ca să evităm o sortare la final
int n, num_queries;

void read_data() {
  scanf("%d %d", &n, &num_queries);
  for (int i = 0; i < n; i++) {
    scanf("%d", &v[i]);
  }
  for (int i = 0; i < num_queries; i++) {
    int l, r;
    scanf("%d %d", &l, &r);
    q[i].l = l - 1;
    q[i].r = r - 1;
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
  f[v[pos]]++;
  power += (u64)v[pos] * (2 * f[v[pos]] - 1);
}

void remove(int pos) {
  f[v[pos]]--;
  power -= (u64)v[pos] * (2 * f[v[pos]] + 1);
}

void mo() {
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
    answer[q[i].orig_index] = power;
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%llu\n", answer[i]);
  }
}

int main() {
  read_data();
  sort_queries_by_left_block();
  mo();

  write_answers();

  return 0;
}
