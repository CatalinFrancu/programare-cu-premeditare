// Complexitate: O(mn + m log m).
#include <algorithm>
#include <set>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 200;
const int MAX_EDGES = 50'000;
const long long INFINITY = 3e18;

struct edge {
  int u, v, g, s;
};

struct mst_graph {
  struct neighbor {
    int v, s;
  };

  struct node {
    std::vector<neighbor> adj;
  };

  node nd[MAX_NODES + 1];
  std::multiset<int> silvers;
  int n, num_comps;

  void init(int _n) {
    n = num_comps = _n;
  }

  int get_max_s() {
    return *silvers.rbegin();
  }

  bool is_connected() {
    return num_comps == 1;
  }

  void remove_neighbor(int u, int v) {
    int i = 0;
    while (nd[u].adj[i].v != v) {
      i++;
    }
    nd[u].adj[i] = nd[u].adj.back();
    nd[u].adj.pop_back();
  }

  void connect(edge e) {
    nd[e.u].adj.push_back({e.v, e.s});
    nd[e.v].adj.push_back({e.u, e.s});
    silvers.insert(e.s);
    num_comps--;
  }

  void disconnect(edge e) {
    remove_neighbor(e.u, e.v);
    remove_neighbor(e.v, e.u);
    silvers.erase(silvers.find(e.s));
    num_comps++;
  }

  edge get_max_edge_dfs(int u, int parent, int target) {
    edge max = { .s = 0 };
    for (neighbor nb: nd[u].adj) {
      if (nb.v != parent) {
        if (nb.v == target) {
          return { .u = u, .v = nb.v, .s = nb.s};
        }
        edge sub = get_max_edge_dfs(nb.v, u, target);
        if (sub.s && (nb.s > sub.s)) {
          sub = { .u = u, .v = nb.v, .s = nb.s };
        }
        if (sub.s > max.s) {
          max = sub;
        }
      }
    }
    return max;
  }

  void include_edge(edge e) {
    edge max = get_max_edge_dfs(e.u, 0, e.v);
    if (!max.s) {
      // u și v nu au fost conectate încă
      connect(e);
    } else if (e.s < max.s) {
      // u și v sînt conectate, dar e este mai bună (mai ieftină în argint)
      disconnect(max);
      connect(e);
    }
  }
};

edge e[MAX_EDGES];
mst_graph mst;
int n, m;
long long g, s;

void read_data() {
  int m_with_self;
  scanf("%d %d %lld %lld", &n, &m_with_self, &g, &s);
  for (int i = 0; i < m_with_self; i++) {
    scanf("%d %d %d %d", &e[m].u, &e[m].v, &e[m].g, &e[m].s);
    if (e[m].u != e[m].v) {
      m++;
    }
  }
}

void sort_edges_by_gold() {
  std::sort(e, e + m, [](edge& a, edge& b) {
    return a.g < b.g;
  });
}

long long scan_g() {
  long long min_cost = INFINITY;
  mst.init(n);

  for (int i = 0; i < m; i++) {
    mst.include_edge(e[i]);
    if (mst.is_connected()) {
      min_cost = std::min(min_cost, g * e[i].g + s * mst.get_max_s());
    }
  }

  return min_cost;
}

int main() {
  read_data();
  sort_edges_by_gold();
  long long answer = scan_g();
  printf("%lld\n", (answer == INFINITY) ? -1 : answer);

  return 0;
}
