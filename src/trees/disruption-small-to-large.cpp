#include <algorithm>
#include <set>
#include <stdio.h>
#include <vector>

typedef unsigned short u16;
typedef std::set<u16> set;

const int MAX_NODES = 50'000;
const int MAX_REPL_PATHS = 50'000;

struct node {
  std::vector<u16> adj;
  std::vector<u16> repl_ids;
  u16 parent;
  int min_cost;
};

struct edge {
  u16 u, v;
};

struct repl_path {
  u16 u, v;
  int cost;
};

node nd[MAX_NODES + 1];
edge e[MAX_NODES];
repl_path repl[MAX_REPL_PATHS];
int n, num_repl_paths;

void read_data() {
  FILE* f = fopen("disrupt.in", "r");

  fscanf(f, "%d %d", &n, &num_repl_paths);
  for (int i = 0; i < n - 1; i++) {
    u16 u, v;
    fscanf(f, "%hd %hd", &u, &v);
    e[i] = { u, v };
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }

  for (int i = 0; i < num_repl_paths; i++) {
    fscanf(f, "%hd %hd %d\n", &repl[i].u, &repl[i].v, &repl[i].cost);
  }

  fclose(f);
}

void sort_repl_paths() {
  std::sort(repl, repl + num_repl_paths, [](repl_path& a, repl_path& b) {
    return a.cost < b.cost;
  });
}

void distribute_repl_paths() {
  for (int i = 0; i < num_repl_paths; i++) {
    nd[repl[i].u].repl_ids.push_back(i);
    nd[repl[i].v].repl_ids.push_back(i);
  }
}

void merge_sets(set& parent, set& child) {
  if (child.size() > parent.size()) {
    parent.swap(child);
  }
  for (u16 id: child) {
    std::pair<set::iterator, bool> p = parent.insert(id);
    if (!p.second) {
      // Inserarea a eșuat deoarece ID-ul este deja în set; șterge-l.
      parent.erase(p.first);
    }
  }
  child.clear();
}

set dfs(int u) {
  set s;
  for (u16 id: nd[u].repl_ids) {
    s.insert(id);
  }

  for (u16 v: nd[u].adj) {
    if (v != nd[u].parent) {
      nd[v].parent = u;
      set s2 = dfs(v);
      merge_sets(s, s2);
    }
  }

  if (s.empty()) {
    nd[u].min_cost = -1;
  } else {
    u16 first = *s.begin();
    nd[u].min_cost = repl[first].cost;
  }

  return s;
}

void compute_answers() {
  FILE* fout = fopen("disrupt.out", "w");

  for (int i = 0; i < n - 1; i++) {
    int child = (nd[e[i].u].parent == e[i].v) ? e[i].u : e[i].v;
    int cost = nd[child].min_cost;
    fprintf(fout, "%d\n", cost);
  }

  fclose(fout);
}

int main() {
  read_data();
  sort_repl_paths();
  distribute_repl_paths();
  dfs(1);
  compute_answers();

  return 0;
}
