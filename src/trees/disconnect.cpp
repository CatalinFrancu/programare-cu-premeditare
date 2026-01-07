#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_NODES_ROUNDED = 1 << 17;
const int T_REMOVE = 1;

// Un arbore de intervale cu operațiile:
//
// update(l, r, val): scrie val peste tot în [l, r].
// query(pos): returnează prima valoare găsită călătorind în sus de la pos.
//
// Presupune că actualizările sînt fie disjuncte, fie imbricate. Cu alte
// cuvinte, nu există suprapuneri parțiale. Actualizările mai înguste au
// prioritate în fața celor mai late.
struct segment_tree_node {
  int val;
  int priority; // cu cît mai mică, cu atît mai importantă

  void update(int new_val, int new_priority) {
    if (new_priority < priority) {
      val = new_val;
      priority = new_priority;
    }
  }
};

struct segment_tree {
  segment_tree_node v[2 * MAX_NODES_ROUNDED];
  int n;

  int next_power_of_2(int x) {
    return 1 << (32 - __builtin_clz(x - 1));
  }

  void init(int n) {
    this->n = next_power_of_2(n);
    for (int i = 1; i < 2 * this->n; i++) {
      v[i].priority = n + 1; // cea mai neimportantă
    }
  }

  int query(int pos) {
    for (pos += n; pos; pos >>= 1) {
      if (v[pos].val) {
        return v[pos].val;
      }
    }
    return 0;
  }

  void update(int l, int r, int val) {
    int priority = r - l + 1;
    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        v[l++].update(val, priority);
      }
      l >>= 1;

      if (!(r & 1)) {
        v[r--].update(val, priority);
      }
      r >>= 1;
    }
  }
};

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int start, finish;
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
segment_tree segtree;
int n, num_ops;
FILE *fin, *fout;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  fscanf(fin, "%d %d", &n, &num_ops);
  for (int i = 1; i < n; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void flatten_tree(int u) {
  static int time = 0;

  nd[u].start = time++;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].start && (v != 1)) { // nu revizita rădăcina
      nd[v].parent = u;
      flatten_tree(v);
    }
  }

  nd[u].finish = time - 1;
}

void mark_node(int u) {
  segtree.update(nd[u].start, nd[u].finish, u);
}

bool bad_node(int u) {
  return (u < 1) || (u > n);
}

void remove_edge(int u, int v) {
  if (bad_node(u) || bad_node(v)) {
    return;
  }
  if (nd[u].parent == v) {
    mark_node(u);
  } else if (nd[v].parent == u) {
    mark_node(v);
  }
}

bool path_exists(int u, int v) {
  if (bad_node(u) || bad_node(v)) {
    return false;
  }

  u = segtree.query(nd[u].start);
  v = segtree.query(nd[v].start);

  return (u == v);
}

void process_ops() {
  int v = 0;

  while (num_ops--) {
    int type, x, y;
    fscanf(fin, "%d %d %d", &type, &x, &y);

    x ^= v;
    y ^= v;

    if (type == T_REMOVE) {
      remove_edge(x, y);
    } else {
      if (path_exists(x, y)) {
        fprintf(fout, "YES\n");
        v = x;
      } else {
        fprintf(fout, "NO\n");
        v = y;
      }
    }
  }
}

int main() {
  fin = fopen("disconnect.in", "r");
  fout = fopen("disconnect.out", "w");

  read_tree();
  flatten_tree(1);
  segtree.init(n);
  process_ops();

  fclose(fin);
  fclose(fout);

  return 0;
}
