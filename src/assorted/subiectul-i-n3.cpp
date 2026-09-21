// Complexitate: O(n^3).
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

void swap(short& a, short& b) {
  int tmp = a; a = b; b = tmp;
}

struct cell {
  int val;
  short dist[DIR]; // distanța pînă la primul element prost ordonat
  // Raza maximă a unei matrice impare centrate în celulă, respectiv a unei
  // matrice pare centrate în colțul de sud-est al celulei. Le contorizăm
  // separat, căci la rotație cele pare migrează în celula vecină.
  short odd, even;

  void improve_score(int odd, int even) {
    this->odd = max(this->odd, odd);
    this->even = max(this->even, even);
  }

  void rotate() {
    int tmp = dist[NORTH];
    dist[NORTH] = dist[EAST];
    dist[EAST] = dist[SOUTH];
    dist[SOUTH] = dist[WEST];
    dist[WEST] = tmp;
  }

  void mirror() {
    swap(dist[NORTH], dist[WEST]);
    swap(dist[SOUTH], dist[EAST]);
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

void compute_north_distance() {
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      cell& x = a[r][c];
      cell& y = a[r - 1][c];
      x.dist[NORTH] = (x.val >= y.val)
        ? (1 + y.dist[NORTH])
        : 1;
    }
  }
}

void compute_distances() {
  for (int i = 0; i < DIR; i++) {
    compute_north_distance();
    rotate_ccw();
  }
}

// Numără doar spiralele centrate în (r,c) care pornesc spre est, apoi spre
// sud. De restul se ocupă rotațiile și oglindirea.
void count_spirals_centered_at(int r, int c) {
  a[r][c].improve_score(1, 0); // YES, WE CAN.

  int r2 = r, c2 = c, side = 1, old_side;
  do {
    old_side = side;
    if (side % 2) {
      // extinde la sud și est
      if ((a[r2][c2].val >= a[r2][c2 + 1].val) &&
          (a[r2][c2 + 1].dist[SOUTH] >= side + 1) &&
          (a[r2 + side][c2 + 1].dist[WEST] >= side + 1)) {
        r2 += side;
        c2 -= (side - 1);
        side++;
      }
    } else {
      // extinde la nord și vest
      if ((a[r2][c2].val >= a[r2][c2 - 1].val) &&
          (a[r2][c2 - 1].dist[NORTH] >= side + 1) &&
          (a[r2 - side][c2 - 1].dist[EAST] >= side + 1)) {
        r2 -= side;
        c2 += (side - 1);
        side++;
      }
    }
  } while (side > old_side);

  a[r][c].improve_score((side + 1) / 2, side / 2);
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
    compute_distances();
    for (int j = 0; j < DIR; j++) {
      count_clockwise_spirals();
      rotate_ccw();
    }
    mirror();
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
