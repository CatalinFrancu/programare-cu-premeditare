// Complexitate: O(q log^2 n).
// Metodă: Descompunere heavy-light.
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_SEGTREE_NODES = 1 << 19;
const int INFINITY = 3 * MAX_NODES;
const int OP_TOGGLE = 1;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;       // fiul cu subarborele maxim
  int head;        // începutul lanțului nostru
  int light_start; // începutul restului subarborelui, după lanțul greu
  int d;           // adîncimea
  int tin, tout;   // timpii DFS
};

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

struct segment_tree {
  int v[MAX_SEGTREE_NODES];
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
    for (int i = 1; i < 2 * this->n; i++) {
      v[i] = INFINITY;
    }
  }

  void set(int pos, int val) {
    pos += n;
    v[pos] = val;
    for (pos >>= 1; pos; pos >>= 1) {
      v[pos] = min(v[2 * pos], v[2 * pos + 1]);
    }
  }

  int get(int pos) {
    return v[pos + n];
  }

  int get_min(int l, int r) {
    int result = INFINITY;
    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        result = min(result, v[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        result = min(result, v[r--]);
      }
      r >>= 1;
    }

    return result;
  }

  int get_min_plus(int l, int r, int delta) {
    int x = get_min(l, r);
    return (x == INFINITY) ? INFINITY : (x + delta);
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
segment_tree seg, seg_diff, seg_2diff;
int n, num_ops;

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

int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {

      nd[v].parent = u;
      nd[v].d = 1 + nd[u].d;
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
  static int time = 0;

  nd[u].head = head;
  nd[u].tin = time++;

  if (nd[u].heavy) {
    decompose_dfs(nd[u].heavy, head);
  }
  nd[u].light_start = time;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && v != nd[u].heavy) {
      decompose_dfs(v, v); // începi un lanț nou
    }
  }

  nd[u].tout = time - 1;
}

void init_segment_trees() {
  seg.init(n);
  seg_diff.init(n);
  seg_2diff.init(n);
}

void toggle(int u) {
  int was_off = seg.get(nd[u].tin) == INFINITY;
  int val = was_off ? nd[u].d : INFINITY;
  seg.set(nd[u].tin, val);

  while (u) {
    seg_diff.set(nd[u].tin, (val == INFINITY) ? INFINITY : (val - nd[u].d));
    seg_2diff.set(nd[u].tin, (val == INFINITY) ? INFINITY : (val - 2 * nd[u].d));
    u = nd[nd[u].head].parent;
    if (u) {
      val = seg.get_min(nd[u].light_start, nd[u].tout);
    }
  }
}

// Găsește distanța minimă pînă la o frunză urcînd din u.
int up_query(int u) {
  int leaf_dist = INFINITY;
  int orig_u = u;

  while (u) {
    int h = nd[u].head;
    int on_path_h_u = seg_2diff.get_min_plus(nd[h].tin, nd[u].tin, nd[orig_u].d);
    int on_path_below_u = seg.get_min_plus(nd[u].tin, nd[u].tout, nd[orig_u].d - 2 * nd[u].d);
    leaf_dist = min(leaf_dist, on_path_h_u);
    leaf_dist = min(leaf_dist, on_path_below_u);
    u = nd[h].parent;
  }

  return leaf_dist;
}

// Găsește distanța în jos pînă la o frunză de la orice nod de pe lanțul greu
// u-v.
int path_score(int u, int v) {
  int on_path_u_v = seg_diff.get_min(nd[u].tin, nd[v].tin);
  int in_subtree_of_v = seg.get_min_plus(nd[v].tin, nd[v].tout, -nd[v].d);
  return min(on_path_u_v, in_subtree_of_v);
}

int query(int u, int v) {
  int orig_u = u, orig_v = v;

  int leaf_dist = INFINITY;
  // Urcă-l pe u sau v cît timp sînt pe lanțuri diferite.
  while (nd[u].head != nd[v].head) {
    if (nd[v].tin > nd[u].tin) {
      int tmp = u; u = v; v = tmp;
    }
    int h = nd[u].head;
    leaf_dist = min(leaf_dist, path_score(h, u));
    u = nd[h].parent;
  }

  // O ultimă operație cînd u și v se află pe același lanț.
  if (nd[u].tin > nd[v].tin) {
    int tmp = u; u = v; v = tmp;
  }
  leaf_dist = min(leaf_dist, path_score(u, v));

  leaf_dist = min(leaf_dist, up_query(u));

  if (leaf_dist == INFINITY) {
    return -1;
  } else {
    int dist_uv = nd[orig_u].d + nd[orig_v].d - 2 * nd[u].d;
    return dist_uv + 2 * leaf_dist;
  }
}

void process_ops() {
  int type, u, v;

  while (num_ops--) {
    scanf("%d %d", &type, &u);
    if (type == OP_TOGGLE) {
      toggle(u);
    } else {
      scanf("%d", &v);
      printf("%d\n", query(u, v));
    }
  }
}

int main() {
  read_tree();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  init_segment_trees();
  process_ops();

  return 0;
}
