#include <stdio.h>

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

long long count_all(int a, int b, int c, int l) {
  long long result = 0;
  int start = max(0, b + c - a); // Mai jos ne asigurăm că h ≥ 0.
  for (int x = start; x <= l; x++) {
    long long h = min(l - x, a + x - b - c);
    result += (h + 1) * (h + 2) / 2;
  }

  return result;
}

int main() {
  int a, b, c, l;
  scanf("%d %d %d %d\n", &a, &b, &c, &l);

  long long result =
    (long long)(l + 3) * (l + 2) * (l + 1) / 6
    - count_all(a, b, c, l)
    - count_all(b, c, a, l)
    - count_all(c, a, b, l);

  printf("%lld\n", result);

  return 0;
}
