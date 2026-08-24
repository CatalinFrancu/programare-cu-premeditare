#include <stdio.h>

const int MAX_VAL = 1'000'000;
const int MAX_FACTORS = 7;

// spf[x] = cel mai mic divizor al lui x
int spf[MAX_VAL + 1];

// num_mult[x] = numărul de multipli de x întîlniți pînă acum
int num_mult[MAX_VAL + 1];

// factorizarea numărului în curs de procesare
int factors[MAX_FACTORS], num_factors;

void compute_smallest_factors() {
  spf[1] = 1;
  for (int i = 2; i <= MAX_VAL; i++) {
    if (!spf[i])  {
      for (int j = i; j <= MAX_VAL; j += i) {
        if (!spf[j]) {
          spf[j] = i;
        }
      }
    }
  }
}

void factorize(int x) {
  num_factors = 0;
  while (x != 1) {
    if (!num_factors || factors[num_factors - 1] != spf[x]) {
      factors[num_factors++] = spf[x];
    }
    x /= spf[x];
  }
}

// Dată fiind factorizarea unui număr liber de pătrate, numără cîte perechi
// coprime formează cu numere anterioare. Folosește pinex. De exemplu, dacă
// numărul este 14, el va fi coprim cu
//
// * toate numerele anterioare,
// * minus multiplii de 2 anteriori,
// * minus multiplii de 7 anteriori,
// * plus multiplii de 14 anteriori.
//
// Incrementează contoarele pentru fiecare dintre divizorii numărului.
long long pinex(int k, int sign, int prod_so_far) {
  if (k == num_factors) {
    return sign * num_mult[prod_so_far]++; // numără, apoi adaugă.
  }

  return
    pinex(k + 1, sign, prod_so_far) +
    pinex(k + 1, -sign, prod_so_far * factors[k]);
}

long long count_coprime_pairs() {
  long long result = 0;
  int n, x;
  scanf("%d", &n);

  while (n--) {
    scanf("%d", &x);
    factorize(x);
    result += pinex(0, +1, 1);
  }

  return result;
}

void write_answer(long long answer) {
  printf("%lld\n", answer);
}

int main() {
  compute_smallest_factors();
  long long answer = count_coprime_pairs();
  write_answer(answer);

  return 0;
}
