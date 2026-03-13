// Implementare recursivă.
#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 500'000;

struct edge {
  int v;
  bool active;
  int next;
};

struct node {
  int adj;
  bool even_degree;
};

node nd[MAX_NODES + 1];
edge e[2 * MAX_EDGES + 1];
int cycle[MAX_EDGES + 1];
int n, m, cycle_len, num_odd;

void add_edge(int u, int v) {
  static int ptr = 1;
  e[ptr] = { v, true, nd[u].adj };
  nd[u].adj = ptr++;
  nd[u].even_degree = !nd[u].even_degree;
  num_odd += nd[u].even_degree ? -1 : +1;
}

void read_graph() {
  FILE* f = fopen("ciclueuler.in", "r");
  fscanf(f, "%d %d", &n, &m);
  for (int i = 1; i <= m; i++) {
    int u, v;
    fscanf(f, "%d %d", &u, &v);
    // Muchia u-v va ocupa două poziții consecutive în e, 2k-1 și 2k.
    add_edge(u, v);
    add_edge(v, u);
  }
  fclose(f);
}

void delete_edge(int ptr) {
  int rev = ((ptr + 1) ^ 1) - 1;
  e[ptr].active = e[rev].active = false;
}

void dfs(int u) {
  // Nu merge implementat cu for. Să considerăm un graf cu n=2 și 500.000 de
  // muchii între nodurile 1 și 2. Atunci la fiecare nivel de recursivitate
  // fiecare nod va parcurge o listă de lungime O(m).
  while (nd[u].adj) {
    int ptr = nd[u].adj;
    nd[u].adj = e[ptr].next;
    if (e[ptr].active) {
      delete_edge(ptr);
      dfs(e[ptr].v);
    }
  }
  cycle[cycle_len++] = u;
}

void write_solution() {
  FILE* f = fopen("ciclueuler.out", "w");
  if (num_odd) {
    fprintf(f, "-1\n");
  } else {
    for (int i = 0; i < m; i++) {
      fprintf(f, "%d ", cycle[i]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

int main() {
  read_graph();
  if (!num_odd) {
    dfs(1);
  }
  write_solution();

  return 0;
}
