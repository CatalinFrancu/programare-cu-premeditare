// Complexitate: O(n^2 * l + 2^n + num_ext * l).
#include <stdio.h>

const int MAX_WORDS = 20;
const int MAX_LEN = 100;
const int MAX_EXTENSIONS = 100;
const int SIGMA = 26;
const int MOD = 1'000'000'007;

typedef unsigned __int128 u128;

struct wildcard {
  char s[MAX_LEN + 1];
  int compat;  // mască de compatibilitate cu celelalte wildcards
  u128 qmask;  // mască cu pozițiile caracterelor '?'
};

int ext[MAX_EXTENSIONS];
int coefs[MAX_LEN + 1];
wildcard w[MAX_WORDS];
int num_words, len, num_ext;

void read_data() {
  FILE* f = fopen("afacere.in", "r");
  fscanf(f, "%d %d %d", &num_words, &len, &num_ext);
  for (int i = 0; i < num_words; i++) {
    fscanf(f, "%s", w[i].s);
  }
  for (int i = 0; i < num_ext; i++) {
    fscanf(f, "%d", &ext[i]);
  }
  fclose(f);
}

void precompute_qmask() {
  for (int i = 0; i < num_words; i++) {
    for (int j = 0; j < len; j++) {
      if (w[i].s[j] == '?') {
        w[i].qmask |= (u128)1 << j;
      }
    }
  }
}

bool compatible_symbols(char a, char b) {
  return (a == '?') || (b == '?') || (a == b);
}

void precompute_compat() {
  for (int i = 0; i < num_words - 1; i++) {
    for (int j = i + 1; j < num_words; j++) {
      int k = 0;
      while ((k < len) && compatible_symbols(w[i].s[k], w[j].s[k])) {
        k++;
      }

      if (k == len) {
        w[i].compat |= 1 << j;
        w[j].compat |= 1 << i;
      }
    }
  }
}

int popcount(u128 x) {
  return
    __builtin_popcountll(x & 0xffffffffffffffffull) +
    __builtin_popcountll(x >> 64);
}

void pie_coefs(int k, int sign, int wmask, u128 qmask) {
  if (k == num_words) {
    coefs[popcount(qmask)] += sign;
  } else {
    pie_coefs(k + 1, sign, wmask, qmask);
    if ((wmask & w[k].compat) == wmask) {
      pie_coefs(k + 1, -sign, wmask | (1 << k), qmask & w[k].qmask);
    }
  }
}

int evaluate(int sigma) {
  long long poly_val = 0;
  long long sigma_pow = 1;
  for (int i = 0; i <= len; i++) {
    poly_val = (poly_val + sigma_pow * coefs[i]) % MOD;
    sigma_pow = sigma_pow * sigma % MOD;
  }
  return poly_val;
}

void evaluate_and_write() {
  FILE* f = fopen("afacere.out", "w");
  for (int i = 0; i < num_ext; i++) {
    fprintf(f, "%d\n", evaluate(SIGMA + ext[i]));
  }
  fclose(f);
}

int main() {
  read_data();
  precompute_qmask();
  precompute_compat();
  pie_coefs(0, -1, 0, ((u128)1 << len) - 1);
  coefs[len]++; // Compensează pentru submulțimea vidă.
  evaluate_and_write();

  return 0;
}
