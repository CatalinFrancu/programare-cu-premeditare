// Complexitate: O(2^(n/2)).
#include <stdio.h>

const int MAX_N = 40;
const int SET_SIZE = 1 << (MAX_N / 2);
const int INFINITY = 2'000'000'000;

int v[MAX_N];
int sum1[SET_SIZE + 2], sum2[SET_SIZE + 2];
int n, n1, n2, size1, size2, target;

void read_data() {
  scanf("%d %d", &n, &target);
  for (int i = 0; i < n; i++) {
    scanf("%d", &v[i]);
  }
}

// Interclasează v[1...n] și v[1...n] + x în v[1...2n].
int include_element(int* v, int n, int x) {
  int i = n, j = n;
  for (int k = 2 * n; k >= 1; k--) {
    if (v[i] > v[j] + x) {
      v[k] = v[i--];
    } else {
      v[k] = v[j--] + x;
    }
  }

  n *= 2;
  while (v[n] > target) {
    n--;
  }

  return n;
}

int gen_sums(int* v, int n, int* dest) {
  int size = 1;
  dest[0] = -INFINITY;
  dest[1] = 0;
  for (int i = 0; i < n; i++) {
    size = include_element(dest, size, v[i]);
  }

  return size;
}

void generate_subset_sums() {
  n1 = n / 2;
  n2 = n - n1;
  size1 = gen_sums(v, n1, sum1);
  size2 = gen_sums(v + n1, n2, sum2);
}

void merge_sets(int* a, int na, int* b, int nb) {
  long long answer = 0;

  a[na + 1] = INFINITY;
  b[0] = -INFINITY;
  int i = 1, j = nb;

  while ((i <= na) && (j > 0)) {
    if (a[i] + b[j] < target) {
      i++;
    } else if (a[i] + b[j] > target) {
      j--;
    } else {
      int starti = i, startj = j;
      while (a[i] == a[starti]) {
        i++;
      }
      while (b[j] == b[startj]) {
        j--;
      }
      answer += (long long)(i - starti) * (startj - j);
    }
  }

  printf("%lld\n", answer);
}

int main() {
  read_data();
  generate_subset_sums();
  merge_sets(sum1, size1, sum2, size2);

  return 0;
}
