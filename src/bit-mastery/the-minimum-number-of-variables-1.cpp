#pragma GCC target("lzcnt,popcnt")
#include <stdio.h>

const int MAX_N = 23;
const int NONE = -1;

// poss[mask] = true dacă și numai dacă putem procesa primele i variabile
// a.î. variabilele curente să fie cele dictate de mască. Notă: i este dat de
// bitul cel mai semnificativ al măștii.
bool poss[1 << MAX_N];
int a[MAX_N], n;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
  }
}

int get_msb(int mask) {
  return 31 - __builtin_clz(mask);
}

int get_lsb(int mask) {
  return get_msb(mask & -mask);
}

bool can_obtain_next(int mask) {
  int msb = get_msb(mask);

  for (; mask; mask &= mask - 1) {
    int x = get_lsb(mask);
    for (int m2 = mask; m2; m2 &= m2 - 1) {
      int y = get_lsb(m2);
      if (a[x] + a[y] == a[msb + 1]) {
        return true;
      }
    }
  }
  return false;
}

void propagate(int mask) {
  int msb = 31 - __builtin_clz(mask);
  // Folosește o variablă nouă.
  poss[mask ^ (1 << (msb + 1))] = true;

  for (int cp = mask; cp; cp &= cp - 1) {
    // Înlocuiește variabila cp & -cp.
    poss[mask ^ (1 << (msb + 1)) ^ (cp & -cp)] = true;
  }
}

void try_all_masks() {
  poss[1] = true;

  for (int mask = 1; mask < (1 << (n - 1)); mask++) {
    if (poss[mask] && can_obtain_next(mask)) {
      propagate(mask);
    }
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int get_min_pop_mask() {
  int min_pop = n + 1;
  for (int mask = 1 << (n - 1); mask < (1 << n); mask++) {
    if (poss[mask]) {
      min_pop = min(min_pop, __builtin_popcount(mask));
    }
  }
  return (min_pop == n + 1) ? NONE : min_pop;
}

int main() {
  read_data();
  try_all_masks();
  int answer = get_min_pop_mask();
  printf("%d\n", answer);

  return 0;
}
