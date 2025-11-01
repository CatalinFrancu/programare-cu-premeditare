// Complexity: O(q log n).
#include <stdio.h>

const int MAX_N = 256 * 1024;
const int MAX_OPS = 200'000;
const int ST_CLEAN = 2;

int next_power_of_2(int n) {
  return 1 << (32 - __builtin_clz(n - 1));
}

// Segment tree with 0/1 values, range set and range sum operations.
//
// Contract: state[x] can be:
// * 0/1 to indicate a value to be set on all of x's descendants, or
// * ST_CLEAN to indicate that the value has been pushed.
//
// sum[node] takes into account state[node].
struct segment_tree {
  int sum[2 * MAX_N];
  int state[2 * MAX_N];
  int n;

  void init(char* s, int len) {
    n = next_power_of_2(len);
    for (int i = 0; i < len; i++) {
      sum[n + i] = s[i] - '0';
    }
    for (int i = len; i < n; i++) {
      sum[n + i] = 0;
    }
    for (int i = 1; i < 2 * n; i++) {
      state[i] = ST_CLEAN;
    }

    build();
  }

  void build () {
    for (int i = n - 1; i; i--) {
      sum[i] = sum[2 * i] + sum[2 * i + 1];
    }
  }

  void push(int x) {
    if (state[x] != ST_CLEAN) {
      state[2 * x] = state[2 * x + 1] = state[x];
      sum[2 * x] = sum[2 * x + 1] = sum[x] / 2;
      state[x] = ST_CLEAN;
    }
  }

  void push_all() {
    for (int i = 1; i < n; i++) {
      push(i);
    }
  }

  void push_path(int x) {
    int bits = __builtin_popcount(n - 1);
    for (int b = bits; b; b--) {
      push(x >> b);
    }
  }

  void pull_path(int x) {
    for (x /= 2; x; x /= 2) {
      if (state[x] == ST_CLEAN) {
        sum[x] = sum[2 * x] + sum[2 * x + 1];
      }
    }
  }

  void range_set(int l, int r, int val) {
    l += n;
    r += n;
    int orig_l = l, orig_r = r;
    int size = 1;

    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        sum[l] = size * val;
        state[l++] = val;
      }
      l >>= 1;

      if (!(r & 1)) {
        sum[r] = size * val;
        state[r--] = val;
      }
      r >>= 1;
      size <<= 1;
    }

    pull_path(orig_l);
    pull_path(orig_r);
  }

  int range_sum(int l, int r) {
    int result = 0;

    l += n;
    r += n;
    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        result += sum[l++];
      }
      l >>= 1;

      if (!(r & 1)) {
        result += sum[r--];
      }
      r >>= 1;
    }

    return result;
  }

  bool equals(char* s, int len) {
    push_all();

    int i = 0;
    while ((i < len) && (s[i] - '0' == sum[n + i])) {
      i++;
    }

    return (i == len);
  }
};

struct operation {
  int l, r;
};

segment_tree st;
char start[MAX_N + 1], finish[MAX_N + 1];
operation op[MAX_OPS];
int len, num_ops;

void read_data() {
  scanf("%d %d %s %s", &len, &num_ops, start, finish);
  for (int i = 0; i < num_ops; i++) {
    scanf("%d %d\n", &op[i].l, &op[i].r);
    op[i].l--;
    op[i].r--;
  }
}

bool process_op(operation op) {
  int num_ones = st.range_sum(op.l, op.r);
  int num_zeroes = (op.r - op.l + 1) - num_ones;
  if (num_ones == num_zeroes) {
    return false;
  } else {
    int majority = (num_ones > num_zeroes) ? 1 : 0;
    st.range_set(op.l, op.r, majority);
    return true;
  }
}

void process_test() {
  read_data();
  st.init(finish, len);
  int i = num_ops - 1;
  while ((i >= 0) && process_op(op[i])) {
    i--;
  }

  bool success = (i < 0) && st.equals(start, len);
  printf("%s\n", success ? "YES" : "NO");
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    process_test();
  }

  return 0;
}
