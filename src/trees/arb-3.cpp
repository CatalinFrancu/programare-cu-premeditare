#include <stdio.h>

const int MAX_NODES = 250'000;
const int MAX_QUERIES = 500'000;

struct cell {
  int val, next;
};

struct node {
  int adj, q_head;
  int subtree_val;
};

struct query {
  int dist;
  int answer;
};

cell list[MAX_NODES + MAX_QUERIES + 1];
node nd[MAX_NODES + 1];
query q[MAX_QUERIES];
int s[MAX_NODES];
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
    fscanf(f, "%d", &nd[u].subtree_val);
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

void subtree_val_dfs(int u) {
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    subtree_val_dfs(v);
    nd[u].subtree_val += nd[v].subtree_val;
  }
}

void dfs(int u, int depth) {
  s[depth] += nd[u].subtree_val;

  for (int ptr = nd[u].q_head; ptr; ptr = list[ptr].next) {
    int i = list[ptr].val;
    q[i].answer = nd[u].subtree_val + s[depth + q[i].dist + 1];
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    dfs(list[ptr].val, depth + 1);
  }

  for (int ptr = nd[u].q_head; ptr; ptr = list[ptr].next) {
    int i = list[ptr].val;
    q[i].answer -= s[depth + q[i].dist + 1];
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
  subtree_val_dfs(1);
  dfs(1, 0);
  write_answers();

  return 0;
}
