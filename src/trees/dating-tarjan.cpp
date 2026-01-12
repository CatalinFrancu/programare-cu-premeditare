#include <algorithm>
#include <stdio.h>
#include <unordered_map>

const int MAX_NODES = 100'000;
const int MAX_QUERIES = 100'000;
const int BLOCK_SIZE = 775; // de ordinul a sqrt(2n)

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

  // Întotdeauna îl alipește pe v la u. Fără *union by rank*.
  void unite(int u, int v) {
    p[find(v)] = find(u);
  }
};

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

struct cell {
  int val, next;
};

struct node {
  int adj;       // listă de adiacență
  int queries;   // listă de interogări pentru acest nod
  int tin, tout; // timpi de intrare și ieșire din DFS
  int fav;
  bool gender;
};

struct query {
  // Redenumește și refolosește cîmpurile pentru a economisi memorie.
  union { int u; int l; };
  union { int v; int r; };
  int lca;
  int orig_index;
};

cell list[2 * MAX_NODES];
cell qlist[2 * MAX_QUERIES + 1];
node nd[MAX_NODES + 1];
int euler[2 * MAX_NODES + 1];
disjoint_set_forest dsf;
query q[MAX_QUERIES];
unsigned answer[MAX_QUERIES];
int n, num_queries;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void add_query(int ind, int u) {
  static int pos = 1;
  qlist[pos] = { ind, nd[u].queries };
  nd[u].queries = pos++;
}

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
    add_edge(u, v);
    add_edge(v, u);
  }
}

void read_queries() {
  scanf("%d", &num_queries);
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d", &q[i].u, &q[i].v);
    add_query(i, q[i].u);
    add_query(i, q[i].v);
    q[i].orig_index = i;
  }
}

void process_lca_queries(int u) {
  for (int ptr = nd[u].queries; ptr; ptr = qlist[ptr].next) {
    int i = qlist[ptr].val;
    int v = (q[i].u == u) ? q[i].v : q[i].u;
    if (nd[v].tin) {
      q[i].lca = dsf.find(v);
    }
  }
}

void dfs(int u) {
  static int time = 0;
  nd[u].tin = ++time;
  euler[time] = u;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    if (!nd[v].tin) {
      dfs(v);
      dsf.unite(u, v);
    }
  }

  process_lca_queries(u);

  nd[u].tout = ++time;
  euler[time] = u;
}

// Acum că știm LCA-urile și timpii Euler, calculează intervalele.
void rewrite_queries() {
  for (int i = 0; i < num_queries; i++) {
    if (nd[q[i].u].tin > nd[q[i].v].tin) {
      int tmp = q[i].u;
      q[i].u = q[i].v;
      q[i].v = tmp;
    }
    if (q[i].lca == q[i].u) {
      q[i].l = nd[q[i].u].tin;
      q[i].r = nd[q[i].v].tin;
      q[i].lca = 0;
    } else {
      q[i].l = nd[q[i].u].tout;
      q[i].r = nd[q[i].v].tin;
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
  read_queries();
  dsf.init(n);
  dfs(1);
  rewrite_queries();
  sort_queries_in_mo_order();
  answer_queries();
  write_answers();

  return 0;
}
