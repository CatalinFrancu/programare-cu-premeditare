#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

cell list[2 * MAX_NODES];
int adj[MAX_NODES + 1];
int diam;

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

void maximize(int& x, int y) {
  x = (x > y) ? x : y;
}

// Returnează lungimea în muchii a căii celei mai lungi de la acest nod la
// orice frunză din subarbore.
int dfs(int u, int parent) {
  int dist = 0; // self
  for (int ptr = adj[u]; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      int l = 1 + dfs(v, u);
      maximize(diam, dist + l); // combină cu fiii anteriori
      maximize(dist, l);        // actualizează calea cea mai lungă
    }
  }
  return dist;
}

int main() {
  read_data();
  dfs(1, 0);
  printf("%d\n", diam);

  return 0;
}
