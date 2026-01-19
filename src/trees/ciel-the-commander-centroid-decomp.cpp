#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int size;
  bool dead;
  char rank; // răspunsul
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int n;

void add_child(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &n);

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_child(u, v);
    add_child(v, u);
  }
}

void size_dfs(int u, int parent) {
  nd[u].size = 1;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((v != parent) && !nd[v].dead) {
      size_dfs(v, u);
      nd[u].size += nd[v].size;
    }
  }
}

int find_centroid(int u, int limit) {
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((nd[v].size < nd[u].size) && !nd[v].dead && (nd[v].size > limit)) {
      return find_centroid(v, limit);
    }
  }

  return u;
}

void decompose(int u, int rank) {
  size_dfs(u, 0);
  u = find_centroid(u, nd[u].size / 2);

  // Aceasta este problema pe care o rezolvăm. Restul codului este șablonul
  // pentru descompunerea în centroizi.
  nd[u].rank = rank;

  nd[u].dead = true;
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].dead) {
      decompose(v, rank + 1);
    }
  }
}

void write_ranks() {
  for (int u = 1; u <= n; u++) {
    putchar(nd[u].rank);
    putchar((u == n) ? '\n' : ' ');
  }
}

int main() {
  read_input_data();
  decompose(1, 'A');
  write_ranks();

  return 0;
}
