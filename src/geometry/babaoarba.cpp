#include "babaoarba.h"
#include <math.h>

const int NUM_JIGGLES = 25;
const int CLOSER = 1;
const int FURTHER = 0;
const int FOUND = -1;

void play() {
  double l = 0.0, r = 2 * M_PI;
  bool found = false;

  int iter = 0;
  do {
    double mid = (l + r) / 2;
    int answer = makeStep(mid + M_PI / 2);
    if (answer == CLOSER) {
      l = mid;
      // There and back again.
      makeStep(mid - M_PI / 2);
    } else if (answer == FURTHER) {
      r = mid;
      makeStep(mid - M_PI / 2);
    } else {
      found = true;
    }
  } while (!found && (++iter < NUM_JIGGLES));

  double angle = (l + r) / 2;

  while (!found) {
    found = (makeStep(angle) == FOUND);
  }
}
