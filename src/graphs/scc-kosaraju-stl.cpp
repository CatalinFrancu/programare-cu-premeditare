#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;

struct node {
  std::vector<int> adj, adjt;
  bool mark;
};

node n[MAX_NODES + 1];
std::vector<int> order;
std::vector<std::vector<int>> comps;
int num_nodes;

void read_graph() {
  FILE* f = fopen("ctc.in", "r");
  int num_edges, u, v;
  fscanf(f, "%d %d", &num_nodes, &num_edges);
  for (int i = 1; i <= num_edges; i++) {
    fscanf(f, "%d %d", &u, &v);
    n[u].adj.push_back(v);
    n[v].adjt.push_back(u);
  }
  fclose(f);
}

void dfs(int u) {
  n[u].mark = true;
  for (auto v: n[u].adj) {
    if (!n[v].mark) {
      dfs(v);
    }
  }

  order.push_back(u);
}

void dfs_driver() {
  for (int u = 1; u <= num_nodes; u++) {
    if (!n[u].mark) {
      dfs(u);
    }
  }
}

void tdfs(int u) {
  n[u].mark = false;
  for (auto v: n[u].adjt) {
    if (n[v].mark) {
      tdfs(v);
    }
  }

  comps.back().push_back(u);
}

void transpose_dfs_driver() {
  for (int i = num_nodes - 1; i >= 0; i--) {
    int u = order[i];
    if (n[u].mark) {
      comps.push_back({});
      tdfs(u);
    }
  }
}

void write_components() {
  FILE* f = fopen("ctc.out", "w");
  fprintf(f, "%lu\n", comps.size());
  for (auto vec: comps)  {
    for (auto u: vec) {
      fprintf(f, "%d ", u);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

int main() {
  read_graph();
  dfs_driver();
  transpose_dfs_driver();
  write_components();

  return 0;
}
