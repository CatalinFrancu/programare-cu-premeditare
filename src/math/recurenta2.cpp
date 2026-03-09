#include <stdio.h>

const int MOD = 666'013;
const int SIZE = 7;

typedef long long matrix[SIZE][SIZE];

int n, x1, x2, a, b, c;

void read_data() {
  FILE* f = fopen("recurenta2.in", "r");
  fscanf(f, "%d %d %d %d %d %d", &a, &b, &c, &x1, &x2, &n);
  fclose(f);
}

void mult(matrix a, matrix b, matrix dest) {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      dest[r][c] = 0;
      for (int i = 0; i < SIZE; i++) {
        dest[r][c] += a[r][i] * b[i][c];
      }
      dest[r][c] %= MOD;
    }
  }
}

void copy(matrix src, matrix dest) {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      dest[r][c] = src[r][c];
    }
  }
}

void identity(matrix a) {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      a[r][c] = 0;
    }
    a[r][r] = 1;
  }
}

void mod_pow(matrix a, int n, matrix dest) {
  matrix aux;
  identity(dest);

  while (n) {
    if (n & 1) {
      mult(dest, a, aux);
      copy(aux, dest);
    }
    mult(a, a, aux);
    copy(aux, a);
    n >>= 1;
  }
}

int solve_recurrence() {
  // Fiecare generație stochează
  //
  //     (X_n, X_{n-1}, n X_n, (n-1) X_{n-1}, S_n, n, 1).
  //
  // Aceasta este matricea de recurență.
  matrix m = {
    { a,     b,  0,  0,  0,  0,  c },
    { 1,     0,  0,  0,  0,  0,  0 },
    { a, 2 * b,  a,  b,  0,  c,  c },
    { 0,     0,  1,  0,  0,  0,  0 },
    { a, 2 * b,  a,  b,  1,  c,  c },
    { 0,     0,  0,  0,  0,  1,  1 },
    { 0,     0,  0,  0,  0,  0,  1 },
  };

  // Acesta este vectorul pentru n = 2.
  long long v[SIZE] = { x2, x1, 2 * x2, x1, 2 * x2 + x1, 2, 1 };

  matrix p;
  mod_pow(m, n - 2, p);

  // Acum S_n este p[4] * v.
  long long result = 0;
  for (int i = 0; i < SIZE; i++) {
    result += p[4][i] * v[i];
  }

  return result % MOD;
}

void write_answer(int answer) {
  FILE* f = fopen("recurenta2.out", "w");
  fprintf(f, "%d\n", answer);
  fclose(f);
}

int main() {
  read_data();
  int answer = solve_recurrence();
  write_answer(answer);

  return 0;
}
