#include <stdio.h>

const int MAX_VAL = 1'000'000;

int factor[MAX_VAL + 1];
char u[MAX_VAL + 1];

// multiples[x] = cîte numere de la intrare se divid cu x
int multiples[MAX_VAL + 1];

void compute_smallest_factors() {
  factor[1] = 1;
  for (int i = 2; i <= MAX_VAL; i++) {
    if (!factor[i])  {
      for (int j = i; j <= MAX_VAL; j += i) {
        if (!factor[j]) {
          factor[j] = i;
        }
      }
    }
  }
}

void compute_mobius() {
  u[1] = 1;
  for (int i = 2; i <= MAX_VAL; i++) {
    if (factor[i] == factor[i / factor[i]]) {
      u[i] = 0;
    } else {
      u[i] = -u[i / factor[i]];
    }
  }
}

void read_data() {
  int n, x;

  scanf("%d", &n);
  while (n--) {
    scanf("%d", &x);
    multiples[x]++;
  }
}

void compute_multiples() {
  for (int d = 1; d <= MAX_VAL; d++) {
    for (int m = 2 * d; m <= MAX_VAL; m += d) {
      multiples[d] += multiples[m];
    }
  }
}

void count_coprime_triplets() {
  long long answer = 0;
  for (int gcd = 1; gcd <= MAX_VAL; gcd++) {
    if (u[gcd]) {
      int m = multiples[gcd];
      long long comb = (long long)m * (m - 1) * (m - 2) / 6;
      answer += (long long)u[gcd] * comb;
    }
  }
  printf("%lld\n", answer);
}

int main() {
  compute_smallest_factors();
  compute_mobius();
  read_data();
  compute_multiples();
  count_coprime_triplets();

  return 0;
}
