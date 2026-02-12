#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 100'000;
const int GRAY = 1;
const int BLACK = 2;

struct edge {
  int v, next;
};

struct node {
  int adj;
  char color;
};

// O pădure de mulțimi disjuncte augmentată cu mărimea și nodurile fiecărei
// rădăcini.
struct disjoint_set_forest {
  int parent[MAX_NODES + 1];
  int size[MAX_NODES + 1];
  int next[MAX_NODES + 1];
  int last[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
    for (int u = 1; u <= n; u++) {
      parent[u] = u;
      size[u] = 1;
      next[u] = 0;
      last[u] = u;
    }
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
      parent[v] = u;
      size[u] += size[v];
      next[last[u]] = v;
      last[u] = last[v];
    }
  }
};

node nd[MAX_NODES + 1];
edge e[MAX_EDGES + 1];
disjoint_set_forest dsf;
int n;

void add_edge(int u, int v) {
  static int pos = 1;
  e[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_graph_and_wcc() {
  int num_edges, u, v;
  scanf("%d %d", &n, &num_edges);
  dsf.init(n);

  while (num_edges--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    dsf.merge(u, v);
  }
}

bool find_cycle(int u) {
  if (nd[u].color == GRAY) {
    return true;
  } else if (nd[u].color == BLACK) {
    return false;
  }

  nd[u].color = GRAY;
  int ptr = nd[u].adj;
  while (ptr && !find_cycle(e[ptr].v)) {
    ptr = e[ptr].next;
  }
  nd[u].color = BLACK;

  return ptr;
}

int satisfy_wcc(int u) {
  int size = dsf.size[u];
  while (u && !find_cycle(u)) {
    u = dsf.next[u];
  }
  return u ? size : (size - 1);
}

void satisfy_all_wccs() {
  int answer = 0;
  for (int u = 1; u <= n; u++) {
    if (dsf.parent[u] == u) {
      answer += satisfy_wcc(u);
    }
  }
  printf("%d\n", answer);
}

int main() {
  read_graph_and_wcc();
  satisfy_all_wccs();

  return 0;
}
