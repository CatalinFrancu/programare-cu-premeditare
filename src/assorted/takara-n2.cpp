#include <algorithm>
#include <deque>
#include <stdio.h>

const int MAX_N = 5'000;

int n;
int c[MAX_N];
int d[MAX_N][MAX_N];

struct row_queue {
  std::deque<int> q;
  int i;

  void init(int i) {
    this->i = i;
    q.clear();
    q.push_back(i);
  }

  int cost(int j) {
    return d[i][j - 1] + c[j];
  }

  int first() {
    return cost(q.front());
  }

  void advance(int j) {
    while (d[i][q.front() - 1] < d[q.front() + 1][j]) {
      q.pop_front();
    }
    int z = cost(j);
    while (!q.empty() && (z < cost(q.back()))) {
      q.pop_back();
    }
    q.push_back(j);
  }
};

struct col_queue {
  std::deque<int> q;
  int j;

  void init(int j) {
    this->j = j;
    q.push_back(j);
  }

  int cost(int i) {
    return c[i] + d[i + 1][j];
  }

  int first() {
    return cost(q.front());
  }

  void advance(int i) {
    while (d[i][q.front() - 1] > d[q.front() + 1][j]) {
      q.pop_front();
    }
    int z = cost(i);
    while (!q.empty() && (z < cost(q.back()))) {
      q.pop_back();
    }
    q.push_back(i);
  }
};

row_queue rq;
col_queue cq[MAX_N];

void read_data() {
  FILE *f = fopen("takara.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &c[i]);
  }
  fclose(f);
}

void compute_costs() {
  for (int i = n - 1; i >= 0; i--) {
    rq.init(i);
    cq[i].init(i);

    for (int j = i + 1; j < n; j++) {
      rq.advance(j);
      cq[j].advance(i);
      d[i][j] = std::min(rq.first(), cq[j].first());
    }
  }
}

void write_result() {
  FILE* f = fopen("takara.out", "w");
  fprintf(f, "%d\n", d[0][n - 1]);
  fclose(f);
}

int main() {
  read_data();
  compute_costs();
  write_result();

  return 0;
}
