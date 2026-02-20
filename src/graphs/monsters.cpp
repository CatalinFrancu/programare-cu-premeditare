#include <stdio.h>
#include <queue>

const int MAX_SIZE = 1'000;
const int INFINITY = MAX_SIZE * MAX_SIZE + 1;

struct point {
  short r, c;
};

struct cell {
  int monster_dist, hero_dist;
  char dir;
  bool wall, edge;
};

std::queue<point> q;
cell a[MAX_SIZE + 2][MAX_SIZE + 2];
int rows, cols;
point hero;

void init_distances() {
  for (short r = 1; r <= rows; r++) {
    for (short c = 1; c <= cols; c++) {
      a[r][c].monster_dist = INFINITY;
      a[r][c].hero_dist = INFINITY;
    }
  }
}

void read_data() {
  scanf("%d %d ", &rows, &cols);

  init_distances();

  for (short r = 1; r <= rows; r++) {
    for (short c = 1; c <= cols; c++) {
      switch (getchar()) {
        case '#':
          a[r][c].wall = true;
          break;
        case 'A':
          hero = { r, c };
          break;
        case 'M':
          q.push({ r, c });
          a[r][c].monster_dist = 0;
          break;
      }
    }
    getchar();
  }
}

void pad_matrix() {
  for (int r = 0; r <= rows + 1; r++) {
    a[r][0].wall = a[r][cols + 1].wall = true;
  }
  for (int c = 0; c <= cols + 1; c++) {
    a[0][c].wall = a[rows + 1][c].wall = true;
  }
}

void mark_edges() {
  for (int r = 1; r <= rows; r++) {
    a[r][1].edge = a[r][cols].edge = true;
  }
  for (int c = 1; c <= cols; c++) {
    a[1][c].edge = a[rows][c].edge = true;
  }
}

void monster_move(short r, short c, int dist) {
  if (!a[r][c].wall && a[r][c].monster_dist == INFINITY) {
    a[r][c].monster_dist = dist;
    q.push({ r, c });
  }
}

void monster_bfs() {
  while (!q.empty()) {
    point p = q.front();
    q.pop();
    int d = a[p.r][p.c].monster_dist;

    monster_move(p.r, p.c - 1, d + 1);
    monster_move(p.r, p.c + 1, d + 1);
    monster_move(p.r - 1, p.c, d + 1);
    monster_move(p.r + 1, p.c, d + 1);
  }
}

void hero_move(short r, short c, int dist, char dir) {
  if (!a[r][c].wall &&
      (a[r][c].hero_dist == INFINITY) &&
      (dist < a[r][c].monster_dist)) {
    a[r][c].hero_dist = dist;
    a[r][c].dir = dir;
    q.push({ r, c });
  }
}

// Returnează ultimul punct explorat.
point hero_bfs() {
  a[hero.r][hero.c].hero_dist = 0;
  q.push(hero);
  point p = { 0, 0 };

  while (!q.empty() && !a[p.r][p.c].edge) {
    p = q.front();
    q.pop();
    int d = a[p.r][p.c].hero_dist;

    hero_move(p.r, p.c - 1, d + 1, 'L');
    hero_move(p.r, p.c + 1, d + 1, 'R');
    hero_move(p.r - 1, p.c, d + 1, 'U');
    hero_move(p.r + 1, p.c, d + 1, 'D');
  }

  return p;
}

void trace(int r, int c) {
  if (a[r][c].hero_dist) {
    switch (a[r][c].dir) {
      case 'L': trace(r, c + 1); break;
      case 'R': trace(r, c - 1); break;
      case 'U': trace(r + 1, c); break;
      case 'D': trace(r - 1, c); break;
    }
    putchar(a[r][c].dir);
  }
}

void solution(point dest) {
  if (a[dest.r][dest.c].edge) {
    printf("YES\n%d\n", a[dest.r][dest.c].hero_dist);
    trace(dest.r, dest.c);
    putchar('\n');
  } else {
    printf("NO\n");
  }
}

int main() {
  read_data();
  pad_matrix();
  mark_edges();
  monster_bfs();
  point dest = hero_bfs();
  solution(dest);

  return 0;
}
