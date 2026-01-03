#include <stdio.h>

const int MAX_NODES = 400'000;
const int MAX_NODES_ROUNDED = 1 << 19;
const int T_PAINT = 1;

typedef unsigned long long u64;

// Un arbore de segmente care admite operațiile:
//
// * range_set(int l, int r, int val)
//   Atribuie valoarea val pe [l, r]. Presupune că 1 ≤ val ≤ 60.
// * range_count_distinct(int l, int r)
//   Returnează numărul de valori distincte din [l, r].
//
// Contract: mask[x] este masca valorilor din subarborele lui x. Dacă dirty[x]
// este nonzero, atunci dirty[x] trebuie scris peste tot în subarborele lui x
// și mask[x] deja respectă dirty[x].
struct segment_tree {
  u64 mask[2 * MAX_NODES_ROUNDED];
  char dirty[2 * MAX_NODES_ROUNDED];
  int n, bits;

  int next_power_of_2(int n) {
    return 1 << (32 - __builtin_clz(n - 1));
  }

  void init(int size) {
    n = next_power_of_2(size);
    bits = __builtin_popcount(n - 1);
  }

  void raw_set(int pos, char val) {
    mask[pos + n] = 1ull << val;
  }

  void build() {
    for (int i = n - 1; i; i--) {
      mask[i] = mask[2 * i] | mask[2 * i + 1];
    }
  }

  void push(int x) {
    if (dirty[x]) {
      dirty[2 * x] = dirty[2 * x + 1] = dirty[x];
      mask[2 * x] = mask[2 * x + 1] = mask[x];
      dirty[x] = 0;
    }
  }

  void push_path(int x) {
    for (int b = bits; b; b--) {
      push(x >> b);
    }
  }

  void pull_path(int x) {
    for (x /= 2; x; x /= 2) {
      if (!dirty[x]) {
        mask[x] = mask[2 * x] | mask[2 * x + 1];
      }
    }
  }

  void range_set(int l, int r, int val) {
    l += n;
    r += n;
    int orig_l = l, orig_r = r;

    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        mask[l] = 1ull << val;
        dirty[l++] = val;
      }
      l >>= 1;

      if (!(r & 1)) {
        mask[r] = 1ull << val;
        dirty[r--] = val;
      }
      r >>= 1;
    }

    pull_path(orig_l);
    pull_path(orig_r);
  }

  int range_count_distinct(int l, int r) {
    u64 result = 0;

    l += n;
    r += n;
    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        result |= mask[l++];
      }
      l >>= 1;

      if (!(r & 1)) {
        result |= mask[r--];
      }
      r >>= 1;
    }

    return __builtin_popcountll(result);
  }

};

struct cell {
  int v, next;
};

struct node {
  int adj; // lista de adiacență
  int time_in, time_out;
  unsigned char color;
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
segment_tree segtree;
int n, num_ops;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  scanf("%d %d", &n, &num_ops);
  for (int u = 1; u <= n; u++) {
    scanf("%hhd", &nd[u].color);
  }
  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void flatten_tree(int u) {
  static int time = 0;

  nd[u].time_in = time++;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].time_in && (v != 1)) { // nu revizita rădăcina
      flatten_tree(v);
    }
  }

  nd[u].time_out = time - 1;
}

void make_segtree() {
  segtree.init(n);
  for (int u = 1; u <= n; u++) {
    segtree.raw_set(nd[u].time_in, nd[u].color);
  }
  segtree.build();
}

void process_ops() {
  while (num_ops--) {
    int type, u, color;
    scanf("%d %d", &type, &u);
    int l = nd[u].time_in, r = nd[u].time_out;

    if (type == T_PAINT) {
      scanf("%d", &color);
      segtree.range_set(l, r, color);
    } else {
      int colors = segtree.range_count_distinct(l, r);
      printf("%d\n", colors);
    }
  }
}

int main() {
  read_tree();
  flatten_tree(1);
  make_segtree();
  process_ops();

  return 0;
}
