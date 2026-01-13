#include <stdio.h>

const int MAX_NODES = 200'000;

struct list {
  int val, next;
};

struct node {
  int val;
  int time_in, time_out;
  int ptr; // pointer în lista de adiacență
};

struct fenwick_tree {
  long long v[2 * MAX_NODES + 1];
  int n;

  void build(int n) {
    this->n = n;
    for (int i = 1; i <= n; i++) {
      int j = i + (i & -i);
      if (j <= n) {
        v[j] += v[i];
      }
    }
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  long long sum(int pos) {
    long long s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }
};

list adj[2 * MAX_NODES];
node nd[MAX_NODES + 1];
fenwick_tree fen;
int n, num_queries;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  adj[ptr] = { v, nd[u].ptr };
  nd[u].ptr = ptr++;
}

void read_input_data() {
  scanf("%d %d", &n, &num_queries);
  for (int u = 1; u <= n; u++) {
    scanf("%d", &nd[u].val);
  }

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void flatten(int u) {
  static int time = 0;

  nd[u].time_in = ++time;

  for (int ptr = nd[u].ptr; ptr; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (!nd[v].time_in) {
      flatten(v);
    }
  }

  nd[u].time_out = ++time;
}

void build_fenwick_tree() {
  for (int u = 1; u <= n; u++) {
    fen.v[nd[u].time_in] = nd[u].val;
    fen.v[nd[u].time_out] = -nd[u].val;
  }

  fen.build(2 * n);
}

void process_queries() {
  while (num_queries--) {
    int type, u;
    scanf("%d %d", &type, &u);
    if (type == 1) {
      int val;
      scanf("%d", &val);
      int delta = val - nd[u].val;
      fen.add(nd[u].time_in, delta);
      fen.add(nd[u].time_out, -delta);
      nd[u].val = val;
    } else {
      printf("%lld\n", fen.sum(nd[u].time_in));
    }
  }
}

int main() {
  read_input_data();
  flatten(1);
  build_fenwick_tree();
  process_queries();

  return 0;
}
