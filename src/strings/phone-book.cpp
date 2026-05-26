#include <stdio.h>
#include <string.h>

const int MAX_SUM_LEN = 200'000;
const int MAX_SEGTREE_NODES = 1 << 20;

char concat[2 * MAX_SUM_LEN];
char* s[MAX_SUM_LEN];
int answer[MAX_SUM_LEN];
int num_s;
int c_len; // Lungimea șirurilor concatenate, inclusiv delimitatorii \0.

void read_data() {
  scanf("%d", &num_s);
  for (int i = 0; i < num_s; i++) {
    s[i] = concat + c_len;
    scanf(" %s", s[i]);
    c_len += strlen(s[i]) + 1;
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
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

  int p[2 * MAX_SUM_LEN];
  int c[2 * MAX_SUM_LEN];
  int p2[2 * MAX_SUM_LEN];
  int c2[2 * MAX_SUM_LEN];

  int cnt[256];

  int lcp[2 * MAX_SUM_LEN];
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
    return st.range_min(i + 1, j);
  }

  // p2[] și c2[] nu ne mai trebuie. Înlocuiește-l pe p2 cu ID-ul șirului din
  // care provine sufixul. Înlocuiește-l pe c2 cu deplasarea în acel șir.
  void replace_by_string_id() {
    int id = 0, len = strlen(s);
    for (int i = 0; i < n; i++) {
      p2[c[i]] = id;
      c2[c[i]] = len--;
      if (s[i] == '\0') {
        id++;
        len = strlen(s + i + 1);
      }
    }
  }
};

suffix_array suf;

// Toate sufixele din [start, end) provin din același șir. Dorim să selectăm
// un prefix diferit de sufixul de la start-1 (dacă există) și de cel la end
// (dacă există).
void process_batch(int start, int end) {
  int id = suf.p2[start];
  for (int i = start; i < end; i++) {
    int lcp_start = (start > num_s) ? suf.get_lcp(start - 1, i) : 0;
    int lcp_end = (end < c_len) ? suf.get_lcp(i, end) : 0;
    int to_take = 1 + max(lcp_start, lcp_end);
    if (to_take <= suf.c2[i]) {
      answer[id] = min(answer[id], to_take);
    }
  }
}

void compute_answers() {
  for (int i = 0; i < num_s; i++) {
    answer[i] = c_len; // infinit
  }
  int i = num_s; // Sari peste sufixele goale.
  while (i < c_len) {
    int j = i + 1;
    while ((j < c_len) && (suf.p2[j] == suf.p2[i])) {
      j++;
    }
    process_batch(i, j);
    i = j;
  }
}

void write_answers() {
  for (int i = 0; i < num_s; i++) {
    printf("%d\n", (answer[i] == c_len) ? -1 : answer[i]);
  }
}

int main() {
  read_data();
  suf.init(concat, c_len);
  suf.build();
  suf.build_lcp();
  suf.replace_by_string_id();
  compute_answers();
  write_answers();

  return 0;
}
