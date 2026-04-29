// Metodă: Ford Fulkerson + DFS.
#include <stdio.h>

const int MAX_NODES = 1'000;
const int MAX_EDGES = 5'000;
const int NIL = -1;
const int INFINITY = 1'000'000'001;

struct edge {
  int cap;
  short v, next;
};

struct node {
  short adj;
  bool seen;
};

edge e[2 * MAX_EDGES];
node nd[MAX_NODES + 1];
int n;

void add_edge(short u, short v, int cap) {
  static int pos = 0;

  e[pos] = { cap, v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  int m;
  scanf("%d %d", &n, &m);
  for (int u = 1; u <= n; u++) {
    nd[u].adj = NIL;
  }
  while (m--) {
    int u, v, c;
    scanf("%d %d %d", &u, &v, &c);
    add_edge(u, v, c);
    add_edge(v, u, 0);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void reset_dfs() {
  for (int u = 1; u <= n; u++) {
    nd[u].seen = false;
  }
}

// Găsește un drum de creștere de la u la destinație și pompează cel mult
// max_pump unități pe el. Returnează fluxul pompat efectiv sau 0 dacă nu
// există niciun drum de creștere.
int dfs(int u, int max_pump) {
  nd[u].seen = true;
  if (u == n) {
    return max_pump;
  }

  int pumped = 0;
  for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    int residual = e[pos].cap;
    if (!pumped && residual && !nd[v].seen) {
      pumped = dfs(v, min(max_pump, residual));
      if (pumped) {
        e[pos].cap -= pumped;
        e[pos ^ 1].cap += pumped;
      }
    }
  }

  return pumped;
}

long long ford_fulkerson() {
  long long flow = 0, augment;

  do {
    reset_dfs();
    augment = dfs(1, INFINITY);
    flow += augment;
  } while (augment);

  return flow;
}

int main() {
  read_data();
  long long max_flow = ford_fulkerson();
  printf("%lld\n", max_flow);

  return 0;
}
