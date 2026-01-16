// Complexitate: O((n + q) sqrt q).
// Metodă: Descompunere în radical după interogări.
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_OPS = 200'000;
const int MAX_LOG = 19;
const int BLOCK_SIZE = 2'000; // determinat experimental
const int INFINITY = 2 * MAX_NODES;

const int OP_TOGGLE = 1;
const int OP_QUERY = 2;

struct cell {
  int v, next;
};

cell list[2 * MAX_NODES];

struct node {
  int adj;
  int depth;

  // binary lifting cu doi pointeri per nod
  int parent, jump;

  int dist; // distanța pînă la cea mai apropiată frunză safe

  // distanța pînă la cea mai apropiată frunză safe de la orice nod subîntins
  // de jump pointer (excluzînd nodul destinație)
  int jdist;

  bool active;
  bool safe;
};

struct operation {
  int type, u, v;
};

// O simplă coadă.
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

// O colecție care menține valori distincte între 1 și MAX_NODES și admite
// adăugări și ștergeri în O(1) și iterarea prin valori în O(nr. de valori).
struct tracker {
  int size;
  int t[BLOCK_SIZE];
  int pos[MAX_NODES + 1]; // BLOCK_SIZE = absent

  void init(int max_val) {
    for (int val = 1; val <= max_val; val++) {
      pos[val] = BLOCK_SIZE;
    }
    size = 0;
  }

  void clear() {
    for (int i = 0; i < size; i++) {
      pos[t[i]] = BLOCK_SIZE;
    }
    size = 0;
  }

  void add(int val) {
    pos[val] = size;
    t[size++] = val;
  }

  void remove(int val) {
    int p = pos[val];
    t[p] = t[size - 1]; // move last element in place of val
    pos[t[p]] = p;
    pos[val] = BLOCK_SIZE;
    size--;
  }

  void toggle(int val) {
    if (pos[val] == BLOCK_SIZE) {
      add(val);
    } else {
      remove(val);
    }
  }
};

node nd[MAX_NODES + 1];
operation op[MAX_OPS];
tracker unsafe;
queue q;
int n, num_ops;

int log(int x) {
  return 31 - __builtin_clz(x);
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

// Liniarizare cu RMQ, cu care putem răspunde la interogări de LCA în O(1).
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

void read_data() {
  scanf("%d %d", &n, &num_ops);

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  for (int i = 0; i < num_ops; i++) {
    scanf("%d %d", &op[i].type, &op[i].u);
    if (op[i].type == OP_QUERY) {
      scanf("%d", &op[i].v);
    }
  }
}

// Calculează părinți, adîncimi, jump pointers și liniarizarea.
void initial_dfs(int u) {
  int u2 = nd[u].jump, u3 = nd[u2].jump;
  bool equal = (nd[u2].depth - nd[u].depth == nd[u3].depth - nd[u2].depth);
  l.append(u);

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {
      nd[v].depth = 1 + nd[u].depth;
      nd[v].parent = u;
      nd[v].jump = equal ? u3 : u;
      initial_dfs(v);
      l.append(u);
    }
  }
}

// Colecteaza un bloc cu tel mult BLOCK_SIZE operații toggle(). Marchează ca
// safe frunzele care sînt active acum și care nu vor fi modificate în bloc.
// Inițializează trackerul cu frunzele unsafe, dar care încep ca active.
// Returnează finalul blocului.
int classify_leaves(int start) {
  int num_toggles = 0;
  int end = start;

  unsafe.clear();
  for (int u = 1; u <= n; u++) {
    nd[u].safe = nd[u].active;
  }

  while ((end < num_ops) && (num_toggles < BLOCK_SIZE)) {
    if (op[end].type == OP_TOGGLE) {
      int u = op[end].u;
      if (nd[u].safe) {
        // Frunza este unsafe, dar începe ca activă.
        unsafe.add(u);
        nd[u].safe = false;
      }
      num_toggles++;
    }
    end++;
  }

  return end;
}

// Recalculează cîmpul dist pentru toate nodurile.
void bfs_from_safe_leaves() {
  q.init();

  for (int u = 1; u <= n; u++) {
    if (nd[u].safe) {
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

// Recalculează cîmpul jdist pentru toate nodurile.
void jdist_dfs(int u) {
  nd[u].jdist = nd[u].dist;

  int p = nd[u].parent;
  if (nd[u].jump != p) {
    int p2 = nd[p].jump;
    nd[u].jdist = min(nd[u].jdist, nd[p].jdist);
    nd[u].jdist = min(nd[u].jdist, nd[p2].jdist);
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != p) {
      jdist_dfs(v);
    }
  }
}

// Returnează finalul blocului.
int preprocess_block(int start) {
  int end = classify_leaves(start);
  bfs_from_safe_leaves();
  jdist_dfs(1);

  return end;
}

// Returnează distanța minimă la o frunză safe de la orice nod de pe calea [u,
// ancestor].
int upwards_path_min(int u, int ancestor) {
  int result = nd[ancestor].dist;

  while (u != ancestor) {
    if (nd[u].jump && (nd[nd[u].jump].depth >= nd[ancestor].depth)) {
      result = min(result, nd[u].jdist);
      u = nd[u].jump;
    } else {
      result = min(result, nd[u].dist);
      u = nd[u].parent;
    }
  }

  return result;
}

int query(int u, int v) {
  // Rezultatul vizitării unei frunze safe.
  int w = l.lca(u, v);
  int leaf_dist = min(upwards_path_min(u, w), upwards_path_min(v, w));
  int dist_uv = nd[u].depth + nd[v].depth - 2 * nd[w].depth;
  int result = 2 * leaf_dist + dist_uv;

  // Rezultatul vizitării unei frunze unsafe.
  for (int i = 0; i < unsafe.size; i++) {
    int leaf = unsafe.t[i];
    result = min(result, l.dist(u, leaf) + l.dist(leaf, v));
  }

  return (result < INFINITY) ? result : -1;
}

void toggle(int u) {
  unsafe.toggle(u);
  nd[u].active = !nd[u].active;
}

void process_ops() {
  int block_end = 0;

  for (int i = 0; i < num_ops; i++) {
    if (i == block_end) {
      block_end = preprocess_block(i);
    }
    if (op[i].type == OP_TOGGLE) {
      toggle(op[i].u);
    } else {
      printf("%d\n", query(op[i].u, op[i].v));
    }
  }
}

int main() {
  read_data();
  initial_dfs(1);
  l.build_rmq_table();
  unsafe.init(n);
  process_ops();

  return 0;
}
