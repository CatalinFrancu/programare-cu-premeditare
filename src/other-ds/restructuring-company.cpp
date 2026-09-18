// Complexitate: O((n + q) log* n).
#include <stdio.h>

const int MAX_N = 200'000;
const int T_MERGE = 1;
const int T_MERGE_RANGE = 2;
const int T_QUERY = 3;

struct disjoint_set_forest {
  int p[MAX_N + 1];

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
    }
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  // Unește v de u. Fără union by rank.
  void unite(int u, int v) {
    p[find(v)] = find(u);
  }

  bool are_united(int u, int v) {
    return find(u) == find(v);
  }
};

disjoint_set_forest dsf;
int end[MAX_N + 1];

void merge_range(int x, int y) {
  while (x < y) {
    dsf.unite(x, y);
    int old_end = end[x];
    end[x] = y;
    x = old_end + 1;
  }
}

int main() {
  int n, q;
  scanf("%d %d", &n, &q);

  dsf.init(n);
  for (int i = 1; i <= n; i++) {
    end[i] = i;
  }

  while (q--) {
    int type, x, y;
    scanf("%d %d %d", &type, &x, &y);
    if (type == T_MERGE) {
      dsf.unite(x, y);
    } else if (type == T_MERGE_RANGE) {
      merge_range(x, y);
    } else { // T_QUERY
      printf(dsf.are_united(x,  y) ? "YES\n" : "NO\n");
    }
  }

  return 0;
}
