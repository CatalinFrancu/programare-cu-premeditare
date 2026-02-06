#include <algorithm>
#include <stdio.h>

const int MAX_N = 100'000;

struct slope {
  int dx, dy;

  // Ne interesează doar pante în [0°, 90°). Vom reformula o pantă de 110° ca
  // pe o pantă de 20° pentru a le număra împreună.
  void set(int dx, int dy) {
    // Rotește cu 90° pînă cînd ajungem la x > 0 și y >= 0.
    while ((dx <= 0) || (dy < 0)) {
      int tmp = dy;
      dy = -dx;
      dx = tmp;
    }
    this->dx = dx;
    this->dy = dy;
  }

  bool operator ==(const slope& o) const {
    return (long long)dx * o.dy == (long long)dy * o.dx;
  }

  bool operator <(const slope& o) const {
    return (long long)dx * o.dy < (long long)dy * o.dx;
  }
};

slope s[MAX_N];
int n;

void read_slopes() {
  FILE* f = fopen("arhitect.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    int x1, y1, x2, y2;
    fscanf(f, "%d %d %d %d", &x1, &y1, &x2, &y2);
    s[i].set(x2 - x1, y2 - y1);
  }
  fclose(f);
}

int count_duplicates() {
  std::sort(s, s + n);
  int run = 1, max_run = 1;
  for (int i = 1; i < n; i++) {
    run = (s[i] == s[i - 1]) ? (run + 1) : 1;
    if (run > max_run) {
      max_run = run;
    }
  }
  return max_run;
}

void write_result(int result) {
  FILE* f = fopen("arhitect.out", "w");
  fprintf(f, "%d\n", result);
  fclose(f);
}

int main() {
  read_slopes();
  int result = count_duplicates();
  write_result(result);
  return 0;
}
