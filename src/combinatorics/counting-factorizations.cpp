// Complexitate: O(n^2).
#include <stdio.h>

const int MAX_N = 2'022;
const int MAX_VAL = 1'000'000;
const int MOD = 998'244'353;

short freq[MAX_VAL + 1];
short pfreq[2 * MAX_N]; // frecvența numerelor prime
int fact[2 * MAX_N + 1], inv_fact[2 * MAX_N + 1];
int d[2 * MAX_N + 1];   // spațiu pentru programarea dinamică
int n;
int num_primes;         // distincte
long long comp_contrib; // contribuția de la numere compuse

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < 2 * n; i++) {
    int x;
    scanf("%d", &x);
    freq[x]++;
  }
}

int bin_exp(int b, int e) {
  long long result = 1;

  while (e) {
    if (e & 1) {
      result = result * b % MOD;
    }
    b = (long long)b * b % MOD;
    e >>= 1;
  }

  return result;
}

void compute_fact() {
  fact[0] = 1;
  for (int i = 1; i <= 2 * n; i++) {
    fact[i] = (long long)i * fact[i - 1] % MOD;
  }

  inv_fact[2 * n] = bin_exp(fact[2 * n], MOD - 2);
  for (int i = 2 * n - 1; i >= 0; i--) {
    inv_fact[i] = (long long)(i + 1) * inv_fact[i + 1] % MOD;
  }
}

bool is_prime(int x) {
  for (int d = 2; d * d <= x; d++) {
    if (x % d == 0) {
      return false;
    }
  }

  return (x != 1);
}

void collect_primes() {
  comp_contrib = fact[n];
  for (int i = 1; i <= MAX_VAL; i++) {
    if (freq[i]) {
      if (is_prime(i)) {
        pfreq[num_primes++] = freq[i];
      } else {
        comp_contrib = comp_contrib * inv_fact[freq[i]] % MOD;
      }
    }
  }
}

// d[i][j] = Suma contribuțiilor dacă alegem j prime începînd cu poziția i.
void build_dp() {
  d[0] = 1;

  for (int i = num_primes - 1; i >= 0; i--) {
    // Există num_primes - i numere prime de la i la num_primes - 1.
    for (int j = num_primes - i; j; j--) {
      int no_take = (long long)inv_fact[pfreq[i]] * d[j] % MOD;
      int take = (long long)inv_fact[pfreq[i] - 1] * d[j - 1] % MOD;
      d[j] = (take + no_take) % MOD;
    }

    d[0] = (long long)inv_fact[pfreq[i]] * d[0] % MOD;
  }
}

void wrap_it_up() {
  int answer = comp_contrib * d[n] % MOD;
  printf("%d\n", answer);
}

int main() {
  read_data();
  compute_fact();
  collect_primes();
  build_dp();
  wrap_it_up();

  return 0;
}
