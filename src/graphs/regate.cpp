#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 200'000;

struct edge {
  int v, cost, next;
};

struct bridge {
  int u, v, cost;
};

struct node {
  int adj;
  int comp; // componenta biconexă din care face parte
  int d;
  int low;
  long long cost; // inițial costul, apoi îl înlocuim cu răspunsul pentru nod
};

node nd[MAX_NODES + 1];
edge e[2 * MAX_EDGES + 1];
bridge b[MAX_NODES];
int st[MAX_NODES + 1], ss; // stiva pentru algoritmul lui Tarjan
int ord[MAX_NODES];
int n, comps;

// Invarianți:
// * size[u] este relevant doar pentru rădăcinile din DSF;
// * costul unei valori este suma din cost[] de la valoare la rădăcină.
struct disjoint_set_forest {
  long long cost[MAX_NODES];
  int p[MAX_NODES];
  int size[MAX_NODES];

  void init(int n) {
    for (int i = 0; i < n; i++) {
      p[i] = i;
    }
  }

  int find(int u) {
    // Compresia căii cu menținerea invariantului de cost.
    if (p[u] == u) {
      return u;
    } else {
      int r = find(p[u]);
      if (p[u] != r) {
        cost[u] += cost[p[u]];
        p[u] = r;
      }
      return r;
    }
  }

  int get_size(int u) {
    return size[find(u)];
  }

  long long get_cost(int u) {
    int r = find(u);
    // Acum fie r = u, fie r = p[u].
    return (r == u) ? cost[u] : (cost[u] + cost[r]);
  }

  inline void unite(int u, int v, long long bridge_cost) {
    u = find(u);
    v = find(v);
    cost[u] += bridge_cost * size[v];
    cost[v] += bridge_cost * size[u] - cost[u];
    p[v] = u;
    size[u] += size[v];
  }
};

disjoint_set_forest ds;

int min(int x, int y) {
  return (x < y) ? x : y;
}

// Scoate o componentă din stivă pînă la nodul root.
void pop_component(int root) {
  do {
    nd[st[--ss]].comp = comps;
    ds.size[comps]++;
  } while (st[ss] != root);
  comps++;
}

void find_bridges(int u, int depth) {
  nd[u].d = nd[u].low = depth;
  st[ss++] = u;

  for (int pos = nd[u].adj; pos; pos = e[pos].next) {
    int v = e[pos].v;
    if (!nd[v].d) {
      // Muchie înainte.
      find_bridges(v, depth + 1);
      nd[u].low = min(nd[u].low, nd[v].low);

      if (nd[v].low > nd[u].d) {
        // La v începe o nouă componentă. Notează componenta și puntea.
        b[comps] = { u, v, e[pos].cost };
        pop_component(v);
      }
    } else if (nd[v].d < nd[u].d - 1) {
      // Muchie înapoi.
      nd[u].low = min(nd[u].low, nd[v].d);
    }
  }
}

void add_edge(int u, int v, int cost) {
  static int ptr = 1;
  e[ptr] = { v, cost, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_data() {
  FILE* f = fopen("regate.in", "r");
  int m, u, v, c;

  fscanf(f, "%d %d", &n, &m);
  for (u = 1; u <= n; u++) {
    fscanf(f, "%lld", &nd[u].cost);
  }
  while (m--) {
    fscanf(f, "%d %d %d", &u, &v, &c);
    add_edge(u, v, c);
    add_edge(v, u, c);
  }
  fclose(f);
}

void sort_bridges() {
  std::sort(b, b + comps - 1, [](bridge& a, bridge& b) {
    return a.cost > b.cost;
  });
}

void sort_nodes() {
  for (int i = 0; i < n; i++) {
    ord[i] = i + 1;
  }
  std::sort(ord, ord + n, [](int u, int v) {
    return nd[u].cost > nd[v].cost;
  });
}

void process_bridge(bridge b) {
  int c1 = nd[b.u].comp;
  int c2 = nd[b.v].comp;
  ds.unite(c1, c2, b.cost);
}

// Nodul u este mai ieftin decît toate punțile de pînă acum. Folosește acest
// cost pentru conectarea cu toți vecinii din componenta sa (unificată). La
// acest cost vom adăuga conectarea cu alte componente cu punți mai ieftine,
// dar acel cost deocamdată nu îl știm. De aceea, scădem costul prezent al
// componentei și îl vom readăuga la final. Prin diferență, obținem exact
// costul interconectării prin punți viitoare.
void process_node(int u) {
  int sz = ds.get_size(nd[u].comp);
  nd[u].cost = nd[u].cost * (sz - 1) - ds.get_cost(nd[u].comp);
}

void merge_bridges_and_nodes() {
  ds.init(comps);

  int i = 0, j = 0;
  while ((i < comps - 1) || (j < n)) {
    if ((j == n) || ((i < comps - 1) && (b[i].cost > nd[ord[j]].cost))) {
      process_bridge(b[i++]);
    } else {
      process_node(ord[j++]);
    }
  }
}

void write_data() {
  FILE* f = fopen("regate.out", "w");
  for (int u = 1; u <= n; u++) {
    fprintf(f, "%lld\n", ds.get_cost(nd[u].comp) + nd[u].cost);
  }
  fclose(f);
}

int main() {
  read_data();
  find_bridges(1, 1);
  pop_component(1); // DFS-ul lasă o componentă pe stivă.
  sort_bridges();
  sort_nodes();
  merge_bridges_and_nodes();
  write_data();

  return 0;
}
