// Implementarea ideii din https://codeforces.com/contest/1665/submission/236496990
#include <stdio.h>

const int NUM_MODULI = 4;

struct congruence {
  int base;
  int exponent;
  int modulus;      // sinonim pentru base^exponent
  int max_exponent; // scopul final
  int rem;          // candidatul curent pentru a satisface x mod base^exponent

  congruence(int base, int max_exponent) {
    this->base = base;
    this->max_exponent = max_exponent;
  }

  void reset() {
    exponent = 1;
    modulus = base;
    // Testează candidații în ordine descrescătoare ca să evităm cazuri-limită
    // cu 0.
    rem = base - 1;
  }

  void advance(int gcd) {
    // Am reușit să satisfacem x mod base^exponent?
    bool found = false;
    if (gcd % modulus == 0) {
      found = true;
    } else if (rem < 2 * modulus / base) {
      // Prin eliminare, următorul candidat este satisfăcător.
      rem -= modulus / base;
      found = true;
    }

    if (found) {
      // Am învățat că rem este satisfăcător modulo modulus.
      exponent++;
      if (!is_solved()) {
        modulus *= base;
        // Ia-o de la început.
        rem += (base - 1) * modulus / base;
      }
    } else {
      rem -= modulus / base;
    }
  }

  bool is_solved() {
    return exponent > max_exponent;
  }

  // Pregătește congruența pentru CRT-ul final: x ≡ rem % modulus.
  void flip() {
    exponent = 0; // Marchează-l ca nerezolvat.
    rem = (modulus - rem) % modulus;
  }
};

congruence c[NUM_MODULI] = {
  congruence(2, 11),
  congruence(3, 6),
  congruence(5, 2),
  congruence(7, 2),
};

unsigned ask(unsigned a, unsigned b) {
  printf("? %u %u\n", a, b);
  fflush(stdout);

  int response;
  scanf("%d", &response);
  return response;
}

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

int inverse(int x, int mod) {
  int y, k, d;
  extended_euclid_iterative(x, mod, d, y, k);
  return (y >= 0) ? y : (y + mod);
}

bool finished() {
  bool all_solved = true;
  for (int i = 0; i < NUM_MODULI; i++) {
    all_solved &= c[i].is_solved();
  }
  return all_solved;
}

void solve_crt(unsigned& solution, unsigned& big_mod) {
  big_mod = 1;
  for (int i = 0; i < NUM_MODULI; i++) {
    if (!c[i].is_solved()) {
      big_mod *= c[i].modulus;
    }
  }

  solution = 0;
  for (int i = 0; i < NUM_MODULI; i++) {
    if (!c[i].is_solved()) {
      int prod_exclude = big_mod / c[i].modulus;
      int inv = inverse(prod_exclude, c[i].modulus);
      long long all = (long long)c[i].rem * prod_exclude % big_mod * inv;
      solution = (all + solution) % big_mod;
    }
  }
}

void learn_from_question() {
  unsigned common_rem, big_mod;
  solve_crt(common_rem, big_mod);
  int gcd = ask(common_rem, common_rem + big_mod);

  for (int i = 0; i < NUM_MODULI; i++) {
    if (!c[i].is_solved()) {
      c[i].advance(gcd);
    }
  }
}

void solve_test_case() {
  for (int i = 0; i < NUM_MODULI; i++) {
    c[i].reset();
  }

  while (!finished()) {
    learn_from_question();
  }

  for (int i = 0; i < NUM_MODULI; i++) {
    c[i].flip();
  }

  unsigned x, big_mod;
  solve_crt(x, big_mod);
  printf("! %u\n", x);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test_case();
  }

  return 0;
}
