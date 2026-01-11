#include <stdio.h>

const int MAX_NODES = 100'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth;
  int time_in, time_out;
  int parent;
  int jump;

  int subtree_size() {
    return time_out - time_in + 1;
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int n;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &n);

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

// Traversează arborele și calculează părinții, adîncimile și *jump pointers*.
void dfs(int u) {
  static int time = 1;
  nd[u].time_in = time++;

  int u2 = nd[u].jump, u3 = nd[u2].jump;
  bool equal = (nd[u2].depth - nd[u].depth == nd[u3].depth - nd[u2].depth);
  int dest = equal ? u3 : u;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].depth) {
      nd[v].depth = 1 + nd[u].depth;
      nd[v].parent = u;
      nd[v].jump = dest;
      dfs(v);
    }
  }

  nd[u].time_out = time - 1;
}

bool is_ancestor(int u, int v) {
  return
    (nd[u].time_in <= nd[v].time_in) &&
    (nd[u].time_out >= nd[v].time_out);
}

int two_ptr_lca(int u, int v) {
  // Găsește cel mai jos strămoș al lui u care este și strămoș al lui v.
  while (!is_ancestor(u, v)) {
    if (nd[u].jump && !is_ancestor(nd[u].jump, v)) {
      u = nd[u].jump;
    } else {
      u = nd[u].parent;
    }
  }

  return u;
}

int get_ancestor_at_depth(int u, int d) {
  while (nd[u].depth > d) {
    u = (nd[nd[u].jump].depth >= d) ? nd[u].jump : nd[u].parent;
  }
  return u;
}

int query(int u, int v) {
  if ((nd[u].depth + nd[v].depth) % 2) {
    return 0;
  }

  if (u == v) {
    return n;
  }

  int l = two_ptr_lca(u, v);
  if (nd[u].depth == nd[v].depth) {
    int u2 = get_ancestor_at_depth(u, nd[l].depth + 1);
    int v2 = get_ancestor_at_depth(v, nd[l].depth + 1);
    return n - nd[u2].subtree_size() - nd[v2].subtree_size();
  }

  if (nd[u].depth < nd[v].depth) {
    int tmp = u; u = v; v = tmp;
  }

  int mid_level = nd[l].depth + (nd[u].depth - nd[v].depth) / 2;
  int u2 = get_ancestor_at_depth(u, mid_level + 1);
  int mid = nd[u2].parent;
  return nd[mid].subtree_size() - nd[u2].subtree_size();
}

void answer_queries() {
  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", query(u, v));
  }
}

int main() {
  read_input_data();
  nd[1].depth = 1;
  dfs(1);
  answer_queries();

  return 0;
}
