#include <algorithm>
#include <stdio.h>

const int MAX_BITS = 100;
const int MAX_QUERIES = 100'000;

struct query {
  long long pos;
  int orig_ind;
};

query q[MAX_QUERIES];
bool ans[MAX_QUERIES];
long long choose[MAX_BITS + 1][MAX_BITS + 1];
int n, num_queries;

void read_input_data() {
  FILE* f = fopen("trim.in", "r");
  fscanf(f, "%d %d", &n, &num_queries);
  for (int i = 0; i < num_queries; i++) {
    fscanf(f, "%lld", &q[i].pos);
    q[i].pos--;
    q[i].orig_ind = i;
  }
  fclose(f);
}

void precompute_combinations() {
  choose[0][0] = 1;
  for (int i = 1; i <= n; i++) {
    choose[i][0] = 1;
    for (int j = 1; j <= i; j++) {
      choose[i][j] = choose[i - 1][j] + choose[i - 1][j - 1];
    }
  }
}

void sort_queries() {
  std::sort(q, q + num_queries, [] (query a, query b) {
    return a.pos < b.pos;
  });
}

// Consideră toate măștile de n biți cu k biți de 1, unde biții 0 și n-1 sînt
// 1. Există C(n-2, k-2) astfel de măști. Din combinarea #comb (0-based)
// returnează bitul #pos (0-based, numărînd de la MSB spre LSB).
bool find_bit(int n, int k, long long comb, int pos) {
  if (pos == 0 || pos == n - 1) {
    return true;
  }

  n -= 2;
  k -= 2;
  pos--;

  while (pos--) {
    if (comb >= choose[n - 1][k]) {
      comb -= choose[n - 1][k];
      k--;
    }
    n--;
  }

  return (comb >= choose[n - 1][k]);
}

struct bit_stream {
  int qind;          // indicele interogării curente
  long long emitted; // numărul de biți emiși pînă acum

  // Grupul curent. De exemplu, pentru n = 5, vom emite grupurile:
  //
  // * numere cu 1 bit de 1 și de lățime 1
  // * numere cu 2 biți de 1 și de lățimi 2, 3, 4, 5
  // * numere cu 3 biți de 1 și de lățimi 3, 4, 5
  // * numere cu 4 biți de 1 și de lățimi 4, 5
  // * numere cu 5 biți de 1 și de lățime 5
  int num_bits, width;

  // Procesează separat numerele pe 1 bit. Există n astfel de numere.
  void answer_width_1() {
    while (qind < num_queries && q[qind].pos < n) {
      ans[q[qind++].orig_ind] = true;
    }
    emitted = n;
  }

  void advance_group() {
    if (width == n) {
      width = ++num_bits;
    } else {
      width++;
    }
  }

  void answer_queries_for_group() {
    long long combinations = choose[width - 2][num_bits - 2];
    int repetitions = n + 1 - width;
    long long comb_size = combinations * repetitions * width;

    while ((qind < num_queries) && (q[qind].pos < emitted + comb_size)) {
      long long offset = q[qind].pos - emitted;
      long long combination = offset / (repetitions * width);
      offset %= width;
      ans[q[qind++].orig_ind] = find_bit(width, num_bits, combination, offset);
    }

    emitted += comb_size;
  }

  // Emite biții în grupuri și răspunde la întrebări în ordine crescătoare.
  void emit() {
    qind = 0;
    num_bits = 1;
    width = n;

    answer_width_1();

    while (qind < num_queries) {
      advance_group();
      answer_queries_for_group();
    }
  }
};

bit_stream bs;

void write_answers() {
  FILE* f = fopen("trim.out", "w");
  for (int i = 0; i < num_queries; i++) {
    fputc(ans[i] + '0', f);
  }
  fputc('\n', f);
  fclose(f);
}

int main() {
  read_input_data();
  precompute_combinations();
  sort_queries();
  bs.emit();
  write_answers();

  return 0;
}
