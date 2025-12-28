#include <stdio.h>

const int MAX_NODES = 200'000;
const int NIL = -1;

struct list {
  int v, next;
};

struct node {
  int parent;
  int ptr; // pointer în lista de adiacență
  int time_in, time_out;
};

list adj[2 * MAX_NODES];
node n[MAX_NODES + 1];
int num_nodes, num_queries;

void add_neighbor(int u, int v, int pos) {
  adj[pos] = { v, n[u].ptr };
  n[u].ptr = pos;
}

void read_tree() {
  scanf("%d %d", &num_nodes, &num_queries);
  for (int u = 1; u <= num_nodes; u++) {
    n[u].ptr = NIL;
  }

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v, 2 * i);
    add_neighbor(v, u, 2 * i + 1);
  }
}

void euler_tour(int u, int parent) {
  static int time = 0;

  n[u].parent = parent;
  n[u].time_in = ++time;
  for (int ptr = n[u].ptr; ptr != NIL; ptr = adj[ptr].next) {
    int v = adj[ptr].v;
    if (v != parent) {
      euler_tour(v, u);
    }
  }
  n[u].time_out = ++time;
}

bool is_ancestor(int u, int v) {
  return
    (n[u].time_in <= n[v].time_in) &&
    (n[u].time_out >= n[v].time_out);
}

void process_queries() {
  while (num_queries--) {
    int size, u, lowest;
    bool has_path = true;
    scanf("%d %d", &size, &lowest);
    lowest = n[lowest].parent;

    while (--size) {
      scanf("%d", &u);
      u = n[u].parent;
      if (is_ancestor(lowest, u)) {
        lowest = u;
      } else if (!is_ancestor(u, lowest)) {
        has_path = false;
      }
    }

    printf(has_path ? "YES\n" : "NO\n");
  }
}

int main() {
  read_tree();
  euler_tour(1, 1);
  process_queries();

  return 0;
}
