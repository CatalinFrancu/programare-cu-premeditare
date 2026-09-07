#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

const int MAX_COMB = 200'000;
const int MOD = 666'013;

struct zone {
  int x1, y1, x2, y2;

  void translate(int dx) {
    x1 += dx;
    x2 += dx;
  }

  void mirror_diag() {
    std::swap(x1, y1);
    std::swap(x2, y2);
  }
};

zone a, b;
int fact[MAX_COMB + 1], inv_fact[MAX_COMB + 1];

void read_zones() {
  FILE* f = fopen("manhattan.in", "r");
  fscanf(f, "%d %d %d %d", &a.x1, &a.y1, &a.x2, &a.y2);
  fscanf(f, "%d %d %d %d", &b.x1, &b.y1, &b.x2, &b.y2);
  fclose(f);
}

void normalize_zones() {
  // Asigură-te că există un x care separă zonele.
  if ((a.y1 > b.y2) || (b.y1 > a.y2)) {
    a.mirror_diag();
    b.mirror_diag();
  }

  // Adu zona A în stînga.
  if (a.x1 > b.x1) {
    std::swap(a, b);
  }

  // Adu latura stîngă a lui A pe axa Oy.
  b.translate(-a.x1);
  a.translate(-a.x1);

  // Acum B este complet la dreapta lui A.
}

// Calculează x^{-1} % MOD ca x^{MOD-2} % MOD.
int inverse(int x) {
  long long result = 1;
  int e = MOD - 2;

  while (e) {
    if (e & 1) {
      result = result * x % MOD;
    }
    x = (long long)x * x % MOD;
    e >>= 1;
  }

  return result;
}

void compute_factorials() {
  fact[0] = 1;
  for (int i = 1; i <= MAX_COMB; i++) {
    fact[i] = (long long)i * fact[i - 1] % MOD;
  }

  inv_fact[MAX_COMB] = inverse(fact[MAX_COMB]);
  for (int i = MAX_COMB - 1; i >= 0; i--) {
    inv_fact[i] = (long long)(i + 1) * inv_fact[i + 1] % MOD;
  }
}

int comb(int n, int k) {
  return (long long)fact[n] * inv_fact[k] * inv_fact[n - k] % MOD;
}

// Numără drumurile de la linia (0,0)-(A.x2,0) la linia (A.x1,d)-(A.x2,d)
int count_paths_dist(int d) {
  return (
    + comb(b.x2 + d + 2, d + 2)
    - comb(b.x2 + d + 1 - a.x2, d + 2)
    - comb(b.x1 + d + 1, d + 2)
    + comb(b.x1 + d - a.x2, d + 2)
    + 2 * MOD) % MOD;
}

// Numără perechile de linii din A și B aflate la distanță d pe verticală.
int line_pairs_having_dist(int d) {
  int y1 = b.y1 - d, y2 = b.y2 - d;
  // Reuniunea intervalelor (A.y1,A.y2) și (y1,y2).
  return std::max(std::min(y2, a.y2) - std::max(y1, a.y1) + 1, 0);
}

// Numără drumurile între oricare linie din A și oricare linie din B, grupînd
// perechile de linii după distanța dintre ele.
int count_paths() {
  long long result = 0;

  for (int d = b.y1 - a.y2; d <= b.y2 - a.y1; d++) {
    result += (long long)line_pairs_having_dist(d) *
      count_paths_dist(abs(d)) % MOD;
  }

  return result % MOD;
}

void write_answer(int answer) {
  FILE* f = fopen("manhattan.out", "w");
  fprintf(f, "%d\n", answer);
  fclose(f);
}

int main() {
  read_zones();
  normalize_zones();
  compute_factorials();
  int answer = count_paths();
  write_answer(answer);

  return 0;
}
