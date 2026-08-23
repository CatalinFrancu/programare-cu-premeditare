#include <algorithm>
#include <stdio.h>

typedef unsigned long long u64;

const int MAX_BULBS = 1'000;
const int MAX_SWITCHES = 30;
const int FRAC_LEFT = 18;
const int HASH_BASE = 37;
const u64 HASH_MOD = 18'446'744'073'709'551'263ull;
const int HASH_MAP_SIZE = 1 << (FRAC_LEFT + 2); // 4x numărul de valori
const char NONE = -1;

u64 powers[MAX_BULBS];

// tabelă hash cu linear probing
struct hash_map {
  u64 key[HASH_MAP_SIZE];
  char val[HASH_MAP_SIZE];

  void init() {
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
      val[i] = NONE;
    }
  }

  int find_pos(u64 k) {
    int pos = k % HASH_MAP_SIZE;
    while ((key[pos] != k) && (val[pos] != NONE)) {
      pos = (pos + 1) % HASH_MAP_SIZE;
    }
    return pos;
  }

  void minimize(u64 k, char v) {
    int pos = find_pos(k);
    if ((val[pos] == NONE) || (v < val[pos])) {
      key[pos] = k;
      val[pos] = v;
    }
  }

  char find(u64 k) {
    int pos = find_pos(k);
    return val[pos];
  }
};

struct state {
  u64 mask;
  char touched;
};

u64 sw[MAX_SWITCHES];
hash_map left_map;
state right_table[(1 << (MAX_SWITCHES - FRAC_LEFT)) + 1];
int n, s, num_days, left, right;

u64 read_bitset() {
  u64 mask = 0;
  int k, pos;
  scanf("%d", &k);
  while (k--) {
    scanf("%d", &pos);
    pos--;
    mask ^= powers[pos];
  }
  return mask;
}

void read_switches() {
  scanf("%d %d %d", &n, &s, &num_days);
  for (int i = 0; i < s; i++) {
    sw[i] = read_bitset();
  }
  left = s * FRAC_LEFT / MAX_SWITCHES;
  right = s - left;
}

void compute_powers() {
  powers[0] = 1;
  for (int i = 1; i < MAX_BULBS; i++) {
    powers[i] = (__int128)powers[i - 1] * HASH_BASE % HASH_MOD;
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void rec_left(int level, char touched, u64 mask) {
  if (level == left) {
    left_map.minimize(mask, touched);
  } else {
    rec_left(level + 1, touched, mask);
    rec_left(level + 1, touched + 1, mask ^ sw[level]);
  }
}

void create_left_hash_map() {
  left_map.init();
  rec_left(0, 0, 0);
}

void rec_right(int level, char touched, u64 mask) {
  static int pos = 0;

  if (level == s) {
    right_table[pos++] = { mask, touched };
  } else {
    rec_right(level + 1, touched, mask);
    rec_right(level + 1, touched + 1, mask ^ sw[level]);
  }
}

void create_right_table() {
  rec_right(left, 0, 0);
  std::sort(right_table, right_table + (1 << right), [](state a, state b) {
    return a.touched < b.touched;
  });
  right_table[1 << right] = { 0, MAX_SWITCHES + 1 }; // santinelă
}

void answer_queries() {
  while (num_days--) {
    u64 day = read_bitset();
    int min_touched = s + 1;

    int p = 0;
    while (right_table[p].touched < min_touched) {
      u64 desired = day ^ right_table[p].mask;
      char left_touched = left_map.find(desired);
      if (left_touched != NONE) {
        min_touched = min(min_touched, left_touched + right_table[p].touched);
      }
      p++;
    }
    printf("%d\n", (min_touched == s + 1) ? -1 : min_touched);
  }
}

int main() {
  compute_powers();
  read_switches();
  create_left_hash_map();
  create_right_table();
  answer_queries();

  return 0;
}
