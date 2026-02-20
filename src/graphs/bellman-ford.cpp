#include <stdio.h>

const int MAX_NODES = 50'000;
const int MAX_EDGES = 250'000;
const int INFINITY = 100'000'000;

struct edge {
  unsigned short u, v;
  short cost;
};

edge e[MAX_EDGES];
int d[MAX_NODES + 1];
int n, m;
bool negative_cycle;

void read_data() {
  FILE* f = fopen("bellmanford.in", "r");
  fscanf(f, "%d %d", &n, &m);
  for (int i = 0; i < m; i++) {
    fscanf(f, "%hu %hu %hd", &e[i].u, &e[i].v, &e[i].cost);
  }
  fclose(f);
}

bool relax_edge(edge& e) {
  if (d[e.u] + e.cost < d[e.v]) {
    d[e.v] = d[e.u] + e.cost;
    return true;
  } else {
    return false;
  }
}

void bellman_ford() {
  for (int u = 1; u <= n; u++) {
    d[u] = INFINITY;
  }
  d[1] = 0;

  bool any_decreases = true;
  int stage = 0;
  while ((stage < n) && any_decreases) {
    any_decreases = false;
    for (int i = 0; i < m; i++) {
      any_decreases |= relax_edge(e[i]);
    }
    stage++;
  }

  negative_cycle = any_decreases;
}

void write_answer() {
  FILE* f = fopen("bellmanford.out", "w");
  if (negative_cycle) {
    fprintf(f, "Ciclu negativ!\n");
  } else {
    for (int u = 2; u <= n; u++) {
      fprintf(f, "%d ", d[u]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

int main() {
  read_data();
  bellman_ford();
  write_answer();

  return 0;
}
