#include <stdio.h>

const int MAX_TRIANGLES = 150;

struct point {
  int x, y;

  bool on_board(int a, int b) {
    return a * y + b * x <= a * b;
  }
};

// Ar sta mai bine pe triangle, dar aceasta ar crea o dependință circulară.
long long triangle_area(point p, point q, point r) {
  return (r.x - p.x) * (q.y - p.y) - (r.y - p.y) * (q.x - p.x);
}

struct segment {
  point p, q;

  // Exclude intersecțiile degenerate (pe drepte-suport confundate sau la
  // capetele segmentelor).
  bool intersects(segment other) {
    long long a1 = triangle_area(p, q, other.p);
    long long a2 = triangle_area(p, q, other.q);
    long long a3 = triangle_area(other.p, other.q, p);
    long long a4 = triangle_area(other.p, other.q, q);
    return (a1 * a2 < 0) && (a3 * a4 < 0);
  }
};

struct triangle {
  point p, q, r;

  void init(FILE* f) {
    fscanf(f, "%d %d %d %d %d %d", &p.x, &p.y, &q.x, &q.y, &r.x, &r.y);
    if (area() < 0) {
      point tmp = q;
      q = r;
      r = tmp;
    }
  }

  long long area() {
    return triangle_area(p, q, r);
  }

  bool on_board(int a, int b) {
    return
      p.on_board(a, b) &&
      q.on_board(a, b) &&
      r.on_board(a, b);
  }

  // Două scenarii:
  //
  // 1. două segmente se intersectează (nedegenerat), sau
  // 2. unul dintre triunghiuri are toate punctele în interiorul sau pe
  //    laturile celuilalt.
  bool overlaps(triangle& other) {
    return
      intersects_segment({other.p, other.q}) ||
      intersects_segment({other.q, other.r}) ||
      intersects_segment({other.r, other.p}) ||

      contains(other) ||
      other.contains(*this);
  }

  bool intersects_segment(segment s) {
    return
      segment(p, q).intersects(s) ||
      segment(q, r).intersects(s) ||
      segment(r, p).intersects(s);
  }

  bool contains(triangle& other) {
    return
      contains(other.p) &&
      contains(other.q) &&
      contains(other.r);
  }

  bool contains(point a) {
    return
      (triangle_area(p, q, a) >= 0) &&
      (triangle_area(q, r, a) >= 0) &&
      (triangle_area(r, p, a) >= 0);
  }
};

triangle t[MAX_TRIANGLES];
FILE *fin, *fout;

void process_test() {
  int a, b, n;
  fscanf(fin, "%d %d %d", &a, &b, &n);

  bool all_on_board = true;
  bool overlaps = false;
  long long total_area = 0;

  for (int i = 0; i < n; i++) {
    t[i].init(fin);
    total_area += t[i].area();
    all_on_board = all_on_board && t[i].on_board(a, b);
    for (int j = 0; j < i; j++) {
      overlaps = overlaps || t[i].overlaps(t[j]);
    }
  }

  bool same_area = (total_area == a * b);
  fprintf(fout, "%d\n", same_area && all_on_board && !overlaps);
}

int main() {
  fin = fopen("part.in", "r");
  fout = fopen("part.out", "w");

  int num_tests;
  fscanf(fin, "%d", &num_tests);
  while (num_tests--) {
    process_test();
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
