#include <stdio.h>

const int MAX_N = 19;
const int MAX_K = 4;
const int MOD = 40'099;

// Fiecare celulă codifică o combinație (l,r). De aceea mărimile sînt
// pătratice.
typedef int vector[MAX_K * MAX_K];
typedef int matrix[MAX_K * MAX_K][MAX_K * MAX_K];

int z, k, n;
int daily_demand[MAX_N + 1];

// Funcții utilitare. Sînt plicticoase. Dar tocmai asta este ideea.
// Programele noastre nu trebuie să fie ezoterice. Nu trebuie să existe
// obscuritate intenționată. Nu trebuie să existe surprize. Programul
// propriu-zis este destul de dificil fără să-l mai și poluăm cu aceste
// blocuri. Dacă vreți senzații tari, dați-vă în roller coaster.

void vec_zero(vector a) {
  for (int i = 0; i < k * k; i++) {
    a[i] = 0;
  }
}

int vec_sum(vector a) {
  int sum = 0;
  for (int i = 0; i < k * k; i++) {
    sum += a[i];
  }

  return sum % MOD;
}

void vec_copy(vector src, vector dest) {
  for (int i = 0; i < k * k; i++) {
    dest[i] = src[i];
  }
}

void mat_copy(matrix src, matrix dest) {
  for (int r = 0; r < k * k; r++) {
    vec_copy(src[r], dest[r]);
  }
}

void mat_identity(matrix a) {
  for (int r = 0; r < k * k; r++) {
    vec_zero(a[r]);
    a[r][r] = 1;
  }
}

// a *= b
void mat_mult(matrix a, matrix b) {
  static matrix aux;

  for (int r = 0; r < k * k; r++) {
    for (int c = 0; c < k * k; c++) {
      long long x = 0;
      for (int i = 0; i < k * k; i++) {
        x += a[r][i] * b[i][c];
      }
      aux[r][c] = x % MOD;
    }
  }
  mat_copy(aux, a);
}

// Îl distruge pe a.
void mat_power(matrix a, int n, matrix dest) {
  mat_identity(dest);
  while (n) {
    if (n & 1) {
      mat_mult(dest, a);
    }
    mat_mult(a, a);
    n >>= 1;
  }
}

void read_data() {
  FILE* f = fopen("marfa.in", "r");
  fscanf(f, "%d %d %d", &z, &k, &n);

  for (int i = 1; i <= n; i++) {
    fscanf(f, "%d", &daily_demand[i]);
  }
  fclose(f);
}

// Date fiind numerele de posibilități pentru fiecare (l < k, r < k),
// calculează noile numere după o zi cu cererea dată.
void advance_day(vector a, int demand) {
  static vector aux;
  vec_zero(aux);

  for (int l = 0; l < k; l++) {
    for (int r = 0; r < k; r++) {
      int q = a[l * k + r];
      if (demand == 0) {
        // resetează l și r la 0
        aux[0] += q;
      } else if (demand == 1) {
        // crește-l pe l și resetează-l pe r sau viceversa
        if (l < k - 1) {
          aux[(l + 1) * k] += q;
        }
        if (r < k - 1) {
          aux[r + 1] += q;
        }
      } else {
        // crește-le și pe l și pe r
        if ((l < k - 1) && (r < k - 1)) {
          aux[(l + 1) * k + (r + 1)] += q;
        }
      }
    }
  }

  vec_copy(aux, a);
}

void make_transition_matrices(matrix week, matrix rem) {
  vector a;

  for (int l = 0; l < k; l++) {
    for (int r = 0; r < k; r++) {
      vec_zero(a);
      a[l * k + r] = 1;

      for (int dow = 1; dow <= n; dow++) { // ziua din săptămînă
        advance_day(a, daily_demand[dow]);
        if (dow == z % n) {
          vec_copy(a, rem[l * k + r]);
        }
      }

      vec_copy(a, week[l * k + r]);
    }
  }
}

int count_ways() {
  matrix week, rem, all;

  make_transition_matrices(week, rem);
  mat_power(week, z / n, all);
  if (z % n) {
    mat_mult(all, rem);
  }

  // Începem în starea 0 și putem termina în orice stare.
  return vec_sum(all[0]);
}

void write_answer(int answer) {
  FILE* f = fopen("marfa.out", "w");
  fprintf(f, "%d\n", answer);
  fclose(f);
}

int main() {
  read_data();
  int answer = count_ways();
  write_answer(answer);

  return 0;
}
