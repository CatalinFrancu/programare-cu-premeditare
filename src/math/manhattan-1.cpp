#include <algorithm>
#include <stdio.h>

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

// Numără căile de la linia (0,0)-(w,0) la zona (B.x1, d1)-(B.x2, d2).
// Presupune că 0 ≤ d1 ≤ d2.
int count_line_to_zone_paths(int d1, int d2) {
  int x1 = b.x1, x2 = b.x2, w = a.x2;

  int result =
    + (comb(x2 + d2 + 3, x2 + 1) - comb(x2 + d1 + 2, x2 + 1))
    - (comb(x2 + d2 + 2 - w, x2 - w) - comb(x2 + d1 + 1 - w, x2 - w))
    - (comb(x1 + d2 + 2, x1) - comb(x1 + d1 + 1, x1))
    + (comb(x1 + d2 + 1 - w, x1 - 1 - w) - comb(x1 + d1 - w, x1 - 1 - w));

  return (result + 8 * MOD) % MOD;
}

// Numără drumurile între oricare linie din A și zona B, spărgînd zona B în
// două dacă linia din A o intersectează.
int count_paths() {
  long long result = 0;

  for (int h = a.y1; h <= a.y2; h++) {
    if (h <= b.y1) {
      result += count_line_to_zone_paths(b.y1 - h, b.y2 - h);
    } else if (h >= b.y2) {
      result += count_line_to_zone_paths(h - b.y2, h - b.y1);
    } else {
      result += count_line_to_zone_paths(0, b.y2 - h);
      result += count_line_to_zone_paths(1, h - b.y1);
    }
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
