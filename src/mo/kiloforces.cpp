#include <algorithm>
#include <stdio.h>

const int MAX_PROBLEMS = 100'000;
const int MAX_QUERIES = 100'000;
const int MAX_PENALTY = 100'000;
const int BLOCK_SIZE = 400;

struct problem {
  int score;
  int penalty;
};

struct query {
  int l, r;
  int id;
};

struct fen_data {
  long long sum;
  int cnt;

  fen_data operator +=(fen_data& other) {
    sum += other.sum;
    cnt += other.cnt;
    return *this;
  }
};

struct fenwick_tree {
  fen_data v[MAX_PENALTY + 1];

  fen_data prefix_sum(int pos) {
    fen_data d = { 0, 0 };
    while (pos) {
      d += v[pos];
      pos &= pos - 1;
    }
    return d;
  }

  void add(int pos, fen_data val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= MAX_PENALTY);
  }

  void check(int pos) {
    add(pos, { .sum = pos, .cnt = 1 });
  }

  void uncheck(int pos) {
    add(pos, { .sum = -pos, .cnt = -1 });
  }
};

struct mo_data {
  fenwick_tree fen;
  long long score;
  int width;

  void include(problem p) {
    fen_data d = fen.prefix_sum(p.penalty);
    score += p.score;
    score -= d.sum;
    score -= (long long)p.penalty * (width - d.cnt);
    width++;
    fen.check(p.penalty);
  }

  void exclude(problem p) {
    fen.uncheck(p.penalty);
    fen_data d = fen.prefix_sum(p.penalty);
    score -= p.score;
    score += d.sum;
    score += (long long)p.penalty * (width - d.cnt - 1);
    width--;
  }
};

problem p[MAX_PROBLEMS];
query q[MAX_QUERIES];
long long answer[MAX_QUERIES];
mo_data mod;
int n, num_queries;

void read_data() {
  scanf("%d %d", &n, &num_queries);
  for (int i = 1; i <= n; i++) {
    scanf("%d", &p[i].score);
  }
  for (int i = 1; i <= n; i++) {
    scanf("%d", &p[i].penalty);
  }
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d", &q[i].l, &q[i].r);
    q[i].id = i;
  }
}

void sort_queries_in_mo_order() {
  std::sort(q, q + num_queries, [](query a, query b) {
    int x = a.l / BLOCK_SIZE, y = b.l / BLOCK_SIZE;
    if (x != y) {
      return (x < y);
    } else if (x % 2) {
      return a.r > b.r;
    } else {
      return a.r < b.r;
    }
  });
}

void mo() {
  int l = 1, r = 0; // gol

  for (int i = 0; i < num_queries; i++) {
    while (l > q[i].l) {
      mod.include(p[--l]);
    }
    while (r < q[i].r) {
      mod.include(p[++r]);
    }
    while (l < q[i].l) {
      mod.exclude(p[l++]);
    }
    while (r > q[i].r) {
      mod.exclude(p[r--]);
    }

    answer[q[i].id] = mod.score;
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%lld\n", answer[i]);
  }
}

int main() {
  read_data();
  sort_queries_in_mo_order();
  mo();

  write_answers();

  return 0;
}
