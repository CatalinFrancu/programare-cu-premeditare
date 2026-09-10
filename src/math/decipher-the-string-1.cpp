#include <stdio.h>
#include <string.h>

const int SIGMA = 26;
const int MAX_LENGTH = 10'000;

typedef char str[MAX_LENGTH + 1];

str orig, t, query, ans[3];
int n;

void read_data() {
  scanf("%s", t);
  n = strlen(t);
}

void build_query(int change_every) {
  int val = -1;
  for (int i = 0; i < n; i++) {
    if (i % change_every == 0) {
      val = (val + 1) % SIGMA;
    }
    query[i] = val + 'a';
  }
}

void build_and_send_query(int change_every, char* ans) {
  build_query(change_every);
  printf("? %s\n", query);
  fflush(stdout);
  scanf(" %s", ans);
}

void restore_orig() {
  for (int i = 0; i < n; i++) {
    int c0 = ans[0][i] - 'a';
    int c1 = ans[1][i] - 'a';
    int c2 = ans[2][i] - 'a';
    int pos = c0 + c1 * SIGMA + c2 * SIGMA * SIGMA;
    orig[pos] = t[i];
  }
}

void write_answer() {
  printf("! %s\n", orig);
}

int main() {
  read_data();
  build_and_send_query(1, ans[0]);
  build_and_send_query(SIGMA, ans[1]);
  build_and_send_query(SIGMA * SIGMA, ans[2]);
  restore_orig();
  write_answer();

  return 0;
}
