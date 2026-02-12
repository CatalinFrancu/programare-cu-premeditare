#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;

struct node {
  std::vector<int> adj;
  int in_degree;
};

std::vector<int> order;
node nd[MAX_NODES + 1];
int n;

void read_graph() {
  int num_edges, u, v;

  scanf("%d %d", &n, &num_edges);
  while (num_edges--) {
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].in_degree++;
  }
}

void bfs() {
  std::queue<int> q;
  for (int u = 1; u <= n; u++) {
    if (!nd[u].in_degree) {
      q.push(u);
    }
  }

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    order.push_back(u);
    for (auto v: nd[u].adj) {
      nd[v].in_degree--;
      if (!nd[v].in_degree) {
        q.push(v);
      }
    }
  }
}

void write_order() {
  if ((int)order.size() == n) {
    for (int u: order) {
      printf("%d ", u);
    }
    printf("\n");
  } else {
    printf("IMPOSSIBLE\n");
  }
}

int main() {
  read_graph();
  bfs();
  write_order();

  return 0;
}
