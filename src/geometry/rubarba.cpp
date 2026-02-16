#include <algorithm>
#include <stdio.h>

const int MAX_N = 100'000;

struct point {
  int x, y;

  point operator - (const point& other) {
    return { x - other.x, y - other.y };
  }
};

point p[MAX_N];
int st[MAX_N];  // stivă pentru Graham's scan
int n;

void read_data() {
  FILE* f = fopen("rubarba.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d %d", &p[i].x, &p[i].y);
  }
  fclose(f);
}

void find_min_y() {
  int min = 0;
  for (int i = 1; i < n; i++) {
    if ((p[i].y < p[min].y) || (p[i].y == p[min].y && p[i].x < p[min].x)) {
      min = i;
    }
  }
  std::swap(p[0], p[min]);
}

long long dot(point a, point b) {
  return (long long)a.x * b.x + (long long)a.y * b.y;
}

long long area(point a, point b, point c) {
  return
    (long long)(b.x - a.x) * (c.y - b.y) -
    (long long)(b.y - a.y) * (c.x - b.x);
}

long long dist2(point a, point b) {
  return dot(b - a, b - a);
}

void polar_sort() {
  std::sort(p + 1, p + n, [](point a, point b) {
    long long d = area(p[0], a, b);
    return (d > 0) ||
      ((d == 0) && (dist2(a, p[0]) < dist2(b, p[0])));
  });
}

void graham_scan() {
  int ss = 0;
  for (int i = 0; i < n; i++) {
    while ((ss >= 2) &&
           (area(p[st[ss - 2]], p[st[ss - 1]], p[i]) <= 0)) {
      ss--;
    }
    st[ss++] = i;
  }

  // Colectează punctele de pe înfășurătoare în ordine trigonometrică.
  for (int i = 0; i < ss; i++) {
    p[i] = p[st[i]];
  }
  n = ss;
}

int next(int k) {
  return (k == n - 1) ? 0 : (k + 1);
}

// Returnează aria triunghiului sprijinit pe b1b2 cu extremele r (dreapta), a
// (opus) și l (stînga).
//
// Înălțimea dreptunghiului este distanța de la a la b1b2, pe care o putem
// exprima ca 2 * aria(Δab1b2) / |b1b2|. Lățimea dreptunghiului este proiecția
// vectorului r - l pe dreapta b1b2. Aceasta prin definiție este (r - l) ·
// b1b2 / |b1b2| (produsul scalar). Aria dreptunghiului este deci:
//
// (r - l) · b1b2 * aria(Δab1b2) / |b1b2|^2
long double rectangle_area(int b1, int b2, int r, int a, int l) {
  return (long double)dot(p[b2] - p[b1], p[r] - p[l])
    / dist2(p[b1], p[b2])
    * area(p[b1], p[b2], p[a]);
}

void find_extremes(int b1, int& b2, int& r, int& a, int& l) {
  b2 = next(b1);
  point base = p[b2] - p[b1];

  while (dot(base, p[next(r)] - p[b1]) > dot(base, p[r] - p[b1])) {
    r = next(r);
  }

  a = r;
  while (area(p[b1], p[b2], p[next(a)]) > area(p[b1], p[b2], p[a])) {
    a = next(a);
  }

  l = a;
  while (dot(base, p[next(l)] - p[b1]) < dot(base, p[l] - p[b1])) {
    l = next(l);
  }
}

// Pentru o explicație vizuală a funționării algoritmului, vezi
// https://www.youtube.com/watch?v=OaGvH450jRM
long double rotating_calipers() {
  int j, r = 1, a, l;
  long double min_area = 0;

  for (int i = 0; i < n; i++) {
    find_extremes(i, j, r, a, l);
    long double area = rectangle_area(i, j, r, a, l);
    if (!i || (area < min_area)) {
      min_area = area;
    }
  }

  return min_area;
}

long double find_min_area() {
  find_min_y();
  polar_sort();
  graham_scan();

  return (n <= 2) ? 0 : rotating_calipers();
}

void write_result(long double result) {
  FILE* f = fopen("rubarba.out", "w");
  fprintf(f, "%.2Lf\n", result);
  fclose(f);
}

int main() {
  read_data();
  long double result = find_min_area();
  write_result(result);

  return 0;
}
