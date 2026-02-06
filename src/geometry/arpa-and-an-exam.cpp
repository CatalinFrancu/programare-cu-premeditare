#include <iostream>

long long dist2(long long ax, long long ay, long long bx, long long by) {
  return ((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
}

int main() {
  long long ax, ay, bx, by, cx, cy;
  std::cin >> ax >> ay >> bx >> by >> cx >> cy;

  bool noncollinear = ((cx - ax) * (by - ay) - (cy - ay) * (bx - ax) != 0);
  bool equidistant = dist2(ax, ay, bx, by) == dist2(bx, by, cx, cy);

  std::cout << ((noncollinear && equidistant) ? "Yes\n" : "No\n");

  return 0;
}
