#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 400'000;

struct edge {
  int u, v, c;
};

struct disjoint_set_forest {
  int parent[MAX_NODES + 1];
  int n, num_trees;

  void init(int n) {
    this->n = n;
    for (int u = 1; u <= n; u++) {
      parent[u] = u;
    }
    num_trees = n;
  }

  int find(int u) {
    return (parent[u] == u)
      ? u
      : (parent[u] = find(parent[u]));
  }

  // Returnează true dacă chiar a unificat u cu v.
  bool merge(int u, int v) {
    u = find(u);
    v = find(v);
    if (u != v) {
      parent[u] = v;
      num_trees--;
      return true;
    } else {
      return false;
    }
  }

  int get_num_trees() {
    return num_trees;
  }
};

disjoint_set_forest dsf;
edge e[MAX_EDGES];
int n, m, build_cost, total_cost;

void read_and_filter_edges() {
  int real_m;
  FILE* f = fopen("rusuoaica.in", "r");

  fscanf(f, "%d %d %d", &n, &real_m, &build_cost);

  while (real_m--) {
    fscanf(f, "%d %d %d", &e[m].u, &e[m].v, &e[m].c);
    if (e[m].c <= build_cost) {
      m++;
    } else {
      total_cost -= e[m].c;
    }
  }

  fclose(f);
}

void sort_edges() {
  std::sort(e, e + m, [] (edge& a, edge& b) {
    return a.c < b.c;
  });
}

void kruskal() {
  dsf.init(n);
  sort_edges();

  for (int i = 0; i < m; i++) {
    if (dsf.merge(e[i].u, e[i].v)) {
      total_cost += e[i].c;
    } else {
      total_cost -= e[i].c;
    }
  }

  total_cost += build_cost * (dsf.get_num_trees() - 1);
}

void write_answer() {
  FILE* f = fopen("rusuoaica.out", "w");
  fprintf(f, "%d\n", total_cost);
  fclose(f);
}

int main() {
  read_and_filter_edges();
  kruskal();
  write_answer();

  return 0;
}
