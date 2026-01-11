#include <stdio.h>

const int MAX_NODES = 500'000;
const int MAX_LOG = 19;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth;
  int jump[MAX_LOG];
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int n, log_2;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &n);
  log_2 = 31 - __builtin_clz(n);

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

// Traversează arborele și calculează adîncimile și jump pointers.
void dfs(int u, int parent) {
  nd[u].depth = 1 + nd[parent].depth;
  nd[u].jump[0] = parent;
  for (int i = 0; i < log_2; i++) {
    nd[u].jump[i + 1] = nd[nd[u].jump[i]].jump[i];
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      dfs(v, u);
    }
  }
}

int lca(int u, int v) {
  if (nd[v].depth > nd[u].depth) {
    int tmp = u; u = v; v = tmp;
  }

  // Adu u și v la aceeași adîncime. Compară d[u] și d[v] bit cu bit.
  int pow = 0;
  while (nd[u].depth > nd[v].depth) {
    if ((nd[u].depth & (1 << pow)) != (nd[v].depth & (1 << pow))) {
      u = nd[u].jump[pow];
    }
    pow++;
  }

  if (u == v) {
    return u;
  } else {
    for (int i = log_2; i >= 0; i--) {
      if (nd[u].jump[i] != nd[v].jump[i]) {
        u = nd[u].jump[i];
        v = nd[v].jump[i];
      }
    }

    return nd[u].jump[0];
  }
}

void answer_queries() {
  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", lca(u, v));
  }
}

int main() {
  read_input_data();
  dfs(1, 0);
  answer_queries();

  return 0;
}
