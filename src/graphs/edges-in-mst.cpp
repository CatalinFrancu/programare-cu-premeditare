#include <algorithm>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 100'000;

const int T_SOME = 0; // răspunsul implicit, dacă nu este suprascris
const int T_NONE = 1;
const int T_ALL = 2;

struct edge {
  int u, v;
  int cost;
  int orig;
};

struct disjoint_set_forest {
  int p[MAX_NODES + 1];

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
    }
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  void unite(int u, int v) {
    p[find(v)] = find(u);
  }

  bool are_united(int u, int v) {
    return find(u) == find(v);
  }
};

struct reduced_edge {
  int v, orig; // O muchie u-v care are indexul „orig” în datele de intrare.
};

struct node {
  std::vector<reduced_edge> adj;
  int time, low;
};

edge e[MAX_EDGES];
disjoint_set_forest dsf;
node nd[MAX_NODES + 1];
int roots[2 * MAX_NODES], num_roots; // Rădăcini pentru grupa Kruskal curentă.
int answer[MAX_EDGES];
int n, m;

void read_data() {
  scanf("%d %d", &n, &m);
  for (int i = 0; i < m; i++) {
    scanf("%d %d %d", &e[i].u, &e[i].v, &e[i].cost);
    e[i].orig = i;
  }
}

void sort_edges() {
  std::sort(e, e + m, [](edge& a, edge& b) {
    return a.cost < b.cost;
  });
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int time;

void bridge_dfs(int u, int orig) {
  nd[u].time = nd[u].low = ++time;

  for (reduced_edge r: nd[u].adj) {
    if (r.orig != orig) {
      int v = r.v;
      if (!nd[v].time) {
        bridge_dfs(v, r.orig);
        nd[u].low = min(nd[u].low, nd[v].low);
        if (nd[v].low > nd[u].time) {
          answer[r.orig] = T_ALL;
        }

      } else {
        if (nd[v].time < nd[u].time) {
          nd[u].low = min(nd[u].low, nd[v].time);
        }
      }
    }
  }
}

void bridge_dfs_driver() {
  time = 0;
  for (int i = 0; i < num_roots; i++) {
    if (!nd[roots[i]].time) {
      bridge_dfs(roots[i], -1);
    }
  }
}

void dfs_cleanup() {
  for (int i = 0; i < num_roots; i++) {
    int u = roots[i];
    nd[u].adj.clear();
    nd[u].time = nd[u].low = 0;
  }
  num_roots = 0;
}

// Este important ca acest bloc să ruleze în O(batch_size). Dacă buclele scapă
// în O(n), vor duce la O(n²) global. De aceea, colectează capetele muchiilor
// și rulează DFS doar din acele noduri.
void process_batch(int start, int end) {
  num_roots = 0;

  for (int i = start; i < end; i++) {
    int u = dsf.find(e[i].u), v = dsf.find(e[i].v);
    if (u == v) {
      answer[e[i].orig] = T_NONE;
    } else {
      nd[u].adj.push_back({v, e[i].orig});
      nd[v].adj.push_back({u, e[i].orig});
      roots[num_roots++] = u;
      roots[num_roots++] = v;
    }
  }

  bridge_dfs_driver();
  dfs_cleanup();

  // Acum aplică modificările.
  for (int i = start; i < end; i++) {
    dsf.unite(e[i].u, e[i].v);
  }
}

void kruskal() {
  sort_edges();
  dsf.init(n);

  int i = 0;
  while (i < m) {
    int j = i + 1;
    while ((j < m) && (e[j].cost == e[i].cost)) {
      j++;
    }
    process_batch(i, j);
    i = j;
  }
}

void write_answers() {
  for (int i = 0; i < m; i++) {
    switch (answer[i]) {
      case T_SOME: printf("at least one\n"); break;
      case T_NONE: printf("none\n"); break;
      case T_ALL:  printf("any\n"); break;
    }
  }
}

int main() {
  read_data();
  kruskal();
  write_answers();

  return 0;
}
