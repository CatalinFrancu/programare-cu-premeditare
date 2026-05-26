#include <stdio.h>
#include <string.h>

const int MAX_N = 200'000;
const int MAX_SEGTREE_NODES = 1 << 20;

char s[2 * MAX_N + 1]; // A și B reunite.
int n;

void read_strings() {
  scanf("%d %s ", &n, s);
  scanf("%s", s + n);
  for (int i = 0; i < n; i++) {
    s[n + i] ^= 1;
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

struct min_segment_tree {
  int m[MAX_SEGTREE_NODES];
  int n;

  void init(int* v, int _n) {
    n = next_power_of_2(_n);
    for (int i = 0; i < _n; i++) {
      m[n + i] = v[i];
    }

    for (int i = n - 1; i; i--) {
      m[i] = min(m[2 * i], m[2 * i + 1]);
    }
  }

  int range_min(int l, int r) {
    l += n;
    r += n;
    int result = n;

    while (l <= r)  {
      if (l & 1) {
        result = min(result, m[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        result = min(result, m[r--]);
      }
      r >>= 1;
    }

    return result;
  }
};

struct suffix_array {
  const char* s;
  int n, num_classes;

  int p[2 * MAX_N + 1];
  int c[2 * MAX_N + 1];
  int p2[2 * MAX_N + 1];
  int c2[2 * MAX_N + 1];

  int cnt[256];

  int lcp[2 * MAX_N + 1];
  min_segment_tree st;

  void init(char* s, int n) {
    this->s = s;
    this->n = n;
  }

  int circ(int x, int n) {
    return (x >= n) ? (x - n) : x;
  }

  void build_single_letters() {
    memset(cnt, 0, sizeof(int) * 256);
    for (int i = 0; i < n; i++) {
      cnt[(int)s[i]]++;
    }
    for (int i = 1; i <= 'z'; i++) {
      cnt[i] += cnt[i - 1];
    }

    for (int i = 0; i < n; i++) {
      p[--cnt[(int)s[i]]] = i;
    }

    c[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      c[p[i]] = c[p[i - 1]] + (s[p[i]] != s[p[i - 1]]);
    }
    num_classes = 1 + c[p[n - 1]];
  }

  void build_prefix_len(int len) {
    memset(cnt, 0, sizeof(int) * num_classes);
    for (int i = 0; i < n; i++) {
      cnt[c[i]]++;
    }
    for (int i = 1; i < num_classes; i++) {
      cnt[i] += cnt[i - 1];
    }

    for (int i = 0; i < n; i++) {
      p2[i] = circ(p[i] - len + n, n);
    }
    for (int i = n - 1; i >= 0; i--) {
      p[--cnt[c[p2[i]]]] = p2[i];
    }

    c2[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      bool diff =  (c[p[i]] != c[p[i - 1]]) ||
        (c[circ(p[i] + len, n)] != c[circ(p[i - 1] + len, n)]);
      c2[p[i]] = c2[p[i - 1]] + diff;
    }
    num_classes = 1 + c2[p[n - 1]];
    memcpy(c, c2, sizeof(int) * n);
  }

  void build() {
    build_single_letters();

    int len = 1;
    while (num_classes < n) {
      build_prefix_len(len);
      len <<= 1;
    }
  }

  void build_lcp() {
    int l = 0;
    for (int i = 0; i < n; i++) {
      int k = c[i];
      if (k) {
        int j = p[k - 1];
        while (s[i + l] == s[j + l]) {
          l++;
        }
        lcp[k] = l;

        if (l) {
          l--;
        }
      }
    }

    st.init(lcp, n + 1);
  }

  int get_lcp(int i, int j) {
    if (i > j) {
      int tmp = i; i = j; j = tmp;
    }
    return st.range_min(i + 1, j);
  }
};

suffix_array suf;

bool query(int x, int y, int l) {
  int p1 = suf.c[x];
  int p2 = suf.c[y + n];
  int pref = suf.get_lcp(p1, p2);

  return (pref >= l) || (s[x + pref] == '0');
}

void process_queries() {
  int q, x, y, l;

  scanf("%d", &q);
  while (q--) {
    scanf("%d %d %d", &x, &y, &l);
    printf("%d ", query(x, y, l));
  }
  printf("\n");
}

int main() {
  read_strings();
  suf.init(s, 2 * n + 1);
  suf.build();
  suf.build_lcp();
  process_queries();

  return 0;
}
