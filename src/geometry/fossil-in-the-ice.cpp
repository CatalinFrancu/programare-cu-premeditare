#include <algorithm>
#include <stdio.h>

const int MAX_N = 100000;

typedef struct {
  int x, y;
} point;

point p[MAX_N];
int st[MAX_N];  // stivă pentru Graham's scan
int n;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d", &p[i].x, &p[i].y);
  }
}

long long area(point a, point b, point c) {
  return
    (long long)(b.x - a.x) * (c.y - b.y) -
    (long long)(b.y - a.y) * (c.x - b.x);
}

long long dist2(point a, point b) {
  return
    (long long)(a.x - b.x) * (a.x - b.x) +
    (long long)(a.y - b.y) * (a.y - b.y);
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

long long max(long long a, long long b) {
  return (a > b) ? a : b;
}

int next(int k) {
  return (k == n - 1) ? 0 : (k + 1);
}

long long rotating_calipers() {
  int j = 0, j2 = 1;
  long long result = 0;

  for (int i = 0; i < n; i++) {
    // Mică optimizare: calculează d(i, j) o singură dată.
    long long d = dist2(p[i], p[j]), d2;
    while ((d2 = dist2(p[i], p[j2])) > d) {
      d = d2;
      j = j2;
      j2 = next(j2);
    }
    result = max(result, d);
    // Invariant: j este cel mai depărtat punct de i.
  }

  return result;
};

long long find_diameter() {
  if (n <= 1) {
    return 0;
  } else if (n == 2)  {
    return dist2(p[0], p[1]);
  } else {
    find_min_y();
    polar_sort();
    graham_scan();
    return rotating_calipers();
  }
}

int main(void) {
  int num_tests;

  scanf("%d", &num_tests);

  while (num_tests--) {
    read_data();
    long long diameter = find_diameter();
    printf("%lld\n", diameter);
  }

  return 0;
}
