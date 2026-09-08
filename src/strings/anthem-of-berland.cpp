#include <stdio.h>
#include <string.h>

const int MAX_LENGTH = 10'000'000;
const int SQRT = 3'200;
const int SIGMA = 26;
const int NONE = -1;

char s[MAX_LENGTH + 1], p[SQRT];
int pi[SQRT];
int delta[SQRT][SIGMA];
int d[2][SQRT];
int ns, np, row;

void read_data() {
  scanf("%s %s", s, p);
  ns = strlen(s);
  np = strlen(p);
}

void build_pi() {
  int k = 0;
  for (int q = 1; q < np; q++) {
    while ((k > 0) && (p[k] != p[q])) {
      k = pi[k - 1];
    }
    if (p[k] == p[q]) {
      k++;
    }
    pi[q] = k;
  }
}

// Vezi Cormen ediția a 4-a, exercițiul 32.4-8.
void build_delta() {
  delta[0][p[0] - 'a'] = 1;

  for (int c = 'a'; c <= 'z'; c++) {
    for (int q = 1; q <= np; q++) {
      delta[q][c - 'a'] = ((q < np) && (p[q] == c))
        ? (q + 1)
        : delta[pi[q - 1]][c - 'a'];
    }
  }
}

void improve(int& x, int y) {
  x = (x > y) ? x : y;
}

void build_dp() {
  d[0][0] = 0;
  for (int j = 1; j <= np; j++) {
    d[0][j] = NONE;
  }
  for (int i = 0; i < ns; i++) {
    for (int j = 0; j <= np; j++) {
      d[1 - row][j] = NONE;
    }
    for (int j = 0; j <= np; j++) {
      if (d[row][j] != NONE) {
        int start = (s[i] == '?') ? 'a' : s[i];
        int end = (s[i] == '?') ? 'z' : s[i];
        for (int c = start; c <= end; c++) {
          int new_st = delta[j][c - 'a'];
          improve(d[1 - row][new_st], d[row][j] + (new_st == np));
        }
      }
    }
    row = 1 - row;
  }
}

void write_answer() {
  int max = 0;
  for (int i = 0; i <= np; i++) {
    improve(max, d[row][i]);
  }
  printf("%d\n", max);
}

int main() {
  read_data();
  if (np > ns) {
    printf("0\n"); // paranoia este bună
  } else {
    build_pi();
    build_delta();
    build_dp();
    write_answer();
  }

  return 0;
}
