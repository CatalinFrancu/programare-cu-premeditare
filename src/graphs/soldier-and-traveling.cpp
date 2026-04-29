#include <stdio.h>

const int MAX_CITIES = 100;
const int MAX_ROADS = 200;
const int MAX_NODES = 2 * MAX_CITIES + 2;
const int MAX_EDGES = 3 * MAX_CITIES + 2 * MAX_ROADS;
const int INFINITY = 1'000;
const int NIL = -1;

struct cell {
  short v;
  short cap;
  short next;
};

struct node {
  short adj;
  short parent;
  short edge;

  bool is_reachable() {
    return (parent != NIL);
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

cell e[2 * MAX_EDGES];
node nd[MAX_NODES + 1];
queue q;
int n, num_nodes, num_edges;
int source, sink;
int sum_source, sum_sink;

void add_edge(short u, short v, short c) {
  static int pos = 0;

  e[pos] = { v, c, nd[u].adj };
  nd[u].adj = pos++;
}

void add_edge_pair(short u, short v, short c) {
  add_edge(u, v, c);
  add_edge(v, u, 0);
}

void read_data() {
  int m, val;
  scanf("%d %d", &n, &m);
  num_nodes = sink = 2 * n + 2;
  source = 2 * n + 1;

  for (int u =  1; u <= num_nodes; u++) {
    nd[u].adj = NIL;
  }

  for (int u = 1; u <= n; u++) {
    scanf("%d", &val);
    add_edge_pair(source, u, val);
    sum_source += val;
  }

  for (int u = 1; u <= n; u++) {
    scanf("%d", &val);
    add_edge_pair(u + n, sink, val);
    sum_sink += val;
  }

  for (int u = 1; u <= n; u++) {
    add_edge_pair(u, u + n, INFINITY);
  }

  while (m--) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge_pair(u, v + n, INFINITY);
    add_edge_pair(v, u + n, INFINITY);
  }
}

bool bfs_reachable() {
  for (int u = 1; u <= num_nodes; u++) {
    nd[u].parent = NIL;
  }
  nd[source].parent = source;
  q.clear();
  q.enqueue(source);

  while (!q.is_empty() && !nd[sink].is_reachable()) {
    int u = q.dequeue();
    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      if (!nd[v].is_reachable() && e[pos].cap) {
        nd[v].parent = u;
        nd[v].edge = pos;
        q.enqueue(v);
      }
    }
  }

  return nd[sink].is_reachable();
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int path_minimum() {
  int min_cap = INFINITY;
  int u = num_nodes;
  while (u != source) {
    min_cap = min(min_cap, e[nd[u].edge].cap);
    u = nd[u].parent;
  }
  return min_cap;
}

void pump_on_path(int flow) {
  int u = num_nodes;
  while (u != source) {
    e[nd[u].edge].cap -= flow;
    e[nd[u].edge ^ 1].cap += flow;
    u = nd[u].parent;
  }
}

int edmonds_karp() {
  int flow = 0;

  while (bfs_reachable()) {
    int augment = path_minimum();
    pump_on_path(augment);
    flow += augment;
  }

  return flow;
}

void print_travel_matrix() {
  int dest[n + 1];
  for (int u = 1; u <= n; u++) {
    for (int v = 1; v <= n; v++) {
      dest[v] = 0;
    }
    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v - n;
      int count = e[pos ^ 1].cap;
      dest[v] = count;
    }
    for (int v = 1; v <= n; v++) {
      printf("%d ", dest[v]);
    }
    printf("\n");
  }
}

int main() {
  read_data();

  if (sum_source != sum_sink) {
    printf("NO\n");
  } else {
    int max_flow = edmonds_karp();
    if (max_flow != sum_source) {
      printf("NO\n");
    } else {
      printf("YES\n");
      print_travel_matrix();
    }
  }

  return 0;
}
