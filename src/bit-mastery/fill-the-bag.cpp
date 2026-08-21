#include <stdio.h>

const int MAX_BITS = 29;
const int IMPOSSIBLE = -1;

int c[MAX_BITS + 1];
long long bag_space, to_spare;

int log2(int x) {
  return 31 - __builtin_clz(x);
}

long long min(long long x, long long y) {
  return (x < y) ? x : y;
}

void read_data() {
  int num_boxes, box;
  scanf("%lld %d", &bag_space, &num_boxes);
  to_spare = -bag_space;

  while (num_boxes--) {
    scanf("%d", &box);
    to_spare += box;
    c[log2(box)]++;
  }
}

int get_min_cuts() {
  int cuts = 0;

  for (int i = MAX_BITS; i >= 0; i--) {
    // Preferă cutiile mai mari pentru a evita tăieturile.
    int size = 1 << i;
    int to_take = min(c[i], bag_space / size);
    c[i] -= to_take;
    bag_space -= (long long)to_take * size;

    // Taie o cutie dacă există și dacă nu ne permitem să o sărim. Mută
    // jumătățile cu cutiile mai mici.
    if (i && c[i] && (to_spare < (long long)c[i] * size)) {
      cuts++;
      c[i]--;
      c[i - 1] += 2;
    }

    to_spare -= (long long)c[i] * size;
    c[i] = 0; // curățenie
  }

  return bag_space ? IMPOSSIBLE : cuts;
}

void process_test() {
  read_data();
  printf("%d\n", get_min_cuts());
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    process_test();
  }

  return 0;
}
