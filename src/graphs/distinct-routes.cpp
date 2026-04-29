#include <queue>
#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 1'000;
const int NIL = -1;
const int INFINITY = 1'001;

struct edge {
  short v;
  short cap;
  short next;
};

struct node {
  short adj;
  short parent, edge;
};

edge e[2 * MAX_EDGES];
node nd[MAX_NODES + 1];
std::queue<short> q;
int n, num_edges;

void add_edge(short u, short v, short cap) {
  static int pos = 0;

  e[pos] = { v, cap, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  scanf("%d %d", &n, &num_edges);
  for (int u = 1; u <= n; u++) {
    nd[u].adj = NIL;
  }
  for (int i = 0; i < num_edges; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v, 1);
    add_edge(v, u, 0);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool bfs_reachable() {
  for (int u = 1; u <= n; u++) {
    nd[u].parent = NIL;
  }
  q = {};
  q.push(1);

  while (!q.empty() && (nd[n].parent == NIL)) {
    int u = q.front();
    q.pop();
    for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      // Consideră doar capacitățile pozitive către noduri neexplorate.
      if ((nd[v].parent == NIL) && e[pos].cap) {
        nd[v].parent = u;
        nd[v].edge = pos;
        q.push(v);
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

void clear_parent() {
  for (int u = 1; u <= n; u++) {
    nd[u].parent = NIL;
  }
}

void dfs(int u, int parent, int edge) {
  nd[u].parent = parent;
  nd[u].edge = edge;

  for (int pos = nd[u].adj; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    int rev = pos ^ 1;
    // Follow a regular edge if there is residual flow on the reverse edge.
    if ((nd[v].parent == NIL) &&
        !(pos & 1) &&
        e[rev].cap) {
      dfs(v, u, pos);
    }
  }
}

void print_path(int u, int len) {
  if (u == 1) {
    printf("%d\n1 ", len);
  } else {
    int edge = nd[u].edge;
    e[edge].cap++;
    e[edge ^ 1].cap--;
    print_path(nd[u].parent, len + 1);
    printf("%d ", u);
  }
}

void recover_paths() {
  do {
    clear_parent();
    dfs(1, NIL, NIL);
    if (nd[n].parent != NIL) {
      print_path(n, 1);
      printf("\n");
    }
  } while (nd[n].parent != NIL);
}

int main() {
  read_data();
  int max_flow = ford_fulkerson();
  printf("%d\n", max_flow);
  recover_paths();

  return 0;
}
