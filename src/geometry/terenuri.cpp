#include <map>
#include <math.h>
#include <stdio.h>

struct point {
  double x, y;

  void translate(point other) {
    x -= other.x;
    y -= other.y;
  }

  double dist2() {
    return x * x + y * y;
  }
};

typedef std::map<double, point>::iterator iter;

std::map<double, point> hull;
point center;

// True dacă și numai dacă ∠ABC virează spre dreapta.
bool reflex_angle(point a, point b, point c) {
  return (b.x - a.x) * (c.y - b.y) - (c.x - b.x) * (b.y - a.y) < 0;
}

// Treci la punctul anterior, circular.
iter prev(iter x) {
  return (x == hull.begin())
    ? std::prev(hull.end())
    : std::prev(x);
}

// Treci la punctul următor, circular.
iter next(iter x) {
  x = std::next(x);
  return (x == hull.end())
    ? hull.begin()
    : x;
}

// Returnează hull.end() dacă avem deja un punct pe aceeași rază, dar mai
// depărtat de centru. În acest caz nu avem nimic de făcut.
iter insert(double angle, point p) {
  iter it = hull.lower_bound(angle);
  bool exists = (it != hull.end()) && (it->first == angle);

  if (exists && (p.dist2() <= it->second.dist2())) {
    // Exista deja un punct mai depărtat de centru, pe aceeași rază.
    return hull.end();
  }

  if (exists) {
    // Exista deja un punct, dar mai apropiat de centru.
    it->second = p;
  } else {
    // Inserează la poziția deja căutată.
    it = hull.insert(it, {angle, p});
  }

  return it;
}

void update_hull(point p) {
  p.translate(center);
  double angle = atan2(p.y, p.x);

  iter it = insert(angle, p);
  if (it == hull.end()) {
    return;
  }

  // Curățenie printre punctele anterioare.
  iter b = prev(it), a = prev(b);
  while ((hull.size() > 3) && reflex_angle(a->second, b->second, p)) {
    hull.erase(b);
    b = a;
    a = prev(a);
  }

  // Curățenie printre punctele următoare.
  iter c = next(it), d = next(c);
  while ((hull.size() > 3) && reflex_angle(p, c->second, d->second)) {
    hull.erase(c);
    c = d;
    d = next(d);
  }

  // Curăță și punctul însuși.
  if (reflex_angle(b->second, p, c->second)) {
    hull.erase(it);
  }
}

int main() {
  FILE* fin = fopen("terenuri.in", "r");
  FILE* fout = fopen("terenuri.out", "w");
  int n, m;
  point p, q;

  fscanf(fin, "%d %d", &n, &m);
  fscanf(fin, "%lf %lf %lf %lf", &p.x, &p.y, &q.x, &q.y);
  center = { (p.x + q.x) / 2, (p.y + q.y) / 2 };

  update_hull(p);
  update_hull(q);

  for (int i = 2; i < m + n; i++) {
    fscanf(fin, "%lf %lf", &p.x, &p.y);
    update_hull(p);
    if (i >= n - 1) {
      fprintf(fout, "%lu\n", hull.size());
    }
  }

  fclose(fin);
  fclose(fout);
  return 0;
}
