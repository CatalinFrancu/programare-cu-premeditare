#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_SEGTREE_NODES = 256 * 1024;
const int INF = 1'000'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int jump;
  int depth;
};

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

struct segment_tree_node {
  int min1, min2, max1, max2;

  void set(int val) {
    min1 = max1 = val;
    min2 = INF;
    max2 = -INF;
  }

  void combine(segment_tree_node& other) {
    if (min1 < other.min1) {
      // păstrăm min1
      min2 = min(min2, other.min1);
    } else {
      min2 = min(min1, other.min2);
      min1 = other.min1;
    }
    if (max1 > other.max1) {
      // păstrăm max1
      max2 = max(max2, other.max1);
    } else {
      max2 = max(max1, other.max2);
      max1 = other.max1;
    }
  }
};

// Un arbore de intervale care stochează primul și al doilea minim și
// maxim. După construcția inițială, admite interogări pe interval.
struct segment_tree {
  segment_tree_node v[MAX_SEGTREE_NODES];
  int n;

  int next_power_of_2(int x) {
    return 1 << (32 - __builtin_clz(x - 1));
  }

  void init(int n) {
    this->n = next_power_of_2(n);
  }

  void set(int pos, int val) {
    v[n + pos].set(val);
  }

  void build() {
    for (int i = n - 1; i; i--) {
      v[i] = v[2 * i];
      v[i].combine(v[2 * i + 1]);
    }
  }

  segment_tree_node query(int l, int r) {
    segment_tree_node result = { +INF, +INF, -INF, -INF };

    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        result.combine(v[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        result.combine(v[r--]);
      }
      r >>= 1;
    }

    return result;
  }
};

cell list[MAX_NODES];
node nd[MAX_NODES + 1];
int dfs_order[MAX_NODES];
segment_tree st;
int n, num_queries;

void add_child(int p, int c) {
  static int ptr = 1;
  list[ptr] = { c, nd[p].adj };
  nd[p].adj = ptr++;
}

void read_tree() {
  scanf("%d %d", &n, &num_queries);

  for (int u = 2; u <= n; u++) {
    scanf("%d", &nd[u].parent);
    add_child(nd[u].parent, u);
  }
}

void dfs(int u) {
  static int time = 0;
  dfs_order[time] = u;
  st.set(u, time++);

  int u2 = nd[u].jump, u3 = nd[u2].jump;
  bool equal = (nd[u2].depth - nd[u].depth == nd[u3].depth - nd[u2].depth);

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    nd[v].depth = 1 + nd[u].depth;
    nd[v].parent = u;
    nd[v].jump = equal ? u3 : u;
    dfs(v);
  }
}

int lca(int u, int v) {
  if (nd[v].depth > nd[u].depth) {
    int tmp = u; u = v; v = tmp;
  }

  // Întîi adu-le la același nivel.
  while (nd[u].depth > nd[v].depth) {
    u = (nd[nd[u].jump].depth >= nd[v].depth) ? nd[u].jump : nd[u].parent;
  }

  while (u != v) {
    if (nd[u].jump != nd[v].jump) {
      u = nd[u].jump;
      v = nd[v].jump;
    } else {
      u = nd[u].parent;
      v = nd[v].parent;
    }
  }
  return u;
}

void query(int l, int r, int& kick, int& depth) {
  segment_tree_node m = st.query(l, r);
  int u = dfs_order[m.min1];
  int v = dfs_order[m.min2];
  int w = dfs_order[m.max2];
  int x = dfs_order[m.max1];

  kick = x;
  depth = nd[lca(u, w)].depth;

  int cand = nd[lca(v, x)].depth;
  if (cand > depth) {
    kick = u;
    depth = cand;
  }
}

void process_queries() {
  while (num_queries--) {
    int l, r, kick, depth;
    scanf("%d %d", &l, &r);
    query(l, r, kick, depth);
    printf("%d %d\n", kick, depth);
  }
}

int main() {
  read_tree();
  st.init(n + 1);
  dfs(1);
  st.build();
  process_queries();

  return 0;
}
