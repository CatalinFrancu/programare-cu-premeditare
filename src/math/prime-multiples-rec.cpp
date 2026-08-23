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

long long pinex(int k, long long product_so_far) {
  if (k == num_div) {
    return n / product_so_far;
  }

  long long result = pinex(k + 1, product_so_far);
  if ((__int128)llabs(product_so_far) * d[k] <= n) {
    result += pinex(k + 1, -product_so_far * d[k]);
  }
  return result;
}

void write_output(long long result) {
  printf("%lld\n", result);
}

int main() {
  read_data();
  long long result = n - pinex(0, 1);
  write_output(result);

  return 0;
}
