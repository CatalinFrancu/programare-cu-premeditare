#include <stdio.h>

const int MAX_NODES = 300'000;
const int MAX_EDGES = 300'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth, low;
};

node nd[MAX_NODES + 1];
cell list[2 * MAX_EDGES + 1];
int n, diam;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  int num_edges, u, v;
  scanf("%d %d", &n, &num_edges);

  while (num_edges--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

void minimize(int& x, int y) {
  x = (y < x) ? y : x;
}

void maximize(int& x, int y) {
  x = (y > x) ? y : x;
}

// Returnează distanța maximă în punți de la u la orice frunză.
int dfs(int u, int parent) {
  nd[u].depth = nd[u].low = nd[parent].depth + 1;
  int dist = 0; // distanța maximă văzută în fiii anteriori

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;

    if (!nd[v].depth) {
      int dist2 = dfs(v, u); // distanța maximă văzută în v
      if (nd[v].low > nd[u].depth) {
        dist2++; // u-v este punte
      }
      minimize(nd[u].low, nd[v].low);
      maximize(diam, dist2 + dist);
      maximize(dist, dist2);
    } else if (v != parent) {
      minimize(nd[u].low, nd[v].depth);
    }
  }

  return dist;
}

void write_answer() {
  printf("%d\n", diam);
}

int main() {
  read_data();
  dfs(1, 0);
  write_answer();

  return 0;
}
