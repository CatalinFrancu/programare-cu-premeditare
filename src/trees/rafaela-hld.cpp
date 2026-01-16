// Complexitate: O(q log^2 n).
// Metodă: descompunere heavy-light.
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_SEGTREE_NODES = 1 << 19;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;      // fiul cu cel mai mare subarbore
  int head;       // începutul lanțului nostru
  int tin, tout;  // momentele intrării și ieșirii din DFS
  int init_pop;   // populația inițială
};

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

struct segtree_node {
  int m;    // maximul din intervalul subîntins
  int lazy; // sumă de adăugat la tot intervalul
  // Invariant: pentru nodul curent, m include lazy
};

// Arbore de segmente cu adăugare pe interval și maxim pe interval.
struct segment_tree {
  segtree_node v[MAX_SEGTREE_NODES];
  int n, bits;

  void init(int size) {
    n = next_power_of_2(size);
    bits = __builtin_popcount(n - 1);
  }

  void raw_set(int pos, int val) {
    v[pos + n].m = val;
  }

  void build() {
    for (int pos = n - 1; pos; pos--) {
      v[pos].m = max(v[2 * pos].m, v[2 * pos + 1].m);
    }
  }

  void push(int pos) {
    if (v[pos].lazy) {
      v[2 * pos].m += v[pos].lazy;
      v[2 * pos].lazy += v[pos].lazy;
      v[2 * pos + 1].m += v[pos].lazy;
      v[2 * pos + 1].lazy += v[pos].lazy;
      v[pos].lazy = 0;
    }
  }

  void push_path(int pos) {
    for (int b = bits; b; b--) {
      push(pos >> b);
    }
  }

  void pull_path(int pos) {
    for (pos /= 2; pos; pos /= 2) {
      if (!v[pos].lazy) {
        v[pos].m = max(v[2 * pos].m, v[2 * pos + 1].m);
      }
    }
  }

  void range_add(int l, int r, int val) {
    l += n;
    r += n;
    int orig_l = l, orig_r = r;

    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        v[l].m += val;
        v[l++].lazy += val;
      }
      l >>= 1;

      if (!(r & 1)) {
        v[r].m += val;
        v[r--].lazy += val;
      }
      r >>= 1;
    }

    pull_path(orig_l);
    pull_path(orig_r);
  }

  int range_max(int l, int r) {
    int result = 0;

    l += n;
    r += n;
    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        result = max(result, v[l++].m);
      }
      l >>= 1;

      if (!(r & 1)) {
        result = max(result, v[r--].m);
      }
      r >>= 1;
    }

    return result;
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
segment_tree segtree;
FILE *fin, *fout;
int n, num_queries, total_pop;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  fscanf(fin, "%d %d", &n, &num_queries);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  for (int u = 1; u <= n; u++) {
    fscanf(fin, "%d", &nd[u].init_pop);
    total_pop += nd[u].init_pop;
  }
}

int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {

      nd[v].parent = u;
      int child_size = heavy_dfs(v);
      my_size += child_size;
      nd[u].init_pop += nd[v].init_pop;
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
  segtree.raw_set(nd[u].tin, nd[u].init_pop);

  if (nd[u].heavy) {
    decompose_dfs(nd[u].heavy, head);
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && v != nd[u].heavy) {
      decompose_dfs(v, v); // la v începe un lanț nou
    }
  }

  nd[u].tout = time - 1;
}

void update(int u, int delta) {
  total_pop += delta;
  while (u) {
    int h = nd[u].head;
    segtree.range_add(nd[h].tin, nd[u].tin, delta);
    u = nd[h].parent;
  }
}

int query(int u) {
  int size_of_u = segtree.range_max(nd[u].tin, nd[u].tin);
  int from_parent = total_pop - size_of_u;
  int from_any_child = segtree.range_max(nd[u].tin + 1, nd[u].tout);
  return max(from_parent, from_any_child);
}

void process_ops() {
  char type;
  int u, delta;

  while (num_queries--) {
    fscanf(fin, " %c", &type);
    if (type == 'U') {
      fscanf(fin, "%d %d ", &delta, &u);
      update(u, delta);
    } else {
      fscanf(fin, "%d ", &u);
      fprintf(fout, "%d\n", query(u));
    }
  }
}

int main() {
  fin = fopen("rafaela.in", "r");
  fout = fopen("rafaela.out", "w");

  read_tree();
  segtree.init(n);
  heavy_dfs(1);
  decompose_dfs(1, 1);
  segtree.build();
  process_ops();

  fclose(fin);
  fclose(fout);

  return 0;
}
