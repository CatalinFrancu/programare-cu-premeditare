#include <bitset>
#include <stdio.h>
#include <vector>

const int MAX_N = 10'000;
const int MAX_SEGTREE = 32'768;

typedef std::bitset<MAX_N + 1> bitset;

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

struct segment_tree {
  std::vector<short> exes[MAX_SEGTREE];
  bitset accumulator; // OR-ul tuturor bitseturilor pentru toate intervalele
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
  }

  void add_x(int l, int r, int x) {
    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        exes[l++].push_back(x);
      }
      l >>= 1;

      if (!(r & 1)) {
        exes[r--].push_back(x);
      }
      r >>= 1;
    }
  }

  void dfs(int pos, bitset current) {
    for (int x: exes[pos]) {
      current |= current << x;
    }
    if (pos >= n) {
      accumulator |= current;
    } else {
      dfs(2 * pos, current);
      dfs(2 * pos + 1, current);
    }
  }

  bitset count_obtainable_sums() {
    accumulator.reset();
    bitset current(1);
    dfs(1, current);
    return accumulator;
  }
};

segment_tree st;
int n;

void read_intervals_into_segtree() {
  int num_intervals;
  scanf("%d %d", &n, &num_intervals);
  st.init(n);

  while (num_intervals--) {
    int l, r, x;
    scanf("%d %d %d", &l, &r, &x);
    st.add_x(l - 1, r - 1, x);
  }
}

void write_answer(bitset& answer) {
  int set_bits_in_1_n = (answer >> 1).count() - (answer >> (n + 1)).count();
  printf("%d\n", set_bits_in_1_n);
  for (int i = 1; i <= n; i++) {
    if (answer[i]) {
      printf("%d ", i);
    }
  }
  printf("\n");
}

int main() {
  read_intervals_into_segtree();
  bitset answer = st.count_obtainable_sums();
  write_answer(answer);

  return 0;
}
