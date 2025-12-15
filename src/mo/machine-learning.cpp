#include <algorithm>
#include <stdio.h>
#include <unordered_map>

const int MAX_N = 100'000;
const int BLOCK_SIZE = 2'154; // n^{2/3}
const int MAX_OPS = 100'000;

const int T_QUERY = 1;
const int T_UPDATE = 2;

struct query {
  int l, r, time, answer;
};

struct update {
  int pos, old_val, val, time;
};

int a[MAX_N];
query q[MAX_OPS];
update u[MAX_OPS];
int tmp[MAX_N + MAX_OPS];
int n, num_ops, num_queries, num_updates;

// Date specifice pentru Mo.
int f[MAX_N + MAX_OPS];
int num_having_f[MAX_N + MAX_OPS];

// Renumerotează valorile din vector și actualizările.
struct normalizer {
  std::unordered_map<int, int> map;

  int normalize(int x) {
    auto it = map.find(x);
    if (it == map.end()) {
      int result = 1 + map.size();
      map[x] = result;
      return result;
    } else {
      return it->second;
    }
  }
};

normalizer norm;

void read_array() {
  scanf("%d %d", &n, &num_ops);
  for (int i = 0; i < n; i++) {
    int x;
    scanf("%d", &x);
    a[i] = norm.normalize(x);
  }
}

void read_ops() {
  for (int i = 0; i < n; i++) {
    tmp[i] = a[i];
  }

  u[num_updates++] = { 0, 0, 0, 0 }; // santinelă stînga (t = 0)
  for (int t = 1; t <= num_ops; t++) {
    int type;
    scanf("%d", &type);
    if (type == T_QUERY) {
      int l, r;
      scanf("%d %d", &l, &r);
      q[num_queries++] = { l - 1, r - 1, t };
    } else {
      int pos, val;
      scanf("%d %d", &pos, &val);
      pos--;
      val = norm.normalize(val);
      u[num_updates++] = { pos, tmp[pos], val, t };
      tmp[pos] = val;
    }
  }
  u[num_updates++] = { 0, 0, 0, n + num_ops }; // santinelă dreapta (t = ∞)
}

void sort_queries() {
  std::sort(q, q + num_queries, [](query a, query b) {
    int x = a.l / BLOCK_SIZE, y = b.l / BLOCK_SIZE;
    if (x != y) {
      return (x < y);
    }

    x = a.r / BLOCK_SIZE, y = b.r / BLOCK_SIZE;
    if (x != y) {
      return (x < y);
    }

    return a.time < b.time;
  });
}

void change_frequency(int val, int delta) {
  num_having_f[f[val]]--;
  f[val] += delta;
  num_having_f[f[val]]++;
}

void add(int pos) {
  change_frequency(a[pos], +1);
}

void remove(int pos) {
  change_frequency(a[pos], -1);
}

void change_value(int l, int r, int pos, int val) {
  if ((pos >= l) && (pos <= r)) {
    change_frequency(a[pos], -1);
    change_frequency(val, +1);
  }
  a[pos] = val;
}

int get_mex() {
  int mex = 1;
  while (num_having_f[mex]) {
    mex++;
  }
  return mex;
}

void mo() {
  num_having_f[0] = n + num_updates; // ceva infinit
  int l = 0, r = -1, u_index = 1;
  for (int i = 0; i < num_queries; i++) {
    while (l > q[i].l) {
      add(--l);
    }
    while (r < q[i].r) {
      add(++r);
    }
    while (l < q[i].l) {
      remove(l++);
    }
    while (r > q[i].r) {
      remove(r--);
    }
    while (u[u_index].time < q[i].time) {
      change_value(l, r, u[u_index].pos, u[u_index].val);
      u_index++;
    }
    while (u[u_index - 1].time > q[i].time) {
      u_index--;
      change_value(l, r, u[u_index].pos, u[u_index].old_val);
    }
    q[i].answer = get_mex();
  }
}

void sort_queries_by_time() {
  std::sort(q, q + num_queries, [](query a, query b) {
    return a.time < b.time;
  });
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%d\n", q[i].answer);
  }
}

int main() {
  read_array();
  read_ops();
  sort_queries();
  mo();
  sort_queries_by_time();
  write_answers();

  return 0;
}
