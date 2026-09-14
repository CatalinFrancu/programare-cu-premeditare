#include <algorithm>
#include <stdio.h>

const int MAX_HEIRS = 100;
const int MAX_POINTS = 100;
const int LEFT = +1;
const int RIGHT = -1;

struct point {
  int x, y;
  int orig;
};

int sgn(int x) {
  return (x > 0) - (x < 0);
}

int orientation(point a, point b, point c) {
  int det = (b.x - a.x) * (c.y - b.y) - (c.x - b.x) * (b.y - a.y);
  return sgn(det);
}

int hull[MAX_POINTS], interior[MAX_POINTS];
int num_hull, num_interior;

struct stack {
  point p[MAX_POINTS];
  int size, dir;

  void init(point q, int dir) {
    p[0] = q;
    size = 1;
    this->dir = dir;
  }

  void add(point q) {
    while ((size >= 2) &&
           (orientation(p[size - 2], p[size - 1], q) == -dir)) {
      size--;
      interior[num_interior++] = p[size].orig;
    }
    p[size++] = q;
  }
};

point p[MAX_HEIRS * MAX_POINTS];
stack low, high;
int num_heirs, num_points;

void read_data() {
  FILE* f = fopen("testament.in", "r");
  fscanf(f, "%d %d", &num_points, &num_heirs);
  for (int i = 0; i < num_heirs * num_points; i++) {
    fscanf(f, "%d %d", &p[i].x, &p[i].y);
    p[i].orig = i + 1;
  }
  fclose(f);
}

void sort_points() {
  std::sort(p, p + num_heirs * num_points, [](point a, point b) {
    return (a.x < b.x) ||
      ((a.x == b.x) && (a.y < b.y));
  });
}

void monotone_chains(point* p) {
  high.init(p[0], RIGHT);
  low.init(p[0], LEFT);
  for (int i = 1; i < num_points; i++) {
    if (orientation(p[0], p[num_points - 1], p[i]) == LEFT) {
      high.add(p[i]);
    } else {
      low.add(p[i]);
    }
  }
  // Ultimul punct este pe ambele lanțuri.
  high.add(p[num_points - 1]);
}

void collect_hull() {
  for (int i = 0; i < low.size - 1; i++) {
    hull[num_hull++] = low.p[i].orig;
  }
  for (int i = high.size - 1; i > 0; i--) {
    hull[num_hull++] = high.p[i].orig;
  }
}

void write_hull(FILE* f) {
  fprintf(f, "%d %d", num_hull, num_interior);
  for (int i = 0; i < num_hull; i++) {
    fprintf(f, " %d", hull[i]);
  }
  for (int i = 0; i < num_interior; i++) {
    fprintf(f, " %d", interior[i]);
  }
  fprintf(f, "\n");
}

void divide_points() {
  FILE* f = fopen("testament.out", "w");
  for (int i = 0; i < num_heirs; i++) {
    num_hull = num_interior = 0;
    monotone_chains(p + i * num_points);
    collect_hull();
    write_hull(f);
  }
  fclose(f);
}

int main() {
  read_data();
  sort_points();
  divide_points();

  return 0;
}
