#include <stdio.h>

const int MOD = 1'000'000'007;
const int MAX_NODES = 100;

typedef int matrix[MAX_NODES][MAX_NODES];

matrix adj, aux, p;
int n, k;

void read_graph() {
  int m, u, v;
  scanf("%d %d %d", &n, &m, &k);

  while (m--) {
    scanf("%d %d", &u, &v);
    u--;
    v--;
    adj[u][v]++;
  }
}

void identity(matrix a) {
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < n; c++) {
      a[r][c] = 0;
    }
    a[r][r] = 1;
  }
}

void copy(matrix src, matrix dest) {
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < n; c++) {
      dest[r][c] = src[r][c];
    }
  }
}

// a *= b
void mult(matrix a, matrix b) {
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < n; c++) {
      __int128 x = 0;
      for (int i = 0; i < n; i++) {
        x += (long long)a[r][i] * b[i][c];
      }
      aux[r][c] = x % MOD;
    }
  }
  copy(aux, a);
}

void mod_pow(matrix a, int n, matrix dest) {
  identity(dest);

  while (n) {
    if (n & 1) {
      mult(dest, a);
    }
    mult(a, a);
    n >>= 1;
  }
}

int count_paths() {
  mod_pow(adj, k, p);
  return p[0][n - 1];
}

int main() {
  read_graph();
  printf("%d\n", count_paths());

  return 0;
}
