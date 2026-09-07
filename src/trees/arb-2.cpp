#include <stdio.h>

const int MAX_NODES = 250'000;
const int MAX_QUERIES = 500'000;

struct cell {
  int val, next;
};

struct node {
  int adj, q_head;
  int val;
};

struct query {
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

cell list[MAX_NODES + MAX_QUERIES + 1];
node nd[MAX_NODES + 1];
query q[MAX_QUERIES];
fenwick_tree fen;
int n, num_queries;

void add_to_list(int& head, int u) {
  static int ptr = 1;
  list[ptr] = { u, head };
  head = ptr++;
}

void read_data() {
  int p;

  FILE* f = fopen("arb.in", "r");

  fscanf(f, "%d", &n);

  for (int u = 1; u <= n; u++) {
    fscanf(f, "%d", &nd[u].val);
  }

  for (int u = 2; u <= n; u++) {
    fscanf(f, "%d", &p);
    add_to_list(nd[p].adj, u);
  }

  fscanf(f, "%d", &num_queries);

  for (int i = 0; i < num_queries; i++) {
    fscanf(f, "%d %d", &p, &q[i].dist);
    add_to_list(nd[p].q_head, i);
  }

  fclose(f);
}

void dfs(int u, int depth) {

  for (int ptr = nd[u].q_head; ptr; ptr = list[ptr].next) {
    int i = list[ptr].val;
    q[i].answer -= fen.range_sum(depth, depth + q[i].dist);
  }

  fen.add(depth, nd[u].val);
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    dfs(list[ptr].val, depth + 1);
  }

  for (int ptr = nd[u].q_head; ptr; ptr = list[ptr].next) {
    int i = list[ptr].val;
    q[i].answer += fen.range_sum(depth, depth + q[i].dist);
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
  read_data();
  fen.init(n);
  dfs(1, 1);
  write_answers();
}
