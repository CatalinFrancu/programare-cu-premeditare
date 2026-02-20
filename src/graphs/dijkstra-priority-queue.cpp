#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 50'000;
const int MAX_EDGES = 250'000;
const int INFINITY = 1'000'000'000;

typedef unsigned short u16;

struct edge {
  u16 v, dist;
};

struct node {
  std::vector<edge> adj;
  int dist;
};

struct pq_elt {
  u16 u;
  int dist;

  friend bool operator< (const pq_elt& a, const pq_elt& b) {
    return a.dist > b.dist;
  }
};

node nd[MAX_NODES + 1];
std::priority_queue<pq_elt> pq;
int n;

void read_graph() {
  int num_edges;

  FILE* f = fopen("dijkstra.in", "r");
  fscanf(f, "%d %d", &n, &num_edges);

  while (num_edges--) {
    u16 u, v, dist;
    fscanf(f, "%hd %hd %hd\n", &u, &v, &dist);
    nd[u].adj.push_back({ v, dist});
  }
  fclose(f);
}

void dijkstra() {
  for (int u = 1; u <= n; u++) {
    nd[u].dist = INFINITY;
  }

  nd[1].dist = 0;
  pq.push({1, 0});

  while (!pq.empty()) {
    pq_elt top = pq.top();
    pq.pop();
    int u = top.u;

    if (top.dist == nd[u].dist) {
      for (auto e: nd[u].adj) {
        u16 v = e.v;
        if (nd[u].dist + e.dist < nd[v].dist) {
          nd[v].dist = nd[u].dist + e.dist;
          pq.push({v, nd[v].dist});
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
