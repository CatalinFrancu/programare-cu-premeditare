#include <algorithm>
#include <stdio.h>

const int MAX_N = 100'000;
const int MAX_Q = 100'000;
const int NONE = -1;

struct point {
  int x, y;
  int qindex; // sau NONE dacă acest punct este o bilă, nu o interogare
};

point p[MAX_N + MAX_Q];
int st[MAX_N], ss; // stiva de puncte-candidat
double ans[MAX_Q];
int n, q;

void read_data() {
  FILE* f = fopen("ydist.in", "r");
  fscanf(f, "%d %d", &n, &q);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d %d", &p[i].x, &p[i].y);
    p[i].qindex = NONE;
  }
  for (int i = 0; i < q; i++) {
    fscanf(f, "%d %d", &p[i + n].x, &p[i + n].y);
    p[i + n].qindex = i;
  }
  fclose(f);
}

void polar_sort() {
  // Procesează bilele înaintea interogărilor pe o rază. Vrem să luăm bilele
  // în calcul pentru interogări (cu distanța 0).
  std::sort(p, p + n + q, [](point a, point b) {
    long long disc = (long long)a.y * b.x - (long long)a.x * b.y;
    return (disc > 0) ||
      ((disc == 0) & (a.qindex == NONE));
  });
}

long long orientation(point& a, point& b, point& c) {
  return (long long)(b.x - a.x) * (c.y - b.y)
    - (long long)(c.x - b.x) * (b.y - a.y);
}

void process_point(int i) {
  // Elimină punctele la NE de p[i].
  while (ss &&
         ((p[st[ss - 1]].x >= p[i].x) ||
          (p[st[ss - 1]].y >= p[i].y))) {
    ss--;
  }

  // Elimină punctele care încalcă concavitatea stivei.
  while ((ss >= 2) &&
         (orientation(p[st[ss - 2]], p[st[ss - 1]], p[i]) < 0)) {
    ss--;
  }
  st[ss++] = i;
}

// Calculează distanța verticală de la p la raza (0,0)-r.
double dist_to_ray(point p, point r) {
  return p.y - (double)r.y * p.x / r.x;
}

void process_query(int i) {
  // Dacă ultimul punct din stivă este mai departe de rază decît penultimul,
  // el poate fi șters (pentru razele viitoare el va fi și mai departe).
  double d1 = dist_to_ray(p[st[ss - 1]], p[i]), d2;
  while ((ss >= 2) &&
         ((d2 = dist_to_ray(p[st[ss - 2]], p[i])) < d1)) {
    ss--;
    d1 = d2;
  }
  ans[p[i].qindex] = d1;
}

void scan_points() {
  for (int i = 0; i < n + q; i++) {
    if (p[i].qindex == NONE) {
      process_point(i);
    } else {
      process_query(i);
    }
  }
}

void write_answers() {
  FILE* f = fopen("ydist.out", "w");
  for (int i = 0; i < q; i++) {
    fprintf(f, "%.7f\n", ans[i]);
  }
  fclose(f);
}

int main() {
  read_data();
  polar_sort();
  scan_points();
  write_answers();

  return 0;
}
