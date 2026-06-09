#include <algorithm>
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
  int parent, edge; // părintele și muchia incidentă în SPFA
};

struct queue {
  int v[MAX_NODES];
  bool in_queue[MAX_NODES];
  int head, tail;

  bool is_empty() {
    return head == tail;
  }

  void push(int u) {
    if (!in_queue[u]) {
      in_queue[u] = true;
      v[tail++] = u;
      if (tail == MAX_NODES) {
        tail = 0;
      }
    }
  }

  int pop() {
    int u = v[head++];
    in_queue[u] = false;
    if (head == MAX_NODES) {
      head = 0;
    }
    return u;
  }
};

node nd[MAX_NODES];
edge e[MAX_EDGES + 2];
queue q;
int n, num_thieves, source, sink, total_cost;
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

void spfa() {
  for (int u = 0; u < n; u++) {
    nd[u].d = INF;
  }

  nd[source].d = 0;
  q.push(source);

  while (!q.is_empty()) {
    int u = q.pop();
    for (int pos = nd[u].adj; pos; pos = e[pos].next) {
      int v = e[pos].v;
      if (e[pos].cap && (nd[u].d + e[pos].cost < nd[v].d)) {
        nd[v].d = nd[u].d + e[pos].cost;
        nd[v].parent = u;
        nd[v].edge = pos;
        q.push(v);
      }
    }
  }
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
  do {
    spfa();
    int flow = path_minimum();
    augment_path(flow);
    total_cost += flow * nd[sink].d;
    num_thieves -= flow;
  } while (num_thieves && (nd[sink].d < INF));
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
