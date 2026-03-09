#include <stdio.h>

const int MOD = 666'013;
const int SIZE = 3;

struct matrix {
  long long v[SIZE][SIZE];
};

matrix mult(matrix a, matrix b) {
  matrix res;
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      res.v[r][c] = 0;
      for (int i = 0; i < SIZE; i++) {
        res.v[r][c] += a.v[r][i] * b.v[i][c];
      }
      res.v[r][c] %= MOD;
    }
  }
  return res;
}

matrix mod_pow(matrix a, int n) {
  matrix result = {{ {1, 0, 0}, {0, 1, 0}, {0, 0, 1} }};

  while (n) {
    if (n & 1) {
      result = mult(result, a);
    }
    a = mult(a, a);
    n >>= 1;
  }

  return result;
}

void solve_test(FILE* fin, FILE* fout) {
  int x, y, z, a, b, c, n;
  fscanf(fin, "%d %d %d %d %d %d %d", &x, &y, &z, &a, &b, &c, &n);

  matrix m = {{ {a, b, c}, {1, 0, 0}, {0, 1, 0} }};
  matrix p = mod_pow(m, n - 2);
  int result = (p.v[0][0] * z + p.v[0][1] * y + p.v[0][2] * x) % MOD;

  fprintf(fout, "%d\n", result);
}

int main() {
  FILE* fin = fopen("iepuri.in", "r");
  FILE* fout = fopen("iepuri.out", "w");

  int num_tests;
  fscanf(fin, "%d", &num_tests);
  while (num_tests--) {
    solve_test(fin, fout);
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
