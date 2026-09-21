// Complexitate: O(mn log n).
#include <algorithm>
#include <stdio.h>
#include <vector>

const int MAX_MN = 1000000;
const int LEFT = 0;
const int CENTER = 1;
const int RIGHT = 2;

int n, m;

int max(int x, int y) {
  return (x > y) ? x : y;
}

struct max_fenwick {
  int v[MAX_MN + 1], n;

  void init(int n) {
    this->n = n;
    for (int i = 1; i <= n; i++) {
      v[i] = 0;
    }
  }

  void set(int pos, int val) {
    do {
      v[pos] = max(v[pos], val);
      pos += pos & -pos;
    } while (pos <= n);
  }

  int partial_max(int pos) {
    int m = 0;
    while (pos) {
      m = max(m, v[pos]);
      pos &= pos - 1;
    }
    return m;
  }
};

struct perm {
  int* el;
  int* inv; // inv[i] = poziția lui i în el
  std::vector<int> orig; // indicii originali ai permutărilor duplicate

  void compute_inv() {
    for (int i = 0; i < m; i++) {
      inv[el[i]] = i;
    }
  }

  bool before(int a, int b) {
    return inv[a] < inv[b];
  }

  int get_first_difference(perm other) {
    int i = 0;
    while ((i < m) && (el[i] == other.el[i])) {
      i++;
    }
    return i;
  }

  bool operator ==(perm other) {
    return get_first_difference(other) == m;
  }
};

int buf[MAX_MN], ibuf[MAX_MN];
perm p[MAX_MN];
max_fenwick fen;

void read_data() {
  scanf("%d %d", &n, &m);
  for (int i = 0; i < n; i++) {
    p[i].el = buf + i * m;
    p[i].inv = ibuf + i * m;
    p[i].orig = { i };
    for (int j = 0; j < m; j++) {
      scanf("%d", &p[i].el[j]);
      p[i].el[j]--;
    }
    p[i].compute_inv();
  }
}

void group_duplicates() {
  std::sort(p, p + n, [](perm x, perm y) {
    int pos = x.get_first_difference(y);
    return (pos < m) && (x.el[pos] < y.el[pos]);
  });

  int i = 0;
  for (int j = 0; j < n; j++) {
    if (i && (p[i - 1] == p[j])) {
      p[i - 1].orig.push_back(p[j].orig[0]);
    } else {
      p[i++] = p[j];
    }
  }

  n = i;

  for (int i = 0; i < n; i++) {
    std::sort(p[i].orig.begin(), p[i].orig.end());
  }
}

void get_discriminant(perm x, perm y, int& a, int& b) {
  int pos = x.get_first_difference(y);
  a = x.el[pos];
  b = y.el[pos];
}

int relative_to(perm x, perm y, perm z) {
  int a, b, c, d;
  get_discriminant(x, y, a, b);
  if (z.before(a, b)) {
    get_discriminant(x, z, c, d);
    return y.before(c, d) ? LEFT : CENTER;
  } else {
    get_discriminant(y, z, c, d);
    return x.before(c, d) ? RIGHT : CENTER;
  }
}

// Alege două permutări x și y și partiționează [0...n) în 3.
//
// Invariant:
// * p[0...x) sînt < x
// * p[x+1...i) sînt între x și y
// * p[i] este permutarea evaluată în prezent
// * p[i+1...y) sînt permutări încă neevaluate
// * p[y+1...n) sînt > y
void three_way_partition(int& x, int& y) {
  x = 0;
  y = n - 1;
  int i = 1;
  perm tmp;
  while (i < y) {
    switch (relative_to(p[x], p[y], p[i])) {
      case LEFT:
        tmp = p[i];
        p[i] = p[x + 1];
        p[x + 1] = p[x];
        p[x] = tmp;
        x++;
        break;
      case CENTER:
        // Las-o, las-o, las-o-așa
        i++;
        break;
      case RIGHT:
        tmp = p[i];
        p[i] = p[y - 1];
        p[y - 1] = p[y];
        p[y] = tmp;
        y--;
        break;
    }
  }
}

// Quicksort relativ la un pivot ales, x, și la una dintre margini.
//
// Invariant:
// * p[l...x) sînt < x
// * p[x] este pivotul
// * p[x+1] este permutarea evaluată în prezent
// * p[x+1...y) sînt permutări încă neevaluate
// * p[y...r) sînt > x
void quicksort(int l, int r) { // [l, r)
  if (r - l <= 1) {
    return;
  }

  int x = l, y = r;
  while (x + 1 < y) {
    bool go_left =
      ((l > 0) && (relative_to(p[l - 1], p[x], p[x + 1]) != RIGHT)) ||
      ((r < n) && (relative_to(p[x], p[r], p[x + 1]) == LEFT));
    perm tmp = p[x + 1];

    if (go_left) {
      p[x + 1] = p[x];
      p[x] = tmp;
      x++;
    } else {
      y--;
      p[x + 1] = p[y];
      p[y] = tmp;
    }
  }

  quicksort(l, x);
  quicksort(x + 1, r);
}

void quicksort_driver() {
  int x, y;
  three_way_partition(x, y);
  quicksort(0, x);
  quicksort(x + 1, y);
  quicksort(y + 1, n);
}

void reverse_if_needed() {
  if (p[0].orig[0] > p[n - 1].orig[0]) {
    for (int i = 0, j = n - 1; i < j; i++, j--) {
      perm tmp = p[i];
      p[i] = p[j];
      p[j] = tmp;
    }
  }
}

bool has_double_crossing(perm x, perm y) {
  fen.init(m);
  for (int pos = 0; pos < m; pos++) {
    int val = p[0].el[pos];
    int from = x.inv[val] + 1; // +1 pentru arborele Fenwick
    int to = y.inv[val] + 1;
    if (fen.partial_max(to) > from) {
      return true;
    }
    fen.set(to, from);
  }

  return false;
}

bool all_single_crossings() {
  for (int i = 1; i < n - 1; i++) {
    if (has_double_crossing(p[i], p[i + 1])) {
      return false;
    }
  }

  return true;
}

void write_solution() {
  for (int i = 0; i < n; i++) {
    for (int x: p[i].orig) {
      printf("%d ", x + 1);
    }
  }
  printf("\n");
}

void write_no_solution() {
  printf("-1\n");
}

int main() {
  int num_tests;

  scanf("%d", &num_tests);
  while (num_tests--) {
    read_data();
    group_duplicates();
    quicksort_driver();
    reverse_if_needed();
    if (all_single_crossings()) {
      write_solution();
    } else {
      write_no_solution();
    }
  }

  return 0;
}
