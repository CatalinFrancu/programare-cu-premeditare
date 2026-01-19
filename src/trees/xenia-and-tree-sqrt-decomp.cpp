// Complexitate: O((n + q) sqrt q).
// Metodă: Descompunere în radical după operații.
#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_LOG = 18;
const int BLOCK_SIZE = 1'000; // determinat experimental
const int INFINITY = MAX_NODES;

const int OP_PAINT = 1;

struct cell {
  int v, next;
};

cell list[2 * MAX_NODES];

struct node {
  int adj;
  int depth;
  int dist; // distanța pînă la cel mai apropiat nod roșu
  bool red;
};

struct queue {
  int v[MAX_NODES];
  int head, tail;

  void init() {
    head = tail = 0;
  }

  void enqueue(int u) {
    v[tail++] = u;
  }

  int dequeue() {
    return v[head++];
  }

  bool is_empty() {
    return head == tail;
  }
};

node nd[MAX_NODES + 1];
int fresh[BLOCK_SIZE];
queue q;
int n, num_ops, num_fresh;

int log(int x) {
  return 31 - __builtin_clz(x);
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

// O liniarizare DFS cu repetiție și informații pentru interogări de RMQ și
// LCA în O(1).
struct lca_info {
  int d[MAX_LOG][2 * MAX_NODES];
  int tout[MAX_NODES + 1];
  int n; // lungimea liniarizării

  void append(int u) {
    tout[u] = n;
    d[0][n++] = u;
  }

  void build_rmq_table() {
    for (int l = 1; (1 << l) <= n; l++) {
      for (int i = 0; i <= n - (1 << l); i++) {
        int r = i + (1 << (l - 1));
        d[l][i] = (nd[d[l - 1][i]].depth < nd[d[l - 1][r]].depth)
          ? d[l - 1][i]
          : d[l - 1][r];
      }
    }
  }

  int lca(int u, int v) {
    int left = min(tout[u], tout[v]);
    int right = tout[u] + tout[v] - left;
    int bits = log(right - left + 1);
    int x = d[bits][left];
    int y = d[bits][right - (1 << bits) + 1];
    return (nd[x].depth < nd[y].depth) ? x : y;
  }

  int dist(int u, int v) {
    int l = lca(u, v);
    return nd[u].depth + nd[v].depth - 2 * nd[l].depth;
  }
};

lca_info l;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  scanf("%d %d", &n, &num_ops);

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

// Calculează adîncimile și liniarizarea.
void initial_dfs(int u, int parent) {
  l.append(u);

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      nd[v].depth = 1 + nd[u].depth;
      initial_dfs(v, u);
      l.append(u);
    }
  }
}

void bfs_from_red_nodes() {
  q.init();

  for (int u = 1; u <= n; u++) {
    if (nd[u].red) {
      nd[u].dist = 0;
      q.enqueue(u);
    } else {
      nd[u].dist = INFINITY;
    }
  }

  while (!q.is_empty()) {
    int u = q.dequeue();
    for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
      int v = list[ptr].v;
      if (nd[v].dist == INFINITY) {
        nd[v].dist = nd[u].dist + 1;
        q.enqueue(v);
      }
    }
  }
}

void preprocess_block() {
  bfs_from_red_nodes();
  num_fresh = 0;
}

void paint(int u) {
  nd[u].red = true;
  fresh[num_fresh++] = u;
}

int query(int u) {
  int result = nd[u].dist;
  for (int i = 0; i < num_fresh; i++) {
    result = min(result, l.dist(u, fresh[i]));
  }

  return result;
}

void process_ops() {
  nd[1].red = true;

  for (int i = 0; i < num_ops; i++) {
    if (i % BLOCK_SIZE == 0) {
      preprocess_block();
    }
    int type, u;
    scanf("%d %d", &type, &u);
    if (type == OP_PAINT) {
      paint(u);
    } else {
      printf("%d\n", query(u));
    }
  }
}

int main() {
  read_tree();
  initial_dfs(1, 0);
  l.build_rmq_table();
  process_ops();

  return 0;
}
