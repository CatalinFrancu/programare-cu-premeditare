#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;            // începutul listei de adiacență
  int size;           // mărimea subarborelui, inclusiv nodul însuși
  long long sum_dist; // suma distanțelor la toate celelalte noduri
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int n;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  scanf("%d",  &n);
  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

void size_dfs(int u, int parent, int depth) {
  nd[1].sum_dist += depth;
  nd[u].size = 1;
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      size_dfs(v, u, depth + 1);
      nd[u].size += nd[v].size;
    }
  }
}

void reroot_dfs(int u, int parent) {
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      nd[v].sum_dist = nd[u].sum_dist + n - 2 * nd[v].size;
      reroot_dfs(v, u);
    }
  }
}

void write_answers() {
  for (int u = 1; u <= n; u++) {
    printf("%lld ", nd[u].sum_dist);
  }
  printf("\n");
}

int main() {
  read_data();
  size_dfs(1, 0, 0);
  reroot_dfs(1, 0);
  write_answers();

  return 0;
}
