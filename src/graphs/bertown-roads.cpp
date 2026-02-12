#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 300'000;

struct edge {
  int u, v;
};

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth, low;
};

node nd[MAX_NODES + 1];
cell list[2 * MAX_EDGES + 1];
edge e[MAX_EDGES];
int n, sol_edges;
bool has_bridges;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_graph() {
  int num_edges, u, v;
  scanf("%d %d", &n, &num_edges);

  while (num_edges--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void dfs(int u, int parent) {
  nd[u].depth = nd[u].low = nd[parent].depth + 1;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;

    if (!nd[v].depth) {
      // muchie de arbore
      e[sol_edges++] = { u, v };
      dfs(v, u);
      nd[u].low = min(nd[u].low, nd[v].low);
      has_bridges |= (nd[v].low > nd[u].depth);

    } else if ((v != parent) && (nd[v].depth < nd[u].depth)) {
      // muchie înapoi
      e[sol_edges++] = { u, v };
      nd[u].low = min(nd[u].low, nd[v].depth);

    }
  }
}

void write_answer() {
  if (has_bridges) {
    printf("0\n");
  } else {
    for (int i = 0; i < sol_edges; i++) {
      printf("%d %d\n", e[i].u, e[i].v);
    }
  }
}

int main() {
  read_graph();
  dfs(1, 0);
  write_answer();

  return 0;
}
