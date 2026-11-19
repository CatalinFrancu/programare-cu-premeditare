#include <stdio.h>

const int MAX_N = 200'000;
const int MAX_Q = 200'000;

struct disjoint_set_forest {
  int p[MAX_N + 1]; // Sfîrșitul intervalului.

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

  // Întotdeauna agață u de v.
  void unite(int u, int v) {
    p[find(u)] = find(v);
  }
};

struct query {
  int l, r, color;
};

int color[MAX_N + 1];
query q[MAX_Q];
disjoint_set_forest dsf;
int n, num_queries;

void read_data() {
  scanf("%d %d", &n, &num_queries);
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d %d", &q[i].l, &q[i].r, &q[i].color);
  }
}

void rewind() {
  dsf.init(n);

  for (int i = num_queries - 1; i >= 0; i--) {
    int j = q[i].l;
    while (j <= q[i].r) {
      if (color[j]) {
        j = 1 + dsf.find(j);
      } else {
        color[j] = q[i].color;
        if ((j > 1) && color[j - 1]) {
          dsf.unite(j - 1, j);
        }
        if ((j < n) && color[j + 1]) {
          dsf.unite(j, j + 1);
        }
        j++;
      }
    }
  }
}

void write_colors() {
  for (int i = 1; i <= n; i++) {
    printf("%d\n", color[i]);
  }
}

int main() {
  read_data();
  rewind();
  write_colors();

  return 0;
}
