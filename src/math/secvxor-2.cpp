#include <assert.h>
#include <stdio.h>

const int MAX_N = 100'000;
const int MAX_VAL = 1'000'000;

// Pentru fiecare x, spf[x] reține cel mai mic factor prim al lui x, iar
// rest[x] reține cîtul după eliminarea tuturor factorilor minimi. Pentru a
// economisi memorie și a face spf[] să încapă pe short, prin convenție spf[x]
// = rest[x] = 0 cînd x este prim.
short spf[MAX_VAL + 1];
int rest[MAX_VAL + 1];

bool mult[MAX_VAL + 1];
bool freq[MAX_VAL + 1];
bool gmult[MAX_VAL + 1];
int v[MAX_N], n;

void read_input_data() {
  FILE* f = fopen("secvxor.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &v[i]);
    freq[v[i]] ^= 1;
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

void compute_gmult() {
  for (int d = 2; d <= MAX_VAL; d++) {
    for (int m = d; m <= MAX_VAL; m += d) {
      gmult[d] ^= freq[m];
    }
  }
}

// Vizitează toți divizorii liberi de pătrate ai lui val. Numără valorile la
// stînga și la dreapta lui val care au divizori comuni cu val. Actualizează
// mult[] după numărare.
void count(int val, int prod, bool& left, bool& right) {
  if (val == 1) {
    left ^= mult[prod];
    if (prod > 1) {
      mult[prod] ^= 1;
    }
    right ^= gmult[prod] ^ mult[prod];
  } else {
    int s = spf[val] ? spf[val] : val;
    int r = rest[val] ? rest[val] : 1;
    count(r, prod, left, right);
    count(r, prod * s, left, right);
  }
}

int count_sequences() {
  int answer = 0;
  bool overlaps = false, prev_left = false;

  for (int i = 0; i < n; i++) {
    bool left = false, right = false;
    count(v[i], 1, left, right);
    overlaps ^= prev_left ^ right;
    answer ^= overlaps ? v[i] : 0;
    prev_left = left;
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
  compute_gmult();
  int answer = count_sequences();
  write_answer(answer);

  return 0;
}
