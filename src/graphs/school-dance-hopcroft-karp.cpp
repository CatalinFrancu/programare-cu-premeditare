// Algoritmul Hopcroft-Karp.
#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 500;
const int INFINITY = 32'767;

struct left_node {
  std::vector<short> adj;
  short dist;
  short pair;
};

struct right_node {
  short pair;
};

left_node l[MAX_NODES + 1];
right_node r[MAX_NODES + 1];
int num_l, num_r;

void read_data() {
  int m, u, v;

  scanf("%d %d %d", &num_l, &num_r, &m);
  for (int i = 1; i <= m; i++) {
    scanf("%d %d", &u, &v);
    l[u].adj.push_back(v);
  }
}

std::queue<short> put_unmatched_in_queue() {
  std::queue<short> q;
  for (int u = 1; u <= num_l; u++) {
    if (l[u].pair) {
      l[u].dist = INFINITY;
    } else {
      l[u].dist = 0;
      q.push(u);
    }
  }
  return q;
}

bool bfs() {
  std::queue<short> q = put_unmatched_in_queue();
  bool reachable = false;

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    for (int v: l[u].adj) {
      int p = r[v].pair;
      if (!p) {
        reachable = true;
      } else if (l[p].dist == INFINITY) {
        l[p].dist = l[u].dist + 1;
        q.push(p);
      }
    }
  }

  return reachable;
}

bool dfs(int u) {
  for (int v: l[u].adj) {
    int p = r[v].pair;
    if (!p || ((l[p].dist == l[u].dist + 1) && dfs(p))) {
      l[u].pair = v;
      r[v].pair = u;
      return true;
    }
  }

  l[u].dist = INFINITY;
  return false;
}

int hopcroft_karp() {
  int size = 0;
  while (bfs()) {
    for (int u = 1; u <= num_l; u++) {
      if (!l[u].pair && dfs(u)) {
        size++;
      }
    }
  }
  return size;
}

void recover_matching() {
  for (int u = 1; u <= num_l; u++) {
    if (l[u].pair) {
      printf("%d %d\n", u, l[u].pair);
    }
  }
}

int main() {
  read_data();
  printf("%d\n", hopcroft_karp());
  recover_matching();

  return 0;
}
