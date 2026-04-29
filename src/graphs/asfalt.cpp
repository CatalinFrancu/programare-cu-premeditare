#include <queue>
#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 5'000;
const int INFINITY = 1'000'000'000;
const int DIR_SOURCE = 0;
const int DIR_SINK = 1;
const int NIL = -1;

struct cell {
  int c;
  short v;
  short next;
  bool forward;
};

struct node {
  short adj;
  short parent;
  short edge;
  int dist[2];

  bool is_reachable() {
    return (parent != NIL);
  }
};

struct heap_elt {
  int dist;
  short u;

  friend bool operator< (const heap_elt& a, const heap_elt& b) {
    return a.dist > b.dist;
  }
};

cell e[2 * MAX_EDGES];
node nd[MAX_NODES + 1];
std::priority_queue<heap_elt> h;
int n, num_edges, source, sink;

void add_edge(short u, short v, int c) {
  static int pos = 0;

  e[pos] = { c, v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  FILE *f = fopen("asfalt.in", "r");

  fscanf(f, "%d %d", &n, &num_edges);
  fscanf(f, "%d %d", &source, &sink);

  for (int u = 1; u <= n; u++) {
    nd[u].adj = NIL;
  }

  for (int i = 0; i < num_edges; i++) {
    int u, v, c;
    fscanf(f, "%d %d %d", &u, &v, &c);
    add_edge(u, v, c);
    add_edge(v, u, c);
  }

  fclose(f);
}

void relax_all(int u, int dir) {
  for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
    short v = e[pos].v;
    int new_dist = nd[u].dist[dir] + e[pos].c;
    if (new_dist < nd[v].dist[dir]) {
      nd[v].dist[dir] = new_dist;
      h.push({new_dist, v});
    }
  }
}

void dijkstra(short start, int dir) {
  for (int u = 1; u <= n; u++) {
    nd[u].dist[dir] = INFINITY;
  }

  nd[start].dist[dir] = 0;
  h.push({0, start});

  while (!h.empty()) {
    heap_elt top = h.top();
    h.pop();

    if (top.dist == nd[top.u].dist[dir]) {
      relax_all(top.u, dir);
    }
  }
}

bool is_min_path_edge(int u, int edge_cost, int v) {
  return nd[u].dist[DIR_SOURCE] + edge_cost + nd[v].dist[DIR_SINK] ==
    nd[source].dist[DIR_SINK];
}

void convert_to_flow_network() {
  for (int u = 1; u <= n; u++) {
    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v, c = e[pos].c;
      // Let one node configure the capacities of both edges.
      // Write down which of the two edges goes forward.
      if (u < v) {
        e[pos].c = e[pos ^ 1].c = 0;
        if (is_min_path_edge(u, c, v)) {
          e[pos].c = 1;
          e[pos].forward = true;
        } else if (is_min_path_edge(v, c, u)) {
          e[pos ^ 1].c = 1;
          e[pos ^ 1].forward = true;
        }
      }
    }
  }
}

bool bfs_reachable() {
  std::queue<int> q;

  for (int u = 1; u <= n; u++) {
    nd[u].parent = NIL;
  }
  nd[source].parent = source;
  q.push(source);

  while (!q.empty() && (nd[sink].parent == NIL)) {
    int u = q.front();
    q.pop();

    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      if ((nd[v].parent == NIL) && e[pos].c) {
        nd[v].parent = u;
        nd[v].edge = pos;
        q.push(v);
      }
    }
  }

  return nd[sink].is_reachable();
}

void pump_on_path() {
  int u = sink;
  while (u != source) {
    e[nd[u].edge].c--;
    e[nd[u].edge ^ 1].c++;
    u = nd[u].parent;
  }
}

int edmonds_karp() {
  int flow = 0;

  while (bfs_reachable()) {
    pump_on_path();
    flow++;
  }

  return flow;
}

bool is_cross_cut_flow_edge(int u, int v, int pos) {
  return
    nd[u].is_reachable() &&
    !nd[v].is_reachable() &&
    e[pos].forward &&
    (e[pos ^ 1].c == 1);   // e[pos] is a saturated flow edge
}

void write_min_cut(int flow) {
  FILE* f = fopen("asfalt.out", "w");
  fprintf(f, "%d\n", flow);

  for (int u = 1; u <= n; u++) {
    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      if (is_cross_cut_flow_edge(u, v, pos)) {
        fprintf(f, "%d %d\n", u, v);
      }
    }
  }

  fclose(f);
}

int main() {
  read_data();
  dijkstra(source, DIR_SOURCE);
  dijkstra(sink, DIR_SINK);
  convert_to_flow_network();
  int max_flow = edmonds_karp();
  write_min_cut(max_flow);

  return 0;
}
