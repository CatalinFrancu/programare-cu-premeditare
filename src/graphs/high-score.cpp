#include <stdio.h>
#include <vector>

const int MAX_NODES = 50'000;
const long long INFINITY = 5'000'000'000'000LL;

struct edge {
  int v;
  int cost;
};

struct node {
  std::vector<edge> adj;
  long long dist;
  bool infinite;
  bool can_reach_dest;
};

node nd[MAX_NODES];
int n;

void read_data() {
  int m, u, v, cost;
  scanf("%d %d", &n, &m);
  while (m--) {
    scanf("%d %d %dd", &u, &v, &cost);
    nd[u].adj.push_back({v, -cost});
  }
  nd[n].can_reach_dest = true;
}

bool relax_edge(int u, int v, int cost) {
  nd[u].can_reach_dest |= nd[v].can_reach_dest;
  if ((nd[u].dist != INFINITY) && (nd[u].dist + cost < nd[v].dist)) {
    nd[v].dist = nd[u].dist + cost;
    return true;
  } else {
    return false;
  }
}

void bellman_ford() {
  for (int u = 1; u <= n; u++) {
    nd[u].dist = INFINITY;
  }
  nd[1].dist = 0;

  for (int stage = 1; stage <= n; stage++) {
    for (int u = 1; u <= n; u++) {
      for (edge e: nd[u].adj) {
        if (relax_edge(u, e.v, e.cost) && (stage == n)) {
          nd[e.v].infinite = true;
        }
      }
    }
  }
}

bool reachable_infinite() {
  for (int u = 1; u <= n; u++) {
    if (nd[u].infinite && nd[u].can_reach_dest) {
      return true;
    }
  }
  return false;
}

int main() {
  read_data();
  bellman_ford();
  int inf = reachable_infinite();
  printf("%lld\n", inf ? -1 : -nd[n].dist);

  return 0;
}
