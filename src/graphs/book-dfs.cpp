#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 200'000;

const int WHITE = 0;
const int GRAY = 1;
const int BLACK = 2;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int pass;
  char color;
};

cell list[MAX_EDGES + 1];
node nd[MAX_NODES + 1];
int n, m, max_pass;
bool has_cycle;

void clear_graph() {
  m = 0;
  max_pass = 1;
  has_cycle = false;
  for (int u = 1; u <= n; u++) {
    nd[u].adj = 0;
    nd[u].pass = 1;
    nd[u].color = WHITE;
  }
}

void add_edge(int u, int v) {
  list[++m] = { v, nd[u].adj };
  nd[u].adj = m;
}

void read_graph() {
  scanf("%d", &n);

  clear_graph();
  for (int u = 1; u <= n; u++) {
    int in_degree, v;
    scanf("%d", &in_degree);
    while (in_degree--) {
      scanf("%d", &v);
      add_edge(v, u);
    }
  }
}

void maximize(int& x, int y) {
  if (y > x) {
    x = y;
  }
}

void top_sort(int u) {
  has_cycle |= (nd[u].color == GRAY);
  if (nd[u].color != WHITE) {
    return;
  }

  nd[u].color = GRAY;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    top_sort(v);
    maximize(nd[u].pass, nd[v].pass + (u > v));
    maximize(max_pass, nd[u].pass);
  }

  nd[u].color = BLACK;
}

void solve_test() {
  read_graph();

  for (int u = 1; u <= n; u++) {
    top_sort(u);
  }
  printf("%d\n", has_cycle ? -1 : max_pass);
}

int main() {

  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
