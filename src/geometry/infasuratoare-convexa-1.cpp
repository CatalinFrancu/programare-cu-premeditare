#include <algorithm>
#include <stdio.h>

const int MAX_N = 120'000;

struct point {
  double x, y;
};

point p[MAX_N];
int st[MAX_N], ss;  // stivă pentru Graham's scan
int n;

void read_data() {
  FILE *f = fopen("infasuratoare.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%lf %lf", &p[i].x, &p[i].y);
  }
  fclose(f);
}

void find_extreme() {
  int min = 0;
  for (int i = 1; i < n; i++) {
    if ((p[i].y < p[min].y) || (p[i].y == p[min].y && p[i].x < p[min].x)) {
      min = i;
    }
  }
  std::swap(p[0], p[min]);
}

double orientation(point a, point b, point c) {
  return (b.x - a.x) * (c.y - b.y) - (c.x - b.x) * (b.y - a.y);
}

double dist2(point a, point b) {
  return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

void polar_sort() {
  std::sort(p + 1, p + n, [](point a, point b) {
    return orientation(p[0], a, b) > 0;
  });
}

void graham_scan() {
  ss = 0;
  for (int i = 0; i < n; i++) {
    while ((ss >= 2) &&
           (orientation(p[st[ss - 2]], p[st[ss - 1]], p[i]) < 0)) {
      ss--;
    }
    st[ss++] = i;
  }
}

void write_hull() {
  FILE* f = fopen("infasuratoare.out", "w");
  fprintf(f, "%d\n", ss);
  for (int i = 0; i < ss; i++) {
    fprintf(f, "%.6lf %.6lf\n", p[st[i]].x, p[st[i]].y);
  }
  fclose(f);
}

int main(void) {
  read_data();
  find_extreme();
  polar_sort();
  graham_scan();
  write_hull();

  return 0;
}
