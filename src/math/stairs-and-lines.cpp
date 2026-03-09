#include <stdio.h>

const int HEIGHT = 7;
const int SIZE = 1 << HEIGHT;
const int MOD = 1'000'000'007;

typedef int matrix[SIZE][SIZE];

matrix t;   // matricea de tranziție
matrix acc; // acumulatorul
matrix aux;

void make_transition_matrix(int h) {
  // Forțează pereți verticali de la h la HEIGHT - 1.
  int start_mask = SIZE - (1 << h);

  for (int left = start_mask; left < SIZE; left++) {
    for (int right = start_mask; right < SIZE; right++) {
      t[left][right] = 0;
      // Forțează pereți orizontali la înălțimile 0 și h.
      for (int horiz = 1 + (1 << h); horiz < (1 << (h + 1)); horiz += 2) {
        if ((left & right & horiz & (horiz >> 1)) == 0) {
          t[left][right]++;
        }
      }
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

void copy(matrix src, matrix dest) {
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      dest[r][c] = src[r][c];
    }
  }
}

// a *= b
void mult(matrix a, matrix b, int start) {
  for (int r = start; r < SIZE; r++) {
    for (int c = start; c < SIZE; c++) {
      __int128 x = 0;
      for (int i = 0; i < SIZE; i++) {
        x += (long long)a[r][i] * b[i][c];
      }
      aux[r][c] = x % MOD;
    }
  }
  copy(aux, a);
}

// Matricea a este goală pe primele start linii și coloane.
void bin_exp(matrix a, int n, int start) {
  while (n) {
    if (n & 1) {
      mult(acc, a, start);
    }
    mult(a, a, start);
    n >>= 1;
  }
}

int main() {
  int w;

  identity(acc);
  for (int h = 1; h <= HEIGHT; h++) {
    scanf("%d", &w);
    make_transition_matrix(h);
    bin_exp(t, w, SIZE - (1 << h));
  }

  printf("%d\n", acc[SIZE - 1][SIZE - 1]);
}
