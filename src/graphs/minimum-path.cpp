#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 200'000;
const long long INFINITY = 1'000'000'000'000'000'000LL;

const int NEITHER = 0;
const int REMOVED = 1;
const int ADDED = 2;
const int BOTH = 3;
const int NUM_STATES = 4;

const int IMPOSSIBLE = -1;

const int FACTOR[NUM_STATES][NUM_STATES] = {
  {          1,          0,          2, 1 },
  { IMPOSSIBLE,          1, IMPOSSIBLE, 2 },
  { IMPOSSIBLE, IMPOSSIBLE,          1, 0 },
  { IMPOSSIBLE, IMPOSSIBLE, IMPOSSIBLE, 1 },
};

struct adj_list {
  int v, dist;
  int next;
};

struct node {
  long long dist[NUM_STATES];
  int adj;
};

struct heap_elt {
  long long dist;
  int u;
  unsigned char state;
};

struct heap {
  heap_elt v[9 * MAX_EDGES + 1]; // 9 combinații posibile în FACTOR
  int size;

  bool is_empty() {
    return size == 0;
  }

  void insert(int u, unsigned char state, long long dist) {
    int pos = ++size;
    while ((pos > 1) && (v[pos / 2].dist > dist)) {
      v[pos] = v[pos / 2];
      pos /= 2;
    }

    v[pos] = { dist, u, state };
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

adj_list list[2 * MAX_EDGES + 1];
node nd[MAX_NODES + 1];
heap h;
int n;

void add_edge(int u, int v, int dist) {
  static int ptr = 0;

  list[++ptr] = { v, dist, nd[u].adj };
  nd[u].adj = ptr;
}

void read_graph() {
  int num_edges;

  scanf("%d %d", &n, &num_edges);

  while (num_edges--) {
    int u, v, dist;
    scanf("%d %d %d", &u, &v, &dist);
    add_edge(u, v, dist);
    add_edge(v, u, dist);
  }
}

void relax_all(int u, int su) {
  for (int sv = 0; sv < NUM_STATES; sv++) {
    int factor = FACTOR[su][sv];
    if (factor != IMPOSSIBLE) {
      for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
        int v = list[ptr].v;
        long long new_dist = nd[u].dist[su] + factor * list[ptr].dist;
        if (new_dist < nd[v].dist[sv]) {
          nd[v].dist[sv] = new_dist;
          h.insert(v, sv, new_dist);
        }
      }
    }
  }
}

void dijkstra() {
  for (int u = 1; u <= n; u++) {
    for (int s = 0; s < NUM_STATES; s++) {
      nd[u].dist[s] = INFINITY;
    }
  }

  nd[1].dist[NEITHER] = 0;
  h.insert(1, NEITHER, 0);

  while (!h.is_empty()) {
    heap_elt top = h.remove_min();
    int u = top.u;
    int su = top.state;

    if (top.dist == nd[u].dist[su]) {
      relax_all(u, su);
    }
  }
}

void write_distances() {
  for (int u = 2; u <= n; u++) {
    printf("%lld ", nd[u].dist[BOTH]);
  }
  printf("\n");
}

int main() {
  read_graph();
  dijkstra();
  write_distances();

  return 0;
}
