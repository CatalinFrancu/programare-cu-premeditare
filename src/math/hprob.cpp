#include <stdio.h>

const int SIZE = 24;

typedef double matrix[SIZE][SIZE];

struct perm {
  unsigned char a, b, c, d;
};

const perm UNRANK[SIZE] = {
  { 0, 1, 2, 3}, { 0, 1, 3, 2}, { 0, 2, 1, 3},
  { 0, 2, 3, 1}, { 0, 3, 1, 2}, { 0, 3, 2, 1},
  { 1, 0, 2, 3}, { 1, 0, 3, 2}, { 1, 2, 0, 3},
  { 1, 2, 3, 0}, { 1, 3, 0, 2}, { 1, 3, 2, 0},
  { 2, 0, 1, 3}, { 2, 0, 3, 1}, { 2, 1, 0, 3},
  { 2, 1, 3, 0}, { 2, 3, 0, 1}, { 2, 3, 1, 0},
  { 3, 0, 1, 2}, { 3, 0, 2, 1}, { 3, 1, 0, 2},
  { 3, 1, 2, 0}, { 3, 2, 0, 1}, { 3, 2, 1, 0},
};

unsigned char rank[4][4][4][4];

matrix m, aux, dest;
double p1, p2, p3;
int n;

void make_rank() {
  for (int i = 0; i < SIZE; i++) {
    perm p = UNRANK[i];
    rank[p.a][p.b][p.c][p.d] = i;
  }
}

void read_data(FILE* fin) {
  fscanf(fin, "%d %lf %lf %lf", &n, &p1, &p2, &p3);
}

int rot(int x) {
  perm p = UNRANK[x];
  return rank[p.d][p.a][p.b][p.c];
}

int mirror(int x) {
  perm p = UNRANK[x];
  return rank[p.d][p.c][p.b][p.a];
}

int swap(int x) {
  perm p = UNRANK[x];
  return rank[p.a][p.c][p.b][p.d];
}

void make_matrix() {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      m[i][j] = 0;
    }
    m[i][rot(i)] = p1;
    m[i][mirror(i)] = p2;
    m[i][swap(i)] = p3;
    m[i][i] = 1.0 - p1 - p2 - p3;
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

void copy(matrix src, matrix dest) {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      dest[r][c] = src[r][c];
    }
  }
}

// a *= b
void mult(matrix a, matrix b) {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      aux[r][c] = 0;
      for (int i = 0; i < SIZE; i++) {
        aux[r][c] += a[r][i] * b[i][c];
      }
    }
  }
  copy(aux, a);
}

void bin_exp(matrix a, int n, matrix dest) {
  identity(dest);

  while (n) {
    if (n & 1) {
      mult(dest, a);
    }
    mult(a, a);
    n >>= 1;
  }
}

int main() {
  make_rank();

  FILE* fin = fopen("hprob.in", "r");
  FILE* fout = fopen("hprob.out", "w");

  int num_tests;
  fscanf(fin, "%d", &num_tests);
  while (num_tests--) {
    read_data(fin);
    make_matrix();
    bin_exp(m, n, dest);
    fprintf(fout, "%.5lf\n", dest[0][0]);
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
