// Complexitate: O(mn log* m + m log m).
#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 200;
const int MAX_EDGES = 50'000;
const long long INFINITY = 3'000'000'000'000'000'000LL;

struct edge {
  int u, v, g, s;
};

struct disjoint_set_forest {
  int p[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
    for (int u = 1; u <= n; u++) {
      p[u] = u;
    }
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  bool are_separate(edge e) {
    return find(e.u) != find(e.v);
  }

  void unite(edge e) {
    p[find(e.v)] = find(e.u);
  }
};

edge e[MAX_EDGES];
edge e_by_s[MAX_EDGES];
disjoint_set_forest dsf;
int n, m, m_by_s;
long long g, s;

void read_data() {
  scanf("%d %d %lld %lld", &n, &m, &g, &s);
  for (int i = 0; i < m; i++) {
    scanf("%d %d %d %d", &e[i].u, &e[i].v, &e[i].g, &e[i].s);
  }
}

void sort_edges_by_gold() {
  std::sort(e, e + m, [](edge& a, edge& b) {
    return a.g < b.g;
  });
}

void add_admissible_edge(edge e) {
  int i = m_by_s++;
  while ((i > 0) && (e_by_s[i - 1].s > e.s)) {
    e_by_s[i] = e_by_s[i - 1];
    i--;
  }
  e_by_s[i] = e;
}

void kruskal() {
  dsf.init(n);
  int new_m_by_s = 0;

  for (int i = 0; i < m_by_s; i++) {
    if (dsf.are_separate(e_by_s[i])) {
      dsf.unite(e_by_s[i]);
      e_by_s[new_m_by_s++] = e_by_s[i];
    }
  }

  m_by_s = new_m_by_s;
}

long long try_all_g() {
  long long min_cost = INFINITY;

  int i = 0;
  while (i < m) {
    int j = i;
    while ((j < m) && (e[j].g == e[i].g)) {
      add_admissible_edge(e[j++]);
    }

    kruskal();
    if (m_by_s == n - 1) {
      min_cost = std::min(min_cost, g * e[i].g + s * e_by_s[m_by_s - 1].s);
    }

    i = j;
  }

  return min_cost;
}

int main() {
  read_data();
  sort_edges_by_gold();
  long long answer = try_all_g();
  printf("%lld\n", (answer == INFINITY) ? -1 : answer);

  return 0;
}
