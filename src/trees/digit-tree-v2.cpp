#include <algorithm>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;

struct edge {
  int v, digit;
};

struct node {
  std::vector<edge> adj;
  int size;
};

// Stochează informații despre frecvența unui rest modulo M. Putem interoga
// aceste informații pentru un fiu dat al rădăcinii (0-based) sau pentru toți
// fiii.
struct rem_info {
  struct elem {
    int child, rem, freq;

    bool operator ==(elem other) {
      return (child == other.child) && (rem == other.rem);
    }

    bool operator <(elem other) {
      return (child < other.child) ||
        ((child == other.child) && (rem < other.rem));
    }

    bool operator >(elem other) {
      return (child > other.child) ||
        ((child == other.child) && (rem > other.rem));
    }
  };

  elem c[MAX_NODES], t[MAX_NODES];
  int nc, nt;

  void init() {
    nc = nt = 0;
  }

  void insert(int child, int rem) {
    c[nc++] = { child, rem, 1 };
    t[nt++] = { 0, rem, 1 };
  }

  int merge_duplicates(elem* v, int n) {
    if (n) {
      int j = 1;
      for (int i = 1; i < n; i++) {
        if (v[i] == v[j - 1]) {
          v[j - 1].freq++;
        } else {
          v[j++] = v[i];
        }
      }
      n = j;
    }
    return n;
  }

  void sort() {
    std::sort(c, c + nc);
    std::sort(t, t + nt);

    nc = merge_duplicates(c, nc);
    nt = merge_duplicates(t, nt);
  }

  int count_rem_0() {
    return (nt && (t[0].rem == 0))
      ? t[0].freq
      : 0;
  }

  int find(elem* v, int n, int child, int rem) {
    elem el = { child, rem, 0 };
    int pos = std::lower_bound(v, v + n, el) - v;
    return ((pos < n) && (v[pos] == el)) ? v[pos].freq : 0;
  }

  int count_not_in_child(int child, int rem) {
    return find(t, nt, 0, rem) - find(c, nc, child, rem);
  }
};

node nd[MAX_NODES];
int pow10[MAX_NODES + 1];
int inv_pow10[MAX_NODES + 1];
rem_info r;
int n, mod;
long long num_pairs;

void read_tree() {
  scanf("%d %d", &n, &mod);

  for (int i = 1; i < n; i++) {
    int u, v, digit;
    scanf("%d %d %d", &u, &v, &digit);
    nd[u].adj.push_back({v, digit});
    nd[v].adj.push_back({u, digit});
  }
}

void extended_euclid(int a, int b, int& x, int& y) {
  if (!b) {
    x = 1;
    y = 0;
  } else {
    int xp, yp;
    extended_euclid(b, a % b, xp, yp);
    x = yp;
    y = xp - (a / b) * yp;
  }
}

int inverse(int a, int mod) {
  int x, y;
  extended_euclid(a, mod, x, y);
  return (x + mod) % mod;
}

void precompute_pow10() {
  pow10[0] = 1;
  for (int i = 1; i <= n; i++) {
    pow10[i] = 10ll * pow10[i - 1] % mod;
  }

  long long inv = inverse(10, mod);
  inv_pow10[0] = 1;
  for (int i = 1; i <= n; i++) {
    inv_pow10[i] = inv * inv_pow10[i - 1] % mod;
  }
}

struct pair_counter {
  int child;

  void head_dfs(int u, int parent, int depth, int rem) {
    r.insert(child, rem);

    for (edge e: nd[u].adj) {
      if (e.v != parent) {
        int new_rem = ((long long)pow10[depth] * e.digit + rem) % mod;
        head_dfs(e.v, u, depth + 1, new_rem);
      }
    }
  }

  void tail_dfs(int u, int parent, int depth, int rem) {
    int h = (long long)inv_pow10[depth] * (mod - rem) % mod;

    // Numărul de moduri de a prefixa această coadă cu un început.
    num_pairs += r.count_not_in_child(child, h);

    // Are coada curentă un rest egal cu zero?
    num_pairs += (rem == 0);

    for (edge e: nd[u].adj) {
      if (e.v != parent) {
        int new_rem = ((long long)rem * 10 + e.digit) % mod;
        tail_dfs(e.v, u, depth + 1, new_rem);
      }
    }
  }

  void count_pairs_through(int u) {
    r.init();

    for (edge e: nd[u].adj) {
      child = e.v;
      head_dfs(e.v, u, 1, e.digit % mod);
    }

    r.sort();

    for (edge e: nd[u].adj) {
      child = e.v;
      tail_dfs(e.v, u, 1, e.digit % mod);
    }

    // Numărul de căi de început care au un rest 0.
    num_pairs += r.count_rem_0();
  }
};

pair_counter pc;

void size_dfs(int u, int parent) {
  nd[u].size = 1;

  for (edge e: nd[u].adj) {
    if (e.v != parent) {
      size_dfs(e.v, u);
      nd[u].size += nd[e.v].size;
    }
  }
}

int find_centroid(int u, int limit) {
  for (edge e: nd[u].adj) {
    if ((nd[e.v].size < nd[u].size) && (nd[e.v].size > limit)) {
      return find_centroid(e.v, limit);
    }
  }

  return u;
}

void delete_edge(int u, int v) {
  int i = 0;
  while (nd[u].adj[i].v != v) {
    i++;
  }
  nd[u].adj[i] = nd[u].adj.back();
  nd[u].adj.pop_back();
}

void delete_node(int u) {
  for (edge e: nd[u].adj) {
    delete_edge(e.v, u);
  }
}

void decompose(int u) {
  size_dfs(u, -1);
  u = find_centroid(u, nd[u].size / 2);

  pc.count_pairs_through(u);

  delete_node(u);
  for (edge e: nd[u].adj) {
    decompose(e.v);
  }
}

void write_answer() {
  printf("%lld\n", num_pairs);
}

int main() {
  read_tree();
  precompute_pow10();
  decompose(0);
  write_answer();

  return 0;
}
