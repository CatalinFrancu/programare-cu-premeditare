#include <algorithm>
#include <stdio.h>
#include <vector>

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
};

struct node {
  std::vector<int> adj; // doar muchiile din APM
  std::vector<int> ppl; // ppl[0] este șoferul, care își conduce propriul autobuz
};

edge e[MAX_EDGES];
node nd[MAX_NODES + 1];
disjoint_set_forest dsf;
int n, m, mst_cost;
FILE *fin, *fout;

void read_data() {
  fscanf(fin, "%d %d", &n, &m);
  for (int i = 0; i < m; i++) {
    fscanf(fin, "%d %d %hd", &e[i].u, &e[i].v, &e[i].cost);
  }
}

void kruskal() {
  std::sort(e, e + m, [](edge& a, edge& b) {
    return a.cost < b.cost;
  });
  dsf.init(n);

  mst_cost = 0;
  for (int i = 0; i < m; i++) {
    if (!dsf.are_connected(e[i].u, e[i].v)) {
      mst_cost += e[i].cost;
      dsf.unite(e[i].u, e[i].v);
      nd[e[i].u].adj.push_back(e[i].v);
      nd[e[i].v].adj.push_back(e[i].u);
    }
  }
}

void merge(int u, int v) {
  // small to large
  if (nd[v].ppl.size() > nd[u].ppl.size()) {
    nd[u].ppl.swap(nd[v].ppl);
  }
  for (int p: nd[v].ppl) {
    nd[u].ppl.push_back(p);
    fprintf(fout, "Move %d %d %d\n", p, nd[v].ppl[0], nd[u].ppl[0]);
  }
  nd[v].ppl.clear();
}

void dfs(int u, int parent) {
  nd[u].ppl.push_back(u);

  for (int v: nd[u].adj) {
    if (v != parent) {
      dfs(v, u);
      fprintf(fout, "Drive %d %d %d\n", nd[v].ppl[0], v, u);
      merge(u, v);
    }
  }
}

void gen_ops() {
  fprintf(fout, "%d\n", mst_cost);
  dfs(1, 0);
  fprintf(fout, "Gata\n");
}

void clear() {
  for (int u = 1; u <= n; u++) {
    nd[u].adj.clear();
  }
  nd[1].ppl.clear();
}

int main() {
  fin = fopen("autobuze3.in", "r");
  fout = fopen("autobuze3.out", "w");

  int num_tests;
  fscanf(fin, "%d", &num_tests);
  while (num_tests--) {
    read_data();
    kruskal();
    gen_ops();
    clear();
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
