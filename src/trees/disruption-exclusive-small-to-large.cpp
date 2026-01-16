#include <algorithm>
#include <stdio.h>
#include <vector>

typedef unsigned short u16;

const int MAX_NODES = 50'000;
const int MAX_REPL_PATHS = 50'000;

struct node {
  std::vector<u16> adj;
  std::vector<u16> repl_ids;
  u16 parent;
  u16 heavy;
  u16 min_id;
};

struct edge {
  u16 u, v;
};

struct repl_path {
  u16 u, v;
  int cost;
};

// Un arbore Fenwick de booleeni. Admite operația „cel mai mic bit setat”.
struct fenwick_tree {
  u16 v[MAX_NODES + 1];
  bool raw[MAX_NODES + 1];
  int n;
  int max_p2;

  void init(int n) {
    this->n = n;
    max_p2 = 1 << (31 - __builtin_clz(n));
  }

  void add(int pos, int delta) {
    do {
      v[pos] += delta;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void toggle(int pos) {
    add(pos, raw[pos] ? -1 : +1);
    raw[pos] = !raw[pos];
  }

  // Returnează poziția primului bit setat sau n + 1 dacă arborele este gol.
  int get_first_set_bit() {
    int pos = 0;

    for (int interval = max_p2; interval; interval >>= 1) {
      if ((pos + interval <= n) && (v[pos + interval] == 0)) {
        pos += interval;
      }
    }

    return pos + 1;
  }
};

node nd[MAX_NODES + 1];
edge e[MAX_NODES];
repl_path repl[MAX_REPL_PATHS + 1];
fenwick_tree fen;
int n, num_repl;

void read_data() {
  FILE* f = fopen("disrupt.in", "r");

  fscanf(f, "%d %d", &n, &num_repl);
  for (int i = 0; i < n - 1; i++) {
    u16 u, v;
    fscanf(f, "%hd %hd", &u, &v);
    e[i] = { u, v };
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }

  for (int i = 1; i <= num_repl; i++) {
    fscanf(f, "%hd %hd %d\n", &repl[i].u, &repl[i].v, &repl[i].cost);
  }

  fclose(f);
}

void sort_repl_paths() {
  std::sort(repl + 1, repl + num_repl + 1, [](repl_path& a, repl_path& b) {
    return a.cost < b.cost;
  });
}

void distribute_repl_paths() {
  for (int i = 1; i <= num_repl; i++) {
    nd[repl[i].u].repl_ids.push_back(i);
    nd[repl[i].v].repl_ids.push_back(i);
  }
}

int size_dfs(int u) {
  int size = 1, max_c_size = 0;

  for (u16 v: nd[u].adj) {
    if (v != nd[u].parent) {
      nd[v].parent = u;
      int c = size_dfs(v);
      size += c;
      if (!nd[u].heavy || (c > max_c_size)) {
        nd[u].heavy = v;
        max_c_size = c;
      }
    }
  }

  return size;
}

void toggle_node(int u) {
  for (u16 id: nd[u].repl_ids) {
    fen.toggle(id);
  }
}

void toggle_subtree(int u) {
  toggle_node(u);
  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {
      toggle_subtree(v);
    }
  }
}

void dfs(int u) {
  for (u16 v: nd[u].adj) {
    if ((v != nd[u].parent) && (v != nd[u].heavy)) {
      dfs(v);
      toggle_subtree(v);
    }
  }

  if (nd[u].heavy) {
    dfs(nd[u].heavy);
  }

  for (u16 v: nd[u].adj) {
    if ((v != nd[u].parent) && (v != nd[u].heavy)) {
      toggle_subtree(v);
    }
  }

  toggle_node(u);
  nd[u].min_id = fen.get_first_set_bit();
}

void compute_answers() {
  FILE* fout = fopen("disrupt.out", "w");

  for (int i = 0; i < n - 1; i++) {
    int child = (nd[e[i].u].parent == e[i].v) ? e[i].u : e[i].v;
    int id = nd[child].min_id;
    int cost = (id <= num_repl) ? repl[id].cost : -1;
    fprintf(fout, "%d\n", cost);
  }

  fclose(fout);
}

int main() {
  read_data();
  sort_repl_paths();
  distribute_repl_paths();
  fen.init(num_repl);
  size_dfs(1);
  dfs(1);
  compute_answers();

  return 0;
}
