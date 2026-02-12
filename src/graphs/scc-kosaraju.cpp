#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 200'000;

struct edge {
  int v, next;
};

struct node {
  int adj;
  bool mark;
};

node n[MAX_NODES + 1];
edge e[MAX_EDGES + 1];
int order[MAX_NODES], order_size;
int comp[MAX_NODES + 1], comp_size;
int comp_start[MAX_NODES + 1], num_comps;
int num_nodes;

void read_graph() {
  FILE* f = fopen("ctc.in", "r");
  int num_edges, u, v;
  fscanf(f, "%d %d", &num_nodes, &num_edges);
  for (int i = 1; i <= num_edges; i++) {
    fscanf(f, "%d %d", &u, &v);
    e[i] = { v, n[u].adj };
    n[u].adj = i;
  }
  fclose(f);
}

void dfs(int u) {
  n[u].mark = true;
  for (int ptr = n[u].adj; ptr; ptr = e[ptr].next) {
    int v = e[ptr].v;
    if (!n[v].mark) {
      dfs(v);
    }
  }

  order[order_size++] = u;
}

void dfs_driver() {
  for (int u = 1; u <= num_nodes; u++) {
    if (!n[u].mark) {
      dfs(u);
    }
  }
}

// Transpune graful, reutilizînd vectorul de muchii.
void transpose() {
  int *old_adj = comp; // syntactic sugar
  for (int u = 1; u <= num_nodes; u++) {
    old_adj[u] = n[u].adj;
    n[u].adj = 0;
  }
  for (int u = 1; u <= num_nodes; u++) {
    int ptr = old_adj[u];
    while (ptr) {
      // mută e[ptr] din lista de adiacență a lui u în a lui v.
      int v = e[ptr].v;
      int next = e[ptr].next;
      e[ptr] = { u, n[v].adj };
      n[v].adj = ptr;
      ptr = next;
    }
  }
}

void tdfs(int u) {
  n[u].mark = false;
  for (int ptr = n[u].adj; ptr; ptr = e[ptr].next) {
    int v = e[ptr].v;
    if (n[v].mark) {
      tdfs(v);
    }
  }

  comp[comp_size++] = u;
}

void transpose_dfs_driver() {
  for (int i = num_nodes - 1; i >= 0; i--) {
    int u = order[i];
    if (n[u].mark) {
      comp_start[num_comps++] = comp_size;
      tdfs(u);
    }
  }
  comp_start[num_comps] = num_nodes;
}

void write_components() {
  FILE* f = fopen("ctc.out", "w");
  fprintf(f, "%d\n", num_comps);
  for (int i = 0; i < num_comps; i++) {
    for (int j = comp_start[i]; j < comp_start[i + 1]; j++) {
      fprintf(f, "%d ", comp[j]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

int main() {
  read_graph();
  dfs_driver();
  transpose();
  transpose_dfs_driver();
  write_components();

  return 0;
}
