#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int MAX_COORD = 70'000;

extern void play();

struct point {
  double x, y;

  double dist2() {
    return x * x + y * y;
  }

};

point tassadar;

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int micros = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(micros);
}

int makeStep(double angle) {
  printf("Got called with angle %.6lf\n", angle);
  point new_t = {
    tassadar.x + cos(angle),
    tassadar.y + sin(angle),
  };

  int result;
  if (new_t.dist2() <= 1) {
    result = -1;
  } else if (new_t.dist2() < tassadar.dist2()) {
    result = 1;
  } else {
    result = 0;
  }

  tassadar = new_t;
  printf("New coords (%.6lf,%.6lf), dist %.6lf, result %d\n",
         tassadar.x, tassadar.y, sqrt(tassadar.dist2()), result);

  return result;
}

int main() {
  init_rng();

  tassadar.x = rand() % MAX_COORD + 2;
  tassadar.y = rand() % MAX_COORD + 2;
  printf("Chose %.0lf %.0lf\n", tassadar.x, tassadar.y);
  play();

  return 0;
}
