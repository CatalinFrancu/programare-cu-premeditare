#include <algorithm>
#include <deque>
#include <math.h>
#include <stdio.h>

const int MAX_STONES = 100'000;

struct ipoint {
  int x, y;
};

struct dpoint {
  double x, y;
};

struct line {
  long long a, b, c;
  ipoint p; // un punct cunoscut a fi pe linie
  double angle;

  void from_points(ipoint p, ipoint q) {
    a = p.y - q.y;
    b = q.x - p.x;
    c = (long long)p.x * q.y - (long long)q.x * p.y;
    this->p = p;
    angle = atan2(q.y - p.y, q.x - p.x);
  }

  bool has_left(ipoint p) {
    return a * p.x + b * p.y + c > 0;
  }

  bool has_right(dpoint p) {
    return a * p.x + b * p.y + c < 0;
  }

  dpoint intersect(line h) {
    return {
      (double)(b * h.c - c * h.b) / (a * h.b - b * h.a),
      (double)(c * h.a - a * h.c) / (a * h.b - b * h.a),
    };
  }
};

line l[4 * MAX_STONES];
std::deque<int> dl; // indicii liniilor
std::deque<dpoint> poly;
int num_lines;

void read_data() {
  int n;
  scanf("%d", &n);

  for (int i = 0; i < n; i++) {
    ipoint p, q, r, s;
    scanf("%d %d %d %d %d %d %d %d",
          &p.x, &p.y, &q.x, &q.y, &r.x, &r.y, &s.x, &s.y);
    l[num_lines++].from_points(p, q);
    l[num_lines++].from_points(q, r);
    l[num_lines++].from_points(r, s);
    l[num_lines++].from_points(s, p);
  }
}

void sort_lines() {
  std::sort(l, l + num_lines, [](line& l1, line& l2) {
    return
      (l1.angle < l2.angle) ||
      ((l1.angle == l2.angle) && l2.has_left(l1.p));
  });
}

void unique_lines() {
  int j = 1;
  for (int i = 1; i < num_lines; i++) {
    if (l[i].angle != l[i - 1].angle) {
      l[j++] = l[i];
    }
  }
  num_lines = j;
}

void build_intersection() {
  dl.push_front(0);
  for (int i = 1; i < num_lines; i++) {
    while (!poly.empty() && (l[i].has_right(poly.front()))) {
      dl.pop_front();
      poly.pop_front();
    }
    poly.push_front(l[i].intersect(l[dl.front()]));
    dl.push_front(i);
  }

  unsigned old_size;
  do {
    old_size = dl.size();
    if (l[dl.front()].has_right(poly.back())) {
      dl.pop_back();
      poly.pop_back();
    }

    if (l[dl.back()].has_right(poly.front())) {
      dl.pop_front();
      poly.pop_front();
    }
  } while (dl.size() != old_size);

  poly.push_front(l[dl.front()].intersect(l[dl.back()]));
}

long double compute_area() {
  poly.push_back(poly.front());
  long double area = 0;
  for (unsigned i = 0; i < poly.size() - 1; i++) {
    area += (poly[i + 1].x - poly[i].x) * (poly[i].y + poly[i + 1].y);
  }

  return area / 2;
}

int main() {
  read_data();
  sort_lines();
  unique_lines();
  build_intersection();
  long double area = compute_area();

  printf("%.9Lf\n", area);
  return 0;
}
