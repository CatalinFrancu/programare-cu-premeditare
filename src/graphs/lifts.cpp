#include <algorithm>
#include <queue>
#include <stdio.h>

const int MAX_REQUESTS = 10'000;
const int MAX_NODES = 120'000;
const int MAX_EDGES = 800'000;
const long long INF = 1e9;
const long long INFL = 1e18;

struct request {
  int src, dest;
};

struct edge {
  int v, next;
  int cap;
  long long cost;
};

struct node {
  int adj;
  int parent, edge;  // părintele și muchia incidentă în Dijkstra
  long long d;       // distanța de la sursă
};

request req[MAX_REQUESTS];
int ord[MAX_REQUESTS]; // pentru sortări
node nd[MAX_NODES];
edge e[MAX_EDGES + 2];
int n, num_req, num_elev, source, sink;
long long sink_potential;

void read_data() {
  scanf("%d %d", &num_req, &num_elev);
  for (int i = 0; i < num_req; i++) {
    scanf("%d %d", &req[i].src, &req[i].dest);
  }
}

void add_edge(int u, int v, int cap, int cost) {
  static int pos = 2;

  e[pos] = { v, nd[u].adj, cap, cost };
  nd[u].adj = pos++;
}

void add_edge_pair(int u, int v, int cap, int cost) {
  add_edge(u, v, cap, cost);
  add_edge(v, u, 0, -cost);
}

void sort_ranges(int lo, int mid, int hi) {
  std::sort(ord + lo, ord + mid, [](int a, int b) {
    return (req[a].dest < req[b].dest);
  });
  std::sort(ord + mid, ord + hi, [](int a, int b) {
    return (req[a].src < req[b].src);
  });
}

// Creează noduri noi și leagă-le 1:1 cu intervalul [lo, hi). Înlănțuie-le
// crescător sau descrescător conform cu sort_asc.
int add_nodes(int lo, int hi, bool sort_asc) {
  int n0 = n;
  n += hi - lo;

  for (int i = 0; i < hi - lo; i++) {
    add_edge_pair(n0 + i, ord[lo + i], 1, 0);
  }
  for (int i = 0; i < hi - lo - 1; i++) {
    int from = n0 + i + !sort_asc;
    int to = n0 + i + sort_asc;
    int cost = req[ord[lo + i + 1]].src - req[ord[lo + i]].src;
    add_edge_pair(from, to, num_elev, cost);
  }

  return n0;
}

// Creează muchii de la toate nodurile din [lo, mid) la toate nodurile din
// [mid, hi). Pentru aceasta, interclasează listele ordonate de valori src și
// dest și folosește listele sortate de noduri care încep de la n_asc și
// n_desc.
void add_back_edges(int lo, int mid, int hi, int n_asc, int n_desc) {
  int j = mid;
  for (int i = lo; i < mid; i++) {
    while ((j < hi) && (req[ord[j]].src <= req[ord[i]].dest)) {
      j++;
      n_asc++;
      n_desc++;
    }
    if (j < hi) {
      int cost = req[ord[j]].src - req[ord[i]].dest;
      add_edge_pair(num_req + ord[i], n_asc, 1, cost);
    }
    if (j > mid) {
      int cost = req[ord[i]].dest - req[ord[j - 1]].src;
      add_edge_pair(num_req + ord[i], n_desc - 1, 1, cost);
    }
  }
}

// Grupează comenzile în perechi de intervale de lungime l. Pentru fiecare
// pereche, leagă toate comenzile din primul interval de toate comenzile din
// al doilea. Folosește 2·l noduri suplimentare pentru a crea două liste care
// parcurg comenzile crescător, respectiv descrescător după src.
void add_layer(int l) {
  for (int mid = l; mid < num_req; mid += 2 * l) {
    int lo = mid - l, hi = std::min(mid + l, num_req);
    if (l <= 12) {
      // Avem nevoie de C(l, 2) muchii, cea ce este mai ieftin decît să creăm
      // 2l noduri și 6l muchii.
      for (int i = lo; i < mid; i++) {
        for (int j = mid; j < hi; j++) {
          add_edge_pair(num_req + i, j, 1, std::abs(req[i].dest - req[j].src));
        }
      }
    } else {
      sort_ranges(lo, mid, hi);
      int n_asc = add_nodes(mid, hi, true);
      int n_desc = add_nodes(mid, hi, false);
      add_back_edges(lo, mid, hi, n_asc, n_desc);
    }
  }
}

