#include <stdio.h>

const int MAX_NODES = 500;
const long long INFINITY = 1'000'000'000'000LL;

long long d[MAX_NODES + 1][MAX_NODES + 1];
int n, num_queries;

void init_distances() {
  for (int u = 1; u <= n; u++) {
    for (int v = 1; v <= n; v++) {
      d[u][v] = (u == v) ? 0 : INFINITY;
    }
  }
}

void minimize(long long& x, long long y) {
  if (y < x) {
    x = y;
  }
}

void read_data() {
  int m, u, v, cost;
  scanf("%d %d %d", &n, &m, &num_queries);

  init_distances();

  while (m--) {
    scanf("%d %d %d", &u, &v, &cost);
    // Atenție la muchiile multiple între aceleași noduri.
    minimize(d[u][v], cost);
    minimize(d[v][u], cost);
  }
}

void floyd_warshall() {
  for (int k = 1; k <= n; k++) {
    for (int u = 1; u <= n; u++) {
      for (int v = 1; v <= n; v++) {
        minimize(d[u][v], d[u][k] + d[k][v]);
      }
    }
  }
}

void process_queries() {
  while (num_queries--) {
    int u, v;
    scanf("%d %d", &u, &v);
    printf("%lld\n", (d[u][v] == INFINITY) ? -1 : d[u][v]);
  }
}

int main() {
  read_data();
  floyd_warshall();
  process_queries();

  return 0;
}
