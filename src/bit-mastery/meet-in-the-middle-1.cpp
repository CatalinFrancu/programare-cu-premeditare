// Complexitate: O(n · 2^(n/2)).
#include <algorithm>
#include <stdio.h>

const int MAX_N = 40;
const int SET_SIZE = 1 << (MAX_N / 2);
const int INFINITY = 2'000'000'000;

int v[MAX_N];
int n, target;

struct arr {
  int v[SET_SIZE + 2], size;

  void push(int val) {
    v[size++] = val;
  }

  void rec(int* src, int pos, int sum_so_far) {
    if (pos < 0) {
      push(sum_so_far);
      return;
    }
    rec(src, pos - 1, sum_so_far);
    if (sum_so_far + src[pos] <= target) {
      rec(src, pos - 1, sum_so_far + src[pos]);
    }
  }

  void gen_sums(int* src, int n) {
    push(-INFINITY);
    push(+INFINITY);
    rec(src, n - 1, 0);
    std::sort(v, v + size);
  }
};

arr a, b;

void read_data() {
  scanf("%d %d", &n, &target);
  for (int i = 0; i < n; i++) {
    scanf("%d", &v[i]);
  }
}

void generate_subset_sums() {
  int n1 = n / 2;
  int n2 = n - n1;
  a.gen_sums(v, n1);
  b.gen_sums(v + n1, n2);
}

long long merge_sets() {
  long long answer = 0;
  int i = 1, j = b.size - 1;

  while ((i < a.size) && (j > 0)) {
    if (a.v[i] + b.v[j] < target) {
      i++;
    } else if (a.v[i] + b.v[j] > target) {
      j--;
    } else {
      int starti = i, startj = j;
      while (a.v[i] == a.v[starti]) {
        i++;
      }
      while (b.v[j] == b.v[startj]) {
        j--;
      }
      answer += (long long)(i - starti) * (startj - j);
    }
  }

  return answer;
}

int main() {
  read_data();
  generate_subset_sums();
  long long answer = merge_sets();
  printf("%lld\n", answer);

  return 0;
}
