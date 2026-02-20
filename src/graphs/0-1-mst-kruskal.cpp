#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;

// O structură de mulțimi disjuncte augmentată cu trei informații:
// * numărul de mulțimi;
// * mărimea fiecărei mulțimi;
// * lista de rădăcini.
struct disjoint_set_forest {
  int p[MAX_NODES + 1];
  int s[MAX_NODES + 1];
  int root_pos[MAX_NODES + 1]; // root_pos[u] = poziția lui u în roots[]
  int roots[MAX_NODES + 1];
  int n, num_trees;

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
      s[u] = 1;
      root_pos[u] = u;
      roots[u] = u;
    }
    num_trees = n;
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  void unite(int u, int v) {
    u = find(u);
    v = find(v);
    if (u != v) {
      s[u] += s[v];
      p[v] = u;
      // Înlocuiește-l pe v cu ultimul element din roots și actualizează
      // root_pos.
      int repl = roots[num_trees - 1];
      roots[root_pos[v]] = repl;
      root_pos[repl] = root_pos[v];
      num_trees--;
    }
  }

  void unite_incoming(int u, int* incoming) {
    u = find(u);
    int i = 0;
    while (i < num_trees) {
      if ((roots[i] == u) || (incoming[roots[i]] == s[roots[i]])) {
        i++;
      } else {
        unite(u, roots[i]);
      }
    }
  }
};

std::vector<int> unadj[MAX_NODES + 1];
int incoming[MAX_NODES + 1];
disjoint_set_forest dsf;
int n;

void read_data() {
  int m, u, v;
  scanf("%d %d", &n, &m);
  while (m--) {
    scanf("%d %d", &u, &v);
    unadj[u].push_back(v);
    unadj[v].push_back(u);
  }
}

void process_node(int u) {
  // Numără muchiile 1 care intră în fiecare componentă.
  for (int v: unadj[u]) {
    incoming[dsf.find(v)]++;
  }

  dsf.unite_incoming(u, incoming);

  // Curățenie.
  for (int v: unadj[u]) {
    incoming[dsf.find(v)] = 0;
  }
}

void connectivity() {
  dsf.init(n);
  for (int u = 1; u <= n; u++) {
    process_node(u);
  }
}

int main() {
  read_data();
  connectivity();
  printf("%d\n", dsf.num_trees - 1);

  return 0;
}
