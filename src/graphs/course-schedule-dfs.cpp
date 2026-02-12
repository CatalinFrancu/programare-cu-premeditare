#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;
const int WHITE = 0;
const int GRAY = 1;
const int BLACK = 2;

struct node {
  std::vector<int> adj;
  char color;
};

node nd[MAX_NODES + 1];
std::vector<int> order;
bool has_cycle;
int n;

void read_graph() {
  int num_edges, u, v;

  scanf("%d %d", &n, &num_edges);
  while (num_edges--) {
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
  }
}

void dfs(int u) {
  has_cycle |= (nd[u].color == GRAY);

  if (nd[u].color == WHITE) {
    nd[u].color = GRAY;
    for (auto v: nd[u].adj) {
      dfs(v);
    }
    nd[u].color = BLACK;
    order.push_back(u);
  }
}

void dfs_driver() {
  for (int u = 1; u <= n; u++) {
    dfs(u);
  }
}

void write_order() {
  if (has_cycle) {
    printf("IMPOSSIBLE\n");
  } else {
    for (int i = n - 1; i >= 0; i--) {
      printf("%d ", order[i]);
    }
    printf("\n");
  }
}

int main() {
  read_graph();
  dfs_driver();
  write_order();

  return 0;
}
