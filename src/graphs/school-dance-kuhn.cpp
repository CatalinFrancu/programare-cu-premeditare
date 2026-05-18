// Algoritmul lui Kuhn.
#include <stdio.h>
#include <vector>

const int MAX_N = 500;

struct left_node {
  std::vector<int> adj;
  bool vis;
};

left_node l[MAX_N + 1];
int r[MAX_N + 1];
int num_l, num_r;

void read_data() {
  int num_edges, u, v;
  scanf("%d %d %d", &num_l, &num_r, &num_edges);
  while (num_edges--) {
    scanf("%d %d", &u, &v);
    l[u].adj.push_back(v);
  }
}

void reset_visited() {
  for (int u = 1; u <= num_l; u++) {
    l[u].vis = false;
  }
}

bool dfs(int u) {
  if (l[u].vis) {
    return false;
  }
  l[u].vis = true;

  for (int v: l[u].adj) {
    if (!r[v] || dfs(r[v])) {
      r[v] = u;
      return true;
    }
  }

  return false;
}

int kuhn() {
  int size = 0;
  for (int u = 1; u <= num_l; u++) {
    reset_visited();
    size += dfs(u);
  }
  return size;
}

void write_solution(int max_matching) {
  printf("%d\n", max_matching);
  for (int u = 1; u <= num_r; u++) {
    if (r[u]) {
      printf("%d %d\n", r[u], u);
    }
  }
}

int main() {
  read_data();
  int max_matching = kuhn();
  write_solution(max_matching);

  return 0;
}
