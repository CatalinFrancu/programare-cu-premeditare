#include <stdio.h>

const int MAX_NODES = 1'000'000;
const int MAX_SEGTREE_NODES = 1 << 21;

struct cell {
  int v, next;
};

struct node {
  int adj;
  bool has_coin;
};

int max(int x, int y) {
  return (x > y) ? x : y;
}

int next_power_of_2(int n) {
  return 1 << (32 - __builtin_clz(n - 1));
}

struct segment_tree {
  int v[MAX_SEGTREE_NODES];
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
    for (int i = 1; i < 2 * this->n; i++) {
      v[i] = 0;
    }
  }

  void set(int pos, int val) {
    pos += n;
    v[pos] = val;
    for (pos /= 2; pos; pos /= 2) {
      v[pos] = max(v[2 * pos], v[2 * pos + 1]);
    }
  }

  void optimize(int pos, int& max, int& best_pos) {
    if (v[pos] > max) {
      max = v[pos];
      best_pos = pos;
    }
  }

  void descend_and_erase(int pos) {
    while (pos < n) {
      pos = (v[pos] == v[2 * pos])
        ? (2 * pos)
        : (2 * pos + 1);
    }
    set(pos - n, 0);
  }

  // Returnează maximul din [l, r] și îl înlocuiește cu 0.
  int erase_rmq(int l, int r) {
    int result = -1, pos = 0;

    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        optimize(l++, result, pos);
      }
      l >>= 1;

      if (!(r & 1)) {
        optimize(r--, result, pos);
      }
      r >>= 1;
    }

    // Coboară din pos într-o frunză care are aceeași valoare și șterge
    // valoarea.
    descend_and_erase(pos);

    return result;
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
segment_tree segtree;
int n, list_ptr, dfs_time;

void add_child(int u, int v) {
  list[list_ptr] = { v, nd[u].adj };
  nd[u].adj = list_ptr++;
}

void read_input_data() {
  scanf("%d ", &n);

  for (int u = 1; u <= n; u++) {
    int c = getchar();
    nd[u].has_coin = (c == '1');
    nd[u].adj = 0; // null
  }

  list_ptr = 1;
  for (int u = 2; u <= n; u++) {
    int p;
    scanf("%d", &p);
    add_child(p, u);
  }
}

long long dfs(int u, int depth) {

  long long result = 0;
  int start = dfs_time++;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    result += dfs(list[ptr].v, depth + 1);
  }

  if (nd[u].has_coin) {
    segtree.set(start, depth);
  } else {
    int lowest_coin = segtree.erase_rmq(start, dfs_time - 1);
    if (lowest_coin) {
      segtree.set(start, depth);
      result += lowest_coin - depth;
    }
  }

  return result;
}

void run_test() {
  read_input_data();
  segtree.init(n);
  dfs_time = 0;
  long long moves = dfs(1, 1);
  printf("%lld\n", moves);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    run_test();
  }

  return 0;
}
