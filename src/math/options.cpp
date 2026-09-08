#include <stdio.h>

const int MAX_ROWS = 100;
const int MOD = 666'013;

bool is_land[MAX_ROWS][MAX_ROWS];
int nr, nc, dest_r;
long long dest_c;

struct matrix {
  int a[MAX_ROWS][MAX_ROWS];

  void copy_from(matrix& other) {
    for (int r = 0; r < nr; r++) {
      for (int c = 0; c < nr; c++) {
        a[r][c] = other.a[r][c];
      }
    }
  }

  void identity() {
    for (int r = 0; r < nr; r++) {
      for (int c = 0; c < nr; c++) {
        a[r][c] = 0;
      }
      a[r][r] = 1;
    }
  }

  // self *= m. Nu modifică matricea m.
  void mult_self(matrix& m) {
    static matrix aux;

    for (int r = 0; r < nr; r++) {
      for (int c = 0; c < nr; c++) {
        long long x = 0;
        for (int i = 0; i < nr; i++) {
          x += (long long)a[r][i] * m.a[i][c];
        }
        aux.a[r][c] = x % MOD;
      }
    }
    this->copy_from(aux);
  }

  // Modifică matricea m.
  void power(matrix& m, long long n) {
    identity();
    while (n) {
      if (n & 1) {
        mult_self(m);
      }
      m.mult_self(m);
      n >>= 1;
    }
  }
};

matrix t;   // matricea de tranziție pentru perioade întregi
matrix rem; // matricea de tranziție pentru ultimele dest_c % nc zile

void read_data() {
  // Conceptual îmi este mai simplu să judec matricea de tranziție pentru
  // blocul de coloane [0, nc) astfel: t[r1][r2] = numărul de drumuri care
  // încep pe linia r1 *din stînga blocului* (deci coloana -1) și se termină
  // pe linia r2 de pe coloana nc-1. De aceea, la citire mult prima coloană
  // ultima.
  scanf("%d %d ", &nr, &nc);
  for (int r = 0; r < nr; r++) {
    is_land[r][nc - 1] = getchar() - '0';
    for (int c = 0; c < nc - 1; c++) {
      is_land[r][c] = getchar() - '0';
    }
    getchar();
  }
  scanf("%d %lld", &dest_r, &dest_c);
  dest_r--;    // pentru că lucrăm 0-based
  dest_c -= 2; // pentru că lucrăm 0-based și am rotit matricea
}

void compute_transition_matrices() {
  matrix new_t;

  // Matricea t înainte de a face vreun pas.
  t.identity();

  for (int c = 0; c < nc; c++) {
    for (int r1 = 0; r1 < nr; r1++) {
      for (int r2 = 0; r2 < nr; r2++) {
        if (is_land[r2][c]) {
          int up = (r2 > 0) ? t.a[r1][r2 - 1] : 0;
          int mid = t.a[r1][r2];
          int down = (r2 < nr - 1) ? t.a[r1][r2 + 1] : 0;
          new_t.a[r1][r2] = (up + mid + down) % MOD;
        } else {
          new_t.a[r1][r2] = 0;
        }
      }
    }
    t.copy_from(new_t);
    // Invariant: t este matricea după parcurgerea coloanelor [0...c].
    if (c == dest_c % nc) {
      rem.copy_from(t);
    }
  }
}

int count_ways() {
  matrix overall;
  overall.power(t, dest_c / nc);
  overall.mult_self(rem);

  return overall.a[0][dest_r];
}

int main() {
  read_data();
  compute_transition_matrices();
  int answer = count_ways();
  printf("%d\n", answer);

  return 0;
}
