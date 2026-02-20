#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 200'000;
const int WHITE = 0;
const int BLACK = -1;

struct cell {
  int val, next;
};

struct node {
  int ptr, nptr; // liste de adiacență pentru muchii de arbore/nonarbore
  int state;     // 0 = alb; -1 = negru; d > 0 = gri la adîncime d
  int bad_count;
};

struct disjoint_set_forest {
  int parent[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
    for (int u = 1; u <= n; u++) {
      parent[u] = u;
    }
  }

  int find(int u) {
    return (parent[u] == u)
      ? u
      : (parent[u] = find(parent[u]));
  }

  // Unifică v în u. Fără *union by rank*.
  // Returnează true dacă și numai dacă unificarea chiar a reușit.
  bool merge(int u, int v) {
    u = find(u);
    v = find(v);
    if (u != v) {
      parent[v] = u;
      return true;
    } else {
      return false;
    }
  }
};

cell list[2 * MAX_EDGES + 1];
node nd[MAX_NODES + 1];
int stack[MAX_NODES];
disjoint_set_forest dsf;
int n, list_pos = 1;

void add_tree_edge(int u, int v) {
  list[list_pos] = { v, nd[u].ptr };
  nd[u].ptr = list_pos++;
}

void add_non_tree_edge(int u, int v) {
  list[list_pos] = { v, nd[u].nptr };
  nd[u].nptr = list_pos++;
}

void read_graph_and_kruskal() {
  int num_edges, u, v;

  scanf("%d %d", &n, &num_edges);
  dsf.init(n);
  while (num_edges--) {
    scanf("%d %d", &u, &v);
    if (dsf.merge(u, v)) {
      add_tree_edge(u, v);
      add_tree_edge(v, u);
    } else {
      add_non_tree_edge(u, v);
      add_non_tree_edge(v, u);
    }
  }
}

// Nodul u este nodul DFS activ. Nodul v este fie gri, fie negru.
void mark_subtrees(int u, int v) {
  if (nd[v].state == BLACK) {
    nd[1].bad_count++;
    nd[v].bad_count--;
    nd[u].bad_count--;
  } else {
    int w = stack[nd[v].state + 1];
    nd[w].bad_count++;
    nd[u].bad_count--;
  }
}

void dfs_mark_counts(int u, int depth) {
  nd[u].state = depth;
  stack[depth] = u;

  // Marchează muchiile de nonarbore.
  for (int ptr = nd[u].nptr; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    if (nd[v].state != WHITE) {
      mark_subtrees(u, v);
    }
  }

  // Traversează muchiile de arbore.
  for (int ptr = nd[u].ptr; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    if (nd[v].state == WHITE) {
      dfs_mark_counts(v, depth + 1);
    }
  }

  nd[u].state = BLACK;

}

void dfs_propagate_counts(int u, int parent) {
  for (int ptr = nd[u].ptr; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    if (v != parent) {
      nd[v].bad_count += nd[u].bad_count;
      dfs_propagate_counts(v, u);
    }
  }
}

void write_answers() {
  for (int u = 1; u <= n; u++) {
    char answer = (nd[u].bad_count == 0) ? '1' : '0';
    putchar(answer);
  }
  putchar('\n');
}

int main() {
  read_graph_and_kruskal();

  dfs_mark_counts(1, 1);
  dfs_propagate_counts(1, 0);

  write_answers();

  return 0;
}
