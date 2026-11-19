#include <stdio.h>

const int MAX_N = 300'000;

struct disjoint_set_forest {
  int p[MAX_N];

  void init(int n) {
    for (int u = 0; u < n; u++) {
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

bool occupied[MAX_N];
disjoint_set_forest dsf;
int n;

int find_parking(int pos) {
  if (occupied[pos]) {
    pos = (1 + dsf.find(pos)) % n;
  }

  int prev = (pos - 1 + n) % n;
  int next = (pos + 1) % n;

  occupied[pos] = true;
  if (occupied[prev]) {
    dsf.unite(prev, pos);
  }
  if (occupied[next]) {
    dsf.unite(pos, next);
  }

  return pos;
}

int main() {
  scanf("%d", &n);
  dsf.init(n);
  for (int i = 0; i < n; i++) {
    int pos;
    scanf("%d", &pos);
    pos--;
    char sep = (i < n - 1) ? ' ' : '\n';
    printf("%d%c", 1 + find_parking(pos), sep);
  }

  return 0;
}
