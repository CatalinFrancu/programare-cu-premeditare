// Algoritmul lui Dinic.
#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 1'000;
const int NIL = -1;
const int INFINITY = 1'000'000'001;

struct edge {
  int cap;
  short v, next;
};

struct node {
  short adj;
  short ptr;
  short dist;

  bool is_reachable() {
    return dist != NIL;
  }
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

bool bfs_reachable() {
  for (int u = 1; u <= n; u++) {
    nd[u].dist = NIL;
  }
  nd[1].dist = 0;
  q.clear();
  q.enqueue(1);

  while (!q.is_empty()) {
    int u = q.dequeue();
    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      // Consideră doar capacitățile pozitive către noduri neexplorate.
      if (!nd[v].is_reachable() && e[pos].cap) {
        nd[v].dist = nd[u].dist + 1;
        q.enqueue(v);
      }
    }
  }

  return nd[n].is_reachable();
}

void reset_dfs_pointers() {
  for (int u = 1; u <= n; u++) {
    nd[u].ptr = nd[u].adj;
  }
}

int dfs(int u, int incoming_flow) {
  if (u == n) {
    return incoming_flow;
  }

  int result = 0;
  while (!result && (nd[u].ptr != NIL)) {
    int p = nd[u].ptr;
    int v = e[p].v, cap = e[p].cap;
    if (cap && (nd[v].dist == nd[u].dist + 1)) {
      result = dfs(v, min(cap, incoming_flow));
      e[p].cap -= result;
      e[p ^ 1].cap += result;
    }
    // Rămîi pe aceeași muchie dacă mai este folosibilă. Aceasta scade timpul
    // de rulare de 6 ori.
    if (!result) {
      nd[u].ptr = e[p].next;
    }
  }

  return result;
}

long long dinic() {
  long long flow = 0;

  while (bfs_reachable()) {
    reset_dfs_pointers();
    int augment;
    while ((augment = dfs(1, INFINITY))) {
      flow += augment;
    }
  }

  return flow;
}

int main() {
  read_data();
  long long max_flow = dinic();
  printf("%lld\n", max_flow);

  return 0;
}
