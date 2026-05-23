#include <stdio.h>

const int MAX_N = 100'000;
const int MAX_M = 10'000;
const int NONE = -1;

short s[2 * MAX_N + 1], p[MAX_N];
int pos[MAX_M + 1];
int pi[MAX_N];
int prev_p[MAX_N];     // apariția anterioară a lui p[i] în p
int prev_s[2 * MAX_N]; // apariția anterioară a lui s[i] în s
int perm[MAX_M + 1];
int n, m, shift;

void read_data() {
  FILE* f = fopen("cifru.in", "r");

  fscanf(f, "%d %d", &n, &m);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%hd", &p[i]);
  }
  for (int i = 0; i < n; i++) {
    fscanf(f, "%hd", &s[i]);
    s[i + n] = s[i];
  }

  fclose(f);
}

void build_prev(short* p, int n, int* prev) {
  for (int i = 1; i <= m; i++) {
    pos[i] = NONE;
  }
  for (int i = 0; i < n; i++) {
    prev[i] = pos[p[i]];
    pos[p[i]] = i;
  }
}

bool matches(int k, int i, int* prev) {
  if (prev_p[k] == NONE) {
    return (prev[i] < i - k);
  } else {
    return (i - prev[i] == k - prev_p[k]);
  }
}

void build_pi() {
  pi[0] = 0;
  int k = 0; // starea curentă
  for (int i = 1; i < n; i++) {
    while ((k > 0) && !matches(k, i, prev_p)) {
      k = pi[k - 1];
    }
    if (matches(k, i, prev_p)) {
      k++;
    }
    pi[i] = k;
  }
}

void kmp() {
  int k = 0; // starea curentă
  int i = 0;
  while (k != n) {
    while ((k > 0) && !matches(k, i, prev_s)) {
      k = pi[k - 1];
    }
    if (matches(k, i, prev_s)) {
      k++;
    }
    i++;
  }

  shift = i - n;
}

void build_perm() {
  for (int i = 0; i < n; i++) {
    perm[p[i]] = s[(i + shift) % n];
  }
}

void write_answer() {
  FILE* f = fopen("cifru.out", "w");
  fprintf(f, "%d\n", shift);
  for (int i = 1; i <= m; i++) {
    fprintf(f, "%d%c", perm[i], (i == m) ? '\n' : ' ');
  }
  fclose(f);
}

int main() {
  read_data();
  build_prev(p, n, prev_p);
  build_prev(s, 2 * n, prev_s);
  build_pi();
  kmp();
  build_perm();
  write_answer();

  return 0;
}
