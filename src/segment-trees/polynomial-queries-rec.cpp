// Complexitate: O(q log n)
//
// Metodă: menține un arbore de intervale care admite sume pe interval și
// adăugarea unei progresii pe interval.
#include <stdio.h>

const int MAX_N = 1 << 18;
const int T_UPDATE = 1;

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

int next_power_of_2(int n) {
  return 1 << (32 - __builtin_clz(n - 1));
}

long long progression_sum(long long first, long long step, int len) {
  return first * len + step * (len - 1) * len / 2;
}

// Invarianți:
//
// 1. Valorile reale ale nodurilor subîntines sînt valorile lor v respective
//    plus o progresie aritmetică definită prin first și step.
// 2. v[k] include first[k] și step[k], dar nu și valorile de la strămoși.
// 3. Toate intervalele sînt [închis, deschis).
struct segment_tree {
  long long v[2 * MAX_N];
  long long first[2 * MAX_N];
  long long step[2 * MAX_N];
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
  }

  void set(int pos, int val) {
    v[pos + n] = val;
  }

  void build() {
    for (int i = n - 1; i; i--) {
      v[i] = v[2 * i] + v[2 * i + 1];
    }
  }

  void push(int t, int len) {
    first[2 * t] += first[t];
    step[2 * t] += step[t];
    v[2 * t] += progression_sum(first[t], step[t], len / 2);

    int right_first = first[t] + step[t] * len / 2;
    first[2 * t + 1] += right_first;
    step[2 * t + 1] += step[t];
    v[2 * t + 1] += progression_sum(right_first, step[t], len / 2);

    first[t] = 0;
    step[t] = 0;
  }

  void pull(int t) {
    v[t] = v[2 * t] + v[2 * t + 1];
  }

  // pos1 = poziția unde scriem 1
  void add_progression_helper(int t, int pl, int pr, int l, int r, int pos1) {
    if (l >= r) {
      return;
    } else if ((l == pl) && (r == pr)) {
      int value_at_l = l - pos1 + 1;
      first[t] += value_at_l;
      step[t]++;
      v[t] += progression_sum(value_at_l, 1, r - l);
    } else {
      push(t, pr - pl);
      int mid = (pl + pr) >> 1;
      add_progression_helper(2 * t, pl, mid, l, min(r, mid), pos1);
      add_progression_helper(2 * t + 1, mid, pr, ::max(l, mid), r, pos1);
      pull(t);
    }
  }

  void add_progression(int left, int right) {
    add_progression_helper(1, 0, n, left, right, left);
  }

  long long range_sum_helper(int t, int pl, int pr, int l, int r) {
    if (l >= r) {
      return 0;
    } else if ((l == pl) && (r == pr)) {
      return v[t];
    } else {
      push(t, pr - pl);
      int mid = (pl + pr) >> 1;
      return range_sum_helper(2 * t, pl, mid, l, min(r, mid)) +
        range_sum_helper(2 * t + 1, mid, pr, ::max(l, mid), r);
    }
  }

  long long range_sum(int left, int right) {
    return range_sum_helper(1, 0, n, left, right);
  }
};

segment_tree s;
int n, num_ops;

void read_array() {
  scanf("%d %d", &n, &num_ops);
  s.init(n);
  for (int i = 0; i < n; i++) {
    int x;
    scanf("%d", &x);
    s.set(i, x);
  }
  s.build();
}

void process_ops() {
  while (num_ops--) {
    int type, l, r;
    scanf("%d %d %d", &type, &l, &r);
    if (type == T_UPDATE) {
      s.add_progression(l - 1, r);
    } else {
      printf("%lld\n", s.range_sum(l - 1, r));
    }
  }
}

int main() {
  read_array();
  process_ops();

  return 0;
}
