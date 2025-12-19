#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  bool matched;
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int max_match;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  int n, u, v;

  scanf("%d",  &n);
  for (int i = 1; i < n; i++) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

void dfs(int u, int parent) {
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      dfs(v, u);
      if (!nd[u].matched && !nd[v].matched) {
        max_match++;
        nd[u].matched = nd[v].matched = true;
      }
    }
  }
}

void write_answer() {
  printf("%d\n", max_match);
}

int main() {
  read_data();
  dfs(1, 0);
  write_answer();

  return 0;
}
