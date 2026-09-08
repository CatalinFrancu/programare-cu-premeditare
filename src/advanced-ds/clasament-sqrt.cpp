// Complexitate: O((q + n) sqrt n).
#include <algorithm>
#include <map>
#include <stdio.h>

typedef unsigned short u16;
typedef unsigned long long u64;

const int MAX_N = 50'000;
const int BASE = 60'133;
const int BLOCK_SIZE = 223;

struct student {
  int score;
  int time;

  friend bool operator< (student a, student b) {
    return (a.score > b.score) ||
      ((a.score == b.score) & (a.time < b.time));
  }
};

student s[MAX_N];
int n, num_ops;

struct block {
  u16 v[2 * BLOCK_SIZE];
  int size;
  u64 hash_code; // valoarea numerică în baza n, modulo 2^64

  void compute_hash_code() {
    hash_code = 0;
    for (int i = 0; i < size; i++) {
      hash_code = hash_code * BASE + v[i];
    }
  }

  void init_from_sorted_array(u16* a, int len) {
    size = len;
    for (int i = 0; i < len; i++) {
      v[i] = a[i];
    }
    compute_hash_code();
  }

  // Presupune că blocul nu este gol.
  int last() {
    return v[size - 1];
  }

  void erase_student(int x) {
    int i = 0;
    while (v[i] != x) {
      i++;
    }
    while (i < size - 1) {
      v[i] = v[i + 1];
      i++;
    }
    size--;
    compute_hash_code();
  }

  void insert_student(int x) {
    int i = size++;
    while ((i > 0) && (s[x] < s[v[i - 1]])) {
      v[i] = v[i - 1];
      i--;
    }
    v[i] = x;
    compute_hash_code();
  }
};

struct sqrt_decomposition {
  block b[MAX_N / BLOCK_SIZE + 1];
  int n;
  int nb; // numărul de blocuri
  u64 power[2 * BLOCK_SIZE + 1];
  bool needs_rebalancing;
  u16 ord[MAX_N]; // pentru ordinea inițială și pentru redistribuiri

  // Sortăm scorurile inițiale ca să facem inițializarea în O(n log n). Am putea
  // și să inserăm valorile în structură, dar ne-ar costa O(n sqrt n).
  void sort_scores() {
    for (int i = 0; i < n; i++) {
      ord[i] = i;
    }
    std::sort(ord, ord + n, [](int a, int b) {
      return s[a] < s[b];
    });
  }

  void compute_powers() {
    power[0] = 1;
    for (int i = 1; i <= 2 * BLOCK_SIZE; i++) {
      power[i] = power[i - 1] * BASE;
    }
  }

  void collect_scores() {
    int k = 0;
    for (int i = 0; i < nb; i++) {
      for (int j = 0; j < b[i].size; j++) {
        ord[k++] = b[i].v[j];
      }
    }
  }

  void distribute_scores() {
    nb = 0;
    int start = 0;
    while (start < n) {
      int len = std::min(BLOCK_SIZE, n - start);
      b[nb++].init_from_sorted_array(ord + start, len);
      start += BLOCK_SIZE;
    }
  }

  void init(int n) {
    this->n = n;
    compute_powers();
    sort_scores();
    distribute_scores();
  }

  void erase_student(int x) {
    int i = 0;
    while (s[b[i].last()] < s[x]) {
      i++;
    }
    b[i].erase_student(x);
    needs_rebalancing |= (b[i].size == 1);
  }

  void insert_student(int x) {
    int i = 0;
    while ((i < nb - 1) && (s[b[i].last()] < s[x])) {
      i++;
    }
    b[i].insert_student(x);
    needs_rebalancing |= (b[i].size == 2 * BLOCK_SIZE);
  }

  void rebalance() {
    collect_scores();
    distribute_scores();
    needs_rebalancing = false;
  }

  void update_score(int x, int score, int time) {
    erase_student(x);
    s[x] = { score, time };
    insert_student(x);
    if (needs_rebalancing) {
      rebalance();
    }
  }

  u64 get_hash_code() {
    u64 result = 0;
    for (int i = 0; i < nb; i++) {
      result = result * power[b[i].size] + b[i].hash_code;
    }
    return result;
  }
};

sqrt_decomposition decomp;
FILE *fin, *fout;

void read_scores() {
  fscanf(fin, "%d %d", &n, &num_ops);
  for (int i = 0; i < n; i++) {
    fscanf(fin, "%d", &s[i].score);
  }
}

void process_ops() {
  decomp.init(n);

  std::map<u64, int> seen;
  seen[decomp.get_hash_code()] = 1;

  for (int time = 1; time <= num_ops; time++) {
    int i, score;
    fscanf(fin, "%d %d", &i, &score);
    i--;
    decomp.update_score(i, score, time);
    u64 h = decomp.get_hash_code();
    fprintf(fout, "%d\n", seen[h]);
    seen[h]++;
  }
}

int main() {
  fin = fopen("clasament.in", "r");
  fout = fopen("clasament.out", "w");

  read_scores();
  process_ops();

  fclose(fin);
  fclose(fout);

  return 0;
}
