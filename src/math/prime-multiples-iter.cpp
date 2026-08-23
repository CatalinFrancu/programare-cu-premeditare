#include <stdio.h>
#include <stdlib.h>

const int MAX_DIV = 20;

long long n, d[MAX_DIV];
int num_div;

void read_data() {
  scanf("%lld %d", &n, &num_div);
  for (int i = 0; i < num_div; i++) {
    scanf("%lld", &d[i]);
  }
}

long long make_product(long long mask) {
  long long prod = -1;
  bool fits = true;

  for (int bit = 0; bit < num_div; bit++) {
    if (mask & (1 << bit)) {
      fits &= ((__int128)llabs(prod) * d[bit] <= n);
      prod *= -d[bit];
    }
  }

  return fits ? prod : (n + 1);
}

long long pinex() {
  long long result = 0;

  for (int mask = 1; mask < (1 << num_div); mask++) {
    long long prod = make_product(mask);
    result += n / prod;
  }

  return result;
}

void write_output(long long result) {
  printf("%lld\n", result);
}

int main() {
  read_data();
  long long result = pinex();
  write_output(result);

  return 0;
}
