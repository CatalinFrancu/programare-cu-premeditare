// Complexitate: O(n^2).
#include <stdio.h>

const int MAX_N = 1'000;
const int INFINITY = 2'000'000'000;
const int DIR = 4;
const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;

int max(int x, int y) {
  return (x > y) ? x : y;
}

int min(int a, int b) {
  return (a < b) ? a : b;
}

int min6(int a, int b, int c, int d, int e, int f) {
  return min(a, min(b, min(c, min(d, min(e, f)))));
}

void swap(short& a, short& b) {
  int tmp = a; a = b; b = tmp;
}

struct cell {
  int val;
  short tri[DIR];
  // Raza maximă a unei matrice impare centrate în celulă, respectiv a unei
  // matrice pare centrate în colțul de sud-est al celulei.
  short odd, even;

  void improve_score(int odd, int even) {
    this->odd = max(this->odd, odd);
    this->even = max(this->even, even);
  }

  void rotate() {
    int tmp = tri[NORTH];
    tri[NORTH] = tri[EAST];
    tri[EAST] = tri[SOUTH];
    tri[SOUTH] = tri[WEST];
    tri[WEST] = tmp;
  }

  void mirror() {
    swap(tri[NORTH], tri[WEST]);
    swap(tri[SOUTH], tri[EAST]);
  }
};

cell a[MAX_N + 2][MAX_N + 2];
int n;

void swap_cells(cell& a, cell& b) {
  cell tmp = a; a = b; b = tmp;
}

void read_data() {
  scanf("%d", &n);
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      scanf("%d", &a[r][c].val);
    }
  }
}

void pad_matrix() {
  for (int i = 0; i <= n + 1; i++) {
    a[i][0].val = a[0][i].val = INFINITY;
    a[i][n + 1].val = a[n + 1][i].val = INFINITY;
  }
}

void rotate_ccw() {
  for (int r = 1; r <= n / 2; r++) {
    for (int c = 1; c <= (n + 1) / 2; c++) {
      cell tmp = a[r][c];
      a[r][c] = a[c][n + 1 - r];
      a[c][n + 1 - r] = a[n + 1 - r][n + 1 - c];
      a[n + 1 - r][n + 1 - c] = a[n + 1 - c][r];
      a[n + 1 - c][r] = tmp;
    }
  }
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      a[r][c].rotate();
      a[r][c].even = a[r + 1][c].even;
    }
  }
}

void mirror() {
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c < r; c++) {
      swap_cells(a[r][c], a[c][r]);
    }
  }
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      a[r][c].mirror();
    }
  }
}

bool decreasing(cell& x, cell& y, cell& z) {
  return (x.val >= y.val) && (y.val >= z.val);
}

void compute_north_triangle() {
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      cell& x = a[r - 1][c - 1];
      cell& y = a[r - 1][c];
      cell& z = a[r - 1][c + 1];
      a[r][c].tri[NORTH] = decreasing(x, y, z)
        ? (1 + min(x.tri[NORTH], z.tri[NORTH]))
        : 1;
    }
  }
}

void compute_triangles() {
  for (int i = 0; i < DIR; i++) {
    compute_north_triangle();
    if (i < DIR - 1) {
      rotate_ccw();
    }
  }
}

// Spirala impară se termină în colțul de nord-est. Încearcă să adaugi un
// colțar pe laturile de est, apoi sud.
bool can_extend_to_even_size(int r, int c, int radius) {
  return
    (a[r + 1][c + 1].val >= a[r + 1][c].val) &&
    (a[r][c + 1].tri[NORTH] >= radius) &&
    (a[r][c + 1].tri[EAST] >= radius) &&
    (a[r + 1][c + 1].tri[EAST] >= radius) &&
    (a[r + 1][c + 1].tri[SOUTH] >= radius) &&
    (a[r + 1][c].tri[SOUTH] >= radius);
}

// Numără doar spiralele centrate în (r,c) care pornesc spre est, apoi spre
// sud. De restul se ocupă rotațiile și oglindirea.
void count_spirals_centered_at(int r, int c) {
  if (!decreasing(a[r][c], a[r][c + 1], a[r + 1][c + 1])) {
    a[r][c].improve_score(1, 0);
    return;
  }
  int odd = min6(a[r][c].tri[NORTH],
                 a[r][c].tri[WEST],
                 a[r][c].tri[SOUTH],
                 a[r][c + 1].tri[NORTH] + 1,
                 a[r][c + 1].tri[EAST] + 1,
                 a[r + 1][c + 1].tri[EAST] + 1);
  int even = (odd - 1) + can_extend_to_even_size(r, c, odd);
  a[r][c].improve_score(odd, even);
}

void count_clockwise_spirals() {
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      count_spirals_centered_at(r, c);
    }
  }
}

void count_spirals() {
  for (int i = 0; i < 2; i++) {
    compute_triangles();
    for (int j = 0; j < DIR; j++) {
      count_clockwise_spirals();
      if (j < DIR - 1) {
        rotate_ccw();
      }
    }
    if (!i) {
      mirror();
    }
  }
}

unsigned sum_spirals() {
  unsigned result = 0;
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      result += a[r][c].odd + a[r][c].even;
    }
  }
  return result;
}

int main() {
  read_data();
  pad_matrix();
  count_spirals();
  printf("%u\n", sum_spirals());

  return 0;
}
