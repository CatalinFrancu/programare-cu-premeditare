#include <stdio.h>

typedef unsigned char byte;

const int SIZE = 70;
const int NUM_BOARDS = 916'895; // comb(SIZE, 4)
const byte INFINITY = 0xff;
const int NUM_MOVES = 20; // 4 piese, fiecare cu 2 deplasări și 3 salturi
const int JUMP_TARGET[4][3] = { {1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2} };

int comb[SIZE + 1][5];

struct board {
  byte p[4];
  int rank;

  board operator=(const board& other) {
    for (int i = 0; i < 4; i++) {
      p[i] = other.p[i];
    }
    rank = other.rank;
    return *this;
  }

  void compute_rank() {
    rank =
      (comb[SIZE][4] - comb[SIZE - p[0]][4]) +
      (comb[SIZE - 1 - p[0]][3] - comb[SIZE - p[1]][3]) +
      (comb[SIZE - 1 - p[1]][2] - comb[SIZE - p[2]][2]) +
      (p[3] - p[2] - 1);
  }

  void sort(int i) {
    while ((i > 0) && (p[i] < p[i - 1])) {
      int tmp = p[i]; p[i] = p[i - 1]; p[i - 1] = tmp;
      i--;
    }
    while ((i < 3) && (p[i] > p[i + 1])) {
      int tmp = p[i]; p[i] = p[i + 1]; p[i + 1] = tmp;
      i++;
    }
  }

  bool empty(int pos) {
    return (pos >= 0) && (pos < SIZE) && (p[0] != pos) &&
      (p[1] != pos) && (p[2] != pos) && (p[3] != pos);
  }

  bool make_slide_move(int i, int delta) {
    if (empty(p[i] + delta)) {
      p[i] += delta;
      // nu este nevoie de resortare
      return true;
    } else {
      return false;
    }
  }

  bool make_jump_move(int i, int j) {
    int x = 2 * p[j] - p[i];
    if (empty(x)) {
      p[i] = x;
      sort(i);
      return true;
    } else {
      return false;
    }
  }

  bool make_move(int index) {
    int i = index / 5, j;
    int type = index % 5;

    bool success;
    switch (type) {
      case 0:
      case 1:
      case 2:
        j = JUMP_TARGET[i][type];
        success = make_jump_move(i, j);
        break;

      case 3:
        success = make_slide_move(i, +1);
        break;

      case 4:
        success = make_slide_move(i, -1);
        break;
    }

    if (success) {
      compute_rank();
    }

    return success;
  }
};

const board START = { { 0, 1, 2, 3 }, 0 };

board q[NUM_BOARDS];
byte dist[NUM_BOARDS];

void precompute_combinations() {
  comb[0][0] = 1;
  for (int n = 1; n <= SIZE; n++) {
    comb[n][0] = 1;
    for (int k = 1; k <= 4; k++) {
      comb[n][k] = comb[n - 1][k] + comb[n - 1][k - 1];
    }
  }
}

void bfs() {
  for (int i = 0; i < NUM_BOARDS; i++) {
    dist[i] = INFINITY;
  }
  dist[0] = 0;

  int head = 0, tail = 0;
  q[tail++] = START;

  while (head != tail) {
    board& b = q[head++];
    board b2;

    for (int m = 0; m < NUM_MOVES; m++) {
      b2 = b;
      if (b2.make_move(m) && (dist[b2.rank] == INFINITY)) {
        dist[b2.rank] = 1 + dist[b.rank];
        q[tail++] = b2;
      }
    }
  }
}

void process_queries() {
  int num_tests;
  board b;

  scanf("%d", &num_tests);

  while (num_tests--) {
    scanf("%hhd %hhd %hhd %hhd", &b.p[0], &b.p[1], &b.p[2], &b.p[3]);
    b.p[0]--;
    b.p[1]--;
    b.p[2]--;
    b.p[3]--;
    b.compute_rank();
    printf("%d\n", dist[b.rank]);
  }
}

int main() {
  precompute_combinations();
  bfs();
  process_queries();

  return 0;
}
