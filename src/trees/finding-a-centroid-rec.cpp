#include <stdio.h>
#include <vector>

const int MAX_NODES = 200'000;

struct node {
  std::vector<int> adj;
  int size;
};

node nd[MAX_NODES + 1];
int n;

void read_input_data() {
  scanf("%d", &n);

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
}

void size_dfs(int u) {
  nd[u].size = 1;

  for (int v: nd[u].adj) {
    if (!nd[v].size) {
      size_dfs(v);
      nd[u].size += nd[v].size;
    }
  }
}

int find_centroid(int u) {
  for (int v: nd[u].adj) {
    if ((nd[v].size < nd[u].size) && (nd[v].size > n / 2)) {
      return find_centroid(v);
    }
  }

  return u;
}

int main() {
  read_input_data();
  size_dfs(1);
  int c = find_centroid(1);
  printf("%d\n", c);

  return 0;
}
