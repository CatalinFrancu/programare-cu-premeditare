#include <stdio.h>

const int MAX_N = 1'000'000;
// 2 * 3 * 5 * 7 * 11 * 13 * 17 * 19 > MAX_N
const int MAX_PRIME_FACTORS = 7;
// Răspunsul pentru 1'000'000 510'510 1'000'000 este 6,5 M
const int INFINITY = 7'000'000;

int spf[MAX_N + 1];  // cel mai mic factor prim, e.g. spf[225] = 3
int rest[MAX_N + 1]; // restul după eliminarea factorilor egali, e.g. rest[225] = 25

void sieve() {
  for (int i = 2; i <= MAX_N; i++) {
    if (!spf[i]) {
      for (int j = 1, k = i; k <= MAX_N; j++, k += i) {
        if (!spf[k]) {
          spf[k] = i;
          rest[k] = j;
          while (rest[k] % i == 0) {
            rest[k] /= i;
          }
        }
      }
    }
  }
}

int pinex(int up_to, int remaining, int product_so_far) {
  if (remaining == 1) {
    return up_to / product_so_far;
  }

  return
      pinex(up_to, rest[remaining], product_so_far) +
      pinex(up_to, rest[remaining], -product_so_far * spf[remaining]);
}

int count_coprimes(int p, int up_to) {
  return pinex(up_to, p, 1);
}

int find_kth_coprime(int p, int k) {
  int l = 0, r = INFINITY; // (l exclusiv, r inclusiv]
  while (r - l > 1) {
    int mid = (l + r) / 2;
    if (count_coprimes(p, mid) < k) {
      l = mid;
    } else {
      r = mid;
    }
  }
  return r;
}

void solve_test() {
  int x, p, k;
  scanf("%d %d %d", &x, &p, &k);

  int up_to_x = count_coprimes(p, x);
  int answer = find_kth_coprime(p, k + up_to_x);
  printf("%d\n", answer);
}

int main() {
  sieve();

  int num_tests;
  scanf("%d", &num_tests);

  while (num_tests--) {
    solve_test();
  }

  return 0;
}
