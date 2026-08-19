#include <algorithm>
#include <stdio.h>

const int MAX_QUERIES = 100'000;
const int T_INSERT = 1;
const int T_ERASE = 2;
const int NONE = -1;

struct fenwick_tree {
  int v[MAX_QUERIES + 1];
  int n;

  void init(int _n) {
    n = _n;
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  int prefix_sum(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }
};

struct query {
  int index;
  int type;
  int time;
  int val;
};

query q[MAX_QUERIES];
int answer[MAX_QUERIES];
fenwick_tree fen;
int n;

void read_queries() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d %d", &q[i].type, &q[i].time, &q[i].val);
    q[i].index = i + 1; // 1-based datorită indexării AIB-ului
  }
}

void sort_by_value_then_time() {
  std::sort(q, q + n, [](query& a, query& b) {
    return (a.val < b.val) ||
      ((a.val == b.val) && (a.time < b.time));
  });
}

void process_batch(int start, int end) {
  for (int i = start; i < end; i++) {
    switch (q[i].type) {
      case T_INSERT:
        fen.add(q[i].index, +1);
        break;
      case T_ERASE:
        fen.add(q[i].index, -1);
        break;
      default:
        answer[q[i].index - 1] = fen.prefix_sum(q[i].index);
    }
  }
}

void cleanup_batch(int start, int end) {
  for (int i = start; i < end; i++) {
    switch (q[i].type) {
      case T_INSERT:
        fen.add(q[i].index, -1);
        break;
      case T_ERASE:
        fen.add(q[i].index, +1);
        break;
      default:
        ; // nimic
    }
  }
}

void process_queries() {
  for (int i = 0; i < n; i++) {
    answer[i] = NONE;
  }
  fen.init(n);

  int start = 0;
  while (start < n) {
    int end = start + 1;
    while ((end < n) && (q[end].val == q[start].val)) {
      end++;
    }
    process_batch(start, end);
    cleanup_batch(start, end);
    start = end;
  }
}

void write_answers() {
  for (int i = 0; i < n; i++) {
    if (answer[i] != NONE) {
      printf("%d\n", answer[i]);
    }
  }
}

int main() {
  read_queries();
  sort_by_value_then_time();
  process_queries();
  write_answers();

  return 0;
}
