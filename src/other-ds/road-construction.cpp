#include <stdio.h>

const int MAX_N = 100'000;

int max(int x, int y) {
  return (x > y) ? x : y;
}

struct disjoint_set_forest {
  int p[MAX_N + 1];
  int size[MAX_N + 1];
  int cnt, max_size;

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
      size[u] = 1;
    }
    cnt = n;
    max_size = 1;
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  void unite(int u, int v) {
    u = find(u);
    v = find(v);
    if (u != v) {
      p[v] = u;
      size[u] += size[v];
      cnt--;
      max_size = max(max_size, size[u]);
    }
  }
};

disjoint_set_forest dsf;

int main() {
  int n, m, a, b;
  scanf("%d %d", &n, &m);
  dsf.init(n);

  while (m--) {
    scanf("%d %d", &a, &b);
    dsf.unite(a, b);
    printf("%d %d\n", dsf.cnt, dsf.max_size);
  }

  return 0;
}
