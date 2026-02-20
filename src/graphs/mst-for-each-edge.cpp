// Adaptată după https://codeforces.com/contest/609/submission/129056851
#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 200'000;

struct edge {
  int u, v;
  int weight;
  int orig;
};

struct disjoint_set_forest {
  int p[MAX_NODES + 1];
  int size[MAX_NODES + 1];
  // costul necesar pentru a părăsi componenta prin părinte
  int w[MAX_NODES + 1];

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
      size[u] = 1;
    }
  }

  // Returnează costul maxim pe calea u-v dacă u și v sînt conectate. Altfel
  // unifică u cu v și returnează 0.
  //
  // Trebuie să ne asigurăm că, pentru a părăsi subarborele lui v ca să
  // ajungem la restul arborelui, weight este cea mai scumpă muchie. Ne bazăm
  // pe faptul că procesăm muchiile în ordine crescătoare a costului.
  //
  // Totuși, asta înseamnă că nu putem folosi compresia căii. Dar folosim
  // unirea după mărime ca să ne asigurăm că înălțimea componentelor nu
  // depășește log n.
  int try_unite(int u, int v, int weight) {
    int maxw = 0;

    while ((u != v) && (p[u] != u || p[v] != v)) {
      if ((p[u] == u) || ((p[v] != v) && (size[v] < size[u]))) {
        int tmp = u; u = v; v = tmp;
      }
      maxw = (w[u] > maxw) ? w[u] : maxw;
      u = p[u];
    }

    if (u == v) {
      return maxw;
    } else {
      unite(u, v, weight);
      return 0;
    }
  }

  void unite(int u, int v, int weight) {
    if (size[u] < size[v]) {
      int tmp = u; u = v; v = tmp;
    }

    size[u] += size[v];
    p[v] = u;
    w[v] = weight;
  }
};

edge e[MAX_EDGES];
disjoint_set_forest dsf;
int diff[MAX_EDGES]; // răspunsurile, relative la APM-ul pe care îl vom afla
int n, m;
long long mst_weight;

void read_data() {
  scanf("%d %d", &n, &m);
  for (int i = 0; i < m; i++) {
    scanf("%d %d %d", &e[i].u, &e[i].v, &e[i].weight);
    e[i].orig = i;
  }
}

void sort_edges_by_weight() {
  std::sort(e, e + m, [] (edge& a, edge& b) {
    return a.weight < b.weight;
  });
}

void kruskal() {
  dsf.init(n);
  for (int i = 0; i < m; i++) {
    edge& z = e[i]; // syntactic sugar
    int maxw = dsf.try_unite(z.u, z.v, z.weight);
    if (maxw) {
      // Ca să o introducem pe z în APM, vom scoate o muchie de cost maxw.
      diff[z.orig] = z.weight - maxw;
    } else {
      mst_weight += z.weight;
      diff[z.orig] = 0;
    }
  }
}

void write_answers() {
  for (int i = 0; i < m; i++) {
    printf("%lld\n", mst_weight + diff[i]);
  }
}

int main() {
  read_data();
  sort_edges_by_weight();
  kruskal();
  write_answers();

  return 0;
}
