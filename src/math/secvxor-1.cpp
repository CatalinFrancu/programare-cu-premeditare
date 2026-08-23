#include <stdio.h>

const int MAX_N = 100'000;
const int MAX_VAL = 1'000'000;

// Pentru fiecare x, spf[x] reține cel mai mic factor prim al lui x, iar
// rest[x] reține cîtul după eliminarea tuturor factorilor minimi. Pentru a
// economisi memorie și a face spf[] să încapă pe short, prin convenție spf[x]
// = rest[x] = 0 cînd x este prim.
short spf[MAX_VAL + 1];
int rest[MAX_VAL + 1];

bool multiples[MAX_VAL + 1];
bool right[MAX_N];
int v[MAX_N], n;

void read_input_data() {
  FILE* f = fopen("secvxor.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &v[i]);
  }
  fclose(f);
}

void sieve() {
  for (int p = 2; p * p <= MAX_VAL; p++) {
    if (!spf[p]) {
      spf[p] = p;
      rest[p] = 1;
      for (int k = p, m = k * p; m <= MAX_VAL; k++, m += p) {
        if (!spf[m]) {
          spf[m] = p;
          rest[m] = (spf[k] == p) ? rest[k] : k;
        }
      }
    }
  }
}

void clear_multiples() {
  for (int i = 1; i <= MAX_VAL; i++) {
    multiples[i] = 0;
  }
}

// Vizitează toți divizorii liberi de pătrate ai lui val. Numără valorile care
// au divizori comuni cu val. Actualizează multiples[] după numărare.
bool count(int val, int prod) {
  if (val == 1) {
    int result = multiples[prod];
    if (prod > 1) {
      multiples[prod] ^= 1;
    }
    return result;
  } else {
    int s = spf[val] ? spf[val] : val;
    int r = rest[val] ? rest[val] : 1;
    return count(r, prod) ^ count(r, prod * s);
  }
}

void count_shared_factors_right() {
  for (int i = n - 1; i >= 0; i--) {
    right[i] = count(v[i], 1);
  }
}

int count_shared_factors_left() {
  int answer = 0;
  bool overlaps = false, left = false;

  for (int i = 0; i < n; i++) {
    overlaps ^= right[i] ^ left;
    answer ^= overlaps ? v[i] : 0;
    left = count(v[i], 1);
  }

  return answer;
}

void write_answer(int answer) {
  FILE* f = fopen("secvxor.out", "w");
  fprintf(f, "%d\n", answer);
  fclose(f);
}

int main() {
  read_input_data();
  sieve();
  count_shared_factors_right();
  clear_multiples();
  int answer = count_shared_factors_left();
  write_answer(answer);

  return 0;
}
