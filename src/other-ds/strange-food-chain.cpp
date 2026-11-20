#include <stdio.h>

const int MAX_N = 50'000;
const int NUM_CLASSES = 3;
const int T_EQUIV = 1;

// Invariant: distanța unui nod este suma din dist[] de la nod la rădăcină.
// Două noduri (animale) din același arbore și cu aceeași distanță sînt
// echivalente. Două animale din același arbore ale căror distanțe diferă prin
// 1 modulo 3 înseamnă că primul îl mănîncă pe al doilea.
struct disjoint_set_forest {
  int p[MAX_N + 1];
  int dist[MAX_N + 1];

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
      dist[u] = 0;
    }
  }

  int get_dist(int u) {
    find(u);
    // Acum u este fie rădăcina, fie un fiu al acesteia. În orice caz,
    // distanța rădăcinii este 0.
    return dist[u];
  }

  int find(int u) {
    // Comprimă calea menținînd invariantul.
    if (p[u] == u) {
      return u;
    } else {
      int r = find(p[u]);
      if (p[u] != r) {
        dist[u] = (dist[u] + dist[p[u]]) % NUM_CLASSES;
        p[u] = r;
      }
      return r;
    }
  }

  // Returnează false dacă u și v sînt deja unificate și au altă distanță.
  bool try_unite(int u, int v, int d) {
    int ru = find(u), rv = find(v);

    if (ru == rv) {
      int real_dist = (get_dist(u) - get_dist(v) + NUM_CLASSES) % NUM_CLASSES;
      return d == real_dist;
    }

    int du = get_dist(u);
    int dv = get_dist(v);
    int to_add = (dv - du + d + NUM_CLASSES) % NUM_CLASSES;
    dist[ru] = (dist[ru] + to_add) % NUM_CLASSES;
    p[ru] = rv;
    return true;
  }
};

disjoint_set_forest dsf;
int n;

bool process_statement(int x, int y, int dist) {
  if ((x < 1) || (x > n) || (y < 1) || (y > n)) {
    return false;
  }

  if (dist && (x == y)) {
    return false;
  }

  return dsf.try_unite(x, y, dist);
}

void process_test() {
  int num_ops, type, x, y;
  scanf("%d %d", &n, &num_ops);
  dsf.init(n);

  int num_lies = 0;
  while (num_ops--) {
    scanf("%d %d %d", &type, &x, &y);
    num_lies += !process_statement(x, y, (type == T_EQUIV) ? 0 : 1);
  }

  printf("%d\n", num_lies);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);

  while (num_tests--) {
    process_test();
  }

  return 0;
}
