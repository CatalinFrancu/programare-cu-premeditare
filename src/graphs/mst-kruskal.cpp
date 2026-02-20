#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 400'000;

struct disjoint_set_forest {
  int p[MAX_NODES + 1];

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

  void unite(int u, int v) {
    p[find(v)] = find(u);
  }

  bool are_connected(int u, int v) {
    return find(u) == find(v);
  }
};

struct edge {
  int u, v;
  short cost;
  bool chosen;
};

edge e[MAX_EDGES];
disjoint_set_forest dsf;
int n, m, mst_cost;

void read_data() {
  FILE* f = fopen("apm.in", "r");

  fscanf(f, "%d %d", &n, &m);
  for (int i = 0; i < m; i++) {
    fscanf(f, "%d %d %hd", &e[i].u, &e[i].v, &e[i].cost);
  }

  fclose(f);
}

void kruskal() {
  std::sort(e, e + m, [](edge& a, edge& b) {
    return a.cost < b.cost;
  });
  dsf.init(n);

  for (int i = 0; i < m; i++) {
    if (!dsf.are_connected(e[i].u, e[i].v)) {
      e[i].chosen = true;
      mst_cost += e[i].cost;
      dsf.unite(e[i].u, e[i].v);
    }
  }
}

void write_mst() {
  FILE* f = fopen("apm.out", "w");

  fprintf(f, "%d\n", mst_cost);
  fprintf(f, "%d\n", n - 1);
  for (int i = 0; i < m; i++) {
    if (e[i].chosen) {
      fprintf(f, "%d %d\n", e[i].u, e[i].v);
    }
  }

  fclose(f);
}

int main() {
  read_data();
  kruskal();
  write_mst();

  return 0;
}
