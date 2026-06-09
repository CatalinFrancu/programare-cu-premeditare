#include <algorithm>
#include <queue>
#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 1'000;
const int INF = 1'000'000'000;
const int NIL = -1;

struct edge {
  short cap, cost;
  short v, next;
};

struct node {
  short adj;
  int d;              // distanța de sursă
  short parent, edge; // părintele și muchia incidentă în SPFA
  bool in_queue;
};

node nd[MAX_NODES + 1];
edge e[2 * MAX_EDGES];
std::queue<int> q;
int n, desired_flow, total_cost;

void add_edge(short u, short v, short cap, short cost) {
  static int pos = 0;

  e[pos] = { cap, cost, v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  int m;
  scanf("%d %d %d", &n, &m, &desired_flow);
  for (int u = 1; u <= n; u++) {
    nd[u].adj = NIL;
  }
  while (m--) {
    int u, v, cap, cost;
    scanf("%d %d %d %d", &u, &v, &cap, &cost);
    add_edge(u, v, cap, cost);
    add_edge(v, u, 0, -cost);
  }
}

void enqueue(int u) {
  if (!nd[u].in_queue) {
    nd[u].in_queue = true;
    q.push(u);
  }
}

int dequeue() {
  int u = q.front();
  q.pop();
  nd[u].in_queue = false;
  return u;
}

void spfa() {
  for (int u = 1; u <= n; u++) {
    nd[u].d = INF;
    nd[u].in_queue = false;
  }

  nd[1].d = 0;
  enqueue(1);

  while (!q.empty()) {
    int u = dequeue();
    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      if (e[pos].cap && (nd[u].d + e[pos].cost < nd[v].d)) {
        nd[v].d = nd[u].d + e[pos].cost;
        nd[v].parent = u;
        nd[v].edge = pos;
        enqueue(v);
      }
    }
  }
}

int path_minimum() {
  short min_cap = desired_flow;
  int u = n;
  while (u != 1) {
    min_cap = std::min(min_cap, e[nd[u].edge].cap);
    u = nd[u].parent;
  }
  return min_cap;
}

void augment_path(int flow) {
  int u = n;
  while (u != 1) {
    e[nd[u].edge].cap -= flow;
    e[nd[u].edge ^ 1].cap += flow;
    u = nd[u].parent;
  }
}

void min_cost_max_flow() {
  while (desired_flow && (nd[n].d < INF)) {
    spfa();
    int flow = path_minimum();
    augment_path(flow);
    desired_flow -= flow;
    total_cost += flow * nd[n].d;
  }
}

int main() {
  read_data();
  min_cost_max_flow();
  printf("%d\n", (nd[n].d == INF) ? -1 : total_cost);

  return 0;
}
