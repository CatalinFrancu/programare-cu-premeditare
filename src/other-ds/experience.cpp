#include <stdio.h>

const int MAX_N = 200'000;

// Invariant: experiența unui nod este suma din exp[] de la nod pînă la
// rădăcină.
struct disjoint_set_forest {
  int p[MAX_N + 1];
  int exp[MAX_N + 1];

  void init(int n) {
    for (int u = 0; u < n; u++) {
      p[u] = u;
    }
  }

  void add_exp(int u, int val) {
    exp[find(u)] += val;
  }

  int get_exp(int u) {
    int r = find(u);
    // Acum fie r = u, fie r = p[u].
    return (r == u) ? exp[u] : (exp[u] + exp[r]);
  }

  int find(int u) {
    // Comprimă calea, menținînd invariantul de experiență.
    if (p[u] == u) {
      return u;
    } else {
      int r = find(p[u]);
      if (p[u] != r) {
        exp[u] += exp[p[u]];
        p[u] = r;
      }
      return r;
    }
  }

  void unite(int u, int v) {
    u = find(u);
    v = find(v);
    if (u != v) {
      p[u] = v;
      // Vrem ca u și v să fie unite de acum înainte, dar nu vrem ca u să
      // primească toată experiența pe care v o are deja.
      exp[u] -= exp[v];
    }
  }
};

disjoint_set_forest dsf;
int n;

int main() {
  int num_ops, x, y;
  char type[10];

  scanf("%d %d", &n, &num_ops);
  dsf.init(n);

  while (num_ops--) {
    scanf("%s", type);
    switch (type[0]) {
      case 'a':
        scanf("%d %d", &x, &y);
        dsf.add_exp(x, y);
        break;

      case 'g':
        scanf("%d", &x);
        printf("%d\n", dsf.get_exp(x));
        break;

      case 'j':
        scanf("%d %d", &x, &y);
        dsf.unite(x, y);
        break;
    }
  }

  return 0;
}
