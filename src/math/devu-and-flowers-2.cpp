// v3: Hardcodăm 1/(num_boxes-1)!, doar ca distracție.
#include <stdio.h>

const int MAX_BOXES = 20;
const int MOD = 1'000'000'007;

// DENOM[i] = 1 / (i-1)!
int DENOM[MAX_BOXES + 1] = {
  0, 1, 1, 500000004, 166666668, 41666667, 808333339, 301388891, 900198419,
  487524805, 831947206, 283194722, 571199524, 380933296, 490841026, 320774361,
  821384963, 738836565, 514049213, 639669405, 402087866,
};

long long cap[MAX_BOXES], num_objects;
int num_boxes;

// Returnează combinări(n, num_boxes - 1).
int comb(long long n) {
  n %= MOD; // altfel result * n poate depăși
  n += MOD; // altfel n + 1 - i poate trece pe negativ

  long long result = DENOM[num_boxes];
  for (int i = 1; i < num_boxes; i++) {
    result = result * (n + 1 - i) % MOD;
  }
  return result;
}

void read_data() {
  scanf("%d %lld", &num_boxes, &num_objects);
  for (int i = 0; i < num_boxes; i++) {
    scanf("%lld", &cap[i]);
  }
}

int pie(int box, long long num_objects, int sign) {
  if (box == num_boxes) {
    int c = comb(num_objects + num_boxes - 1);
    return (sign == 1) ? c : (MOD - c);
  }

  int sum = pie(box + 1, num_objects, sign);
  if (num_objects > cap[box]) {
    sum += pie(box + 1, num_objects - cap[box] - 1, -sign);
  }

  return sum % MOD;
}
// Mmmmm, pie.

void write_answer(int answer) {
  printf("%d\n", answer);
}

int main() {
  read_data();
  int answer = pie(0, num_objects, +1);
  write_answer(answer);

  return 0;
}
