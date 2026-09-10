#include <stdio.h>
#include <string.h>

const int SIGMA = 26;
const int MAX_LENGTH = 10'000;
const int NUM_MODULI = 3;
const int MOD[NUM_MODULI] = { 18, 23, 25 }; // de ce nu?
const int BIG_MOD = 18 * 23 * 25;

typedef char str[MAX_LENGTH + 1];

str orig, t, query, ans[NUM_MODULI];
int n;
int coef[NUM_MODULI], rem[NUM_MODULI];

struct euclid {
  int d, x, y;
};

euclid extended_euclid(int a, int b) {
  euclid e;
  e.x = 1;
  e.y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = e.x - q * xp; e.x = tmp;
    tmp = yp; yp = e.y - q * yp; e.y = tmp;
  }
  e.d = a;
  return e;
}

int inverse(int a, int mod) {
  euclid e = extended_euclid(a, mod);
  return (e.x >= 0) ? e.x : (e.x + mod);
}

// Precalculează partea comună a CRT (coeficientul fiecărui modul).
void precompute() {
  for (int i = 0; i < NUM_MODULI; i++) {
    int prod_exclude = BIG_MOD / MOD[i];
    int inv = inverse(prod_exclude % MOD[i], MOD[i]);
    coef[i] = (long long)prod_exclude * inv % BIG_MOD;
  }
}

int solve_crt() {
  long long result = 0;
  for (int i = 0; i < NUM_MODULI; i++) {
    result += (long long)rem[i] * coef[i];
  }
  return result % BIG_MOD;
}

void read_data() {
  scanf("%s", t);
  n = strlen(t);
}

void build_query(int modulo) {
  for (int i = 0; i < n; i++) {
    query[i] = i % modulo + 'a';
  }
}

void send_queries() {
  for (int i = 0; i < NUM_MODULI; i++) {
    build_query(MOD[i]);
    printf("? %s\n", query);
    fflush(stdout);
    scanf(" %s", ans[i]);
  }
}

void restore_orig() {
  for (int i = 0; i < n; i++) {
    for (int m = 0; m < NUM_MODULI; m++) {
      rem[m] = ans[m][i] - 'a';
    }
    int pos = solve_crt();
    orig[pos] = t[i];
  }
}

void write_answer() {
  printf("! %s\n", orig);
}

int main() {
  precompute();
  read_data();
  send_queries();
  restore_orig();
  write_answer();

  return 0;
}
