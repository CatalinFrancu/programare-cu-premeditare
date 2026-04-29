#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 1'000;
const int NIL = -1;
const int INFINITY = 1'000'000;

struct edge {
  short v;
  short cap;
  short next;
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

edge e[4 * MAX_EDGES];
node nd[MAX_NODES + 1];
queue q;
int n;

void add_edge(short u, short v) {
  static int pos = 0;

  e[pos] = { v, 1, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  int m;
  scanf("%d %d", &n, &m);
  for (int u = 1; u <= n; u++) {
    nd[u].adj = NIL;
  }
  while (m--) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
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
      // Consideră doar capacitățile pozitive către noduri neexplorate.
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

int ford_fulkerson() {
  int flow = 0;

  while (bfs_reachable()) {
    int augment = path_minimum();
    pump_on_path(augment);
    flow += augment;
  }

  return flow;
}

void write_min_cut(int flow) {
  printf("%d\n", flow);

  for (int u = 1; u <= n; u++) {
    if ((u == 1) || (nd[u].parent != NIL)) {
      for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
        int v = e[pos].v;
        if ((nd[v].parent == NIL)) {
          printf("%d %d\n", u, v);
        }
      }
    }
  }
}

int main() {
  read_data();
  int max_flow = ford_fulkerson();
  write_min_cut(max_flow);

  return 0;
}
