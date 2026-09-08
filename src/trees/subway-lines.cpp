#include <algorithm>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;
const int OP_UPDATE = 1;
const int OP_QUERY = 2;

struct node {
  std::vector<int> adj;
  int parent;
  int depth;
  int heavy;  // fiul heavy
  int head;   // începutul lanțului
  int lo, hi; // intervalul de înălțimi, relevant doar pentru capetele de lanț
  int op_id;  // ID-ul operației care a setat lo și hi
};

node nd[MAX_NODES + 1];
int n, num_queries;
int op_id; // contor global

void read_tree() {
  scanf("%d %d", &n, &num_queries);
  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
}

int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {
      nd[v].depth = nd[u].depth + 1;
      nd[v].parent = u;
      int child_size = heavy_dfs(v);
      my_size += child_size;
      if (child_size > max_child_size) {
        max_child_size = child_size;
        nd[u].heavy = v;
      }

    }
  }

  return my_size;
}

void decompose_dfs(int u, int head) {
  nd[u].head = head;

  for (int v: nd[u].adj) {
    if (v == nd[u].heavy) {
      decompose_dfs(v, head);
    } else if (v != nd[u].parent) {
      decompose_dfs(v, v); // la v începe un lanț nou
    }
  }
}

int chain_op(int h, int u, int v, int op) {
  if (op == OP_UPDATE) {
    nd[h].lo = nd[u].depth;
    nd[h].hi = nd[v].depth;
    nd[h].op_id = op_id;
  } else if ((nd[h].op_id == op_id) &&    // calea [a,b] a trecut pe aici
             (nd[u].depth <= nd[h].hi) && // intervalele se intersectează
             (nd[v].depth >= nd[h].lo)) {
    return
      std::min(nd[v].depth, nd[h].hi) -
      std::max(nd[u].depth, nd[h].lo) + 1;
  }

  return 0;
}

int visit_path(int u, int v, int op) {
  int result = 0;

  while (nd[u].head != nd[v].head) {
    if (nd[nd[v].head].depth > nd[nd[u].head].depth) {
      std::swap(u, v);
    }
    int h = nd[u].head;
    result += chain_op(h, h, u, op);
    u = nd[h].parent;
  }

  if (nd[u].depth > nd[v].depth) {
    std::swap(u, v);
  }
  result += chain_op(nd[u].head, u, v, op);

  return result;
}

void process_queries() {
  while (num_queries--) {
    op_id++;
    int a, b, c, d;
    scanf("%d %d %d %d", &a, &b, &c, &d);
    visit_path(a, b, OP_UPDATE);
    printf("%d\n", visit_path(c, d, OP_QUERY));
  }
}

int main() {
  read_tree();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  process_queries();

  return 0;
}
