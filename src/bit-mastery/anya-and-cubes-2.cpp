#include <algorithm>
#include <stdio.h>

const int MAX_N = 25;
const int MAX_FACTORIAL = 18;   // 19! > 10^16
const int MAX_LEFT = 1'594'323; // 3^13
const int MAX_RIGHT = 531'441;  // 3^12
const int BITS = 5;

int a[MAX_N], n, num_stickers;
long long target_sum;
long long fact[MAX_FACTORIAL + 1];

struct info {
  long long v;

  long long sum() {
    return v >> BITS;
  }

  int stickers() {
    return v & ((1 << BITS) - 1);
  }

  void set(long long sum, int stickers) {
    v = (sum << BITS) + stickers;
  }
};

info left[MAX_LEFT + 2], right[MAX_RIGHT + 2];
int num_left, num_right;

void precompute_factorials() {
  fact[0] = 1;
  for (int i = 1; i <= MAX_FACTORIAL; i++) {
    fact[i] = fact[i - 1] * i;
  }
}

void read_data() {
  scanf("%d %d %lld", &n, &num_stickers, &target_sum);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
  }
}

struct subset_generator {
  int end;
  info* dest;
  int size;

  void rec(int k, long long sum, char stickers) {
    if (k == end) {
      dest[size++].set(sum, stickers);
      return;
    }

    rec(k + 1, sum, stickers);
    if (sum + a[k] <= target_sum) {
      rec(k + 1, sum + a[k], stickers);
    }
    if ((stickers < num_stickers) &&
        (a[k] <= MAX_FACTORIAL) &&
        (sum + fact[a[k]] <= target_sum)) {
      rec(k + 1, sum + fact[a[k]], stickers + 1);
    }
  }

  void run(int start, int end, info* dest, int& size) {
    this->end = end;
    this->dest = dest;
    this->size = 0;

    rec(start, 0, 0);

    size = this->size;
    // santinele infinite
    dest[size++].set(-(target_sum + 1), 0);
    dest[size++].set((target_sum + 1), 0);

    std::sort(dest, dest + size, [](info a, info b) {
      return a.v < b.v;
    });
  }
};

subset_generator sub_gen;

// Capsule care izolează unele variabile.
struct half_merger {
  int l, r;
  long long result;

  void merge_equal_sums() {
    long long l_sum = left[l].sum();
    int r_start = r;
    while (right[r_start].sum() == right[r].sum()) {
      r_start--;
    }

    while (left[l].sum() == l_sum) {
      if ((r > r_start) &&
          (left[l].stickers() + right[r].stickers() > num_stickers)) {
        r--;
      } else {
        result += r - r_start;
        l++;
      }
    }

    r = r_start;
  }

  long long run() {
    result = 0;
    l = 0;
    r = num_right - 1;

    while (l < num_left) {
      if (left[l].sum() + right[r].sum() < target_sum) {
        l++;
      } else if (left[l].sum() + right[r].sum() > target_sum) {
        r--;
      } else {
        merge_equal_sums();
      }
    }

    return result;
  }
};

half_merger merger;

int main() {
  precompute_factorials();
  read_data();
  sub_gen.run(n / 2, n, left, num_left);
  sub_gen.run(0, n / 2, right, num_right);
  long long ans = merger.run();
  printf("%lld\n", ans);

  return 0;
}
