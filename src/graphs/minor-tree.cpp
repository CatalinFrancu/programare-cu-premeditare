#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 200'000;
const int MAX_BIT = 29;

struct edge {
  int u, v, weight;
};

struct disjoint_set_forest {
  int parent[MAX_NODES + 1];
  int n, num_trees;

  void reset(int n) {
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

  void merge(int u, int v) {
    u = find(u);
    v = find(v);
    if (u != v) {
      parent[u] = v;
      num_trees--;
    }
  }

  bool all_connected() {
    return num_trees == 1;
  }

};

edge e[MAX_EDGES];
disjoint_set_forest ds;
int num_nodes, num_edges, ority;

void read_graph() {
  ority = 0;
  scanf("%d %d", &num_nodes, &num_edges);
  for (int i = 0; i < num_edges; i++) {
    scanf("%d %d %d", &e[i].u, &e[i].v, &e[i].weight);
    ority |= e[i].weight;
  }
}

bool is_connected(int mask) {
  ds.reset(num_nodes);
  int i = 0;
  while ((i < num_edges) && !ds.all_connected()) {
    if ((e[i].weight & mask) == e[i].weight) {
      ds.merge(e[i].u, e[i].v);
    }
    i++;
  }
  return ds.all_connected();
}

void find_ority()  {
  for (int b = MAX_BIT; b >= 0; b--) {
    if ((ority & (1 << b)) &&
        is_connected(ority ^ (1 << b))) {
      ority ^= 1 << b;
    }
  }
}

void write_ority() {
  printf("%d\n", ority);
}

void process_test() {
  read_graph();
  find_ority();
  write_ority();
}

int main() {
  int num_tests;

  scanf("%d", &num_tests);
  while (num_tests--) {
    process_test();
  }

  return 0;
}
