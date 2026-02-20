#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <vector>

const int MAX_NODES = 1'000;
const int INFINITY = 1'000'000'001;
const int NONE = -1;

struct graph {
  std::vector<short> adj[MAX_NODES + 1];

  void clear(int n) {
    for (int u = 1; u <= n; u++) {
      adj[u].resize(0);
    }
  }

  void read_edges() {
    int m, u, v;
    scanf("%d", &m);
    while (m--) {
      scanf("%d %d", &u, &v);
      adj[u].push_back(v);
      adj[v].push_back(u);
    }
  }
};

struct pq_elt {
  short u1, u2;
  int dist;

  friend bool operator<(const pq_elt& a, const pq_elt& b) {
    return a.dist > b.dist;
  }
};

graph g1, g2;
int d[MAX_NODES + 1][MAX_NODES + 1];
int n;
short start1, start2;

void read_data() {
  scanf("%d %hd %hd", &n, &start1, &start2);
  g1.clear(n);
  g2.clear(n);
  g1.read_edges();
  g2.read_edges();
}

int dijkstra() {
  for (int u = 1; u <= n; u++) {
    for (int v = 1; v <= n; v++) {
      d[u][v] = INFINITY;
    }
  }
  int answer = NONE;

  std::priority_queue<pq_elt> pq;
  d[start1][start2] = 0;
  pq.push({start1, start2, 0});

  while (!pq.empty() && (answer == NONE)) {
    pq_elt e = pq.top();
    pq.pop();
    if (e.dist == d[e.u1][e.u2]) {
      for (short v1: g1.adj[e.u1]) {
        for (short v2: g2.adj[e.u2]) {
          if ((e.u1 == e.u2) && (v1 == v2)) {
            answer = e.dist;
          }
          if (e.dist + abs(v1 - v2) < d[v1][v2]) {
            d[v1][v2] = e.dist + abs(v1 - v2);
            pq.push({v1, v2, d[v1][v2]});
          }
        }
      }
    }
  }

  return answer;
}

void solve_test() {
  read_data();
  int answer = dijkstra();
  printf("%d\n", answer);
}

int main() {
  int num_tests;

  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