// [0, num_req):            nodurile din stînga.
// [num_req, 2 num_req):    nodurile din dreapta.
// 2 num_req:               sursa
// 2 num_req + 1:           destinația
// apoi alte noduri alocate pentru structura pe niveluri
void build_graph() {
  source = 2 * num_req;
  sink = 2 * num_req + 1;
  n = 2 * num_req + 2;
  for (int i = 0; i < num_req; i++) {
    add_edge_pair(source, i, 1, 0);
    add_edge_pair(i, i + num_req, 1, -INF);
    add_edge_pair(i + num_req, sink, 1, 0);
  }

  for (int i = 0; i < num_req; i++) {
    ord[i] = i;
  }
  for (int l = 1; l < num_req; l <<= 1) {
    add_layer(l);
  }
}

// Sortarea topologică este suficientă (și, cred, necesară) pentru
// inițializarea potențialelor. Nu este nevoie de Bellman-Ford, întrucît
// graful este un dag.
void top_sort() {
  // Calculează gradele. Folosim cîmpul edge ca să nu declarăm un cîmp nou.
  for (int u = 0; u < n; u++) {
    nd[u].d = INFL;
    for (int pos = nd[u].adj; pos; pos = e[pos].next) {
      if (e[pos].cap) {
        nd[e[pos].v].edge++;
      }
    }
  }

  std::queue<int> q;
  for (int u = 0; u < n; u++) {
    if (!nd[u].edge) {
      nd[u].d = 0;
      q.push(u);
    }
  }

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    for (int pos = nd[u].adj; pos; pos = e[pos].next) {
      if (e[pos].cap) {
        int v = e[pos].v;
        nd[v].d = std::min(nd[v].d, nd[u].d + e[pos].cost);
        if (!--nd[v].edge) {
          q.push(v);
        }
      }
    }
  }
}

void reweight_edges() {
  for (int u = 0; u < n; u++) {
    for (int pos = nd[u].adj; pos; pos = e[pos].next) {
      int v = e[pos].v;
      e[pos].cost += nd[u].d - nd[v].d;
    }
  }

  sink_potential += nd[sink].d;
}

void dijkstra() {
  std::priority_queue<std::pair<long long, int>> pq;

  for (int u = 0; u < n; u++) {
    nd[u].d = INFL;
  }

  nd[source].d = 0;
  pq.push({0, source});

  while (!pq.empty()) {
    auto [dist, u] = pq.top();
    pq.pop();

    if (-dist == nd[u].d) { // pq este un max-heap
      for (int pos = nd[u].adj; pos; pos = e[pos].next) {
        int v = e[pos].v;
        if (e[pos].cap && (nd[u].d + e[pos].cost < nd[v].d)) {
          nd[v].d = nd[u].d + e[pos].cost;
          nd[v].parent = u;
          nd[v].edge = pos;
          pq.push({-nd[v].d, v});
        }
      }
    }
  }
}

void augment_path() {
  for (int u = sink; u != source; u = nd[u].parent) {
    e[nd[u].edge].cap--;
    e[nd[u].edge ^ 1].cap++;
  }
}

long long min_cost_max_flow() {
  long long total_cost = num_req * INF;

  top_sort();
  reweight_edges();

  while (num_elev--) {
    dijkstra();
    reweight_edges();
    augment_path();
    total_cost += sink_potential;
  }

  return total_cost;
}

int main() {
  read_data();
  build_graph();
  long long total_cost = min_cost_max_flow();
  printf("%lld\n", total_cost);

  return 0;
}
