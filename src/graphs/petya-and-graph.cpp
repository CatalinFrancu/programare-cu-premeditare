#include <stdio.h>

const int MAX_N = 1'000;
const int MAX_M = 1'000;

const int MAX_NODES = 1 + MAX_M + MAX_N + 1;
const int MAX_EDGES = MAX_M + 2 * MAX_M + MAX_N;
const int NIL = -1;
const int INFINITY = 1'000'000'001;

struct edge {
  long long cap;
  short v, next;
};

struct node {
  short adj;
  short parent, edge;
};

struct queue {
  int v[MAX_NODES];
  int head, tail;

  void clear() {
    head = tail = 0;
  }

  void enqueue(int x) {
    v[tail++] = x;
  }

  int dequeue() {
    return v[head++];
  }

  bool is_empty() {
    return head == tail;
  }
};

edge e[2 * MAX_EDGES];
node nd[MAX_NODES + 1];
queue q;
long long total_profit;
int n;

void add_edge(short u, short v, long long cap) {
  static int pos = 0;

  e[pos] = { cap, v, nd[u].adj };
  nd[u].adj = pos++;
}

void add_edge_pair(short u, short v, long long cap) {
  add_edge(u, v, cap);
  add_edge(v, u, 0); // muchia reziduală
}

// 1 = sursă
// 2...m+1 = muchii
// m+2...m+n+1 = noduri
// m+n+2 = destinație
void read_data() {
  int orig_n, orig_m;

  scanf("%d %d", &orig_n, &orig_m);
  n = orig_n + orig_m + 2;

  for (int u = 1; u <= n; u++) {
    nd[u].adj = NIL;
  }

  for (int u = orig_m + 2; u < n; u++) {
    int loss;
    scanf("%d", &loss);
    add_edge_pair(u, n, loss);
  }

  for (int e = 2; e <= orig_m + 1; e++) {
    int u, v, profit;
    scanf("%d %d %d", &u, &v, &profit);
    total_profit += profit;
    add_edge_pair(1, e, profit);
    add_edge_pair(e, u + orig_m + 1, INFINITY);
    add_edge_pair(e, v + orig_m + 1, INFINITY);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool bfs_reachable() {
  for (int u = 1; u <= n; u++) {
    nd[u].parent = NIL;
  }
  q.clear();
  q.enqueue(1);

  while (!q.is_empty() && (nd[n].parent == NIL)) {
    int u = q.dequeue();
    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      // Ia în calcul doar capacități pozitive către noduri neexplorate.
      if ((nd[v].parent == NIL) && e[pos].cap) {
        nd[v].parent = u;
        nd[v].edge = pos;
        q.enqueue(v);
      }
    }
  }

  return (nd[n].parent != NIL);
}

int path_minimum() {
  int min_cap = INFINITY;
  int u = n;
  while (u != 1) {
    min_cap = min(min_cap, e[nd[u].edge].cap);
    u = nd[u].parent;
  }
  return min_cap;
}

void pump_on_path(int flow) {
  int u = n;
  while (u != 1) {
    e[nd[u].edge].cap -= flow;
    e[nd[u].edge ^ 1].cap += flow;
    u = nd[u].parent;
  }
}

long long edmonds_karp() {
  long long flow = 0;

  while (bfs_reachable()) {
    int augment = path_minimum();
    pump_on_path(augment);
    flow += augment;
  }

  return flow;
}

int main() {
  read_data();
  long long max_flow = edmonds_karp();
  printf("%lld\n", total_profit - max_flow);

  return 0;
}
