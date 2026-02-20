#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 200'000;
const long long INFINITY = 1e14;

struct edge {
  int v, dist;
};

struct node {
  std::vector<edge> adj;
  long long dist[2]; // costul minim pînă aici cu/fără reducere
};

struct pq_elt {
  long long dist;
  int u;
  bool used;

  friend bool operator< (const pq_elt a, const pq_elt b) {
    return a.dist > b.dist;
  }
};

node nd[MAX_NODES + 1];
std::priority_queue<pq_elt> pq;
int n;

void read_data() {
  int m, u, v, dist;

  scanf("%d %d", &n, &m);

  while (m--) {
    scanf("%d %d %d\n", &u, &v, &dist);
    nd[u].adj.push_back({ v, dist });
  }
}

void improve(int u, bool state_u, int v, bool state_v, int cost) {
  long long d = nd[u].dist[state_u] + cost;
  if (d < nd[v].dist[state_v]) {
    nd[v].dist[state_v] = d;
    pq.push({d, v, state_v});
  }
}

void dijkstra() {
  for (int u = 1; u <= n; u++) {
    nd[u].dist[0] = nd[u].dist[1] = INFINITY;
  }

  nd[1].dist[0] = 0;
  pq.push({0, 1, false});

  while (!pq.empty()) {
    pq_elt t = pq.top();
    pq.pop();

    // Dat fiind că inserăm nodurile de multiple ori, trebuie să verificăm
    // dacă distanța este cea curentă.
    if (t.dist == nd[t.u].dist[t.used]) {
      for (auto e: nd[t.u].adj) {
        if (!t.used) {
          improve(t.u, t.used, e.v, true, e.dist / 2);
        }

        improve(t.u, t.used, e.v, t.used, e.dist);
      }
    }
  }
}

int main() {
  read_data();
  dijkstra();
  printf("%lld\n", nd[n].dist[true]);

  return 0;
}
