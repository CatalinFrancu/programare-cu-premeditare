#include <algorithm>
#include <stdio.h>

const int MAX_N = 100'000;

struct point {
  int x, y;
};

point p[MAX_N];
int st[MAX_N], ss;  // stivă cu indicii punctelor
int n;

void read_data() {
  FILE *f = fopen("infasuratoare.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d %d", &p[i].x, &p[i].y);
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

long long orientation(point a, point b, point c) {
  return ((long long)(b.x - a.x)) * (c.y - b.y)
    - ((long long)(c.x - b.x)) * (b.y - a.y);
}

long long dist2(point a, point b) {
  return ((long long)(a.x - b.x)) * (a.x - b.x)
    + ((long long)(a.y - b.y)) * (a.y - b.y);
}

void polar_sort() {
  std::sort(p + 1, p + n, [] (point a, point b) {
    long long disc = orientation(p[0], a, b);
    if (disc == 0) {
      // a și b sînt coliniare cu p[0]; primul vrem să fie cel mai apropiat
      return dist2(a, p[0]) < dist2(b, p[0]);
    } else {
      return disc > 0;
    }
  });
}

void graham_scan() {
  ss = 0;
  for (int i = 0; i < n; i++) {
    while ((ss >= 2) &&
           (orientation(p[st[ss - 2]], p[st[ss - 1]], p[i]) <= 0)) {
      ss--;
    }
    st[ss++] = i;
  }
}

void write_hull() {
  FILE* f = fopen("infasuratoare.out", "w");
  fprintf(f, "%d\n", ss);
  for (int i = 0; i < ss; i++) {
    fprintf(f, "%d %d\n", p[st[i]].x, p[st[i]].y);
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
