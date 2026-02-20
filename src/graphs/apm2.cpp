#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 10'000;
const int MAX_EDGES = 100'000;

struct edge {
  int u, v, c;
};

int max(int x, int y) {
  return (x > y) ? x : y;
}

struct disjoint_set_forest {
  int parent[MAX_NODES + 1];
  int cost[MAX_NODES + 1]; // costul părăsirii subarborelui
  int size[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
    for (int u = 1; u <= n; u++) {
      parent[u] = u;
      size[u] = 1;
    }
  }

  int find(int u) {
    while (parent[u] != u) {
      u = parent[u];
    }
    return u;
  }

  void merge(int u, int v, int c) {
    u = find(u);
    v = find(v);
    if (u != v) {
      if (size[u] > size[v]) {
        int tmp = u; u = v; v = tmp;
      }
      parent[u] = v;
      cost[u] = c;
      size[v] += size[u];
    }
  }

  int get_max_cost(int u, int v) {
    int result = 0;

    while (u != v) {
      if (size[u] < size[v]) {
        result = max(result, cost[u]);
        u = parent[u];
      } else {
        result = max(result, cost[v]);
        v = parent[v];
      }
    }

    return result;
  }
};

edge e[MAX_EDGES];
disjoint_set_forest dsf;
int num_nodes, num_edges, num_queries;
FILE* fin;

void read_graph() {
  fin = fopen("apm2.in", "r");
  fscanf(fin, "%d %d %d", &num_nodes, &num_edges, &num_queries);
  for (int i = 0; i < num_edges; i++) {
    fscanf(fin, "%d %d %d", &e[i].u, &e[i].v, &e[i].c);
  }
}

void sort_edges_by_cost() {
  std::sort(e, e + num_edges, [] (edge& a, edge& b) {
    return a.c < b.c;
  });
}

void kruskal() {
  sort_edges_by_cost();
  dsf.init(num_nodes);
  for (int i = 0; i < num_edges; i++) {
    dsf.merge(e[i].u, e[i].v, e[i].c);
  }
}

void answer_queries() {
  FILE* fout = fopen("apm2.out", "w");

  while (num_queries--) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    int max_cost = dsf.get_max_cost(u, v);
    fprintf(fout, "%d\n", max_cost - 1);
  }

  fclose(fin);
  fclose(fout);
}

int main() {
  read_graph();
  kruskal();
  answer_queries();

  return 0;
}
