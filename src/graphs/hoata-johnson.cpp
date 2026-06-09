// Nu este nevoie de Bellman-Ford, întrucît graful original este un dag.
#include <algorithm>
#include <queue>
#include <stdio.h>

const int MAX_ROOMS = 300;
const int MAX_WEIGHT = 300;
const int MAX_NODES = MAX_ROOMS * (MAX_WEIGHT + 1) + 1;
const int MAX_EDGES = MAX_NODES * 4; // două muchii per cameră + reziduale
const int INF = 1'000'000'000;

struct edge {
  int cap, cost;
  int v, next;
};

struct node {
  int adj;
  int d;            // distanța de la sursă
  int parent, edge; // părintele și muchia incidentă în Dijkstra
};

typedef std::pair<int, int> pair;

node nd[MAX_NODES];
edge e[MAX_EDGES + 2];
std::priority_queue<pair, std::vector<pair>, std::greater<pair>> pq;
int n, num_thieves, source, sink, total_cost, sink_potential;
int epos;

void add_edge(int u, int v, int cap, int cost) {
  e[epos] = { cap, cost, v, nd[u].adj };
  nd[u].adj = epos++;
}

void add_edge_pair(int u, int v, int cap, int cost) {
  add_edge(u, v, cap, cost);
  add_edge(v, u, 0, -cost);
}

void read_data_and_build_graph() {
  epos = 2; // lasă 0/1 pentru NIL
  total_cost = 0;
  sink_potential = 0;

  int num_rooms, g, val, weight, alarm;
  scanf("%d %d %d", &num_rooms, &num_thieves, &g);
  n = num_rooms * (g + 1) + 1;
  source = 0;
  sink = n - 1;
  for (int u = 0; u < n; u++) {
    nd[u].adj = 0;
  }

  for (int r = 0; r < num_rooms; r++) {
    scanf("%d %d %d", &val, &weight, &alarm);
    for (int w = 0; w <= g; w++) {
      int u = r * (g + 1) + w;

      if (w + weight <= g) {
        add_edge_pair(u, u + weight, num_thieves, -val);
      }

      int v = (r < num_rooms - 1) ? (u + (g + 1)) : sink;
      add_edge_pair(u, v, alarm, 0);
    }
  }
}

void init_potentials() {
  for (int u = 0; u < n; u++) {
    nd[u].d = INF;
  }
  nd[source].d = 0;

  for (int u = 0; u < n; u++) {
    for (int pos = nd[u].adj; pos; pos = e[pos].next) {
      if (e[pos].cap) {
        int v = e[pos].v;
        nd[v].d = std::min(nd[v].d, nd[u].d + e[pos].cost);
      }
    }
  }
}

void reweight_edges() {
  for (int u = 0; u < n; u++) {
    if (nd[u].d < INF) {
      for (int pos = nd[u].adj; pos; pos = e[pos].next) {
        int v = e[pos].v;
        e[pos].cost += nd[u].d - nd[v].d;
      }
    }
  }

  sink_potential += nd[sink].d;
}

bool dijkstra() {
  for (int u = 0; u < n; u++) {
    nd[u].d = INF;
  }

  nd[source].d = 0;
  pq.push({0, source});

  while (!pq.empty()) {
    auto [dist, u] = pq.top();
    pq.pop();

    if (dist == nd[u].d) {
      for (int pos = nd[u].adj; pos; pos = e[pos].next) {
        // Relaxează muchia.
        int v = e[pos].v;
        if (e[pos].cap && (nd[u].d + e[pos].cost < nd[v].d)) {
          nd[v].d = nd[u].d + e[pos].cost;
          nd[v].parent = u;
          nd[v].edge = pos;
          pq.push({nd[v].d, v});
        }
      }
    }
  }

  return (nd[sink].d < INF);
}

int path_minimum() {
  int min_cap = num_thieves;
  int u = sink;
  while (u != source) {
    min_cap = std::min(min_cap, e[nd[u].edge].cap);
    u = nd[u].parent;
  }
  return min_cap;
}

void augment_path(int flow) {
  int u = sink;
  while (u != source) {
    e[nd[u].edge].cap -= flow;
    e[nd[u].edge ^ 1].cap += flow;
    u = nd[u].parent;
  }
}

void min_cost_max_flow() {
  init_potentials();
  reweight_edges();

  while (num_thieves && dijkstra()) {
    reweight_edges();
    int flow = path_minimum();
    augment_path(flow);
    total_cost += flow * sink_potential;
    num_thieves -= flow;
  }
}

void solve_test() {
  read_data_and_build_graph();
  min_cost_max_flow();
  printf("%d\n", (nd[sink].d == INF) ? -1 : -total_cost);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
