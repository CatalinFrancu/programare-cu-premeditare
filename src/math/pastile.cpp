#include <stdio.h>

const int MAX_VAL = 200'000;
const int MOD = 1'000'000'007;

char mobius[MAX_VAL + 1];

// lte[x] = numărul de nuanțe avînd ≤ x pastile
int lte[MAX_VAL + 1], min_val, max_val;

int min(int x, int y) {
  return (x < y) ? x :  y;
}

int max(int x, int y) {
  return (x > y) ? x :  y;
}

void read_frequencies() {
  int n, x;
  min_val = MAX_VAL + 1;
  max_val = 0;

  scanf("%d", &n);
  while (n--) {
    scanf("%d", &x);
    lte[x]++;
    min_val = min(min_val, x);
    max_val = max(max_val, x);
  }
}

void partial_sum_frequencies() {
  for (int i = min_val + 1; i <= max_val; i++) {
    lte[i] += lte[i - 1];
  }
}

// Lentă, dar nediferențiabilă pînă la MAX_VAL.
void compute_mobius(int n) {
  mobius[1] = 1;
  for (int i = 1; i <= n; i++) {
    if (mobius[i]) {
      for (int j = 2 * i; j <= n; j += i) {
        mobius[j] -= mobius[i];
      }
    }
  }
}

int mod_pow(long long b, int e) {
  long long result = 1;

  while (e) {
    if (e & 1) {
      result = result * b % MOD;
    }
    b = b * b % MOD;
    e >>= 1;
  }

  return result;
}

long long count_combos_having_gcd(int gcd) {
  long long prod = 1;

  // Frecvențele de la 2 × gcd pînă la 3 × gcd - 1 oferă 2 variante.
  // Frecvențele de la 3 × gcd pînă la 4 × gcd - 1 oferă 3 variante etc.
  for (int c = 2; c * gcd <= max_val; c++) {
    int start = c * gcd;
    int end = min(start + gcd - 1, max_val);
    int shades = lte[end] - lte[start - 1];
    prod = prod * mod_pow(c, shades) % MOD;
  }

  return prod;
}

int count_coprime_combos() {
  long long result = 0;
  for (int gcd = 1; gcd <= min_val; gcd++) {
    if (mobius[gcd]) {
      result += count_combos_having_gcd(gcd) * mobius[gcd] + MOD;
    }
  }
  return result % MOD;
}

void write_answer(int answer) {
  printf("%d\n", answer);
}

int main() {
  read_frequencies();
  partial_sum_frequencies();
  compute_mobius(min_val);
  int answer = count_coprime_combos();
  write_answer(answer);

  return 0;
}
