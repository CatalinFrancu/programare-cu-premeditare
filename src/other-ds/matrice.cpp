#include <algorithm>
#include <stdio.h>

const int MAX_N2 = 90'000;
const int INFINITY = 1'000'000;

struct disjoint_set_forest {
  int p[MAX_N2];
  int size[MAX_N2];
  int val[MAX_N2]; // valoarea minimă pentru a părăsi componenta

  void init(int n) {
    for (int u = 0; u < n; u++) {
      p[u] = u;
      size[u] = 1;
    }
  }

  int find(int u) {
    while (p[u] != u) {
      u = p[u];
    }
    return u;
  }

  void unite(int u, int v, int cost) {
    u = find(u);
    v = find(v);

    if (u != v) {
      if (size[u] > size[v]) {
        std::swap(u, v);
      }

      size[v] += size[u];
      p[u] = v;
      val[u] = cost;
    }
  }

  int path_min(int u, int v) {
    int min_val = INFINITY;

    while (u != v) {
      if (size[u] > size[v]) {
        std::swap(u, v);
      }
      min_val = std::min(min_val, val[u]);
      u = p[u];
    }

    return min_val;
  }
};

int id[MAX_N2];
int val[MAX_N2];
bool active[MAX_N2];
disjoint_set_forest dsf;
int n, num_queries;

void read_matrix(FILE* f) {
  fscanf(f, "%d %d", &n, &num_queries);
  for (int i = 0; i < n * n; i++) {
    fscanf(f, "%d", &val[i]);
    id[i] = i;
  }
}

void sort_by_value_desc() {
  std::sort(id, id + n * n, [](int a, int b) {
    return val[a] > val[b];
  });
}

void merge_cells() {
  dsf.init(n * n);
  for (int i = 0; i < n * n; i++) {
    int x = id[i]; // syntactic sugar
    int row = x / n, col = x % n;
    if (row && active[x - n]) {
      dsf.unite(x, x - n, val[x]);
    }
    if ((row < n - 1) && active[x + n]) {
      dsf.unite(x, x + n, val[x]);
    }
    if (col && active[x - 1]) {
      dsf.unite(x, x - 1, val[x]);
    }
    if ((col < n - 1) && active[x + 1]) {
      dsf.unite(x, x + 1, val[x]);
    }
    active[x] = true;
  }
}

void answer_queries(FILE* fin, FILE* fout) {
  while (num_queries--) {
    int r1, c1, r2, c2;
    fscanf(fin, "%d %d %d %d", &r1, &c1, &r2, &c2);
    int id1 = (r1 - 1) * n + (c1 - 1);
    int id2 = (r2 - 1) * n + (c2 - 1);
    fprintf(fout, "%d\n", dsf.path_min(id1, id2));
  }
}

int main() {
  FILE* fin = fopen("matrice.in", "r");
  FILE* fout = fopen("matrice.out", "w");

  read_matrix(fin);
  sort_by_value_desc();
  merge_cells();
  answer_queries(fin, fout);

  fclose(fin);
  fclose(fout);

  return 0;
}
