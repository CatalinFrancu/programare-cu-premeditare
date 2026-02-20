#include <queue>
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 400'000;
const int INFINITY = 1'001; // mai mare decît orice cost

struct edge {
  int v;
  short cost;

  // Vezi https://stackoverflow.com/a/3141107/6022817 pentru "const"
  bool operator<(edge other) const {
    return cost > other.cost;
  }
};

struct node {
  std::vector<edge> adj;
  int parent;
  short dist;
  bool visited;
};

node nd[MAX_NODES + 1];
int n, mst_cost;

void read_data() {
  int m;
  FILE* f = fopen("apm.in", "r");

  fscanf(f, "%d %d", &n, &m);
  while (m--) {
    int u, v;
    short cost;
    fscanf(f, "%d %d %hd", &u, &v, &cost);
    nd[u].adj.push_back({v, cost});
    nd[v].adj.push_back({u, cost});
  }

  fclose(f);
}

void prim() {
  // Priority queue cu reinserare. Un nod poate trece prin coadă de mai multe
  // ori, iar prin coadă pot trece în total m noduri.
  std::priority_queue<edge> q;
  for (int u = 2; u <= n; u++) {
    nd[u].dist = INFINITY;
  }
  q.push({1, 0});

  while (!q.empty()) {
    int u = q.top().v, cost = q.top().cost;
    q.pop();

    if (!nd[u].visited) {
      nd[u].visited = true;
      mst_cost += cost;
      for (edge e: nd[u].adj) {
        if (!nd[e.v].visited && (e.cost < nd[e.v].dist)) {
          nd[e.v].dist = e.cost;
          nd[e.v].parent = u;
          q.push({e.v, e.cost});
        }
      }
    }
  }
}

void write_mst() {
  FILE* f = fopen("apm.out", "w");

  fprintf(f, "%d\n", mst_cost);
  fprintf(f, "%d\n", n - 1);
  for (int u = 2; u <= n; u++) {
    fprintf(f, "%d %d\n", nd[u].parent, u);
  }

  fclose(f);
}

int main() {
  read_data();
  prim();
  write_mst();

  return 0;
}
