#include <stdio.h>

const int MAX_NODES = 250'000;
const int MAX_QUERIES = 500'000;

struct cell {
  int val, next;
};

struct node {
  int adj;
  int val;
  int t_in, t_out;
  int depth;
};

struct query {
  int u;
  int dist;
  int answer;
};

struct fenwick_tree {
  int v[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  int prefix_sum(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }

  int range_sum(int x, int y) {
    if (y > n) {
      y = n;
    }
    return prefix_sum(y) - prefix_sum(x - 1);
  }
};

node nd[MAX_NODES + 1];
query q[MAX_QUERIES];
cell list[2 * MAX_NODES + MAX_QUERIES + 1];
int nodes_by_depth[MAX_NODES + 1];
int queries_by_depth[MAX_NODES + 1];
fenwick_tree fen;
int n, num_queries;
FILE* f;

void add_to_list(int& head, int u) {
  static int ptr = 1;
  list[ptr] = { u, head };
  head = ptr++;
}

void read_tree() {
  fscanf(f, "%d", &n);

  for (int u = 1; u <= n; u++) {
    fscanf(f, "%d", &nd[u].val);
  }

  for (int u = 2; u <= n; u++) {
    int p;
    fscanf(f, "%d", &p);
    add_to_list(nd[p].adj, u);
  }
}

void flatten(int u, int depth) {
  static int time = 0;
  nd[u].t_in = ++time;
  nd[u].depth = depth;
  add_to_list(nodes_by_depth[depth], u);

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    flatten(list[ptr].val, depth + 1);
  }

  nd[u].t_out = time;
}

void read_queries() {
  fscanf(f, "%d", &num_queries);

  for (int i = 0; i < num_queries; i++) {
    fscanf(f, "%d %d", &q[i].u, &q[i].dist);
    int d = nd[q[i].u].depth + q[i].dist;
    add_to_list(queries_by_depth[d], i);
  }
}

void bfs() {
  fen.init(n);
  for (int d = 1; d <= n; d++) {
    for (int ptr = nodes_by_depth[d]; ptr; ptr = list[ptr].next) {
      int u = list[ptr].val;
      fen.add(nd[u].t_in, nd[u].val);
    }
    for (int ptr = queries_by_depth[d]; ptr; ptr = list[ptr].next) {
      int i = list[ptr].val;
      int u = q[i].u;
      q[i].answer = fen.range_sum(nd[u].t_in, nd[u].t_out);
    }
  }
}

void write_answers() {
  FILE *f = fopen("arb.out", "w");
  for (int i = 0; i < num_queries; i++) {
    fprintf(f, "%d\n", q[i].answer);
  }
  fclose(f);
}

int main() {
  f = fopen("arb.in", "r");

  read_tree();
  flatten(1, 1);
  read_queries();

  fclose(f);

  bfs();
  write_answers();

  return 0;
}
