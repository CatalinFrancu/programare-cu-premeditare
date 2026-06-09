#include <queue>
#include <stdio.h>

const int MAX_N = 402; // 2n + 2
const int INF = 1'000'000;

struct node {
  int d;   // distanța de sursă
  short p; // părintele în SPFA
  bool in_queue;
};

node nd[MAX_N];
bool cap[MAX_N][MAX_N];
short cost[MAX_N][MAX_N];
std::queue<int> q;
int num_people, n, source, sink;
int total_cost;

void add_edge(int u, int v, int c) {
  cost[u][v] = c;
  cost[v][u] = -c;
  cap[u][v] = true;
  cap[v][u] = false;
}

void read_data_and_build_graph() {
  scanf("%d", &num_people);

  n = 2 * num_people + 2;
  source = 0;
  sink = n - 1;

  for (int u = 1; u <= num_people; u++) {
    add_edge(source, u, 0);
    add_edge(u + num_people, sink, 0);
    for (int v = num_people + 1; v <= 2 * num_people; v++) {
      int c;
      scanf("%d", &c);
      add_edge(u, v, c);
    }
  }
}

void relax_range(int u, int vstart, int vend) {
  for (int v = vstart; v <= vend; v++) {
    if (cap[u][v] && (nd[u].d + cost[u][v] < nd[v].d)) {
      nd[v].d = nd[u].d + cost[u][v];
      nd[v].p = u;
      if (!nd[v].in_queue) {
        nd[v].in_queue = true;
        q.push(v);
      }
    }
  }
}

void relax_neighbors(int u) {
  if (u == source) {
    relax_range(source, 1, num_people);
  } else if (u <= num_people) {
    relax_range(u, source, source);
    relax_range(u, num_people + 1, 2 * num_people);
  } else if (u <= 2 * num_people) {
    relax_range(u, 1, num_people);
    relax_range(u, sink, sink);
  } else { // u == sink
    relax_range(u, num_people + 1, 2 * num_people);
  }
}

void spfa() {
  for (int u = 1; u < n; u++) {
    nd[u] = { .d = INF, .in_queue = false };
  }

  nd[source] = { .d = 0, .in_queue = true };
  q.push(source);

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    nd[u].in_queue = false;
    relax_neighbors(u);
  }
}

void augment_path() {
  int u = sink;
  do {
    int p = nd[u].p;
    cap[p][u] = false;
    cap[u][p] = true;
    u = p;
  } while (u != source);
  total_cost += nd[sink].d;
}

void min_cost_max_flow() {
  for (int i = 0; i < num_people; i++) {
    spfa();
    augment_path();
  }
}

void write_assignment() {
  printf("%d\n", total_cost);
  for (int u = 1; u <= num_people; u++) {
    int v = num_people + 1;
    while (cap[u][v]) {
      v++;
    }
    printf("%d %d\n", u, v - num_people);
  }
}

int main() {
  read_data_and_build_graph();
  min_cost_max_flow();
  write_assignment();

  return 0;
}
