// Complexitate: O((n + q) sqrt n).
#include <algorithm>
#include <stdio.h>

const int MAX_N = 100000;
const int BLOCK_SIZE = 316;
const int MAX_QUERIES = 100000;
const int MAX_VAL = 100000;

struct query {
  int l, r;
  int orig_index;
};

int v[MAX_N];
int f[MAX_VAL + 1]; // frecvențele elementelor
int right_ptr;      // poziția ultimului element adăugat în f
int right_best;     // răspunsul considerînd strict dreapta blocului curent
query q[MAX_QUERIES];
int answer[MAX_QUERIES];
int n, num_queries;

void read_data() {
  FILE* f = fopen("rangemode.in", "r");
  fscanf(f, "%d %d", &n, &num_queries);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &v[i]);
  }
  for (int i = 0; i < num_queries; i++) {
    int l, r;
    fscanf(f, "%d %d", &l, &r);
    q[i].l = l - 1;
    q[i].r = r - 1;
    q[i].orig_index = i;
  }
  fclose(f);
}

void sort_queries_by_left_block() {
  std::sort(q, q + num_queries, [](query a, query b) {
    int x = a.l / BLOCK_SIZE, y = b.l / BLOCK_SIZE;
    return (x < y) || ((x == y) && (a.r < b.r));
  });
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void clear_f() {
  for (int i = 0; i <= MAX_VAL; i++) {
    f[i] = 0;
  }
}

void optimize(int& best, int candidate) {
  if ((f[candidate] > f[best]) ||
      ((f[candidate] == f[best]) && (candidate < best))) {
    best = candidate;
  }
}

void extend_right(int until) {
  while (right_ptr < until) {
    int val = v[++right_ptr];
    f[val]++;
    optimize(right_best, val);
  }
}

int add_current_block(int l, int r) {
  int best = right_best;
  for (int i = l; i <= r; i++) {
    f[v[i]]++;
    optimize(best, v[i]);
  }
  return best;
}

void remove_current_block(int l, int r) {
  for (int i = l; i <= r; i++) {
    f[v[i]]--;
  }
}

void process_query(query q, int last_element_in_block) {
  extend_right(q.r);

  // Interogarea nu trece neapărat în blocul următor. Poate fi complet
  // cuprinsă în blocul curent.
  int right = min(q.r,last_element_in_block);
  int best = add_current_block(q.l, right);
  answer[q.orig_index] = best;
  remove_current_block(q.l, right);
}

void scan_blocks() {
  int qi = 0;
  for (int start = 0; start < n; start += BLOCK_SIZE) {
    int end = start + BLOCK_SIZE;
    right_ptr = end - 1;
    right_best = 0;
    while ((qi < num_queries) && (q[qi].l < end)) {
      process_query(q[qi++], end - 1);
    }
    clear_f();
  }
}

void write_answers() {
  FILE* f = fopen("rangemode.out", "w");
  for (int i = 0; i < num_queries; i++) {
    fprintf(f, "%d\n", answer[i]);
  }
  fclose(f);
}

int main() {
  read_data();
  sort_queries_by_left_block();
  scan_blocks();

  write_answers();

  return 0;
}
