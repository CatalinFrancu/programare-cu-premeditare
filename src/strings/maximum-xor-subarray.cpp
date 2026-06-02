#include <algorithm>
#include <stdio.h>

const int MAX_N = 200'000;
const int NUM_BITS = 30;

struct trie {
  int child[MAX_N * NUM_BITS + 1][2];
  int size = 1;

  void insert(int x) {
    int u = 1;
    for (int bit = NUM_BITS - 1; bit >= 0; bit--) {
      int val = (x >> bit) & 1;
      if (!child[u][val]) {
        child[u][val] = ++size;
      }
      u = child[u][val];
    }
  }

  // Găsește xor-ul maxim între x și o valoare inserată anterior.
  int find_xor(int x) {
    int u = 1, pair = 0;
    for (int bit = NUM_BITS - 1; bit >= 0; bit--) {
      int val = !((x >> bit) & 1);
      if (!child[u][val]) {
        val = !val;
      }
      u = child[u][val];
      pair = (pair << 1) + val;
    }
    return x ^ pair;
  }
};

trie t;

int main() {
  int answer = 0, partial_xor = 0, x, n;
  scanf("%d", &n);

  t.insert(0);
  while (n--) {
    scanf("%d", &x);
    partial_xor ^= x;
    answer = std::max(answer, t.find_xor(partial_xor));
    t.insert(partial_xor);
  }

  printf("%d\n", answer);

  return 0;
}
