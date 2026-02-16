// Teorema lui Pick: A = I + B/2 - 1, deci I = A + 1 - B/2. În practică:
// 2I = 2A + 1 - B, căci formula pentru arie calculează dublul ariei.
#include <stdio.h>
#include <stdlib.h>

struct point {
  long long x, y;
};

int gcd(point p, point q) {
  long long x = llabs(p.x - q.x);
  long long y = llabs(p.y - q.y);
  while (y) {
    int tmp = x % y;
    x = y;
    y = tmp;
  }
  return x;
}

long long vector(point p, point q) {
  return p.x * q.y - p.y * q.x;
}

int main() {
  point first, prev, p;
  long long boundary = 0, twice_area = 0;
  int n;

  FILE* f = fopen("copaci.in", "r");
  fscanf(f, "%d %lld %lld", &n, &first.x, &first.y);
  prev = first;

  for (int i = 1; i < n; i++) {
    fscanf(f, "%lld %lld", &p.x, &p.y);
    twice_area += vector(prev, p);
    boundary += gcd(prev, p);
    prev = p;
  }
  twice_area += vector(prev, first);
  boundary += gcd(prev, first);

  fclose(f);

  f = fopen("copaci.out", "w");
  fprintf(f, "%lld\n", (llabs(twice_area) + 2 - boundary) / 2);
  fclose(f);

  return 0;
}
