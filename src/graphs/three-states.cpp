#include <stdio.h>

const int MAX_N = 1'000;
const int MAX_DEQUE = 1 << 17;
const int NUM_STATES = 3;
const char OPEN = '.';
const char WALL = '#';
const int INFINITY = 100'000'000;

typedef int dist_matrix[MAX_N + 2][MAX_N + 2];

struct point {
  short r, c;
};

const int NUM_DIR = 4;
const point DELTA[NUM_DIR] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };

struct deque {
  point buf[MAX_DEQUE];
  int left, right, size;

  void init() {
    size = 0;
  }

  bool is_empty() {
    return !size;
  }

  void push_left(point p) {
    if (size) {
      left = (left + MAX_DEQUE - 1) % MAX_DEQUE;
    } else {
      left = right = 0;
    }
    buf[left] = p;
    size++;
  }

  void push_right(point p) {
    if (size) {
      right = (right + 1) % MAX_DEQUE;
    } else {
      left = right = 0;
    }
    buf[right] = p;
    size++;
  }

  point pop_left() {
    point result = buf[left];
    size--;
    left = (left + 1) % MAX_DEQUE;
    return result;
  }
};

char map[MAX_N + 2][MAX_N + 2];
dist_matrix dist[NUM_STATES];
deque q;
int rows, cols;

void read_data() {
  scanf("%d %d ", &rows, &cols);
  for (int r = 1; r <= rows; r++) {
    scanf("%s ", &map[r][1]);
  }
}

void pad_map() {
  for (int r = 1; r <= rows; r++) {
    map[r][0] = map[r][cols + 1] = WALL;
  }
  for (int c = 1; c <= cols; c++) {
    map[0][c] = map[rows + 1][c] = WALL;
  }
}

void init_dist(dist_matrix& dist) {
  for (int r = 1; r <= rows; r++) {
    for (int c = 1; c <= cols; c++) {
      dist[r][c] = INFINITY;
    }
  }
}

void bfs(int state, short start_row, short start_col) {
  init_dist(dist[state]);
  dist[state][start_row][start_col] = 0;

  q.init();
  q.push_left({start_row, start_col});

  while (!q.is_empty()) {
    point p = q.pop_left();

    for (int dir = 0; dir < NUM_DIR; dir++) {
      short r = p.r + DELTA[dir].r;
      short c = p.c + DELTA[dir].c;

      if (map[r][c] != WALL) {
        int cost = (map[r][c] == OPEN) ? 1 : 0;
        int my_dist = dist[state][p.r][p.c];
        if (my_dist + cost < dist[state][r][c]) {
          dist[state][r][c] = my_dist + cost;
          if (cost) {
            q.push_right({r, c});
          } else {
            q.push_left({r, c});
          }
        }
      }
    }
  }
}

void bfs_driver() {
  point start[NUM_STATES];
  for (short r = 1; r <= rows; r++) {
    for (short c = 1; c <= cols; c++) {
      if ((map[r][c] != WALL) && (map[r][c] != OPEN)) {
        start[map[r][c] - '1'] = { r, c };
      }
    }
  }

  for (int s = 0; s < NUM_STATES; s++) {
    bfs(s, start[s].r, start[s].c);
  }
}

int min(int x, int y) {
  return (x < y) ?  x : y;
}

int choose_center() {
  int min_sum = INFINITY;

  for (int r = 1; r <= rows; r++) {
    for (int c = 1; c <= cols; c++) {
      int sum = (map[r][c] == '.') ? (1 - NUM_STATES) : 0;
      for (int s = 0; s < NUM_STATES; s++) {
        sum += dist[s][r][c];
      }
      min_sum = min(min_sum, sum);
    }
  }

  return min_sum;
}

void write_answer(int sum_dist) {
  int answer = (sum_dist == INFINITY) ? -1 : sum_dist;
  printf("%d\n", answer);
}

int main() {
  read_data();
  pad_map();
  bfs_driver();
  int sum_dist = choose_center();
  write_answer(sum_dist);

  return 0;
}
