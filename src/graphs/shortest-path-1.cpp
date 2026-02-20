#include <queue>
#include <stdio.h>
#include <unordered_set>
#include <vector>

const int MAX_NODES = 3'000;

struct edge {
  short u, v;
};

struct triplet_set {
  std::unordered_set<long long> s;

  long long hash(int x, int y, int z) {
    return (long long)(x - 1) * MAX_NODES * MAX_NODES +
      (y - 1) * MAX_NODES +
      (z - 1);
  }

  void insert(int x, int y, int z) {
    s.insert(hash(x, y, z));
  }

  bool contains(int x, int y, int z) {
    return s.contains(hash(x, y, z));
  }
};

std::vector<short> adj[MAX_NODES + 1];
short d[MAX_NODES + 1][MAX_NODES + 1];
short prev[MAX_NODES + 1][MAX_NODES + 1];
triplet_set triplets;
int n;

void read_data() {
  int num_edges, num_triplets, u, v, w;
  scanf("%d %d %d\n", &n, &num_edges, &num_triplets);

  while (num_edges--) {
    scanf("%d %d", &u, &v);
    adj[u].push_back(v);
    adj[v].push_back(u);
  }

  while (num_triplets--) {
    scanf("%d %d %d", &u, &v, &w);
    triplets.insert(u, v, w);
  }
}

edge bfs() {
  std::queue<edge> q;
  edge last = { 0, 0 };

  q.push({1, 1});

  while (!q.empty() && (last.v != n)) {
    edge e = q.front();
    q.pop();

    for (short w: adj[e.v]) {
      if (!d[e.v][w] && !triplets.contains(e.u, e.v, w)) {
        d[e.v][w] = 1 + d[e.u][e.v];
        prev[e.v][w] = e.u;
        q.push({e.v, w});
        if (w == n) {
          last = {e.v, w};
        }
      }
    }
  }

  return last;
}

void recover_solution(int u, int v) {
  if (u > 1) {
    recover_solution(prev[u][v], u);
  }
  printf("%d ", u);
}

void write_answer(edge last) {
  if (last.v == n) {
    printf("%d\n", d[last.u][n]);
    recover_solution(last.u, n);
    printf("%d\n", n);
  } else {
    printf("-1\n");
  }
}

int main() {
  read_data();
  edge last = bfs();
  write_answer(last);

  return 0;
}
