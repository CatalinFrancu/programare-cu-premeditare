#include <queue>
#include <stdio.h>

const int MAX_NODES = 300'000;
const int MAX_EDGES = 400'000;
const int INFINITY = 300'000'000; // costul maxim este 1'000

struct edge {
  int v, dist;

  friend bool operator< (const edge& a, const edge& b) {
    return a.dist > b.dist;
  }
};

struct node {
  int adj;
  int dist;
};

struct cell {
  int v, cost, next;
};

struct disjoint_set_forest {
  int p[MAX_NODES + 1];
  int size[MAX_NODES + 1];
  // Momentul minim la care putem părăsi componenta prin părinte.
  int t[MAX_NODES + 1];

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
      size[u] = 1;
    }
  }

  int find(int u) {
    while (p[u] != u) {
      u = p[u];
    }
    return u;
  }

  void unite(int u, int v, int time) {
    u = find(u);
    v = find(v);
    if (u != v) {
      if (size[u] > size[v]) {
        std::swap(u, v);
      }

      p[u] = v;
      size[v] += size[u];
      t[u] = time;
    }
  }

  int get_min_edge(int u, int v) {
    int result = INFINITY;
    while (u != v) {
      if (size[u] < size[v]) {
        result = std::min(result, t[u]);
        u = p[u];
      } else {
        result = std::min(result, t[v]);
        v = p[v];
      }
    }
    return result;
  }
};

node nd[MAX_NODES + 1];
cell list[2 * MAX_EDGES + 1];
int ord[MAX_NODES]; // nodurile în ordinea dispariției
std::priority_queue<edge> pq;
disjoint_set_forest dsf;
int n, num_queries;
FILE *fin, *fout;

void add_edge(int u, int v, int cost) {
  static int ptr = 1;
  list[ptr] = { v, cost, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree_and_init_dist() {
  int m, k, u, v, c;

  fscanf(fin, "%d %d %d %d", &n, &m, &k, &num_queries);

  for (u = 1; u <= n; u++) {
    nd[u].dist = INFINITY;
  }

  while (k--) {
    fscanf(fin, "%d", &u);
    nd[u].dist = 0;
    pq.push({ u, 0 });
  }

  while (m--) {
    fscanf(fin, "%d %d %d", &u, &v, &c);
    add_edge(u, v, c);
    add_edge(v, u, c);
  }
}

void dijkstra() {
  int num_seen = 0;

  while (!pq.empty()) {
    auto [u, dist] = pq.top();
    pq.pop();

    if (dist == nd[u].dist) {
      ord[num_seen++] = u;
      for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
        int v = list[ptr].v;
        int cost = list[ptr].cost;
        if (nd[u].dist + cost < nd[v].dist) {
          nd[v].dist = nd[u].dist + cost;
          pq.push({v, nd[v].dist});
        }
      }
    }
  }
}

void reverse_connectivity() {
  dsf.init(n);

  for (int i = n - 1; i >= 0; i--) {
    int u = ord[i];

    for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
      int v = list[ptr].v;
      if (nd[v].dist >= nd[u].dist) {
        dsf.unite(u, v, nd[u].dist);
      }
    }
  }
}

void process_queries() {
  int u, v, time;

  while (num_queries--) {
    fscanf(fin, "%d %d %d", &u, &v, &time);

    int answer = (u == v)
      ? nd[u].dist   // O, nu! Un caz particular!
      : dsf.get_min_edge(u, v);

    fprintf(fout, "%d\n", std::max(answer - time, 0));
  }
}

int main() {
  fin = fopen("termite.in", "r");
  fout = fopen("termite.out", "w");

  read_tree_and_init_dist();
  dijkstra();
  reverse_connectivity();
  process_queries();

  fclose(fin);
  fclose(fout);

  return 0;
}
