#include <stdio.h>

const int MAX_N = 3000;

struct max_info {
  int val, cnt;
};

typedef max_info mat[MAX_N][MAX_N];

mat sq[2], tr;
int orig[MAX_N][MAX_N];
int nr, nc, k;

void read_data() {
  scanf("%d %d %d", &nr, &nc, &k);

  // Citește liniile în ordine inversă astfel încît triunghiurile să se
  // extindă în ordinea crescătoare a liniilor și coloanelor.
  for (int r = nr - 1; r >= 0; r--) {
    for (int c = 0; c < nc; c++) {
      scanf("%d", &orig[r][c]);
      tr[r][c] = sq[0][r][c] = { orig[r][c], 1};
    }
  }
}

bool odd(int x) {
  return (x & 1) == 1;
}

bool even(int x) {
  return x && ((x & 1) == 0);
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

max_info max3(max_info a, max_info b, max_info c) {
  int m = max(max(a.val, b.val), c.val);

  return {
    m,
    a.cnt * (a.val == m) +
    b.cnt * (b.val == m) +
    c.cnt * (c.val == m)
  };
}

max_info max4(max_info a, max_info b, max_info c, max_info d) {
  int m = max(max(max(a.val, b.val), c.val), d.val);

  return {
    m,
    a.cnt * (a.val == m) +
    b.cnt * (b.val == m) +
    c.cnt * (c.val == m) +
    d.cnt * (d.val == m)
  };
}

void combine_triangles(int size_tr, mat sq, int size_sq) {
  for (int r = 0; r <= nr - (size_tr + size_sq); r++) {
    for (int c = 0; c <= nc - (size_tr + size_sq); c++) {
      tr[r][c] = max3(sq[r][c],
                      tr[r][c + size_sq],
                      tr[r + size_sq][c]);
    }
  }
}

void combine_equal_squares(mat src, int size, mat dest) {
  for (int r = 0; r <= nr - 2 * size; r++) {
    for (int c = 0; c <= nc - 2 * size; c++) {
      dest[r][c] = max4(src[r][c],
                        src[r][c + size],
                        src[r + size][c],
                        src[r + size][c + size]);
    }
  }
}

// Calculează 11x11 din 6x6 în colțurile din stînga-sus și dreapta-jos și 5x5
// în colțurile din dreapta-sus și stînga jos. Centrul este numărat dublu.
void combine_unequal_squares(mat sm, mat lg, int size, mat dest) {
  for (int r = 0; r <= nr - (2 * size + 1); r++) {
    for (int c = 0; c <= nc - (2 * size + 1); c++) {
      dest[r][c] = max4(lg[r][c],
                        sm[r][c + size + 1],
                        sm[r + size + 1][c],
                        lg[r + size][c + size]);
      if (orig[r + size][c + size] == dest[r][c].val) {
        dest[r][c].cnt--;
      }
    }
  }
}

// Calculează toate triunghiurile de mărime t și pătratele de mărimi s0 și s1.
// s0 și s1 pot fi 0. Dacă ambele sînt nonzero, atunci s1 = s0 + 1.
// Triunghiurile sînt formate din două triunghiuri mai mici și un pătrat.
// Pătratele de latură pară sînt formate din patru pătrate mai mici. Pătratele
// impare sînt formate din două perechi de pătrate mai mici (jumătatea
// rotunjită în sus și respectiv în jos).
//
// De exemplu, t=183 se sparge în următoarele straturi. Din fiecare strat mai
// mic îl aflăm pe următorul mai mare.
//
//   compute_all(183,  0,  0)
//   compute_all( 91, 92,  0)
//   compute_all( 45, 46,  0)
//   compute_all( 22, 23,  0)
//   compute_all( 11, 11, 12)
//   compute_all(  5,  5,  6)
//   compute_all(  2,  2,  3)
//   compute_all(  1,  1,  2)
void compute_all(int t, int s0, int s1) {
  if (t == 1) {
    if (s0 == 2) {
      combine_equal_squares(sq[0], 1, sq[0]);
    } else if (s1 == 2) {
      combine_equal_squares(sq[0], 1, sq[1]);
    }
    return;
  }

  int rec_t = t / 2;
  int rec_s0 = s0 ? (s0 / 2) : ((t + 1) / 2);
  int rec_s1 = s1 ? ((s1 + 1) / 2) : (odd(s0) ? (rec_s0 + 1) : 0);
  compute_all(rec_t, rec_s0, rec_s1);

  mat& sq_for_tri = (rec_t + rec_s0 == t) ? sq[0] : sq[1];
  combine_triangles(rec_t, sq_for_tri, t - rec_t);

  // De ex. combină pătrate de laturi 10 și 11 pentru a obține pătrate de
  // laturi 20 și 21. Deoarece suprascriem, trebuie să procesăm latura impară
  // prima.
  int odd_idx = odd(s0) ? 0 : (odd(s1) ? 1 : -1);
  if (odd_idx != -1) {
    combine_unequal_squares(sq[0], sq[1], rec_s0, sq[odd_idx]);
  }

  if (even(s0)) {
    combine_equal_squares(sq[0], rec_s0, sq[0]);
  } else if (even(s1)) {
    combine_equal_squares(sq[1], rec_s1, sq[1]);
  }
}

void write_results() {
  // Răstoarnă din nou liniile în vederea tipăririi.
  for (int r = nr - k; r >= 0; r--) {
    for (int c = 0; c <= nc - k; c++) {
      printf("%d ", tr[r][c].val);
    }
    putchar('\n');
  }
  for (int r = nr - k; r >= 0; r--) {
    for (int c = 0; c <= nc - k; c++) {
      printf("%d ", tr[r][c].cnt);
    }
    putchar('\n');
  }
}

int main() {
  read_data();
  compute_all(k, 0, 0);
  write_results();

  return 0;
}
