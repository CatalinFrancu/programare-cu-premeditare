#include <stdio.h>

const int MAX_N = 500'000;

char s[MAX_N + 1];
int pi[MAX_N];

int main(void) {
  FILE *f = fopen("bart.in", "r");
  fscanf(f, "%s", s);
  fclose(f);

  pi[0] = 0;
  int k = 0, n = 1;
  while (s[n]) {
    while ((k > 0) && (s[k] != s[n])) {
      k = pi[k - 1];
    }
    if (s[k] == s[n]) {
      k++;
    }
    pi[n++] = k;
  }

  s[n - pi[n - 1]] = '\0';

  f = fopen("bart.out", "w");
  fprintf(f, "%s\n", s);
  fclose(f);
}
