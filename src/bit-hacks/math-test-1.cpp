#include <stdio.h>

const int MAX_STUDENTS = 10;
const int MAX_QUESTIONS = 10'000;
const int NIL = -1;

short solvers[MAX_QUESTIONS];
int expect[MAX_STUDENTS];
int n, m;

struct score_chart {
  short points[MAX_QUESTIONS];
  int surprise;

  void improve(const score_chart& other) {
    if (other.surprise > surprise) {
      surprise = other.surprise;
      for (int i = 0; i < m; i++) {
        points[i] = other.points[i];
      }
    }
  }

  void init_surprise(int stud_mask) {
    surprise = 0;
    for (int i = 0; i < n; i++) {
      int coef = (stud_mask & (1 << i)) ? -1 : +1;
      surprise += coef * expect[i];
    }
  }
};

struct list {
  short val, next;
};

list list_buf[MAX_QUESTIONS];

struct list_array {
  int head[2 * MAX_STUDENTS + 1]; // poziții de la -n la +n, deplasate cu +n
  int ptr;
  list* buf;

  void init(list* buf) {
    this->buf = buf;
    ptr = 0;
    for (int i = 0; i <= 2 * n; i++) {
      head[i] = NIL;
    }
  }

  void add(int pos, int val) {
    pos += n;
    buf[ptr].val = val;
    buf[ptr].next = head[pos];
    head[pos] = ptr++;
  }

  int get_list(int pos) {
    return head[pos + n];
  }
};

list_array by_fit_score;

void read_data() {
  scanf("%d %d", &n, &m);
  for (int i = 0; i < n; i++) {
    scanf("%d ", &expect[i]);
  }
  for (int i = 0; i < m; i++) {
    solvers[i] = 0;
  }
  for (int stud = 0; stud < n; stud++) {
    for (int q = 0; q < m; q++) {
      int bit = getchar() - '0';
      solvers[q] |= bit << stud;
    }
    getchar();
  }
}

int get_fit_score(int solvers, int stud_mask) {
  int good = __builtin_popcount(solvers & stud_mask);
  int bad = __builtin_popcount(solvers & ~stud_mask);
  return good - bad;
}

void sort_fit_scores(score_chart& sc) {
  int points = 1;
  for (int score = -n; score <= +n; score++) {
    int ptr = by_fit_score.get_list(score);
    while (ptr != NIL) {
      int q = list_buf[ptr].val;
      sc.points[q] = points;
      sc.surprise += points * score;
      points++;
      ptr = list_buf[ptr].next;
    }
  }
}

void build_score_chart(int stud_mask, score_chart& sc) {
  sc.init_surprise(stud_mask);
  by_fit_score.init(list_buf);
  for (int q = 0; q < m; q++) {
    int score = get_fit_score(solvers[q], stud_mask);
    by_fit_score.add(score, q);
  }
  sort_fit_scores(sc);
}

void try_all_student_masks(score_chart& sol) {
  score_chart sc;
  sol.surprise = -1;

  for (int stud_mask = 0; stud_mask < (1 << n); stud_mask++) {
    build_score_chart(stud_mask, sc);
    sol.improve(sc);
  }
}

void write_solution(score_chart& sol) {
  for (int i = 0; i < m; i++) {
    printf("%d ", sol.points[i]);
  }
  printf("\n");
}

void solve_test() {
  score_chart sol;
  read_data();
  try_all_student_masks(sol);
  write_solution(sol);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
