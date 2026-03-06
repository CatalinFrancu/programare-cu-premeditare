#include <stdio.h>
#include <string.h>
#include <vector>

const int MAX_K = 1'000'000;
const int MAX_LOG = 30;
const int MOD = 1'000'000'007;
const int ROOT_N = 32'768;

typedef unsigned long long u64;

char k[MAX_K + 1];
int n, len_k;

struct powers_of_2 {
  // p: 2^0, 2^1, 2^2, ..., 2^32767
  // q: 2^0, 2^32768, 2^65536, 2^98304, ... 2^(32768*32767)
  int p[ROOT_N], q[ROOT_N];

  void precompute() {
    p[0] = 1;
    for (int i = 1; i < ROOT_N; i++) {
      p[i] = p[i - 1] * 2 % MOD;
    }

    q[0] = 1;
    q[1] = (2 * p[ROOT_N - 1]) % MOD;
    for (int i = 2; i < ROOT_N; i++) {
      q[i] = (u64)q[i - 1] * q[1] % MOD;
    }
  }

  int get(u64 e) {
    e %= (MOD - 1); // Mica teoremă a lui Fermat.
    u64 large = q[e / ROOT_N];
    u64 small = p[e % ROOT_N];
    return small * large % MOD;
  }
};

powers_of_2 pow2;

struct bit_stream {
  std::vector<bool> col[MAX_LOG];
  int height[MAX_LOG];
  int num_cols;

  void collect_heights(int n) {
    num_cols = 0;
    while (n) {
      if (n & 1) {
        height[num_cols++] = (n + 1) / 2;
      }
      n /= 2;
    }
  }

  void distribute_bits() {
    int c = 0;
    for (int b = len_k - 1; b >= 0; b--) {
      col[c].push_back(k[b] - '0');
      height[c]--;
      if (height[c] < height[c + 1]) {
        c++;
      } else {
        c = 0;
      }
    }
  }

  void init(int n) {
    collect_heights(n);
    distribute_bits();
  }

  std::vector<bool> get_next_column() {
    static int pos = 0;
    return col[pos++];
  }
};

bit_stream bs;

int compute_cross(int size) {
  std::vector<bool> col = bs.get_next_column();
  u64 pos = (u64)(size / 2) * (n + 1);

  u64 result = 0;

  if (col.size() && col[0]) {
    result += pow2.get(pos);
  }

  for (int i = 1; i < (int)col.size(); i++) {
    if (col[i]) {
      result += (u64)pow2.get(pos - (u64)n * i)
        + pow2.get(pos + (u64)n * i)
        + pow2.get(pos - i)
        + pow2.get(pos + i);
    }
  }

  return result % MOD;
}

int compute_matrix(int size) {
  if (!size) {
    return 0;
  }

  u64 cross = (size & 1) ? compute_cross(size) : 0;

  int shift = (1 + size) / 2;
  u64 small = compute_matrix(size / 2);
  u64 mult = (u64)(pow2.get(shift) + 1) * (pow2.get((u64)shift * n) + 1) % MOD;
  u64 corners = small * mult % MOD;

  return (corners + cross) % MOD;
}

void read_data() {
  FILE* f = fopen("hipersimetrie.in", "r");
  fscanf(f, "%d %s", &n, k);
  fclose(f);

  len_k = strlen(k);
}

void decrement_k() {
  char* s = k + len_k - 1;
  while (*s == '0') {
    *s = '1';
    s--;
  }
  *s = '0';
}

void write_result(int result) {
  FILE* f = fopen("hipersimetrie.out", "w");
  fprintf(f, "%d\n", result);
  fclose(f);
}

int main() {
  read_data();
  decrement_k();
  pow2.precompute();
  bs.init(n);

  int result = compute_matrix(n);
  write_result(result);

  return 0;
}
