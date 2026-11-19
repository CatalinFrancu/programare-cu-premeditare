#include <stdio.h>

const int MAX_N = 100'000;
const int MAX_VAL = 100'000;
const int T_UPDATE = 1;

// O pădure de mulțimi disjuncte peste poziții.
struct disjoint_set_forest {
  int p[MAX_N + 1];
  int val[MAX_N + 1];

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

  int get_val(int u) {
    return val[find(u)];
  }

  void update_val(int pos, int new_val) {
    val[find(pos)] = new_val;
  }

  // Întotdeauna agață u de v.
  void unite(int u, int v) {
    p[find(u)] = find(v);
  }
};

disjoint_set_forest dsf;
int where[MAX_VAL + 1]; // indicele unei poziții cu această culoare
int n, num_queries;

void read_array_into_dsf() {
  for (int val = 1; val <= MAX_VAL; val++) {
    where[val] = 0;
  }

  scanf("%d %d", &n, &num_queries);
  dsf.init(n);

  for (int i = 1; i <= n; i++) {
    int val;
    scanf("%d", &val);

    if (where[val]) {
      dsf.unite(i, where[val]);
    } else {
      where[val] = i;
      dsf.update_val(i, val);
    }
  }
}

void replace(int x, int y) {
  if (x != y) { // Nu dorim să asignăm where[x] = 0 dacă x == y.
    if (where[x] && where[y]) {
      dsf.unite(where[x], where[y]);
    } else if (where[x] && !where[y]) {
      where[y] = where[x];
      dsf.update_val(where[x], y);
    }
    where[x] = 0;
  }
}

void process_queries() {
  int type, x, y;

  while (num_queries--) {
    scanf("%d", &type);
    if (type == T_UPDATE) {
      scanf("%d %d", &x, &y);
      replace(x, y);
    } else {
      scanf("%d", &x);
      printf("%d\n", dsf.get_val(x));
    }
  }
}

void reset() {
}

int main() {
  int num_tests;

  scanf("%d", &num_tests);
  for (int t = 1; t <= num_tests; t++) {
    printf("Case %d:\n", t);
    read_array_into_dsf();
    process_queries();
  }

  return 0;
}
