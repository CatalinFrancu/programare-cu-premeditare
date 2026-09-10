#include <algorithm>
#include <stdio.h>

const int MAX_N = 100;
const int NUM_PRIMES = 15; // pînă la MAX_N / 2
const int PRIMES[NUM_PRIMES] = {
  2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
};

int n;
long long a[MAX_N + 1];
int cnt[MAX_N];

void read_array() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%lld", &a[i]);
  }
}

bool has_duplicates() {
  std::sort(a, a + n);
  a[n] = a[n - 1]; // santinelă
  int i = 1;
  while (a[i] != a[i - 1]) {
    i++;
  }
  return (i < n);
}

bool some_remainder_has_frequency_under_2_mod(int p) {
  for (int i = 0; i < p; i++) {
    cnt[i] = 0;
  }

  int num_good_remainders = p;
  int i = 0;
  while ((i < n) && num_good_remainders) {
    int rem = a[i] % p;
    if (++cnt[rem] == 2) {
      num_good_remainders--;
    }
    i++;
  }

  return num_good_remainders;
}

bool check_all_remainders() {
  int p = 0;

  while ((p < NUM_PRIMES) &&
         some_remainder_has_frequency_under_2_mod(PRIMES[p])) {
    p++;
  }

  return (p == NUM_PRIMES);
}

void solve_test() {
  read_array();
  if (!has_duplicates() && check_all_remainders()) {
    printf("YES\n");
  } else {
    printf("NO\n");
  }
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);

  while (num_tests--) {
    solve_test();
  }

  return 0;
}
