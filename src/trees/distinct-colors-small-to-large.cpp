#include <stdio.h>
#include <unordered_set>

const int MAX_NODES = 200'000;

typedef std::unordered_set<int> set;

struct cell {
  int v, next;
};

struct node {
  int color, distinct;
  int adj;
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int n;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_data() {
  scanf("%d", &n);

  for (int u = 1; u <= n; u++) {
    scanf("%d", &nd[u].color);
  }

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void merge_into(set& src, set& dest) {
  dest.merge(src);
  src.clear();
}

set dfs(int u) {
  // marchează-l ca vizitat ca să prevenim recursivitatea infinită
  nd[u].distinct = 1;
  set result = { nd[u].color };
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].distinct) {
      set s = dfs(v);
      if (s.size() > result.size()) {
        s.swap(result);
      }
      merge_into(s, result);
    }
  }

  nd[u].distinct = result.size();
  return result;
}

void write_answer() {
  for (int u = 1; u <= n; u++) {
    printf("%d ", nd[u].distinct);
  }
  printf("\n");
}

int main() {
  read_data();
  dfs(1);
  write_answer();

  return 0;
}
