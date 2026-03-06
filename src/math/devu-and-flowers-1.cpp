#include <stdio.h>

const int MAX_BOXES = 20;
const int MOD = 1'000'000'007;

long long cap[MAX_BOXES], num_objects;
int num_boxes, denominator;

// Calculează x^{-1} % MOD ca x^{MOD-2} % MOD.
int inverse(int x) {
  long long result = 1;
  int e = MOD - 2;

  while (e) {
    if (e & 1) {
      result = result * x % MOD;
    }
    x = (long long)x * x % MOD;
    e >>= 1;
  }

  return result;
}

void precompute_denominator() {
  denominator = 1;
  for (int i = 1; i < num_boxes; i++) {
    denominator = (long long)denominator * inverse(i) % MOD;
  }
}

// Returnează combinări(n, num_boxes - 1).
int comb(long long n) {
  n %= MOD; // altfel result * n poate depăși
  n += MOD; // altfel n + 1 - i poate trece pe negativ

  long long result = 1;
  for (int i = 1; i < num_boxes; i++) {
    result = result * (n + 1 - i) % MOD;
  }
  return result * denominator % MOD;
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
  precompute_denominator();
  int answer = pie(0, num_objects, +1);
  write_answer(answer);

  return 0;
}
