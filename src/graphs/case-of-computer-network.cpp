#include <stdio.h>
#include <vector>

const int MAX_NODES = 200'000;

struct node {
  // vecinii
  std::vector<int> adj;
  // celelalte capete ale mesajelor care încep / se termină aici
  std::vector<int> pairs;
  // adîncimea mea și cea mai joasă adîncime accesibilă din subarborele meu
  int depth, low;
  // surse și destinații deschise în subarborele meu
  int num_src, num_dest;
};

struct disjoint_set_forest {
  int parent[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
    for (int u = 1; u <= n; u++) {
      parent[u] = u;
    }
  }

  int find(int u) {
    return (parent[u] == u)
      ? u
      : (parent[u] = find(parent[u]));
  }

  void merge(int u, int v) {
    parent[find(v)] = find(u);
  }
};

node nd[MAX_NODES + 1];
disjoint_set_forest ds;
int n;
bool red_flag;

void read_data() {
  int num_edges, num_queries, u, v;
  scanf("%d %d %d", &n, &num_edges, &num_queries);

  while (num_edges--) {
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }

  for (int i = 1; i <= num_queries; i++) {
    scanf("%d %d", &u, &v);
    nd[u].num_src++;
    nd[v].num_dest++;
    nd[u].pairs.push_back(v);
    nd[v].pairs.push_back(u);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void endpoint_action(int u) {
  for (auto v: nd[u].pairs) {
    if (nd[v].depth) {
      int lca = ds.find(v);
      // Spune-i LCA-ului să închida o pereche (sursă, destinație).
      nd[lca].num_src--;
      nd[lca].num_dest--;
    }
  }
}

void descendant_action(int u, int parent) {
  // Propagă numerele de capete la rădăcina 2ECC.
  nd[parent].num_src += nd[u].num_src;
  nd[parent].num_dest += nd[u].num_dest;
  nd[u].num_src = 0;
  nd[u].num_dest = 0;
}

void bridge_action(int u, int parent) {
  if (nd[u].low > nd[parent].depth) {
    if (nd[u].num_src && nd[u].num_dest) {
      red_flag = true;
    }
  }
}

void dfs(int u, int parent) {
  nd[u].depth = nd[u].low = nd[parent].depth + 1;

  endpoint_action(u);

  int num_parent_edges = 0;
  for (auto v: nd[u].adj) {
    num_parent_edges += (v == parent);

    if (!nd[v].depth) {
      dfs(v, u);
      ds.merge(u, v);
      nd[u].low = min(nd[u].low, nd[v].low);
      descendant_action(v, u);
    } else if ((v != parent) || ((v == parent) && (num_parent_edges > 1))) {
      nd[u].low = min(nd[u].low, nd[v].depth);
    }
  }

  bridge_action(u, parent);
}

void dfs_driver() {
  for (int u = 1; u <= n; u++) {
    if (!nd[u].depth) {
      dfs(u, 0);
      // Asigură-te că niciun mesaj nu circulă între două componente.
      if (nd[u].num_src || nd[u].num_dest) {
        red_flag = true;
      }
    }
  }
}

void write_answer() {
  printf(red_flag ? "No\n" : "Yes\n");
}

int main() {
  read_data();
  ds.init(n);
  dfs_driver();
  write_answer();

  return 0;
}

// If it's worth doing, it's worth doing well.
