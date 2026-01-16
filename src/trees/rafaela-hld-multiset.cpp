// Complexitate: O(q log^2 n).
// Metodă: descompunere heavy-light + multiset.
#include <set>
#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;      // fiul cu cel mai mare subarbore
  int head;       // începutul lanțului nostru
  int pos;        // momentul descoperirii în dfs
  int init_pop;   // populația inițială
  std::multiset<int> set;
};

int max(int x, int y) {
  return (x > y) ? x : y;
}

struct fenwick_tree {
  int v[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void range_add(int l, int r, int val) {
    add(l, val);
    add(r + 1, -val);
  }

  void point_add(int pos, int val) {
    range_add(pos, pos, val);
  }

  int get(int pos) {
    long long s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
fenwick_tree fen;
FILE *fin, *fout;
int n, num_queries, total_pop;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  fscanf(fin, "%d %d", &n, &num_queries);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  for (int u = 1; u <= n; u++) {
    fscanf(fin, "%d", &nd[u].init_pop);
    total_pop += nd[u].init_pop;
  }
}

int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {

      nd[v].parent = u;
      int child_size = heavy_dfs(v);
      my_size += child_size;
      nd[u].init_pop += nd[v].init_pop;
      if (child_size > max_child_size) {
        max_child_size = child_size;
        nd[u].heavy = v;
      }

    }
  }

  return my_size;
}

void decompose_dfs(int u, int head) {
  static int time = 1;

  nd[u].head = head;
  nd[u].pos = time++;
  fen.point_add(nd[u].pos, nd[u].init_pop);

  if (nd[u].heavy) {
    decompose_dfs(nd[u].heavy, head);
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && v != nd[u].heavy) {
      decompose_dfs(v, v); // la v începe un lanț nou
    }
  }
}

void update_parent_set(int h, int delta) {
  int p = nd[h].parent;
  std::multiset<int>& s = nd[p].set; // syntactic sugar
  int old = fen.get(nd[h].pos);
  auto it = s.find(old);
  if (it != s.end()) {
    s.erase(it);
  }
  s.insert(old + delta);
}

void update(int u, int delta) {
  total_pop += delta;
  while (u) {
    int h = nd[u].head;
    update_parent_set(h, delta);
    fen.range_add(nd[h].pos, nd[u].pos, delta);
    u = nd[h].parent;
  }
}

int query(int u) {
  int size_of_u = fen.get(nd[u].pos);
  int max_flow = total_pop - size_of_u;

  if (nd[u].heavy) {
    int from_heavy_child = fen.get(nd[nd[u].heavy].pos);
    max_flow = max(max_flow, from_heavy_child);
  }

  if (!nd[u].set.empty()) {
    max_flow = max(max_flow, *nd[u].set.rbegin());
  }

  return max_flow;
}

void process_ops() {
  char type;
  int u, delta;

  while (num_queries--) {
    fscanf(fin, " %c", &type);
    if (type == 'U') {
      fscanf(fin, "%d %d ", &delta, &u);
      update(u, delta);
    } else {
      fscanf(fin, "%d ", &u);
      fprintf(fout, "%d\n", query(u));
    }
  }
}

int main() {
  fin = fopen("rafaela.in", "r");
  fout = fopen("rafaela.out", "w");

  read_tree();
  fen.init(n);
  heavy_dfs(1);
  decompose_dfs(1, 1);
  process_ops();

  fclose(fin);
  fclose(fout);

  return 0;
}
