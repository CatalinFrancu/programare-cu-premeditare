// Complexitate: O((n + q) log n).
// Metodă: Descompunere în centroizi.
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;
const int MAX_LOG = 17;

struct node {
  std::vector<int> adj;
  int size;
  int level;
  int closest_red;
  int cparent; // părintele în arborele de centroizi
  int cdist[MAX_LOG]; // distanța pînă la părintele centroid de nivel k
  bool dead;
};

node nd[MAX_NODES + 1];
int freq[MAX_NODES];
int n, num_queries;

void read_input_data() {
  scanf("%d %d", &n, &num_queries);

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
}

void size_dfs(int u, int parent) {
  nd[u].size = 1;

  for (int v: nd[u].adj) {
    if ((v != parent) && !nd[v].dead) {
      size_dfs(v, u);
      nd[u].size += nd[v].size;
    }
  }
}

void dist_dfs(int u, int parent, int level) {
  for (int v: nd[u].adj) {
    if ((v != parent) && !nd[v].dead) {
      nd[v].cdist[level] = 1 + nd[u].cdist[level];
      dist_dfs(v, u, level);
    }
  }
}

int find_centroid(int u, int limit) {
  for (int v: nd[u].adj) {
    if ((nd[v].size < nd[u].size) && (nd[v].size > limit)) {
      return find_centroid(v, limit);
    }
  }

  return u;
}

void decompose(int u, int parent, int level) {
  size_dfs(u, 0);
  u = find_centroid(u, nd[u].size / 2);

  nd[u].dead = true;
  nd[u].level = level;
  nd[u].cparent = parent;

  dist_dfs(u, 0, level);

  for (int v: nd[u].adj) {
    if (!nd[v].dead) {
      decompose(v, u, level + 1);
    }
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void paint_red(int u) {
  int center = u;
  for (int l = nd[u].level; l >= 0; l--) {
    nd[center].closest_red = min(nd[center].closest_red, nd[u].cdist[l]);
    center = nd[center].cparent;
  }
}

int find_closest_red(int u) {
  int result = n; // infinit

  int center = u;
  for (int l = nd[u].level; l >= 0; l--) {
    int dist = nd[u].cdist[l] + nd[center].closest_red;
    result = min(result, dist);
    center = nd[center].cparent;
  }
  return result;
}

void init_distances() {
  for (int u = 1; u <= n; u++) {
    nd[u].closest_red = n; // infinit
  }
  paint_red(1);
}

void process_ops() {
  while (num_queries--) {
    int type, u;
    scanf("%d %d", &type, &u);
    if (type == 1) {
      paint_red(u);
    } else {
      printf("%d\n", find_closest_red(u));
    }
  }
}

int main() {
  read_input_data();
  decompose(1, 0, 0);
  init_distances();
  process_ops();

  return 0;
}
