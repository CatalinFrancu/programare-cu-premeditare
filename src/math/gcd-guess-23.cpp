// Metoda descrisă în https://codeforces.com/blog/entry/101663.
#include <stdio.h>

// 8 moduli coprimi între ei si produsul lor.
const int NUM_MODULI = 8;
const int MOD[NUM_MODULI] = { 16, 9, 7, 11, 13, 17, 19, 23 };
const int BIG_MOD = 1'070'845'776;

int coef[NUM_MODULI];
int rem[NUM_MODULI];

void extended_euclid_iterative(int a, int b, int& d, int& x, int& y) {
  x = 1;
  y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = x - q * xp; x = tmp;
    tmp = yp; yp = y - q * yp; y = tmp;
  }
  d = a;
}

// Folosește algoritmul lui Euclid extins, nu algoritmul lui Fermat, deoarece
// unii dintre moduli sînt compuși.
int inverse(int x, int mod) {
  int y, k, d;
  extended_euclid_iterative(x, mod, d, y, k);
  return (y >= 0) ? y : (y + mod);
}

// Precalculează partea comună a CRT (coeficientul fiecărui modul).
void precompute() {
  for (int i = 0; i < NUM_MODULI; i++) {
    int prod_exclude = BIG_MOD / MOD[i];
    int inv = inverse(prod_exclude, MOD[i]);
    coef[i] = (long long)prod_exclude * inv % BIG_MOD;
  }
}

unsigned ask(unsigned a, unsigned b) {
  printf("? %u %u\n", a, b);
  fflush(stdout);

  int response;
  scanf("%d", &response);
  return response;
}

int solve_crt() {
  long long result = 0;
  for (int i = 0; i < NUM_MODULI; i++) {
    result += (long long)rem[i] * coef[i];
  }
  return result % BIG_MOD;
}

void solve_test_case() {
  for (int a = 23; a >= 1; a--) {
    int gcd = ask(a, a + BIG_MOD);
    for (int i = 0; i < NUM_MODULI; i++) {
      if (gcd % MOD[i] == 0) {
        // Poate fi negativ pentru valori mari ale lui a, dar vom găsi unul
        // pozitiv mai tîrziu.
        rem[i] = MOD[i] - a;
        // printf("Am aflat că x ≡ %d mod %d\n", rem[i], MOD[i]);
      }
    }
  }

  int solution = solve_crt();
  printf("! %u\n", solution);
}

int main() {
  precompute();

  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test_case();
  }

  return 0;
}
