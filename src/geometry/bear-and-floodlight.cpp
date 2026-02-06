#define _USE_MATH_DEFINES // https://codeforces.com/blog/entry/51835
#include <math.h>
#include <stdio.h>

const int MAX_N = 20;

double min(double x, double y) {
  return (x < y) ? x : y;
}

double max(double x, double y) {
  return (x > y) ? x : y;
}

struct floodlight {
  int xc, yc;
  double c, s;

  void setAngle(int angle) {
    double rad = M_PI * angle / 180;
    c = cos(rad);
    s = sin(rad);
  }

  // Extinde segmentul vizibil la dreapta de la x, folosind unghiul acestui
  // reflector.
  double extend(double x, int limit) {
    // Rotește (x, 0) cu <c,s> grade în jurul lui (xc, yc).
    double rotx = (x - xc) * c - (0 - yc)* s + xc;
    double roty = (x - xc) * s + (0 - yc)* c + yc;

    if (roty >= yc) {
      // Reflectorul bate la dreapta sau chiar mai sus, deci acoperă axa Ox
      // pînă la infinit (și dincolo!)
      return limit;
    }

    // Intersectează segmentul (xc, yc) - (rotx, roty) cu axa Ox.
    double newx = (xc * roty - yc * rotx) / (roty - yc);
    return min(newx, limit);
  }
};

floodlight f[MAX_N];
double bestx[1 << MAX_N];
int n, startx, endx;

void read_data() {
  scanf("%d %d %d", &n, &startx, &endx);
  for (int i = 0; i < n; i++) {
    int angle;
    scanf("%d %d %d", &f[i].xc, &f[i].yc, &angle);
    f[i].setAngle(angle);
  }
}

double dyn_prog() {
  bestx[0] = startx;
  for (int mask = 1; mask < (1 << n); mask++) {
    bestx[mask] = startx;
    for (int b = 0; b < n; b++) {
      if (mask & (1 << b)) {
        bestx[mask] = max(bestx[mask], f[b].extend(bestx[mask ^ (1 << b)], endx));
      }
    }
  }

  return bestx[(1 << n) - 1] - startx;
}

int main() {
  read_data();
  double result = dyn_prog();
  printf("%0.9lf\n", result);

  return 0;
}
