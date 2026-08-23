#include <stdio.h>

const int MAX_VAL = 1'000'000;
unsigned char num_div[MAX_VAL + 1];

void sieve() {
  for (int div = 1; div <= MAX_VAL; div++) {
    for (int mult = div; mult <= MAX_VAL; mult += div) {
      num_div[mult]++;
    }
  }
}

void process_queries() {
  int num_tests, x;

  scanf("%d", &num_tests);
  while (num_tests--) {
    scanf("%d", &x);
    printf("%d\n", num_div[x]);
  }
}

int main() {
  sieve();
  process_queries();

  return 0;
}
