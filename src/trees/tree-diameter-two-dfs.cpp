#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct rec {
  int node, dist;

  void improve(rec child) {
    if (child.dist + 1 > dist) {
      dist = child.dist + 1;
      node = child.node;
    }
  }
};

cell list[2 * MAX_NODES];
int adj[MAX_NODES + 1];

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, adj[u] };
  adj[u] = pos++;
}

void read_data() {
  int n, u, v;

  scanf("%d",  &n);
  for (int i = 1; i < n; i++) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

rec dfs(int u, int parent) {
  rec result = { u, 0 }; // nodul însuși
  for (int ptr = adj[u]; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      result.improve(dfs(v, u));
    }
  }
  return result;
}

int main() {
  read_data();
  rec farthest = dfs(1, 0);
  rec farthest2 = dfs(farthest.node, 0);
  int diam = farthest2.dist;
  printf("%d\n", diam);

  return 0;
}
