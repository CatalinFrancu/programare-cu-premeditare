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

struct heap_elt {
  u16 u;
  int dist;
};

struct heap {
  heap_elt v[MAX_EDGES + 1];
  int size;

  bool is_empty() {
    return size == 0;
  }

  void insert(u16 u, int dist) {
    // Deplasează gaura în sus cît timp se poate.
    int pos = ++size;
    while ((pos > 1) && (v[pos / 2].dist > dist)) {
      v[pos] = v[pos / 2];
      pos /= 2;
    }

    // Umple gaura cu nodul u.
    v[pos] = { u, dist };
  }

  heap_elt remove_min() {
    heap_elt result = v[1];
    heap_elt save = v[size--];

    if (size) {
      // Deplasează gaura în jos cît timp se poate.
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

      // Umple gaura cu elementul save.
      v[x] = save;
    }

    return result;
  }
};

adj_list list[MAX_EDGES + 1];
node nd[MAX_NODES + 1];
heap h;
int n;

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
  h.insert(1, 0);

  while (!h.is_empty()) {
    heap_elt top = h.remove_min();
    int u = top.u;

    if (top.dist == nd[u].dist) {
      for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
        int v = list[ptr].v;
        int edge_dist = list[ptr].dist;
        if (nd[u].dist + edge_dist < nd[v].dist) {
          nd[v].dist = nd[u].dist + edge_dist;
          h.insert(v, nd[v].dist);
        }
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
