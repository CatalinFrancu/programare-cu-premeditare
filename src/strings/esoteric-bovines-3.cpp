// Idee inspirată de https://kilonova.ro/submissions/372990
#include <algorithm>
#include <stdio.h>

const int MAX_N = 200'000;
const int NUM_BITS = 60;

struct range_pair {
  int la, ra, lb, rb;
  int za, zb; // punctul din [la, ra) pînă unde avem zerouri
};

range_pair p[2][MAX_N];
int row, np[2];
long long a[MAX_N], b[MAX_N];
int n;
long long k;

void read_data() {
  scanf("%d %lld", &n, &k);
  for (int i = 0; i < n; i++) {
    scanf("%lld", &a[i]);
  }
  for (int i = 0; i < n; i++) {
    scanf("%lld", &b[i]);
  }
}

long long count_zeroes(int bit) {
  long long cnt = 0;
  for (int i = 0; i < np[row]; i++) {
    range_pair& q = p[row][i];
    q.za = q.la;
    while ((q.za < q.ra) && (((a[q.za] >> bit) & 1) == 0)) {
      q.za++;
    }
    q.zb = q.lb;
    while ((q.zb < q.rb) && (((b[q.zb] >> bit) & 1) == 0)) {
      q.zb++;
    }
    cnt += (long long)(q.za - q.la) * (q.zb - q.lb) +
      (long long)(q.ra - q.za) * (q.rb - q.zb);
  }
  return cnt;
}

void add_range(range_pair q) {
  if ((q.la < q.ra) && (q.lb < q.rb)) {
    p[1 - row][np[1 - row]++] = q;
  }
}

void push_once(int target_val) {
  np[1 - row] = 0;
  for (int i = 0; i < np[row]; i++) {
    range_pair q = p[row][i];
    if (target_val == 0) {
      add_range({q.la, q.za, q.lb, q.zb});
      add_range({q.za, q.ra, q.zb, q.rb});
    } else {
      add_range({q.la, q.za, q.zb, q.rb});
      add_range({q.za, q.ra, q.lb, q.zb});
    }
  }
}

long long push_ranges() {
  p[0][0] = { 0, n, 0, n };
  row = 0;
  np[0] = 1;

  long long answer = 0;

  for (int bit = NUM_BITS - 1; bit >= 0; bit--) {
    long long z = count_zeroes(bit);
    if (z < k) {
      k -= z;
      push_once(1);
      answer += 1ll << bit;
    } else {
      push_once(0);
    }
    row = 1 - row;
  }

  return answer;
}

void solve_test_case() {
  read_data();
  std::sort(a, a + n);
  std::sort(b, b + n);
  long long ans = push_ranges();
  printf("%lld\n", ans);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);

  while (num_tests--) {
    solve_test_case();
  }

  return 0;
}
