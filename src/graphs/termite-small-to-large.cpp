#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 300'000;
const int MAX_EDGES = 400'000;
const int MAX_QUERIES = 400'000;
const int INFINITY = 300'000'000; // costul maxim este 1'000

struct query {
  int a, b;
  int t;
  int disconnect_time;

  int get_delta_t() {
    return std::max(disconnect_time - t, 0);
  }
};

struct qlist {
  int id, next;
};

struct edge {
  int v, dist;

  friend bool operator< (const edge& a, const edge& b) {
    return a.dist > b.dist;
  }
};

struct elist {
  int v, cost, next;
};

struct node {
  int adj;
  int dist;
};

node nd[MAX_NODES + 1];
elist e[2 * MAX_EDGES + 1];
query q[MAX_QUERIES];
int ord[MAX_NODES]; // nodurile în ordinea dispariției
std::priority_queue<edge> pq;
int n, num_queries;

struct disjoint_set_forest {
  int p[MAX_NODES + 1];
  qlist list[2 * MAX_QUERIES + 1]; // spațiu pentru celulele din liste
  int queries[MAX_NODES + 1]; // lista de interogări a fiecărui nod
  int qsize[MAX_NODES + 1];   // mărimea listei de interogări a fiecărui nod

  void add_query(int id, int u) {
    static int ptr = 1;
    list[ptr] = { id, queries[u] };
    queries[u] = ptr++;
    qsize[u]++;
  }

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
    }

    for (int i = 0; i < num_queries; i++) {
      add_query(i, q[i].a);
      add_query(i, q[i].b);
    }
  }

  int find(int u) {
    return (p[u] == u) ? u : (p[u] = find(p[u]));
  }

  void move_queries(int u, int v, int time) {
    int ptr = queries[u];
    while (ptr) {
      int next = list[ptr].next; // Îl salvăm căci poate fi distrus mai jos.
      int id = list[ptr].id;
      if (q[id].disconnect_time == INFINITY) {
        // Calculează componenta celuilalt capăt al interogării.
        int compa = find(q[id].a), compb = find(q[id].b);
        int w = (u == compa) ? compb : compa;
        if (w == v) {
          // Acesta este momentul la care q[id] se (de)conectează.
          q[id].disconnect_time = time;
        } else {
          // Mută celula la lista lui v.
          list[ptr].next = queries[v];
          queries[v] = ptr;
          qsize[v]++;
        }
      }
      ptr = next;
    }
  }

  inline void unite(int u, int v, int time) {
    u = find(u);
    v = find(v);

    if (u != v) {
      if (qsize[u] > qsize[v]) {
        std::swap(u, v);
      }
      move_queries(u, v, time);
      p[u] = v;
    }
  }
};

disjoint_set_forest dsf;

void add_edge(int u, int v, int cost) {
  static int ptr = 1;
  e[ptr] = { v, cost, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_data_and_init_dist() {
  FILE* f = fopen("termite.in", "r");
  int m, k, u, v, c;

  fscanf(f, "%d %d %d %d", &n, &m, &k, &num_queries);

  for (u = 1; u <= n; u++) {
    nd[u].dist = INFINITY;
  }

  while (k--) {
    fscanf(f, "%d", &u);
    nd[u].dist = 0;
    pq.push({ u, 0 });
  }

  while (m--) {
    fscanf(f, "%d %d %d", &u, &v, &c);
    add_edge(u, v, c);
    add_edge(v, u, c);
  }

  for (int i = 0; i < num_queries; i++) {
    fscanf(f, "%d %d %d", &q[i].a, &q[i].b, &q[i].t);
    q[i].disconnect_time = INFINITY;
  }

  fclose(f);
}

void dijkstra() {
  int num_seen = 0;

  while (!pq.empty()) {
    auto [u, dist] = pq.top();
    pq.pop();

    if (dist == nd[u].dist) {
      ord[num_seen++] = u;
      for (int ptr = nd[u].adj; ptr; ptr = e[ptr].next) {
        int v = e[ptr].v, cost = e[ptr].cost;
        if (nd[u].dist + cost < nd[v].dist) {
          nd[v].dist = nd[u].dist + cost;
          pq.push({v, nd[v].dist});
        }
      }
    }
  }
}

void reverse_connectivity() {
  // Caz particular: interogări cu a == b.
  for (int i = 0; i < num_queries; i++) {
    if (q[i].a == q[i].b) {
      q[i].disconnect_time = nd[q[i].a].dist;
    }
  }

  dsf.init(n);

  for (int i = n - 1; i >= 0; i--) {
    int u = ord[i];

    for (int ptr = nd[u].adj; ptr; ptr = e[ptr].next) {
      int v = e[ptr].v;
      if (nd[v].dist >= nd[u].dist) {
        dsf.unite(u, v, nd[u].dist);
      }
    }
  }
}

void write_answers() {
  FILE* f = fopen("termite.out", "w");
  for (int i = 0; i < num_queries; i++) {
    fprintf(f, "%d\n", q[i].get_delta_t());
  }
  fclose(f);
}

int main() {
  read_data_and_init_dist();
  dijkstra();
  reverse_connectivity();
  write_answers();

  return 0;
}
