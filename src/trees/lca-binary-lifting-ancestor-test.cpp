#include <stdio.h>

const int MAX_NODES = 500'000;
const int MAX_LOG = 19;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int time_in, time_out;
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

// Traversează arborele și calculează timpii de intrare în noduri și jump
// pointers.
void dfs(int u, int parent) {
  static int time = 0;
  nd[u].time_in = time++;

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

  nd[u].time_out = time++;
}

bool is_ancestor(int u, int v) {
  return
    (nd[u].time_in <= nd[v].time_in) &&
    (nd[u].time_out >= nd[v].time_out);
}

int lca(int u, int v) {
  if (is_ancestor(u, v)) {
    return u;
  }

  // Găsește cel mai de sus strămoș al lui u care *nu* este strămoș al lui v.
  for (int i = log_2; i >= 0; i--) {
    if (nd[u].jump[i] && !is_ancestor(nd[u].jump[i], v)) {
      u = nd[u].jump[i];
    }
  }

  // Acum părintele lui u este strămoș al lui v.
  return nd[u].jump[0];
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
