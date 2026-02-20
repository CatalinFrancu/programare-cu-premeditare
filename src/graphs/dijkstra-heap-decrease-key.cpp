#include <stdio.h>

const int MAX_NODES = 50'000;
const int MAX_EDGES = 250'000;
const int INFINITY = 1'000'000'000;

typedef unsigned short u16;

struct adj_list {
  unsigned short v, dist;
  int next;
};

struct node {
  int adj;
  int dist;
};

adj_list list[MAX_EDGES + 1];
node nd[MAX_NODES + 1];
int n;

struct heap {
  u16 v[MAX_NODES + 1];
  u16 pos[MAX_NODES + 1]; // pos[u] = 0  <==>  nodul u lipsește din heap
  int size;

  bool is_empty() {
    return size == 0;
  }

  void decrease_key(int u) {
    if (!pos[u]) {
      v[++size] = u;
      pos[u] = size;
    }

    // Deplasează gaura în sus cît timp se poate.
    int x = pos[u];
    while ((x > 1) && (nd[v[x / 2]].dist > nd[u].dist)) {
      v[x] = v[x / 2];
      pos[v[x]] = x;
      x /= 2;
    }

    // Umple gaura cu nodul u.
    v[x] = u;
    pos[u] = x;
  }

  u16 remove_min() {
    int result = v[1];
    pos[result] = 0;

    int u = v[size--];

    if (size) {
      // Deplasează gaura în jos cît timp se poate.
      bool swapped = true;
      int x = 1;
      while ((2 * x <= size) && swapped) {
        int l = 2 * x, r = 2 * x + 1;
        int y = (r <= size) && (nd[v[r]].dist < nd[v[l]].dist)
          ? r : l;

        if (nd[v[y]].dist < nd[u].dist) {
          v[x] = v[y];
          pos[v[x]] = x;
          x = y;
        } else {
          swapped = false;
        }
      }

      // Umple gaura cu nodul u.
      v[x] = u;
      pos[u] = x;
    }

    return result;
  }

};

heap h;

void add_edge(u16 u, u16 v, u16 dist) {
  static int ptr = 0;

  list[++ptr] = { v, dist, nd[u].adj };
  nd[u].adj = ptr;
}

void read_graph() {
  int num_edges;

  FILE* f = fopen("dijkstra.in", "r");
  fscanf(f, "%d %d", &n, &num_edges);

  while (num_edges--) {
    int u, v, dist;
    fscanf(f, "%d %d %d\n", &u, &v, &dist);
    add_edge(u, v, dist);
  }
  fclose(f);
}

void dijkstra() {
  for (int u = 1; u <= n; u++) {
    nd[u].dist = INFINITY;
  }

  nd[1].dist = 0;
  h.decrease_key(1);

  while (!h.is_empty()) {
    int u = h.remove_min();
    for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
      int v = list[ptr].v;
      int edge_dist = list[ptr].dist;
      if (nd[u].dist + edge_dist < nd[v].dist) {
        nd[v].dist = nd[u].dist + edge_dist;
        h.decrease_key(v);
      }
    }
  }
}

void write_distances() {
  FILE* f = fopen("dijkstra.out", "w");
  for (int u = 2; u <= n; u++) {
    if (nd[u].dist == INFINITY) {
      nd[u].dist = 0;
    }
    fprintf(f, "%d ", nd[u].dist);
  }
  fprintf(f, "\n");
  fclose(f);
}

int main() {
  read_graph();
  dijkstra();
  write_distances();

  return 0;
}
