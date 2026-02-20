#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 200'000;
const long long INFINITY = 1e15;

const int NEITHER = 0;
const int REMOVED = 1;
const int ADDED = 2;
const int BOTH = 3;
const int NUM_STATES = 4;

const int IMPOSSIBLE = -1;

const int FACTOR[NUM_STATES][NUM_STATES] = {
  {          1,          0,          2, 1 },
  { IMPOSSIBLE,          1, IMPOSSIBLE, 2 },
  { IMPOSSIBLE, IMPOSSIBLE,          1, 0 },
  { IMPOSSIBLE, IMPOSSIBLE, IMPOSSIBLE, 1 },
};

struct edge {
  int v, dist;
};

struct node {
  long long dist[NUM_STATES];
  std::vector<edge> adj;
};

struct pq_elt {
  long long dist;
  int u;
  unsigned char state;

  friend bool operator< (const pq_elt& a, const pq_elt& b) {
    return a.dist > b.dist;
  }
};

node nd[MAX_NODES + 1];
std::priority_queue<pq_elt> pq;
int n;

void read_graph() {
  int num_edges;

  scanf("%d %d", &n, &num_edges);

  while (num_edges--) {
    int u, v, dist;
    scanf("%d %d %d", &u, &v, &dist);
    nd[u].adj.push_back({v, dist});
    nd[v].adj.push_back({u, dist});
  }
}

void relax_all(int u, int su) {
  for (unsigned char sv = 0; sv < NUM_STATES; sv++) {
    int factor = FACTOR[su][sv];
    if (factor != IMPOSSIBLE) {
      for (auto e: nd[u].adj) {
        int v = e.v;
        long long new_dist = nd[u].dist[su] + factor * e.dist;
        if (new_dist < nd[v].dist[sv]) {
          nd[v].dist[sv] = new_dist;
          pq.push({new_dist, v, sv});
        }
      }
    }
  }
}

void dijkstra() {
  for (int u = 1; u <= n; u++) {
    for (int s = 0; s < NUM_STATES; s++) {
      nd[u].dist[s] = INFINITY;
    }
  }

  nd[1].dist[NEITHER] = 0;
  pq.push({0, 1, NEITHER});

  while (!pq.empty()) {
    pq_elt top = pq.top();
    pq.pop();
    int u = top.u;
    int su = top.state;

    if (top.dist == nd[u].dist[su]) {
      relax_all(u, su);
    }
  }
}

void write_distances() {
  for (int u = 2; u <= n; u++) {
    printf("%lld ", nd[u].dist[BOTH]);
  }
  printf("\n");
}

int main() {
  read_graph();
  dijkstra();
  write_distances();

  return 0;
}
