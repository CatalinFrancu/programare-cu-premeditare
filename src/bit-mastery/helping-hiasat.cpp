#include <iostream>
#include <unordered_map>

const int MAX_M = 40;
const int T_CHANGE = 1;

// Normalizează numele.
struct normalizer {
  std::unordered_map<std::string, int> map;

  int normalize(std::string s) {
    auto it = map.find(s);
    if (it == map.end()) {
      int result = map.size();
      map[s] = result;
      return result;
    } else {
      return it->second;
    }
  }
};

long long exclude[MAX_M];
char max_submask[1 << (MAX_M / 2)];
int m, half;

int max(int x, int y) {
  return (x > y) ? x : y;
}

void exclude_pairs(long long mask) {
  for (int b = 0; b < m; b++) {
    if (mask & (1ll << b)) {
      exclude[b] |= mask ^ (1ll << b);
    }
  }
}

void read_data_and_create_exclusions() {
  int n, type;
  long long mask = 0;
  normalizer norm;

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::cin >> n >> m;
  half = m / 2;

  while (n--) {
    std::cin >> type;
    if (type == T_CHANGE) {
      exclude_pairs(mask);
      mask = 0;
    } else {
      std::string s;
      std::cin >> s;
      int id = norm.normalize(s);
      mask |= 1ll << id;
    }
  }

  exclude_pairs(mask);
}

// k = bitul curent
// mask = o mască pe primii k biți
void rec_left(int k, int mask) {
  if (k == half) {
    max_submask[mask] = __builtin_popcount(mask);
  } else {
    rec_left(k + 1, mask);
    if ((exclude[k] & mask) == 0) {
      rec_left(k + 1, mask | (1 << k));
    }
  }
}

void propagate_feasible_masks_left() {
  for (int mask = 0; mask < (1 << half); mask++) {
    if (!max_submask[mask]) { // optimizare de viteză
      for (int b = 0; b < half; b++) {
        if (mask & (1 << b)) {
          max_submask[mask] = max(max_submask[mask],
                                  max_submask[mask ^ (1 << b)]);
        }
      }
    }
  }
}

int max_pop = 0;

void rec_right(int k, long long mask, int excluded_left) {
  if (k == m) {
    int allowed_left = ((1 << half) - 1) ^ excluded_left;
    int pop = __builtin_popcountll(mask) + max_submask[allowed_left];
    max_pop = max(max_pop, pop);
  } else {
    rec_right(k + 1, mask, excluded_left);
    if ((exclude[k] & mask) == 0) {
      int excluded_left_by_k = exclude[k] & ((1 << half) - 1);
      rec_right(k + 1, mask | (1ll << k), excluded_left | excluded_left_by_k);
    }
  }
}

int main() {
  read_data_and_create_exclusions();
  rec_left(0, 0);
  propagate_feasible_masks_left();
  rec_right(half, 0, 0);
  std::cout << max_pop << '\n';

  return 0;
}
