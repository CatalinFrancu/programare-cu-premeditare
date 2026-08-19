#include <stdio.h>

const int MAX_LEN = 1 << 17;
const int SIGMA = 26;

const int ST_DESC = 0;
const int ST_ASC = 1;
const int ST_CLEAN = 2;

// Parametrii necesari pentru a itera crescător / descrescător.
const int LOOP_START[2] = { SIGMA - 1, 0 };
const int LOOP_END[2]   = {        -1, SIGMA };
const int LOOP_STEP[2]  = {        -1, +1 };

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

struct node {
  int f[SIGMA];
  unsigned char state;

  int get_first_nonzero() {
    int i = 0;
    while (!f[i]) {
      i++;
    }
    return i;
  }

  void add(node& a, node& b) {
    for (int i = 0; i < SIGMA; i++) {
      f[i] = a.f[i] + b.f[i];
    }
  }

  void accumulate(node& src) {
    for (int i = 0; i < SIGMA; i++) {
      f[i] += src.f[i];
    }
  }

  // Sparge this în a și b. Îl lasă intact pe this.
  void split(node& a, node& b, int a_size) {
    for (int i = LOOP_START[state]; i != LOOP_END[state]; i += LOOP_STEP[state]) {
      int to_a = min(a_size, f[i]);
      a.f[i] = to_a;
      b.f[i] = f[i] - to_a;
      a_size -= to_a;
    }
    a.state = b.state = state;
  }

  // Distribuie quantity litere din this în dest.
  void distribute(node& dest, int quantity) {
    for (int i = LOOP_START[state]; i != LOOP_END[state]; i += LOOP_STEP[state]) {
      int to_dest = min(quantity, f[i]);
      dest.f[i] = to_dest;
      f[i] -= to_dest;
      quantity -= to_dest;
    }
    dest.state = state;
  }
};

struct segment_tree {
  node v[2 * MAX_LEN], acc;
  int orig_len, n;

  void init(char* s, int len) {
    orig_len = len;
    n = next_power_of_2(len);

    for (int i = 0; i < len; i++) {
      int c = s[i] - 'a';
      v[n + i].f[c] = 1;
    }

    for (int i = n - 1; i; i--) {
      v[i].add(v[2 * i], v[2 * i + 1]);
    }

    for (int i = 1; i < 2 * n; i++) {
      v[i].state = ST_CLEAN;
    }
  }

  void push(int pos, int size) {
    if (v[pos].state != ST_CLEAN) {
      int l = 2 * pos, r = 2 * pos + 1;
      v[pos].split(v[l], v[r], size / 2);
      v[pos].state = ST_CLEAN;
    }
  }

  void push_all() {
    for (int start = 1; start < n; start *= 2) {
      int size = n / start;
      for (int i = start; i < 2 * start; i++) {
        push(i, size);
      }
    }
  }

  void pull(int pos) {
    v[pos].add(v[2 * pos], v[2 * pos + 1]);
  }

  void collect(int node, int pl, int pr, int l, int r) {
    if ((l == pl) && (r == pr)) {
      acc.accumulate(v[node]);
    } else if (l < r) {
      push(node, pr - pl);
      int mid = (pl + pr) >> 1;
      collect(2 * node, pl, mid, l, min(r, mid));
      collect(2 * node + 1, mid, pr, max(l, mid), r);
    }
  }

  void distribute(int node, int pl, int pr, int l, int r) {
    if ((l == pl) && (r == pr)) {
      acc.distribute(v[node], pr - pl);
    } else if (l < r) {
      int mid = (pl + pr) >> 1;
      distribute(2 * node, pl, mid, l, min(r, mid));
      distribute(2 * node + 1, mid, pr, max(l, mid), r);
      pull(node);
    }
  }

  void sort(int l, int r, int dir) {
    collect(1, 0, n, l, r);
    acc.state = dir;
    distribute(1, 0, n, l, r);
  }

  void export_(char* dest) {
    push_all();

    for (int i = 0; i < orig_len; i++) {
      dest[i] = 'a' + v[n + i].get_first_nonzero();
    }
  }
};

char s[MAX_LEN + 2];
segment_tree st;
int len, num_ops;

void read_string() {
  scanf("%d %d %s", &len, &num_ops, s);
}

void process_ops() {
  int left, right, dir;

  while (num_ops--) {
    scanf("%d %d %d", &left, &right, &dir);
    st.sort(left - 1, right, dir); // 0-based, [închis, deschis)
  }
}

void write_answer() {
  printf("%s\n", s);
}

int main() {
  read_string();
  st.init(s, len);
  process_ops();
  st.export_(s);
  write_answer();

  return 0;
}
