#include <algorithm>
#include <stdio.h>
#include <unordered_map>
#include <vector>

const int MAX_NODES = 100'000;
const int MAX_QUERIES = 100'000;
const int BLOCK_SIZE = 775; // de ordinul a sqrt(2n)

struct normalizer {
  std::unordered_map<int, int> map;

  int normalize(int x) {
    auto it = map.find(x);
    if (it == map.end()) {
      int result = 1 + map.size();
      map[x] = result;
      return result;
    } else {
      return it->second;
    }
  }
};

struct node {
  std::vector<int> adj; // listă de adiacență
  int tin, tout;        // timpi de intrare și ieșire din DFS
  int parent, jump;     // pointeri pentru LCA
  int depth;
  int fav;
  bool gender;
};

struct query {
  int l, r;
  int lca;
  int orig_index;
};

node nd[MAX_NODES + 1];
int euler[2 * MAX_NODES + 1];
query q[MAX_QUERIES];
unsigned answer[MAX_QUERIES];
int n, num_queries;

void read_tree() {
  scanf("%d ", &n);
  for (int u = 1; u <= n; u++) {
    nd[u].gender = (getchar() == '1');
    getchar();
  }

  normalizer norm;
  for (int u = 1; u <= n; u++) {
    int x;
    scanf("%d", &x);
    nd[u].fav = norm.normalize(x);
  }

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
}

void dfs(int u) {
  static int time = 0;
  nd[u].tin = ++time;
  euler[time] = u;

  int u2 = nd[u].jump, u3 = nd[u2].jump;
  bool equal = (nd[u2].depth - nd[u].depth == nd[u3].depth - nd[u2].depth);

  for (auto v: nd[u].adj) {
    if (!nd[v].tin) {
      nd[v].depth = 1 + nd[u].depth;
      nd[v].parent = u;
      nd[v].jump = equal ? u3 : u;
      dfs(v);
    }
  }

  nd[u].tout = ++time;
  euler[time] = u;
}

bool is_ancestor(int u, int v) {
  return
    (nd[u].tin <= nd[v].tin) &&
    (nd[u].tout >= nd[v].tout);
}

int lca(int u, int v) {
  // Găsește cel mai jos strămoș al lui u care este și strămoș al lui v.
  while (!is_ancestor(u, v)) {
    if (nd[u].jump && !is_ancestor(nd[u].jump, v)) {
      u = nd[u].jump;
    } else {
      u = nd[u].parent;
    }
  }

  return u;
}

void read_queries() {
  scanf("%d", &num_queries);
  for (int i = 0; i < num_queries; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    int l = lca(u, v);
    if (nd[u].tin > nd[v].tin) {
      std::swap(u, v);
    }
    if (l == u) {
      q[i] = { nd[u].tin, nd[v].tin, 0, i };
    } else {
      q[i] = { nd[u].tout, nd[v].tin, l, i };
    }
  }
}

void sort_queries_in_mo_order() {
  std::sort(q, q + num_queries, [](query a, query b) {
    int x = a.l / BLOCK_SIZE, y = b.l / BLOCK_SIZE;
    if (x != y) {
      return (x < y);
    } else if (x % 2) {
      return a.r > b.r;
    } else {
      return a.r < b.r;
    }
  });
}

struct mo_tracker {
  bool on[MAX_NODES + 1];  // nodurile care apar exact o dată în interval
  int f[MAX_NODES + 1][2]; // frecvența numerelor favorite pentru fiecare gen
  int l, r;
  unsigned num_pairs;

  void init() {
    l = 1;
    r = 0;
  }

  void toggle(int pos) {
    int u = euler[pos];
    on[u] = !on[u];
    int sign = on[u] ? +1 : -1;
    f[nd[u].fav][nd[u].gender] += sign;
    num_pairs += sign * f[nd[u].fav][!nd[u].gender];
  }

  unsigned query(int target_l, int target_r, int extra_fav, bool extra_gender) {
    while (l > target_l) {
      toggle(--l);
    }
    while (r < target_r) {
      toggle(++r);
    }
    while (l < target_l) {
      toggle(l++);
    }
    while (r > target_r) {
      toggle(r--);
    }

    if (extra_fav) {
      return num_pairs + f[extra_fav][!extra_gender];
    } else {
      return num_pairs;
    }
  }
};

mo_tracker tracker;

void answer_queries() {
  tracker.init();
  for (int i = 0; i < num_queries; i++) {
    int extra_fav = nd[q[i].lca].fav;
    int extra_gender = nd[q[i].lca].gender;
    unsigned result = tracker.query(q[i].l, q[i].r, extra_fav, extra_gender);
    answer[q[i].orig_index] = result;
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%u\n", answer[i]);
  }
}

int main() {
  read_tree();
  nd[1].depth = 1;
  dfs(1);
  read_queries();
  sort_queries_in_mo_order();
  answer_queries();
  write_answers();

  return 0;
}
