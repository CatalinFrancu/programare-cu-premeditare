#include <stdio.h>

const int MAX_NODES = 400'000;
const int MAX_EDGES = 400'000;

struct edge {
  int u, v;
};

struct cell {
  int edge_index, next;
};

struct node {
  int adj;
  int depth, low;
};

node nd[MAX_NODES + 1];
cell list[2 * MAX_EDGES + 1];
edge e[MAX_EDGES + 1];
int n, m, stack_size, max_comp_size, max_comp_node;

void add_edge(int u, int edge_index) {
  static int pos = 1;
  list[pos] = { edge_index, nd[u].adj };
  nd[u].adj = pos++;
}

void read_graph() {
  scanf("%d %d", &n, &m);

  for (int i = 1; i <= m; i++) {
    scanf("%d %d", &e[i].u, &e[i].v);
    add_edge(e[i].u, i);
    add_edge(e[i].v, i);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void pop_bcc(int root, int root_stack_pos) {
  if (stack_size - root_stack_pos > max_comp_size) {
    max_comp_size = stack_size - root_stack_pos;
    max_comp_node = root;
  }
  stack_size = root_stack_pos;
}

void dfs(int u, int parent) {
  nd[u].depth = nd[u].low = nd[parent].depth + 1;
  int stack_pos = stack_size++;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    edge& f = e[list[ptr].edge_index];
    int v = (f.u == u) ? f.v : f.u;

    if (!nd[v].depth) {
      dfs(v, u);
      nd[u].low = min(nd[u].low, nd[v].low);
    } else if ((v != parent) && (nd[v].depth < nd[u].depth)) {
      nd[u].low = min(nd[u].low, nd[v].depth);
    }
  }

  if (nd[u].low > nd[parent].depth) {
    pop_bcc(u, stack_pos);
  }
}

void wipe_depths() {
  for (int u = 1; u <= n; u++) {
    nd[u].depth = 0;
  }
}

void orient_edges(int u, int parent) {
  nd[u].depth =  nd[parent].depth + 1;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    edge& f = e[list[ptr].edge_index];
    int v = (f.u == u) ? f.v : f.u;

    if (!nd[v].depth) {
      f = { v, u };
      orient_edges(v, u);
    } else if ((v != parent) && (nd[v].depth < nd[u].depth)) {
      f = { v, u };
    }
  }
}

void write_answer() {
  printf("%d\n", max_comp_size);
  for (int i = 1; i <= m; i++) {
    printf("%d %d\n", e[i].u, e[i].v);
  }
}

int main() {
  read_graph();
  dfs(1, 0);
  wipe_depths();
  orient_edges(max_comp_node, 0);
  write_answer();

  return 0;
}
