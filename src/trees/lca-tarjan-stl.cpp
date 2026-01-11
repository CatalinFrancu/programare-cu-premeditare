#include <stdio.h>
#include <vector>

const int MAX_NODES = 500'000;
const int MAX_QUERIES = 500'000;

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

  // Întotdeauna îl leagă pe v de u. Fără *union by rank*.
  void unite(int u, int v) {
    p[find(v)] = find(u);
  }
};

struct node {
  std::vector<int> adj, queries;
  bool visited;
};

struct query {
  int u, v, lca;
};

node nd[MAX_NODES + 1];
query q[MAX_QUERIES];
disjoint_set_forest  dsf;
int n, num_queries;

void read_input_data() {
  scanf("%d", &n);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }

  scanf("%d", &num_queries);
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d", &q[i].u, &q[i].v);
    nd[q[i].u].queries.push_back(i);
    nd[q[i].v].queries.push_back(i);
  }
}

void offline_lca(int u) {
  nd[u].visited = true;

  for (int v: nd[u].adj) {
    if (!nd[v].visited) {
      offline_lca(v);
      dsf.unite(u, v);
    }
  }

  for (int i: nd[u].queries) {
    int v = (q[i].u == u) ? q[i].v : q[i].u;
    if (nd[v].visited) {
      q[i].lca = dsf.find(v);
    }
  }
}

void answer_queries() {
  for (int i = 0; i < num_queries; i++) {
    printf("%d\n", q[i].lca);
  }
}

int main() {
  read_input_data();
  dsf.init(n);
  offline_lca(1);
  answer_queries();

  return 0;
}
