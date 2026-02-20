#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 100'000;
const long long INFINITY = 1LL << 62;
const int MOD = 1'000'003'751;

const int FORWARD = 0;
const int REVERSE = 1;

struct cell {
  int v;
  int cost;
  int next;
};

struct node {
  int adj[2];
  long long dist[2];

  // Numărul de moduri de a ajunge la acest nod mergînd înainte/înapoi.
  // Matematic, reprezentarea este incorectă. Numărul de moduri poate ușor să
  // depășească 64 de biți. De exemplu, formăm triunghiuri cu fiecare trei
  // noduri și le înlănțuim. Atunci numărul de moduri de a ajunge la
  // destinație este 2^(n/3). Operăm modulo un număr prim mare și ne rugăm la
  // Zeus.
  int ways[2];
};

struct heap_elt {
  int u;
  long long dist;
};

struct heap {
  heap_elt v[MAX_EDGES + 1];
  int size;

  bool is_empty() {
    return size == 0;
  }

  void insert(int u, long long dist) {
    int pos = ++size;
    while ((pos > 1) && (v[pos / 2].dist > dist)) {
      v[pos] = v[pos / 2];
      pos /= 2;
    }

    v[pos] = { u, dist };
  }

  heap_elt remove_min() {
    heap_elt result = v[1];
    heap_elt save = v[size--];

    if (size) {
      bool swapped = true;
      int x = 1;
      while ((2 * x <= size) && swapped) {
        int l = 2 * x, r = 2 * x + 1;
        int y = (r <= size) && (v[r].dist < v[l].dist)
          ? r : l;

        if (v[y].dist < save.dist) {
          v[x] = v[y];
          x = y;
        } else {
          swapped = false;
        }
      }

      v[x] = save;
    }

    return result;
  }
};

cell list[2 * MAX_EDGES + 1];
node nd[MAX_NODES + 1];
heap h;
int n, num_edges, source, sink;

void add_edge(int u, int v, int dist) {
  static int ptr = 0;

  list[++ptr] = { v, dist, nd[u].adj[FORWARD] };
  nd[u].adj[FORWARD] = ptr;

  list[++ptr] = { u, dist, nd[v].adj[REVERSE] };
  nd[v].adj[REVERSE] = ptr;

  // Notă: a k-a muchie ocupă pozițiile 2k-1 și 2k în list[].
}

void read_graph() {
  scanf("%d %d %d %d", &n, &num_edges, &source, &sink);

  for (int i = 0; i < num_edges; i++) {
    int u, v, dist;
    scanf("%d %d %d", &u, &v, &dist);
    add_edge(u, v, dist);
  }
}

void dijkstra(int source, int dir) {
  for (int u = 1; u <= n; u++) {
    nd[u].dist[dir] = INFINITY;
    nd[u].ways[dir] = 0;
  }

  nd[source].dist[dir] = 0;
  nd[source].ways[dir] = 1;
  h.insert(source, 0);

  while (!h.is_empty()) {
    heap_elt top = h.remove_min();
    int u = top.u;

    // Nu face efortul dacă distanța lui u s-a modificat între timp.
    if (top.dist == nd[u].dist[dir]) {
      for (int ptr = nd[u].adj[dir]; ptr; ptr = list[ptr].next) {
        int v = list[ptr].v;
        long long old_dist = nd[v].dist[dir];
        long long new_dist = top.dist + list[ptr].cost;

        if (new_dist < old_dist) {
          nd[v].dist[dir] = new_dist;
          nd[v].ways[dir] = nd[u].ways[dir];
          h.insert(v, new_dist);
        } else if (new_dist == old_dist) {
          nd[v].ways[dir] = (nd[v].ways[dir] + nd[u].ways[dir]) % MOD;
        }
      }
    }
  }
}

void compute_answers() {
  long long min_dist = nd[sink].dist[FORWARD];
  int min_ways = nd[sink].ways[FORWARD];

  for (int i = 1; i <= num_edges; i++) {
    int u = list[2 * i].v;
    int v = list[2 * i - 1].v;
    int cost = list[2 * i].cost;

    long long dist_me = nd[u].dist[FORWARD] + cost + nd[v].dist[REVERSE];
    int ways_me = (long long)nd[u].ways[FORWARD] * nd[v].ways[REVERSE] % MOD;

    bool is_bridge = (dist_me == min_dist) && (ways_me == min_ways);

    if (is_bridge) {
      printf("YES\n");
    } else {
      long long guarantee = min_dist - nd[u].dist[FORWARD] - nd[v].dist[REVERSE] - 1;
      if (guarantee >= 1) {
        printf("CAN %lld\n", cost - guarantee);
      } else {
        printf("NO\n");
      }
    }
  }
}

int main() {
  read_graph();
  dijkstra(source, FORWARD);
  dijkstra(sink, REVERSE);
  compute_answers();

  return 0;
}
