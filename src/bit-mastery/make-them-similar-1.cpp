#include <stdio.h>

const int MAX_N = 100;
const int BITS = 15;
const int MAX_TUPLES = 1 << (BITS + 1);
const bool LEFT = false;
const bool RIGHT = true;

int n;

struct tuple {
  char diff[MAX_N - 1];
  bool half;
  short mask;

  bool matches(const tuple& other) {
    if (half == other.half) {
      return false;
    }

    int i = 0;
    while ((i < n - 1) && (diff[i] == other.diff[i]))  {
      i++;
    }

    return (i == n - 1);
  }
};

int a[MAX_N], b[MAX_N];
tuple t[MAX_TUPLES];
int perm[MAX_TUPLES];
int num_tuples;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    int x;
    scanf("%d", &x);
    a[i] = x >> BITS;
    b[i] = x & ((1 << BITS) - 1);
  }
}

void iterate_masks(int* v, bool half) {
  for (int mask = 0; mask < (1 << BITS); mask++) {
    int pc0 = __builtin_popcount(mask ^ v[0]);
    tuple& x = t[num_tuples];
    x.mask = mask;
    x.half = half;

    for (int i = 1; i < n; i++) {
      int pc = __builtin_popcount(mask ^ v[i]);
      x.diff[i - 1] = (half == LEFT) ? (pc - pc0) : (pc0 - pc);
    }

    num_tuples++;
  }
}

void swap(int i, int j) {
  int tmp = perm[i];
  perm[i] = perm[j];
  perm[j] = tmp;
}

// Sortează tuplurile t[first...last), care sînt egale pînă la poziția pos-1.
// Modifică perm[] în pasul de partiționare, dar nu modifică t[], căci ar fi
// costisitor.
void ternary_sort(int first, int last, int pos) {
  if (last - first <= 1) {
    return;
  }

  int i = first, left = first, right = last;
  char pivot = t[perm[first]].diff[pos];

  // Invariant de buclă:
  // * perm[first ... left)  a pos-a diferență este < pivot
  // * perm[left ... i)      a pos-a diferență este == pivot
  // * perm[i]               tuplul în curs de evaluare
  // * perm(i ... right)     tupluri încă neevaluate
  // * perm[right ... last)  a pos-a diferență este > pivot
  while (i < right) {
    char x = t[perm[i]].diff[pos];
    if (x < pivot) {
      swap(i++, left++);
    } else if (x > pivot)  {
      swap(i, --right);
    } else {
      i++;
    }
  }

  ternary_sort(first, left, pos);
  if (pos < n - 1) {
    ternary_sort(left, right, pos + 1);
  }
  ternary_sort(right, last, pos);
}

void sort_tuples() {
  for (int i = 0; i < num_tuples; i++) {
    perm[i] = i;
  }
  ternary_sort(0, num_tuples, 0);
}

void print_solution(int i, int j) {
  int high = (t[i].half == LEFT) ? t[i].mask : t[j].mask;
  int low = (t[i].half == LEFT) ? t[j].mask : t[i].mask;
  int x = (high << BITS) + low;
  printf("%d\n", x);
}

void find_match() {
  int i = 1;

  while ((i < num_tuples) && !t[perm[i - 1]].matches(t[perm[i]])) {
    i++;
  }

  if (i == num_tuples) {
    printf("-1\n");
  } else {
    print_solution(perm[i - 1], perm[i]);
  }
}

int main() {
  read_data();
  iterate_masks(a, LEFT);
  iterate_masks(b, RIGHT);
  sort_tuples();
  find_match();

  return 0;
}
