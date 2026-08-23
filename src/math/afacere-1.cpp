// Complexitate: O(2^n * l + num_ext * l).
#include <stdio.h>

const int MAX_WORDS = 20;
const int MAX_LEN = 100;
const int MAX_EXTENSIONS = 100;
const int SIGMA = 26;
const int MOD = 1'000'000'007;

struct pie_data {
  char s[MAX_LEN + 1];
  int num_question_marks;

  void init(int len) {
    for (int i = 0; i < len; i++) {
      s[i] = '?';
    }
    num_question_marks = len;
  }

  // Adaugă un wildcard la setul curent de restricții.
  bool combine(pie_data src, char* w) {
    num_question_marks = src.num_question_marks;

    bool compatible = true;
    int i = 0;

    while (w[i] && compatible) {
      if (w[i] == '?') {
        // Nimic nu se schimbă. Dacă s[i] era '?' rămîne '?', dacă era impus
        // rămîne impus.
        s[i] = src.s[i];
      } else if (src.s[i] == '?') {
        // Pierdem o libertate pe coloana i: aveam '?', dar a venit o literă.
        s[i] = w[i];
        num_question_marks--;
      } else {
        s[i] = w[i];
        compatible = (src.s[i] == w[i]);
      }
      i++;
    }
    return compatible;
  }

};

char s[MAX_WORDS][MAX_LEN + 1];
int ext[MAX_EXTENSIONS];
int coefs[MAX_LEN + 1];
pie_data pd; // datele inițiale pentru pinex
int num_words, len, num_ext;

void read_data() {
  FILE* f = fopen("afacere.in", "r");
  fscanf(f, "%d %d %d", &num_words, &len, &num_ext);
  for (int i = 0; i < num_words; i++) {
    fscanf(f, "%s", s[i]);
  }
  for (int i = 0; i < num_ext; i++) {
    fscanf(f, "%d", &ext[i]);
  }
  fclose(f);
}

void pie_coefs(int k, int sign, pie_data& pd) {
  if (k == num_words) {
    coefs[pd.num_question_marks] += sign;
  } else {
    pie_coefs(k + 1, sign, pd);
    pie_data pd2;
    if (pd2.combine(pd, s[k])) {
      pie_coefs(k + 1, -sign, pd2);
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
  pd.init(len);
  pie_coefs(0, -1, pd);
  coefs[len]++; // Compensează pentru submulțimea vidă.
  evaluate_and_write();

  return 0;
}
