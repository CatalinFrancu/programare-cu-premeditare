// Metodă: Caută binar greutatea maximă necesară și rulează algoritmul
// Hopcroft-Karp pentru fiecare candidat.
#include <algorithm>
#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 10'000;
const int MAX_EDGES = 100'000;
const int INF_DIST = 32'767;
const int INF_WEIGHT = 1'000'000'001;

struct edge {
  short v;
  int weight;
};

struct left_node {
  std::vector<edge> adj;
  short dist;
  short pair;
};

struct right_node {
  short pair;
};

int weights[MAX_EDGES + 1];
left_node l[MAX_NODES + 1];
right_node r[MAX_NODES + 1];
int num_nodes, num_edges;
int max_weight;

void read_data() {
  short u, v;
  int weight;

  scanf("%d %d", &num_nodes, &num_edges);
  for (int i = 0; i < num_edges; i++) {
    scanf("%hd %hd  %d", &u, &v, &weight);
    l[u].adj.push_back({v, weight});
    weights[i] = weight;
  }
}

void sort_weights() {
  std::sort(weights, weights + num_edges);
  for (int u = 1; u <= num_nodes; u++) {
    std::sort(l[u].adj.begin(), l[u].adj.end(), [](edge a, edge b) {
      return a.weight < b.weight;
    });
    l[u].adj.push_back({0, INF_WEIGHT}); // santinelă
  }
}

std::queue<short> put_unmatched_in_queue() {
  std::queue<short> q;
  for (int u = 1; u <= num_nodes; u++) {
    if (l[u].pair) {
      l[u].dist = INF_DIST;
    } else {
      l[u].dist = 0;
      q.push(u);
    }
  }
  return q;
}

bool bfs() {
  std::queue<short> q = put_unmatched_in_queue();
  bool reachable = false;

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    for (int i = 0; l[u].adj[i].weight <= max_weight; i++) {
      int v = l[u].adj[i].v;
      int p = r[v].pair;
      if (!p) {
        reachable = true;
      } else if (l[p].dist == INF_DIST) {
        l[p].dist = l[u].dist + 1;
        q.push(p);
      }
    }
  }

  return reachable;
}

bool dfs(int u) {
  for (int i = 0; l[u].adj[i].weight <= max_weight; i++) {
    int v = l[u].adj[i].v;
    int p = r[v].pair;
    if (!p || ((l[p].dist == l[u].dist + 1) && dfs(p))) {
      l[u].pair = v;
      r[v].pair = u;
      return true;
    }
  }

  l[u].dist = INF_DIST;
  return false;
}

bool hopcroft_karp() {
  for (int u = 1; u <= num_nodes; u++) {
    l[u].pair = r[u].pair = 0;
  }

  int size = 0;
  while (bfs()) {
    for (int u = 1; u <= num_nodes; u++) {
      if (!l[u].pair && dfs(u)) {
        size++;
      }
    }
  }
  return (size == num_nodes);
}

int bin_search_max_weight() {
  // lo = Număr de muchii cunoscut ca fiind imposibil.
  // hi = Număr de muchii cunoscut a fi posibil, cu o mențiune. Pornim cu hi =
  // num_edges, care înseamnă „chiar dacă folosim toate muchiile, tot nu găsim
  // un cuplaj maxim”. Subliniem că nu accesăm niciodată weights[num_edges].
  int lo = num_nodes - 2, hi = num_edges;
  while (hi - lo > 1) {
    int mid = (lo + hi) / 2;
    max_weight = weights[mid];
    if (hopcroft_karp()) {
      hi = mid;
    } else {
      lo = mid;
    }
  }

  return (hi == num_edges) ? -1 : weights[hi];
}

int main() {
  read_data();
  sort_weights();
  int result = bin_search_max_weight();
  printf("%d\n", result);

  return 0;
}
