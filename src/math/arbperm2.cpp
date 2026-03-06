// Complexitate: O(n log n).
#include <stdio.h>

const int MAX_N = 100'000;

struct fenwick_tree {
  int v[MAX_N + 1];
  int n, max_p2;

  void init(int n) {
    this->n = n;
    max_p2 = 1 << (31 - __builtin_clz(n));
    for (int i = 1; i <= n; i++) {
      v[i] = 0;
    }
  }

  void set(int pos) {
    do {
      v[pos]++;
      pos += pos & -pos;
    } while (pos <= n);
  }

  int prefix_sum(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }

  // Returnează ultima poziție unde numărul de zerouri ≤ k, datorită faptului
  // că AIB-ul este 1-based, pe cînd logica programului este 0-based.
  int get_kth_zero(int k) {
    int pos = 0;

    for (int size = max_p2; size; size >>= 1) {
      if ((pos + size <= n) && (size - v[pos + size] <= k)) {
        k -= size - v[pos + size];
        pos += size;
      }
    }

    return pos;
  }
};

int p[MAX_N];       // permutarea originală
int ord[MAX_N + 1]; // ord[val] = ordinea lui val printre elemente < val
fenwick_tree fen;
int n, k;

void read_data() {
  FILE* f = fopen("arbperm2.in", "r");
  fscanf(f, "%d %d", &n, &k);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &p[i]);
  }
  fclose(f);
}

void init_ord() {
  fen.init(n);
  for (int i = 0; i < n; i++) {
    ord[p[i]] = fen.prefix_sum(p[i]);
    fen.set(p[i]);
  }
}

void advance_ord() {
  for (int i = n; i >= 1; i--) {
    int old_k = k;
    k = (ord[i] + k) / i;
    ord[i] = (ord[i] + old_k) % i;
  }
}

void rebuild_p() {
  fen.init(n);
  for (int i = n; i >= 1; i--) {
    int pos = fen.get_kth_zero(ord[i]);
    p[pos] = i;
    fen.set(pos + 1);
  }
}

void write_answer() {
  FILE* f = fopen("arbperm2.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(f, "%d ", p[i]);
  }
  fprintf(f, "\n");
  fclose(f);
}

int main() {
  read_data();
  init_ord();
  advance_ord();
  rebuild_p();
  write_answer();

  return 0;
}
