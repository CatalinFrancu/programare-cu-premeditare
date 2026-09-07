#include <algorithm>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;
const int MAX_QUERIES = 100'000;

// determinate experimental
const int BLOCK_SIZE_MO = 1'300;
const int BLOCK_SIZE_MEX = 250;

// Al 100.001-lea număr prim este 1.299.721
// https://t5k.org/lists/small/100000.txt
const int MAX_VAL = 1'300'000;
const int NONE = -1;

struct prime_list {
  bool is_composite[MAX_VAL + 1];
  int p[MAX_NODES + 1];
  int n;

  void build() {
    for (int d = 2; d * d <= MAX_VAL; d++) {
      if (!is_composite[d]) {
        for (int i = d * d; i <= MAX_VAL; i += d) {
          is_composite[i] = 1;
        }
      }
    }

    for (int i = 2; n <= MAX_NODES; i++) {
      if (!is_composite[i]) {
        p[n++] = i;
      }
    }
  }

  int find(int val) {
    int pos = std::lower_bound(p, p + n, val) - p;
    return ((pos < n) && (p[pos] == val))
      ? pos
      : NONE;
  }

  int get(int k) {
    return p[k];
  }
};

prime_list primes;

struct query {
  int l, r;
  int extra_val;
  int orig_index;
};

struct node {
  std::vector<int> adj;
  int tin, tout;
  int parent, jump; // pointerii pentru LCA
  int depth;
  int val;
};

struct tree {
  node nd[MAX_NODES + 1];
  int euler[2 * MAX_NODES + 1];

  void read() {
    int n, u, v, val;

    scanf("%d", &n);
    for (int u = 1; u <= n; u++) {
      scanf("%d", &val);
      nd[u].val = primes.find(val);
    }

    for (int i = 1; i < n; i++) {
      scanf("%d %d", &u, &v);
      nd[u].adj.push_back(v);
      nd[v].adj.push_back(u);
    }
  }

  void flatten(int u) {
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
        flatten(v);
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

  query make_query(int& u, int& v) {
    if (nd[u].tin > nd[v].tin) {
      std::swap(u, v);
    }
    int l = lca(u, v);
    if (l == u) {
      return { nd[u].tin, nd[v].tin, NONE };
    } else {
      return { nd[u].tout, nd[v].tin, nd[l].val };
    }
  }
};

tree t;
query q[MAX_QUERIES];
int answer[MAX_QUERIES];
int num_queries;

void read_queries() {
  scanf("%d", &num_queries);
  for (int i = 0; i < num_queries; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    q[i] = t.make_query(u, v);
    q[i].orig_index = i;
  }
}

void sort_queries_in_mo_order() {
  std::sort(q, q + num_queries, [](query a, query b) {
    int x = a.l / BLOCK_SIZE_MO, y = b.l / BLOCK_SIZE_MO;
    if (x != y) {
      return (x < y);
    } else if (x % 2) {
      return a.r > b.r;
    } else {
      return a.r < b.r;
    }
  });
}

struct mex_tracker {
  int f[MAX_NODES + 1];
  int bl_nonzero[(MAX_NODES + 1) / BLOCK_SIZE_MEX];

  void add(int x) {
    if (x != NONE) {
      if (++f[x] == 1) {
        bl_nonzero[x / BLOCK_SIZE_MEX]++;
      }
    }
  }

  void remove(int x) {
    if (x != NONE) {
      if (--f[x] == 0) {
        bl_nonzero[x / BLOCK_SIZE_MEX]--;
      }
    }
  }

  int get_mex() {
    int bl = 0;
    while (bl_nonzero[bl] == BLOCK_SIZE_MEX) {
      bl++;
    }

    int x = bl * BLOCK_SIZE_MEX;
    while (f[x]) {
      x++;
    }
    return x;
  }
};

struct mo_tracker {
  bool on[MAX_NODES + 1];  // nodurile care apar exact o dată
  mex_tracker mex;
  int l, r;

  void init() {
    l = 1;
    r = 0;
  }

  // smelly code: nu ar trebui să aibă acces la datele interne ale lui t
  void toggle(int pos) {
    int u = t.euler[pos];
    on[u] = !on[u];

    if (on[u]) {
      mex.add(t.nd[u].val);
    } else {
      mex.remove(t.nd[u].val);
    }
  }

  unsigned query(int target_l, int target_r, int extra_val) {
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

    mex.add(extra_val);
    int result = mex.get_mex();
    mex.remove(extra_val);
    return result;
  }
};

mo_tracker tracker;

void answer_queries() {
  tracker.init();
  for (int i = 0; i < num_queries; i++) {
    int mex = tracker.query(q[i].l, q[i].r, q[i].extra_val);
    answer[q[i].orig_index] = primes.get(mex);
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%u\n", answer[i]);
  }
}

int main() {
  primes.build();
  t.read();
  t.flatten(1);
  read_queries();
  sort_queries_in_mo_order();
  answer_queries();
  write_answers();

  return 0;
}
