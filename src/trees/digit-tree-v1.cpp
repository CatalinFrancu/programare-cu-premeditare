// Method: Centroid decomposition.
// Complexity: O(n log^2 n).
#include <map>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;

struct edge {
  int v, digit;
};

struct node {
  std::vector<edge> adj;
  int size;
  bool dead;
};

node nd[MAX_NODES];
int pow10[MAX_NODES + 1];
int inv_pow10[MAX_NODES + 1];
int n, mod;
long long num_pairs;

void read_tree() {
  scanf("%d %d", &n, &mod);

  for (int i = 1; i < n; i++) {
    int u, v, digit;
    scanf("%d %d %d", &u, &v, &digit);
    nd[u].adj.push_back({v, digit % mod});
    nd[v].adj.push_back({u, digit % mod});
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
  std::map<int, int> freq;

  void head_dfs(int u, int parent, int depth, int rem, int sign) {
    freq[rem] += sign;

    for (edge e: nd[u].adj) {
      if ((e.v != parent) && !nd[e.v].dead) {
        int new_rem = ((long long)pow10[depth] * e.digit + rem) % mod;
        head_dfs(e.v, u, depth + 1, new_rem, sign);
      }
    }
  }

  void tail_dfs(int u, int parent, int depth, int rem) {
    int h = (long long)inv_pow10[depth] * (mod - rem) % mod;

    // Numărul de moduri de a prefixa această coadă cu un început.
    num_pairs += freq[h];

    // Are coada curentă un rest egal cu zero?
    num_pairs += (rem == 0);

    for (edge e: nd[u].adj) {
      if ((e.v != parent) && !nd[e.v].dead) {
        int new_rem = ((long long)rem * 10 + e.digit) % mod;
        tail_dfs(e.v, u, depth + 1, new_rem);
      }
    }
  }

  void count_pairs_through(int u) {
    freq.clear();

    for (edge e: nd[u].adj) {
      if (!nd[e.v].dead) {
        head_dfs(e.v, u, 1, e.digit, +1);
      }
    }

    for (edge e: nd[u].adj) {
      if (!nd[e.v].dead) {
        head_dfs(e.v, u, 1, e.digit, -1);
        tail_dfs(e.v, u, 1, e.digit);
        head_dfs(e.v, u, 1, e.digit, +1);
      }
    }

    // Numărul de căi de început care au un rest 0.
    num_pairs += freq[0];
  }
};

pair_counter pc;

void size_dfs(int u, int parent) {
  nd[u].size = 1;

  for (edge e: nd[u].adj) {
    if ((e.v != parent) && !nd[e.v].dead) {
      size_dfs(e.v, u);
      nd[u].size += nd[e.v].size;
    }
  }
}

int find_centroid(int u, int limit) {
  for (edge e: nd[u].adj) {
    if ((nd[e.v].size < nd[u].size) && !nd[e.v].dead && (nd[e.v].size > limit)) {
      return find_centroid(e.v, limit);
    }
  }

  return u;
}

void decompose(int u) {
  size_dfs(u, -1);
  u = find_centroid(u, nd[u].size / 2);

  pc.count_pairs_through(u);

  nd[u].dead = true;
  for (edge e: nd[u].adj) {
    if (!nd[e.v].dead) {
      decompose(e.v);
    }
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
